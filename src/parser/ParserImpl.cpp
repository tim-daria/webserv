#include "parser/ParserImpl.hpp"

#include <cctype>
#include <cstdlib>
#include <sstream>
#include <stdexcept>

#include "Logger.hpp"

ParserImpl::ParserImpl(const std::string& content, const std::string& sourceName)
    : _sourceName(sourceName), _pos(0) {
    LOG_INFO("Lexer starts making vector of tokens");
    Lexer lexer(content);
    while (true) {
        Token t = lexer.nextToken();
        _tokens.push_back(t);
        if (t.type == TOKEN_END) break;
    }
}

std::vector<ServerConfig> ParserImpl::parseConfig() {
    std::vector<ServerConfig> servers;
    while (!isEnd()) {
        servers.push_back(parseServerBlock());
    }
    if (servers.empty()) {
        throwError(peek(), "no server blocks found");
    }
    return servers;
}

const Token& ParserImpl::peek() const { return _tokens[_pos]; }

const Token& ParserImpl::consume() {
    const Token& t = _tokens[_pos];
    if (_pos < _tokens.size()) _pos++;
    return t;
}

bool ParserImpl::isEnd() const { return peek().type == TOKEN_END; }

void ParserImpl::expectType(TokenType type, const std::string& message) {
    if (peek().type != type) throwError(peek(), message);
    consume();
}

Token ParserImpl::expectWordToken(const std::string& message) {
    if (peek().type != TOKEN_WORD) throwError(peek(), message);
    Token token = peek();
    consume();
    return token;
}

std::string ParserImpl::expectWord(const std::string& message) {
    Token token = expectWordToken(message);
    return token.text;
}

void ParserImpl::expectKeyword(const std::string& keyword) {
    if (peek().type != TOKEN_WORD || peek().text != keyword)
        throwError(peek(), "expected '" + keyword + "'");
    consume();
}

void ParserImpl::throwError(const Token& token, const std::string& message) {
    std::ostringstream oss;
    oss << _sourceName << ":" << token.line << ":" << token.column << ": " << message;
    LOG_ERROR(oss.str());
    throw std::runtime_error(oss.str());
}

bool ParserImpl::isNumber(const std::string& text) {
    if (text.empty()) return false;
    for (size_t i = 0; i < text.size(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(text[i]))) return false;
    }
    return true;
}

int ParserImpl::parseInt(const std::string& text, int minValue, int maxValue) {
    if (!isNumber(text)) return -1;
    long value = std::strtol(text.c_str(), NULL, 10);
    if (value < minValue || value > maxValue) return -1;
    return static_cast<int>(value);
}

bool ParserImpl::isValidMethod(const std::string& method) {
    return method == "GET" || method == "POST" || method == "DELETE";
}

bool ParserImpl::isOnOff(const std::string& value) { return value == "on" || value == "off"; }

bool ParserImpl::isIpAddress(const std::string& value) {
    std::string numStr;
    size_t dot = 0;
    size_t pos = 0;
    for (int i = 0; i < 3; ++i) {
        dot = value.find('.', pos);
        if (dot == std::string::npos) return false;
        numStr = value.substr(pos, dot - pos);
        if (parseInt(numStr, 0, 255) == -1) return false;
        pos = dot + 1;
    }
    dot = value.find('.', pos);
    if (dot != std::string::npos) return false;
    numStr = value.substr(pos);
    if (parseInt(numStr, 0, 255) == -1) return false;
    return true;
}

ServerConfig ParserImpl::parseServerBlock() {
    expectKeyword("server");
    expectType(TOKEN_LBRACE, "expected '{' after server");

    ServerConfig cfg = ServerConfig::makeDefault();
    RouteConfig serverDefaults;
    serverDefaults.applyDefaults();
    cfg.routes.clear();

    bool hasExplicitListen = false;
    bool hasExplicitErrorPages = false;
    std::string pendingCgiExt;
    std::vector<ParsedRoute> parsedRoutes;

    while (peek().type != TOKEN_RBRACE) {
        if (peek().type == TOKEN_END) throwError(peek(), "unexpected end of file");
        if (peek().type == TOKEN_WORD && peek().text == "location") {
            parsedRoutes.push_back(parseLocationBlock(pendingCgiExt));
            continue;
        }
        parseServerDirective(cfg, serverDefaults, hasExplicitListen, hasExplicitErrorPages,
                             pendingCgiExt);
    }
    expectType(TOKEN_RBRACE, "expected '}' after server block");

    if (!pendingCgiExt.empty()) throwError(peek(), "cgi_ext without cgi_path");
    // Check for mandatory server root directive
    // LOG_DEBUG("serverDefaults.rootDirectory: " + serverDefaults.rootDirectory);
    // if (serverDefaults.rootDirectory.empty()) throwError(peek(), "no server root location");
    if (!hasExplicitListen && !cfg.listen.empty()) {
        checkAddress(cfg.listen[0], peek());
    }
    if (parsedRoutes.empty()) {
        cfg.routes.push_back(serverDefaults);
    } else {
        for (size_t i = 0; i < parsedRoutes.size(); ++i) {
            applyServerDefaults(parsedRoutes[i], serverDefaults);
            cfg.routes.push_back(parsedRoutes[i].route);
        }
    }
    return cfg;
}

ParsedRoute ParserImpl::parseLocationBlock(std::string&) {
    expectKeyword("location");
    std::string path = expectWord("expected location path");
    if (path.size() > 1 && path[path.size() - 1] == '/') {
        path.erase(path.size() - 1);
    }
    expectType(TOKEN_LBRACE, "expected '{' after location path");

    ParsedRoute parsed;
    parsed.route = RouteConfig();
    parsed.route.url = path;
    parsed.hasRoot = false;
    parsed.hasIndex = false;
    parsed.hasMethods = false;
    parsed.hasAutoindex = false;
    parsed.hasAuth = false;
    parsed.hasReturn = false;
    parsed.hasCgi = false;
    parsed.hasUpload = false;
    parsed.hasMaxsize = false;

    std::string pendingCgiExt;
    while (peek().type != TOKEN_RBRACE) {
        if (peek().type == TOKEN_END) throwError(peek(), "unexpected end of file");
        parseLocationDirective(parsed, pendingCgiExt);
    }
    expectType(TOKEN_RBRACE, "expected '}' after location block");

    if (!pendingCgiExt.empty()) throwError(peek(), "cgi_ext without cgi_path");
    return parsed;
}

void ParserImpl::applyServerDefaults(ParsedRoute& parsed, const RouteConfig& serverDefaults) {
    if (!parsed.hasRoot) parsed.route.rootDirectory = serverDefaults.rootDirectory;
    if (!parsed.hasIndex) parsed.route.defaultFile = serverDefaults.defaultFile;
    if (!parsed.hasMethods) parsed.route.acceptedMethods = serverDefaults.acceptedMethods;
    if (!parsed.hasAutoindex) parsed.route.directoryListing = serverDefaults.directoryListing;
    if (!parsed.hasAuth) parsed.route.authRequired = serverDefaults.authRequired;
    if (!parsed.hasReturn) {
        parsed.route.hasReturn = serverDefaults.hasReturn;
        parsed.route.returnStatus = serverDefaults.returnStatus;
        parsed.route.returnUri = serverDefaults.returnUri;
    }
    if (!parsed.hasCgi) parsed.route.cgiHandlers = serverDefaults.cgiHandlers;
    if (!parsed.hasUpload) parsed.route.uploadDirectory = serverDefaults.uploadDirectory;
    if (!parsed.hasMaxsize) parsed.route.clientMaxBodySize = serverDefaults.clientMaxBodySize;
}

void ParserImpl::parseServerDirective(ServerConfig& cfg, RouteConfig& serverDefaults,
                                      bool& hasExplicitListen, bool& hasExplicitErrorPages,
                                      std::string& pendingCgiExt) {
    Token directiveToken = expectWordToken("expected directive");
    std::string directive = directiveToken.text;

    if (directive == "listen") {
        Token valueToken = expectWordToken("expected listen value");
        std::string value = valueToken.text;
        if (!hasExplicitListen) {
            cfg.listen.clear();
            hasExplicitListen = true;
        }
        std::string host = "0.0.0.0";
        std::string portStr = value;
        size_t colon = value.find(':');
        if (colon != std::string::npos) {
            host = value.substr(0, colon);
            portStr = value.substr(colon + 1);
            // if (host.empty()) throwError(valueToken, "empty host in listen");
            if (host == "localhost") host = "127.0.0.1";
            if (portStr.empty()) throwError(valueToken, "empty port in listen");
        }
        if (!isIpAddress(host)) throwError(valueToken, "invalid host address");
        int port = parseInt(portStr, 1, 65535);
        if (port == -1) throwError(valueToken, "invalid listen port");
        checkAddress(std::make_pair(host, port), valueToken);
        cfg.add_listen(host, port);
        expectType(TOKEN_SEMICOLON, "expected ';' after listen");
        return;
    }
    if (directive == "server_name") {
        cfg.serverName = expectWord("expected server_name value");
        expectType(TOKEN_SEMICOLON, "expected ';' after server_name");
        return;
    }
    if (directive == "root") {
        serverDefaults.rootDirectory = expectWord("expected root path");
        cfg.rootPath = serverDefaults.rootDirectory;
        expectType(TOKEN_SEMICOLON, "expected ';' after root");
        return;
    }
    if (directive == "client_max_body_size") {
        Token valueToken = expectWordToken("expected client_max_body_size value");
        int size = parseInt(valueToken.text, 1, 2147483647);
        if (size == -1) throwError(valueToken, "invalid client_max_body_size");
        cfg.clientMaxBodySize = static_cast<size_t>(size);
        serverDefaults.clientMaxBodySize = static_cast<size_t>(size);
        expectType(TOKEN_SEMICOLON, "expected ';' after client_max_body_size");
        return;
    }
    if (directive == "error_page") {
        Token statusToken = expectWordToken("expected error_page status");
        int status = parseInt(statusToken.text, 100, 599);
        if (status == -1) throwError(statusToken, "invalid error_page status");
        std::string path = expectWord("expected error_page path");
        if (!hasExplicitErrorPages) {
            cfg.errorPages.clear();
            hasExplicitErrorPages = true;
        }
        cfg.errorPages[status] = path;
        expectType(TOKEN_SEMICOLON, "expected ';' after error_page");
        return;
    }
    if (directive == "index") {
        serverDefaults.defaultFile = expectWord("expected index file");
        expectType(TOKEN_SEMICOLON, "expected ';' after index");
        return;
    }
    if (directive == "limit_except") {
        serverDefaults.acceptedMethods.clear();
        bool hasMethod = false;
        while (peek().type != TOKEN_SEMICOLON) {
            Token methodToken = expectWordToken("expected method");
            if (!isValidMethod(methodToken.text)) throwError(methodToken, "invalid method");
            std::string method = methodToken.text;
            serverDefaults.add_acceptedMethod(method);
            hasMethod = true;
        }
        if (!hasMethod) throwError(peek(), "limit_except requires methods");
        expectType(TOKEN_SEMICOLON, "expected ';' after limit_except");
        return;
    }
    if (directive == "autoindex") {
        Token valueToken = expectWordToken("expected autoindex value");
        if (!isOnOff(valueToken.text)) throwError(valueToken, "autoindex expects on/off");
        serverDefaults.directoryListing = (valueToken.text == "on");
        expectType(TOKEN_SEMICOLON, "expected ';' after autoindex");
        return;
    }
    if (directive == "auth_required") {
        Token valueToken = expectWordToken("expected auth_required value");
        if (!isOnOff(valueToken.text)) throwError(valueToken, "auth_required expects on/off");
        serverDefaults.authRequired = (valueToken.text == "on");
        expectType(TOKEN_SEMICOLON, "expected ';' after auth_required");
        return;
    }
    if (directive == "return") {
        Token statusToken = expectWordToken("expected return status");
        int status = parseInt(statusToken.text, 100, 599);
        if (status == -1) throwError(statusToken, "invalid return status");
        serverDefaults.hasReturn = true;
        serverDefaults.returnStatus = status;
        serverDefaults.returnUri = expectWord("expected return uri");
        expectType(TOKEN_SEMICOLON, "expected ';' after return");
        return;
    }
    if (directive == "cgi_ext") {
        if (!pendingCgiExt.empty()) throwError(peek(), "cgi_ext without cgi_path");
        Token valueToken = expectWordToken("expected cgi_ext value");
        pendingCgiExt = valueToken.text;
        expectType(TOKEN_SEMICOLON, "expected ';' after cgi_ext");
        return;
    }
    if (directive == "cgi_path") {
        if (pendingCgiExt.empty()) throwError(peek(), "cgi_path without cgi_ext");
        std::string path = expectWord("expected cgi_path value");
        serverDefaults.set_cgiHandler(pendingCgiExt, path);
        pendingCgiExt.clear();
        expectType(TOKEN_SEMICOLON, "expected ';' after cgi_path");
        return;
    }
    if (directive == "upload_store") {
        serverDefaults.uploadDirectory = expectWord("expected upload_store path");
        expectType(TOKEN_SEMICOLON, "expected ';' after upload_store");
        return;
    }

    throwError(directiveToken, "unknown directive: " + directive);
}

void ParserImpl::parseLocationDirective(ParsedRoute& parsed, std::string& pendingCgiExt) {
    Token directiveToken = expectWordToken("expected directive");
    std::string directive = directiveToken.text;

    if (directive == "root") {
        parsed.route.rootDirectory = expectWord("expected root path");
        parsed.hasRoot = true;
        expectType(TOKEN_SEMICOLON, "expected ';' after root");
        return;
    }
    if (directive == "index") {
        parsed.route.defaultFile = expectWord("expected index file");
        parsed.hasIndex = true;
        expectType(TOKEN_SEMICOLON, "expected ';' after index");
        return;
    }
    if (directive == "limit_except") {
        parsed.route.acceptedMethods.clear();
        bool hasMethod = false;
        while (peek().type != TOKEN_SEMICOLON) {
            Token methodToken = expectWordToken("expected method");
            if (!isValidMethod(methodToken.text)) throwError(methodToken, "invalid method");
            parsed.route.add_acceptedMethod(methodToken.text);
            hasMethod = true;
        }
        if (!hasMethod) throwError(peek(), "limit_except requires methods");
        parsed.hasMethods = true;
        expectType(TOKEN_SEMICOLON, "expected ';' after limit_except");
        return;
    }
    if (directive == "autoindex") {
        Token valueToken = expectWordToken("expected autoindex value");
        if (!isOnOff(valueToken.text)) throwError(valueToken, "autoindex expects on/off");
        parsed.route.directoryListing = (valueToken.text == "on");
        parsed.hasAutoindex = true;
        expectType(TOKEN_SEMICOLON, "expected ';' after autoindex");
        return;
    }
    if (directive == "auth_required") {
        Token valueToken = expectWordToken("expected auth_required value");
        if (!isOnOff(valueToken.text)) throwError(valueToken, "auth_required expects on/off");
        parsed.route.authRequired = (valueToken.text == "on");
        parsed.hasAuth = true;
        expectType(TOKEN_SEMICOLON, "expected ';' after auth_required");
        return;
    }
    if (directive == "return") {
        Token statusToken = expectWordToken("expected return status");
        int status = parseInt(statusToken.text, 100, 599);
        if (status == -1) throwError(statusToken, "invalid return status");
        parsed.route.hasReturn = true;
        parsed.route.returnStatus = status;
        parsed.route.returnUri = expectWord("expected return uri");
        parsed.hasReturn = true;
        expectType(TOKEN_SEMICOLON, "expected ';' after return");
        return;
    }
    if (directive == "upload_store") {
        parsed.route.uploadDirectory = expectWord("expected upload_store path");
        parsed.hasUpload = true;
        expectType(TOKEN_SEMICOLON, "expected ';' after upload_store");
        return;
    }
    if (directive == "cgi_ext") {
        if (!pendingCgiExt.empty()) throwError(peek(), "cgi_ext without cgi_path");
        Token valueToken = expectWordToken("expected cgi_ext value");
        pendingCgiExt = valueToken.text;
        expectType(TOKEN_SEMICOLON, "expected ';' after cgi_ext");
        return;
    }
    if (directive == "cgi_path") {
        if (pendingCgiExt.empty()) throwError(peek(), "cgi_path without cgi_ext");
        std::string path = expectWord("expected cgi_path value");
        parsed.route.set_cgiHandler(pendingCgiExt, path);
        parsed.hasCgi = true;
        pendingCgiExt.clear();
        expectType(TOKEN_SEMICOLON, "expected ';' after cgi_path");
        return;
    }
    if (directive == "client_max_body_size") {
        Token valueToken = expectWordToken("expected client_max_body_size value");
        int size = parseInt(valueToken.text, 1, 2147483647);
        if (size == -1) throwError(valueToken, "invalid client_max_body_size");
        parsed.hasMaxsize = true;
        parsed.route.clientMaxBodySize = static_cast<size_t>(size);
        expectType(TOKEN_SEMICOLON, "expected ';' after client_max_body_size");
        return;
    }

    throwError(directiveToken, "unknown directive in location: " + directive);
}

void ParserImpl::checkAddress(std::pair<std::string, int> new_addr, const Token& valueToken) {
    std::string host = new_addr.first;
    int port = new_addr.second;
    for (size_t i = 0; i < _addresses.size(); ++i) {
        if (port == _addresses[i].second) {
            if (host == "0.0.0.0" || _addresses[i].first == "0.0.0.0") {
                std::string msg = "port " + toString(port) + " is already declared";
                throwError(valueToken, msg);
            }
            if (host == _addresses[i].first) {
                std::string msg = "address " + host + ":" + toString(port) + " is already declared";
                throwError(valueToken, msg);
            }
        }
    }
    _addresses.push_back(new_addr);
}

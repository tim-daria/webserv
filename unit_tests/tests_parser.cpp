/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tests_parser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 16:46:18 by nefimov           #+#    #+#             */
/*   Updated: 2026/06/02 17:13:05 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "catch.hpp"
#include "parser/Parser.hpp"

static std::string sampleConfig() {
    return "server {\n"
           "    listen 8080;\n"
           "    server_name localhost;\n"
           "    root ./www;\n"
           "    client_max_body_size 1048576;\n"
           "    error_page 404 /errors/404.html;\n"
           "    index index.html;\n"
           "    location / {\n"
           "        limit_except GET POST;\n"
           "    }\n"
           "    location /uploads {\n"
           "        limit_except GET POST DELETE;\n"
           "        autoindex on;\n"
           "    }\n"
           "}\n";
}

TEST_CASE("Parser parses default-like config", "[Parser]") {
    std::vector<ServerConfig> configs = Parser::parseString(sampleConfig(), "sample");
    REQUIRE(configs.size() == 1);

    const ServerConfig& cfg = configs[0];
    REQUIRE(cfg.listen.size() == 1);
    REQUIRE(cfg.listen[0].second == 8080);
    REQUIRE(cfg.serverName == "localhost");
    REQUIRE(cfg.routes.size() == 2);

    const RouteConfig& root = cfg.routes[0];
    REQUIRE(root.url == "/");
    REQUIRE(root.rootDirectory == "./www");
    REQUIRE(root.defaultFile == "index.html");
    REQUIRE(root.isMethodAllowed("GET"));
    REQUIRE(root.isMethodAllowed("POST"));
    REQUIRE(!root.isMethodAllowed("DELETE"));

    const RouteConfig& uploads = cfg.routes[1];
    REQUIRE(uploads.url == "/uploads");
    REQUIRE(uploads.directoryListing);
    REQUIRE(uploads.isMethodAllowed("DELETE"));
}

TEST_CASE("Parser parses default.conf config file", "[Parser]") {
    std::vector<ServerConfig> configs = Parser::parseFile("unit_tests/test_configs/default.conf");
    REQUIRE(configs.size() == 1);

    const ServerConfig& cfg = configs[0];
    REQUIRE(cfg.listen.size() == 1);
    REQUIRE(cfg.listen[0].second == 8080);
    REQUIRE(cfg.serverName == "localhost");
    REQUIRE(cfg.clientMaxBodySize == 1048576);

    REQUIRE(cfg.errorPages.size() == 9);
    REQUIRE(cfg.errorPages.at(400) == "/errors/400.html");
    REQUIRE(cfg.errorPages.at(401) == "/errors/401.html");
    REQUIRE(cfg.errorPages.at(403) == "/errors/403.html");
    REQUIRE(cfg.errorPages.at(404) == "/errors/404.html");
    REQUIRE(cfg.errorPages.at(405) == "/errors/405.html");
    REQUIRE(cfg.errorPages.at(411) == "/errors/411.html");
    REQUIRE(cfg.errorPages.at(413) == "/errors/413.html");
    REQUIRE(cfg.errorPages.at(500) == "/errors/500.html");
    REQUIRE(cfg.errorPages.at(501) == "/errors/501.html");

    REQUIRE(cfg.routes.size() == 10);

    auto findRoute = [&](const std::string& url) -> const RouteConfig* {
        for (size_t i = 0; i < cfg.routes.size(); ++i) {
            if (cfg.routes[i].url == url) return &cfg.routes[i];
        }
        return nullptr;
    };

    const RouteConfig* root = findRoute("/");
    REQUIRE(root != nullptr);
    REQUIRE(root->rootDirectory == "./www");
    REQUIRE(root->defaultFile == "index.html");
    REQUIRE(root->isMethodAllowed("GET"));
    REQUIRE(root->isMethodAllowed("POST"));
    REQUIRE(!root->isMethodAllowed("DELETE"));

    const RouteConfig* upload = findRoute("/upload");
    REQUIRE(upload != nullptr);
    REQUIRE(upload->isMethodAllowed("GET"));
    REQUIRE(upload->isMethodAllowed("POST"));
    REQUIRE(!upload->isMethodAllowed("DELETE"));

    const RouteConfig* uploads = findRoute("/uploads");
    REQUIRE(uploads != nullptr);
    REQUIRE(uploads->directoryListing);
    REQUIRE(uploads->isMethodAllowed("DELETE"));

    const RouteConfig* del = findRoute("/delete");
    REQUIRE(del != nullptr);
    REQUIRE(del->isMethodAllowed("GET"));
    REQUIRE(del->isMethodAllowed("DELETE"));

    const RouteConfig* redirected_here = findRoute("/redirected_here");
    REQUIRE(redirected_here != nullptr);
    REQUIRE(redirected_here->isMethodAllowed("GET"));
    REQUIRE(!redirected_here->isMethodAllowed("POST"));

    const RouteConfig* redirect = findRoute("/redirect");
    REQUIRE(redirect != nullptr);
    REQUIRE(redirect->hasReturn);
    REQUIRE(redirect->returnStatus == 301);
    REQUIRE(redirect->returnUri == "/redirected_here");

    const RouteConfig* assets = findRoute("/assets");
    REQUIRE(assets != nullptr);
    REQUIRE(assets->isMethodAllowed("GET"));
    REQUIRE(!assets->isMethodAllowed("POST"));

    const RouteConfig* method_not_allowed = findRoute("/method_not_allowed");
    REQUIRE(method_not_allowed != nullptr);
    REQUIRE(method_not_allowed->isMethodAllowed("DELETE"));
    REQUIRE(!method_not_allowed->isMethodAllowed("GET"));

    const RouteConfig* auth_required = findRoute("/auth_required");
    REQUIRE(auth_required != nullptr);
    REQUIRE(auth_required->isMethodAllowed("GET"));
    REQUIRE(auth_required->authRequired);

    const RouteConfig* cgi = findRoute("/cgi-bin");
    REQUIRE(cgi != nullptr);
    REQUIRE(cgi->isMethodAllowed("GET"));
    REQUIRE(cgi->isMethodAllowed("POST"));
    REQUIRE(cgi->cgiHandlers.count(".py") == 1);
    REQUIRE(cgi->cgiHandlers.at(".py") == "/usr/bin/python3");
}

TEST_CASE("Check default index", "[Parser]") {
    SECTION("Correct input all different index directives") {
        std::string sampleConfig(
            "server {"
            "    listen 127.0.0.1:8080;\n"
            "    index index_0.html;\n"
            "    \n"
            "    location / {\n"
            "        index index_1.html;\n"
            "    }\n"
            "    location /download/ {\n"
            "        index index_2.html;\n"
            "    }\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].routes.size() == 2);
        REQUIRE(configs[0].routes[0].defaultFile == "index_1.html");
        REQUIRE(configs[0].routes[1].defaultFile == "index_2.html");
    }

    SECTION("Correct input all root index directives") {
        std::string sampleConfig(
            "server {"
            "    listen 127.0.0.1:8080;\n"
            "    index index_0.html;\n"
            "    \n"
            "    location / {\n"
            "    }\n"
            "    location /download/ {\n"
            "    }\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].routes.size() == 2);
        REQUIRE(configs[0].routes[0].defaultFile == "index_0.html");
        REQUIRE(configs[0].routes[1].defaultFile == "index_0.html");
    }

    SECTION("Correct input all default index directives") {
        std::string sampleConfig(
            "server {"
            "    listen 127.0.0.1:8080;\n"
            "    \n"
            "    location / {\n"
            "    }\n"
            "    location /download/ {\n"
            "    }\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].routes.size() == 2);
        REQUIRE(configs[0].routes[0].defaultFile == "index.html");
        REQUIRE(configs[0].routes[1].defaultFile == "index.html");
    }

    SECTION("Correct input one default index directives") {
        std::string sampleConfig(
            "server {"
            "    listen 127.0.0.1:8080;\n"
            "    \n"
            "    location / {\n"
            "        index index_1.html;\n"
            "    }\n"
            "    location /download/ {\n"
            "    }\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].routes.size() == 2);
        REQUIRE(configs[0].routes[0].defaultFile == "index_1.html");
        REQUIRE(configs[0].routes[1].defaultFile == "index.html");
    }
}

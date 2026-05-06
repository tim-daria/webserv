/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/30 13:53:22 by dtimofee          #+#    #+#             */
/*   Updated: 2026/04/07 21:34:31 by tsemenov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Handler.hpp"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sstream>

Handler::Handler() {}

Handler::Handler(const ServerConfig& config) : _serverConfig(config) {}

Handler::~Handler() {}

int Handler::checkPath(const std::string& path, struct stat& info) {
    if (stat(path.c_str(), &info) != 0) {
        return 404;
    }
    if (S_ISDIR(info.st_mode)) {
        if (access(path.c_str(), X_OK) != 0) {
            return 403;
        }
        return 0;
    }
    if (access(path.c_str(), R_OK) != 0) {
        return 403;
    }
    return 0;
}

std::string Handler::getContentType(const std::string& path) {
    size_t dot_pos = path.rfind('.');
    if (dot_pos == std::string::npos) {
        return "application/octet-stream";
    }
    std::string ext = path.substr(dot_pos);
    struct MimeType {
        std::string extension;
        std::string type;
    };
    MimeType mimeTypes[] = {{".html", "text/html"},
                            {".css", "text/css"},
                            {".js", "application/javascript"},
                            {".jpg", "image/jpeg"},
                            {".jpeg", "image/jpeg"},
                            {".png", "image/png"},
                            {".gif", "image/gif"},
                            {".pdf", "application/pdf"},
                            {".txt", "text/plain"},
                            {".json", "application/json"},
                            {".ico", "image/x-icon"}};
    size_t size = sizeof(mimeTypes) / sizeof(mimeTypes[0]);
    for (size_t i = 0; i < size; i++) {
        if (mimeTypes[i].extension == ext) {
            return mimeTypes[i].type;
        }
    }
    return "application/octet-stream";
}

HttpResponse Handler::serveFile(const std::string& path) {
    int fd = open(path.c_str(), O_RDONLY);
    if (fd == -1) {
        makeError(404);
    }
    std::string body;
    char buf[1024];
    int bytes;
    while (bytes = read(fd, buf, sizeof(buf)) > 0) {
        body.append(buf, bytes);
    }
    std::vector<std::pair<std::string, std::string> > headers;
    headers.push_back(std::make_pair("Content-Type", getContentType(path)));
    headers.push_back(std::make_pair("Content-Length", std::to_string(body.size())));
    return HttpResponse(200, body, headers);
}

HttpResponse Handler::handleDirectory(const std::string& path, RouteConfig* _location) {}

HttpResponse Handler::handle_request(HttpRequest& request) {
    RouteConfig* _location = _serverConfig.findMatchingLocation(request.path);
    if (!_location) {
        return makeError(404);
    }
    if (!_location->isMethodAllowed(request.method)) {
        return makeError(405);
    }
    if (request.method == "GET") {
        return handleGet(request, _location);
    } else if (request.method == "POST" || request.method == "DELETE") {
        return get_default_response(request);
    }
    return makeError(405);
}

HttpResponse Handler::handleGet(const HttpRequest& request, RouteConfig* _location) {
    std::string fullPath = _location->rootDirectory + request.path;
    struct stat info;
    int status = checkPath(fullPath, info);
    if (status != 0) {
        makeError(status);
    }
    if (S_ISDIR(info.st_mode)) {
        return handleDirectory(fullPath, _location);
    }
    return serveFile(fullPath);
}

HttpResponse Handler::get_default_response(const HttpRequest&) {
    std::stringstream body;
    std::stringstream len;
    std::vector<std::pair<std::string, std::string> > default_headers;
    body << "<h1>Hello webserv!</h1>";
    default_headers.push_back(std::make_pair("Content-Type", "text/html"));
    len << body.str().size();
    default_headers.push_back(std::make_pair("Content-Length", len.str()));
    return HttpResponse(200, body.str(), default_headers);
}

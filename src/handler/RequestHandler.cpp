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

#include "RequestHandler.hpp"

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sstream>

#include "PathUtils.hpp"

Handler::Handler(ServerConfig& config)
    : _serverConfig(config), _errorHandler(), _fileService(), _autoIndex() {}

// Handler::~Handler() {}

// int Handler::checkPath(const std::string& path, struct stat& info) {
//     if (stat(path.c_str(), &info) != 0) {
//         return HTTP_NOT_FOUND;
//     }
//     if (S_ISDIR(info.st_mode)) {
//         if (access(path.c_str(), X_OK) != 0) {
//             return HTTP_FORBIDDEN;
//         }
//         return HTTP_OK;
//     }
//     if (access(path.c_str(), R_OK) != 0) {
//         return HTTP_FORBIDDEN;
//     }
//     return HTTP_OK;
// }

// std::string Handler::getContentType(const std::string& path) {
//     size_t dot_pos = path.rfind('.');
//     if (dot_pos == std::string::npos) {
//         return "application/octet-stream";
//     }
//     std::string ext = path.substr(dot_pos);
//     struct MimeType {
//         std::string extension;
//         std::string type;
//     };
//     MimeType mimeTypes[] = {{".html", "text/html"},
//                             {".css", "text/css"},
//                             {".js", "application/javascript"},
//                             {".jpg", "image/jpeg"},
//                             {".jpeg", "image/jpeg"},
//                             {".png", "image/png"},
//                             {".gif", "image/gif"},
//                             {".pdf", "application/pdf"},
//                             {".txt", "text/plain"},
//                             {".json", "application/json"},
//                             {".ico", "image/x-icon"}};
//     size_t size = sizeof(mimeTypes) / sizeof(mimeTypes[0]);
//     for (size_t i = 0; i < size; i++) {
//         if (mimeTypes[i].extension == ext) {
//             return mimeTypes[i].type;
//         }
//     }
//     return "application/octet-stream";
// }

HttpResponse Handler::serveFile(const std::string& path) {
    std::string body;
    int status = _fileService.readFile(path, body);
    if (status != HTTP_OK) {
        return _errorHandler.makeError(status);
    }
    return HttpResponse::make(HTTP_OK, body, PathUtils::getContentType(path));
    // std::vector<std::pair<std::string, std::string> > headers;
    // headers.push_back(std::make_pair("Content-Type", PathUtils::getContentType(path)));
    // headers.push_back(std::make_pair("Content-Length", std::to_string(body.size())));
    // return HttpResponse(HTTP_OK, body, headers);
}

// HttpResponse Handler::generateListing(const std::string& path) {
//     DIR* dir = opendir(path.c_str());
//     if (!dir) {
//         return _errorHandler.makeError(HTTP_FORBIDDEN);
//     }
//     struct dirent* dp;
//     while ((dp = readdir(dir)) != NULL) {
//     }
// }

HttpResponse Handler::handleDirectory(const std::string& path, const std::string& uri,
                                      RouteConfig* _location) {
    struct stat info;
    std::string index_path = path + "/" + _location->defaultFile;
    int status = _fileService.checkPath(index_path, info);
    if (status == HTTP_OK && S_ISREG(info.st_mode)) {
        return serveFile(index_path);
    }
    if (status == HTTP_NOT_FOUND && _location->directoryListing) {
        std::string body;
        int dir_status = _autoIndex.generate(path, uri, body);
        if (dir_status == HTTP_OK) {
            return HttpResponse::make(HTTP_OK, body, "text/html");
        }
    }
    return _errorHandler.makeError(HTTP_FORBIDDEN);
}

HttpResponse Handler::handleGet(const HttpRequest& request, RouteConfig* _location) {
    std::string fullPath = _location->rootDirectory + request.path;
    struct stat info;
    int status = _fileService.checkPath(fullPath, info);
    if (status != HTTP_OK) {
        return _errorHandler.makeError(status);
    }
    if (S_ISDIR(info.st_mode)) {
        return handleDirectory(fullPath, request.path, _location);
    }
    return serveFile(fullPath);
}

HttpResponse Handler::handle_request(HttpRequest& request) {
    RouteConfig* _location = _serverConfig.findMatchingLocation(request.path);
    if (!_location) {
        return _errorHandler.makeError(HTTP_NOT_FOUND);
    }
    if (!_location->isMethodAllowed(request.method)) {
        return _errorHandler.makeError(HTTP_METHOD_NOT_ALLOWED);
    }
    if (request.method == "GET") {
        return handleGet(request, _location);
    } else if (request.method == "POST" || request.method == "DELETE") {
        return get_default_response(request);
    }
    return _errorHandler.makeError(HTTP_METHOD_NOT_ALLOWED);
}

HttpResponse Handler::get_default_response(const HttpRequest&) {
    std::stringstream body;
    std::stringstream len;
    std::vector<std::pair<std::string, std::string> > default_headers;
    body << "<h1>Hello webserv!</h1>";
    default_headers.push_back(std::make_pair("Content-Type", "text/html"));
    len << body.str().size();
    default_headers.push_back(std::make_pair("Content-Length", len.str()));
    return HttpResponse(HTTP_OK, body.str(), default_headers);
}

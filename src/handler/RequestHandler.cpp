/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/30 13:53:22 by dtimofee          #+#    #+#             */
/*   Updated: 2026/05/20 20:49:59 by tsemenov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdlib>
#include <ctime>
#include <sstream>

#include "Logger.hpp"
#include "PathUtils.hpp"

Handler::Handler(ServerConfig& config)
    : _serverConfig(config), _errorHandler(config.errorPages), _fileService(), _autoIndex() {}

Handler::~Handler() {}

HttpResponse Handler::serveFile(const std::string& path) {
    std::string body;
    int status = _fileService.readFile(path, body);
    if (status != HTTP_OK) {
        LOG_WARNING("Reading file failed: " + path);
        return _errorHandler.makeError(status);
    }
    return HttpResponse::make(HTTP_OK, body, PathUtils::getContentType(path));
}

HttpResponse Handler::handleDirectory(const std::string& path, const std::string& uri,
                                      const RouteConfig* _location) {
    struct stat info;
    std::string indexPath = PathUtils::bildPathForDirectory(path, _location->defaultFile);
    LOG_DEBUG("Path to default file: " + indexPath);
    int status = _fileService.checkPath(indexPath, info);
    if (status == HTTP_OK && S_ISREG(info.st_mode)) {
        LOG_INFO("Serving default file in directory: " + indexPath);
        return serveFile(indexPath);
    }
    LOG_WARNING("Finding default file failed");
    if (status == HTTP_NOT_FOUND && _location->directoryListing) {
        std::string body;
        int dirStatus = _autoIndex.generate(path, uri, body);
        if (dirStatus == HTTP_OK) {
            LOG_INFO("Generating list of existing files in directory:" + path);
            return HttpResponse::make(HTTP_OK, body, "text/html");
        }
    }
    return _errorHandler.makeError(HTTP_FORBIDDEN);
}

HttpResponse Handler::handleGet(const HttpRequest& request, const RouteConfig* _location) {
    std::string fullPath = _location->rootDirectory + request.getPath();
    LOG_DEBUG("GET request for path: " + fullPath);

    struct stat info;
    int status = _fileService.checkPath(fullPath, info);
    if (status != HTTP_OK) {
        LOG_WARNING("Path check failed: " + fullPath);
        return _errorHandler.makeError(status);
    }
    if (S_ISDIR(info.st_mode)) {
        LOG_INFO("Serving directory: " + fullPath);
        return handleDirectory(fullPath, request.getPath(), _location);
    }
    LOG_INFO("Serving file: " + fullPath);
    return serveFile(fullPath);
}

HttpResponse Handler::handlePost(const HttpRequest& request, const RouteConfig* _location) {
    if (_location->uploadDirectory.empty()) {
        LOG_WARNING("No uploadPath");
        return _errorHandler.makeError(HTTP_FORBIDDEN);
    }
    std::string uploadPath = _location->rootDirectory + _location->uploadDirectory;
    LOG_DEBUG("POST request for path: " + uploadPath);

    struct stat info;
    int status = _fileService.checkUploadDirectory(uploadPath, info);
    if (status != HTTP_OK) {
        LOG_WARNING("Path check failed: " + uploadPath);
        return _errorHandler.makeError(status);
    }

    std::ostringstream filename;
    filename << uploadPath << "/" << std::time(0) << "_" << std::rand();
    LOG_INFO("Created a file: " + uploadPath);
    if (!_fileService.writeFile(filename.str(), request.getBody())) {
        LOG_WARNING("Writing to file failed: " + uploadPath);
        return _errorHandler.makeError(HTTP_INTERNAL_ERROR);
    }
    HttpResponse res = HttpResponse::make(HTTP_CREATED, "", "text/html");

    res.addHeader("Location", filename.str());
    return res;
}

HttpResponse Handler::handleDelete(const HttpRequest& request, const RouteConfig* _location) {
    std::string fullPath = _location->rootDirectory + request.getPath();
    LOG_DEBUG("DELETE request for path: " + fullPath);

    struct stat info;
    int status = _fileService.checkPath(fullPath, info);
    if (status != HTTP_OK) {
        LOG_WARNING("Path check failed: " + fullPath);
        return _errorHandler.makeError(status);
    }
    if (S_ISDIR(info.st_mode)) {
        LOG_WARNING("Trying to delete directory: " + fullPath);
        return _errorHandler.makeError(HTTP_FORBIDDEN);
    }
    if (!_fileService.deleteFile(fullPath)) {
        LOG_WARNING("Failed to delete a file: " + fullPath);
        return _errorHandler.makeError(HTTP_INTERNAL_ERROR);
    }
    return HttpResponse::make(HTTP_NO_CONTENT, "", "text/html");
}

HttpResponse Handler::handle_request(HttpRequest& request) {
    LOG_INFO("Handling request");
    const RouteConfig* _location = _serverConfig.findMatchingLocation(request.getPath());
    if (!_location) {
        LOG_WARNING("Location matching failed: " + request.getPath());
        return _errorHandler.makeError(HTTP_NOT_FOUND);
    }
    LOG_DEBUG("Found matching location: " + _location->url);
    if (!_location->isMethodAllowed(request.getMethod())) {
        LOG_WARNING("Method check failed: " + request.getMethod());
        return _errorHandler.makeError(HTTP_METHOD_NOT_ALLOWED);
    }
    if (request.getMethod() == "GET") {
        return handleGet(request, _location);
    } else if (request.getMethod() == "POST") {
        return handlePost(request, _location);
    } else if (request.getMethod() == "DELETE") {
        return handleDelete(request, _location);
    }
    return _errorHandler.makeError(HTTP_METHOD_NOT_IMPLEMENTED);
}

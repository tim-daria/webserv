#include "RequestHandler.hpp"

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdlib>
#include <sstream>

#include "Logger.hpp"
#include "PathUtils.hpp"

Handler::Handler(ServerConfig& config)
    : _serverConfig(config),
      _errorHandler(config),
      _cgiHandler(config),
      _fileService(),
      _autoIndex() {}

Handler::~Handler() {}

HttpResponse Handler::makeRedirection(const std::string& root, int status,
                                      const std::string& path) {
    std::string fullPath = PathUtils::concatenatePath(root, path);
    HttpResponse response = HttpResponse::make(status, "", "text/html");
    response.addHeader("Location", path);
    return response;
}

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
    std::string indexPath = PathUtils::concatenatePath(path, _location->defaultFile);
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
    return _errorHandler.makeError(status);
}

HttpResponse Handler::handleGet(const HttpRequest& request, const RouteConfig* _location) {
    std::string fullPath = PathUtils::concatenatePath(_location->rootDirectory, request.getPath());
    LOG_DEBUG("GET request for path: " + fullPath);

    struct stat info;
    int status = _fileService.checkPath(fullPath, info);
    if (status != HTTP_OK) {
        LOG_WARNING("Path check failed: " + fullPath);
        return _errorHandler.makeError(status);
    }
    if (S_ISDIR(info.st_mode)) {
        if (!PathUtils::endsWithSlash(fullPath)) {
            LOG_INFO("Directory without slash, redirecting: " + fullPath + "/");
            return makeRedirection(_serverConfig.rootPath, HTTP_MOVED_PERMANENTLY,
                                   request.getPath() + "/");
        }
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
    std::string uploadPath =
        PathUtils::concatenatePath(_location->rootDirectory, _location->uploadDirectory);
    LOG_DEBUG("POST request for path: " + uploadPath);

    struct stat info;
    int status = _fileService.checkUploadDirectory(uploadPath, info);
    if (status != HTTP_OK) {
        LOG_WARNING("Path check failed: " + uploadPath);
        return _errorHandler.makeError(status);
    }
    std::string contentType = request.getHeader("Content-Type");
    std::string body;
    std::string filename;

    if (contentType.find("multipart/form-data") != std::string::npos) {
        filename = request.extractMultipartFilename();
        LOG_DEBUG("Filename is " + filename);
        body = request.extractMultipartBody();
        if (body.empty()) {
            LOG_WARNING("Failed to parse multipart body");
            return _errorHandler.makeError(HTTP_BAD_REQUEST);
        }
    } else {
        body = request.getBody();
        filename = PathUtils::generateFilename(contentType);
    }

    std::string fullPath = PathUtils::concatenatePath(uploadPath, filename);
    LOG_INFO("Created a file: " + fullPath);
    if (!_fileService.writeFile(fullPath, body)) {
        LOG_WARNING("Writing to file failed: " + fullPath);
        return _errorHandler.makeError(HTTP_INTERNAL_ERROR);
    }
    HttpResponse res = HttpResponse::make(HTTP_CREATED, "", "text/html");

    res.addHeader("Location", _location->uploadDirectory + "/" + filename);
    return res;
}

HttpResponse Handler::handleDelete(const HttpRequest& request, const RouteConfig* _location) {
    std::string fullPath = PathUtils::concatenatePath(_location->rootDirectory, request.getPath());
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

HttpResponse Handler::handle_request(const HttpRequest& request) {
    LOG_INFO("Handling request");
    const RouteConfig* _location = _serverConfig.findMatchingLocation(request.getPath());
    if (!_location) {
        LOG_WARNING("Location matching failed: " + request.getPath());
        return _errorHandler.makeError(HTTP_NOT_FOUND);
    }
    LOG_DEBUG("Found matching location: " + _location->url);
    if (_location->hasReturn) {
        LOG_INFO("Redirecting to: " + _location->returnUri);
        return makeRedirection(_serverConfig.rootPath, _location->returnStatus,
                               _location->returnUri);
    }
    if (!_location->isMethodAllowed(request.getMethod())) {
        LOG_WARNING("Method check failed: " + request.getMethod());
        return _errorHandler.makeError(HTTP_METHOD_NOT_ALLOWED);
    }
    if (!request.checkMaxBodySize(_location->clientMaxBodySize)) {
        return _errorHandler.makeError(HTTP_PAYLOAD_TOO_LARGE);
    }
    if (_location->isCGI(request.getPath())) {
        LOG_INFO("CGI detected on path: " + request.getPath());
        return _cgiHandler.execute(request, _location);
    }
    if (request.getMethod() == "GET") {
        return handleGet(request, _location);
    } else if (request.getMethod() == "POST") {
        return handlePost(request, _location);
    } else if (request.getMethod() == "DELETE") {
        return handleDelete(request, _location);
    }
    return _errorHandler.makeError(HTTP_METHOD_NOT_ALLOWED);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtimofee <dtimofee@student.42berlin.de>    #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-06-08 17:51:59 by dtimofee          #+#    #+#             */
/*   Updated: 2026-06-08 17:51:59 by dtimofee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIHandler.hpp"

#include <sys/stat.h>

#include "Logger.hpp"
#include "PathUtils.hpp"

CGIHandler::CGIHandler(const ServerConfig& conf)
    : _config(conf), _fileService(), _errorHandler(conf), _scriptPath(NULL) {}

std::vector<std::string> CGIHandler::buildEnvironment(const HttpRequest& request,
                                                      const std::string& script_path) {
    std::vector<std::string> env;

    env.push_back("REQUEST_METHOD=" + request.getMethod());
    // env.push_back("SCRIPT_FILENAME=" + script_path);
    env.push_back("PATH_INFO=" + request.getPath());
    env.push_back("CONTENT_LENGTH=" + std::to_string(request.getBody().size()));
    env.push_back("SERVER_NAME" + _config.serverName);
    env.push_back("SERVER_PORT=8080");
    env.push_back("GATEWAY_INTERFACE=CGI/1.1");
    env.push_back("QUERY_STRING=");
    std::string content_type = request.getHeader("Content-Type");
    if (!content_type.empty()) env.push_back("CONTENT_TYPE=" + content_type);

    return env;
}

char** CGIHandler::toCharArray(const std::vector<std::string>& env) {
    // +1 for terminating NULL — execve asks for NULL terminated array
    char** arr = new char*[env.size() + 1];

    for (size_t i = 0; i < env.size(); i++) {
        arr[i] = new char[env[i].size() + 1];
        std::copy(env[i].begin(), env[i].end(), arr[i]);
        arr[i][env[i].size()] = '\0';
    }
    arr[env.size()] = NULL;
    return arr;
}

void CGIHandler::freeCharArray(char** arr, size_t size) {
    for (size_t i = 0; i < size; i++) {
        delete[] arr[i];
    }
    delete[] arr;
}

std::string CGIHandler::runScript(const RouteConfig* _location, char** env,
                                  const std::string& body) {}

HttpResponse CGIHandler::execute(const HttpRequest& request, const RouteConfig* _location) {
    _scriptPath = PathUtils::concatenatePath(_location->rootDirectory, request.getPath());

    struct stat info;
    int status = _fileService.checkCGI(_scriptPath, info);
    if (status != HTTP_OK) {
        LOG_WARNING("Path check failed: " + _scriptPath);
        return _errorHandler.makeError(status);
    }
    std::vector<std::string> env_vec = buildEnvironment(request, _scriptPath);
    LOG_INFO("Environment have been built successfully");
    char** env = toCharArray(env_vec);

    std::string output = runScript(_location, env, request.getBody());
    freeCharArray(env, env_vec.size());
    if (output.empty()) {
        LOG_WARNING("Failed to get output");
        return _errorHandler.makeError(HTTP_INTERNAL_ERROR);
    }
    LOG_INFO("Output recieved successfully");
    return HttpResponse::make(HTTP_OK, output, "text/html");
}

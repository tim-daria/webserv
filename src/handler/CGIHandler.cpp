/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 17:51:59 by dtimofee          #+#    #+#             */
/*   Updated: 2026/06/09 18:55:39 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIHandler.hpp"

#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "Logger.hpp"
#include "PathUtils.hpp"

CGIHandler::CGIHandler(const ServerConfig& conf)
    : _config(conf), _fileService(), _errorHandler(conf), _scriptPath("") {}

std::vector<std::string> CGIHandler::buildEnvironment(const HttpRequest& request) {
    std::vector<std::string> env;

    env.push_back("REQUEST_METHOD=" + request.getMethod());
    // env.push_back("SCRIPT_FILENAME=" + script_path);
    env.push_back("PATH_INFO=" + _scriptPath);
    env.push_back("CONTENT_LENGTH=" + toString(request.getBody().size()));
    env.push_back("SERVER_NAME" + _config.serverName);
    env.push_back("SERVER_PORT=8080");
    env.push_back("GATEWAY_INTERFACE=CGI/1.1");
    env.push_back("QUERY_STRING=");
	env.push_back("SERVER_PROTOCOL=HTTP/1.0");
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

std::string CGIHandler::findInterpreter(const RouteConfig* _location) {
    for (std::map<std::string, std::string>::const_iterator it = _location->cgiHandlers.begin();
         it != _location->cgiHandlers.end(); ++it) {
        if (_scriptPath.find(it->first)) {
            return it->second;
        }
    }
    return "";
}

std::string CGIHandler::runScript(const RouteConfig* _location, char** env, const std::string& body,
                                  size_t envSize) {
    int pipe_in[2];
    int pipe_out[2];

    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1) {
        LOG_WARNING("Piping failed");
        return "";
    }
    pid_t pid = fork();
    if (pid == -1) {
        LOG_WARNING("Forking failed");
        return "";
    }
    if (pid == 0) {
        if (dup2(pipe_in[0], STDIN_FILENO) == -1) {
            LOG_WARNING("Dup2 failed");
            return "";
        }
        if (dup2(pipe_out[1], STDOUT_FILENO) == -1) {
            LOG_WARNING("Dup2 failed");
            return "";
        }
        close(pipe_in[1]);
        close(pipe_out[0]);
        std::string interpreterPath = findInterpreter(_location);
        if (interpreterPath.empty()) {
            LOG_WARNING("Failed to find interpreter for path:" + _scriptPath);
            return "";
        }
        char* argv[] = {
            const_cast<char*>(interpreterPath.c_str()),
            const_cast<char*>(_scriptPath.c_str()),
            NULL,
        };
        if (execve(interpreterPath.c_str(), argv, env) == -1) {
            LOG_WARNING("Execution failed");
            freeCharArray(env, envSize);
            _exit(1);
        }
    }
    close(pipe_in[0]);
    close(pipe_out[1]);
    if (!body.empty()) {
        write(pipe_in[1], body.c_str(), body.size());
    }
    close(pipe_in[1]);

    std::string output;
    char buf[1024];
    int bytes;
    while ((bytes = read(pipe_out[0], buf, sizeof(buf))) > 0) {
        output.append(buf, bytes);
    }
    close(pipe_out[0]);
    int wstatus;
    waitpid(pid, &wstatus, 0);
    if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) != 0) {
        return "";
    }
    return output;
}

HttpResponse CGIHandler::execute(const HttpRequest& request, const RouteConfig* _location) {
    _scriptPath = PathUtils::concatenatePath(_location->rootDirectory, request.getPath());

    struct stat info;
    int status = _fileService.checkCGI(_scriptPath, info);
    if (status != HTTP_OK) {
        LOG_WARNING("Path check failed: " + _scriptPath);
        return _errorHandler.makeError(status);
    }
    std::vector<std::string> env_vec = buildEnvironment(request);
    LOG_INFO("Environment have been built successfully");
    char** env = toCharArray(env_vec);

    std::string output = runScript(_location, env, request.getBody(), env_vec.size());
    freeCharArray(env, env_vec.size());
    if (output.empty()) {
        LOG_WARNING("Failed to get output");
        return _errorHandler.makeError(HTTP_INTERNAL_ERROR);
    }
    LOG_INFO("Script executed successfully");
    return HttpResponse::make(HTTP_OK, output, "text/html");
}

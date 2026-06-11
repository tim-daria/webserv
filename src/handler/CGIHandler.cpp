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

#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "Logger.hpp"
#include "PathUtils.hpp"

CGIHandler::CGIHandler(const ServerConfig& conf)
    : _config(conf),
      _fileService(),
      _errorHandler(conf),
      _scriptPath(""),
      _scriptDir(""),
      _scriptName("") {}

std::vector<std::string> CGIHandler::buildEnvironment(const HttpRequest& request) {
    std::vector<std::string> env;

    env.push_back("REQUEST_METHOD=" + request.getMethod());
    env.push_back("SCRIPT_FILENAME=" + _scriptPath);
    env.push_back("SCRIPT_NAME=" + _scriptName);
    env.push_back("PATH_INFO=");
    env.push_back("CONTENT_LENGTH=" + toString(request.getBody().size()));
    env.push_back("SERVER_NAME=" + _config.serverName);
    env.push_back("SERVER_PORT=" + toString(_config.listen[0].second));
    env.push_back("GATEWAY_INTERFACE=CGI/1.1");
    env.push_back("QUERY_STRING=" + request.getQuery());
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
        if (_scriptPath.size() >= it->first.size() &&
            _scriptPath.compare(_scriptPath.size() - it->first.size(), it->first.size(),
                                it->first) == 0) {
            return it->second;
        }
    }
    return "";
}

std::string CGIHandler::parentProcess(s_data& info, const std::string& body) {
    close(info.pipe_in[0]);
    close(info.pipe_out[1]);
    if (!body.empty()) {
        write(info.pipe_in[1], body.c_str(), body.size());
    }
    close(info.pipe_in[1]);

    int wstatus;
    int timeout = 5;
    pid_t result = 0;
    for (int i = 0; i < timeout * 10; i++) {
        result = waitpid(info.pid, &wstatus, WNOHANG);
        if (result != 0) {
            break;
        }
        usleep(100000);
    }
    if (result == 0) {
        LOG_WARNING("CGI script timed out — killing process");
        kill(info.pid, SIGKILL);
        waitpid(info.pid, &wstatus, 0);
        close(info.pipe_out[0]);
        return "";
    }
    std::string output;
    char buf[1024];
    int bytes;
    while ((bytes = read(info.pipe_out[0], buf, sizeof(buf))) > 0) {
        output.append(buf, bytes);
    }
    close(info.pipe_out[0]);
    return output;
}

std::string CGIHandler::runScript(const RouteConfig* _location, char** env,
                                  const std::string& body) {
    t_data info;

    std::string interpreterPath = findInterpreter(_location);
    if (interpreterPath.empty()) {
        LOG_WARNING("Failed to find interpreter for path:" + _scriptPath);
        return "";
    }
    if (pipe(info.pipe_in) == -1 || pipe(info.pipe_out) == -1) {
        LOG_WARNING("Piping failed");
        return "";
    }
    info.pid = fork();
    if (info.pid == -1) {
        LOG_WARNING("Forking failed");
        return "";
    }
    if (info.pid == 0) {
        if (dup2(info.pipe_in[0], STDIN_FILENO) == -1) {
            LOG_WARNING("Dup2 failed");
            _exit(1);
        }
        if (dup2(info.pipe_out[1], STDOUT_FILENO) == -1) {
            LOG_WARNING("Dup2 failed");
            _exit(1);
        }
        close(info.pipe_in[0]);
        close(info.pipe_in[1]);
        close(info.pipe_out[0]);
        close(info.pipe_out[1]);
        char* argv[] = {
            const_cast<char*>(interpreterPath.c_str()),
            const_cast<char*>(_scriptName.c_str()),
            NULL,
        };
        chdir(_scriptDir.c_str());
        if (execve(interpreterPath.c_str(), argv, env) == -1) {
            LOG_WARNING("Script execution failed");
            _exit(1);
        }
    }
    return parentProcess(info, body);
}

HttpResponse CGIHandler::execute(const HttpRequest& request, const RouteConfig* _location) {
    _scriptPath = PathUtils::concatenatePath(_location->rootDirectory, request.getPath());
    LOG_DEBUG("Scriptpath:" + _scriptPath);
    size_t lastSlash = _scriptPath.rfind('/');
    if (lastSlash == std::string::npos) {
        LOG_WARNING("/ in the path couldn't be detected");
        return _errorHandler.makeError(HTTP_BAD_REQUEST);
    }
    _scriptDir = _scriptPath.substr(0, lastSlash);
    _scriptName = _scriptPath.substr(lastSlash + 1);

    struct stat info;
    int status = _fileService.checkCGI(_scriptPath, info);
    if (status != HTTP_OK) {
        LOG_WARNING("Path check failed: " + _scriptPath);
        return _errorHandler.makeError(status);
    }
    std::vector<std::string> env_vec = buildEnvironment(request);
    LOG_INFO("Environment has been built successfully");
    char** env = toCharArray(env_vec);

    std::string output = runScript(_location, env, request.getBody());
    freeCharArray(env, env_vec.size());
    if (output.empty()) {
        LOG_WARNING("CGI execution failed");
        return _errorHandler.makeError(HTTP_INTERNAL_ERROR);
    }
    LOG_INFO("Script executed successfully");
    return HttpResponse::make(HTTP_OK, output, "text/html");
}

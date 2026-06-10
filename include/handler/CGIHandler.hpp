/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtimofee <dtimofee@student.42berlin.de>    #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-06-08 17:52:05 by dtimofee          #+#    #+#             */
/*   Updated: 2026-06-08 17:52:05 by dtimofee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "ErrorHandler.hpp"
#include "FileService.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class CGIHandler {
   private:
    const ServerConfig& _config;
    FileService _fileService;
    ErrorHandler _errorHandler;
    std::string _scriptPath;
    std::string _scriptDir;
    std::string _scriptName;

    std::vector<std::string> buildEnvironment(const HttpRequest& request);
    std::string runScript(const RouteConfig* _location, char** env, const std::string& body,
                          size_t size);
    std::string findInterpreter(const RouteConfig* _location);
    char** toCharArray(const std::vector<std::string>& env);
    void freeCharArray(char** arr, size_t size);

   public:
    CGIHandler(const ServerConfig& conf);

    HttpResponse execute(const HttpRequest& request, const RouteConfig* _location);
};

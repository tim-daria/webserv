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
    FileService _fileService;
    ErrorHandler _errorHandler;
    const ServerConfig& _config;
    std::string _scriptPath;

    std::vector<std::string> buildEnvironment(const HttpRequest& request,
                                              const std::string& script_path);
    std::string runScript(const RouteConfig* _location, char** env, const std::string& body);
    char** toCharArray(const std::vector<std::string>& env);
    void freeCharArray(char** arr, size_t size);

   public:
    CGIHandler(const ServerConfig& conf);

    HttpResponse execute(const HttpRequest& request, const RouteConfig* _location);
};

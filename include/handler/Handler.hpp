/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtimofee <dtimofee@student.42berlin.de>    #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-03-26 13:30:10 by dtimofee          #+#    #+#             */
/*   Updated: 2026-03-26 13:30:10 by dtimofee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "RouteConfig.hpp"
#include "ServerConfig.hpp"

class Handler {
   private:
    ServerConfig _serverConfig;

    Handler(const Handler& other);
    Handler& operator=(const Handler& other);

    int checkPath(const std::string& path, struct stat& info);

    HttpResponse get_default_response(const HttpRequest& request);
    HttpResponse handleGet(const HttpRequest& request, RouteConfig* _location);
    // HttpResponse handlePost(HttpRequest request);
    // HttpResponse handleDelete(HttpRequest request);
    HttpResponse handleDirectory(const std::string& path, RouteConfig* _location);
    HttpResponse serveFile(const std::string& path);

    HttpResponse makeError(int status);
    // std::string readFile(std::string path);
    std::string getContentType(const std::string& path);
    HttpResponse generateListing(const std::string& path);

   public:
    Handler();
    Handler(const ServerConfig& config);
    ~Handler();

    HttpResponse handle_request(HttpRequest& request);
};

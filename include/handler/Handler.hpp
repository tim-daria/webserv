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
    RouteConfig _config;
    ServerConfig _serverConfig;

    Handler(const Handler& other);
    Handler& operator=(const Handler& other);

    HttpResponse get_default_response(const HttpRequest& request);
    // HttpResponse handleGet(HttpRequest request);
    // HttpResponse handlePost(HttpRequest request);
    // HttpResponse handleDelete(HttpRequest request);

   public:
    Handler();
    Handler(const ServerConfig& config);
    ~Handler();

    HttpResponse handle_request(HttpRequest& request);
};

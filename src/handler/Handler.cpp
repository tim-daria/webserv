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

#include "Handler.hpp"

#include <sstream>

Handler::Handler() {}

Handler::Handler(const ServerConfig& config) : _serverConfig(config) {}

Handler::~Handler() {}

HttpResponse Handler::handle_request(HttpRequest& request) {
    // Browsers automatically request /favicon.ico — return 404 with empty body
    if (request.path == "/favicon.ico") {
        std::vector<std::pair<std::string, std::string> > headers;
        headers.push_back(std::make_pair("Content-Length", "0"));
        return HttpResponse(404, "", headers);
    }
    // TODO: route by method and path using config (Part 2)
    return get_default_response(request);
}

HttpResponse Handler::get_default_response(const HttpRequest&) {
    std::stringstream body;
    std::stringstream len;
    std::vector<std::pair<std::string, std::string> > default_headers;
    body << "<h1>Hello webserv!</h1>";
    default_headers.push_back(std::make_pair("Content-Type", "text/html"));
    len << body.str().size();
    default_headers.push_back(std::make_pair("Content-Length", len.str()));
    return HttpResponse(200, body.str(), default_headers);
}

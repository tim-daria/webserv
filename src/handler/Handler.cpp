/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtimofee <dtimofee@student.42berlin.de>    #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-03-30 13:53:22 by dtimofee          #+#    #+#             */
/*   Updated: 2026-03-30 13:53:22 by dtimofee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Handler.hpp"

#include <sstream>

Handler::Handler() {}

Handler::~Handler() {}

// Handler::Handler(RouteConfig config): config_(config) {};

HttpResponse Handler::handle(HttpRequest& request) { return get_default_response(request); }

HttpResponse Handler::get_default_response(const HttpRequest&) {
    std::stringstream body;
    std::vector<std::pair<std::string, std::string> > default_headers;
    body << "<h1>Hello webserv!</h1>";
    default_headers.push_back(std::make_pair("Content-Type", "text/html"));
    default_headers.push_back(std::make_pair("Content-Length", std::to_string(body.str().size())));
    return HttpResponse(200, body.str(), default_headers);
}

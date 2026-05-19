/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/26 17:06:49 by nefimov           #+#    #+#             */
/*   Updated: 2026/05/20 00:14:55 by tsemenov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"

#include <iostream>

void ServerConfig::print() { std::cout << *this << std::endl; }

ServerConfig ServerConfig::makeDefault() {
    ServerConfig cfg;
    cfg.applyDefaults();
    return cfg;
}

void ServerConfig::applyDefaults() {
    listen.clear();
    errorPages.clear();
    routes.clear();

    serverName = "webserv";
    add_listen("127.0.0.1", 8080);
    set_errorPage(400, "www/errors/400.html");
    set_errorPage(403, "www/errors/403.html");
    set_errorPage(404, "www/errors/404.html");
    set_errorPage(405, "www/errors/405.html");
    set_errorPage(411, "www/errors/411.html");
    set_errorPage(413, "www/errors/413.html");
    set_errorPage(500, "www/errors/500.html");
    set_errorPage(501, "www/errors/501.html");
    clientMaxBodySize = 1048576;

    RouteConfig r;
    r.url = "/";
    r.rootDirectory = "www";
    r.defaultFile = "index.html";
    r.add_acceptedMethod("GET");
    r.add_acceptedMethod("POST");
    add_route(r);

    RouteConfig method_not_allowed;
    method_not_allowed.url = "/method_not_allowed";
    method_not_allowed.rootDirectory = "www";
    method_not_allowed.defaultFile = "index.html";
    method_not_allowed.add_acceptedMethod("DELETE");
    add_route(method_not_allowed);
}

// Made const so it can be called on const ServerConfig& (e.g. in RequestValidator).
// Returns const RouteConfig* because the caller should not mutate config data:
const RouteConfig* ServerConfig::findMatchingLocation(std::string path) const {
    // Deleting / ath the end of the path /photos/ → /photos
    if (path.length() > 1 && path[path.length() - 1] == '/')
        path = path.substr(0, path.length() - 1);

    const RouteConfig* best_match = NULL;
    size_t best_length = 0;

    // const_iterator required inside a const method — routes is const here:
    for (std::vector<RouteConfig>::const_iterator it = routes.begin(); it != routes.end(); ++it) {
        // Does the request path start with this location?
        if (path.find(it->url) != 0) continue;

        // Check end of the word — protection against /photo matches to /photos
        size_t url_len = it->url.size();
        bool boundary = (path.size() == url_len) || (path[url_len] == '/') || (it->url == "/");

        if (!boundary) continue;

        // Is this match longer then previous one?
        if (url_len > best_length) {
            best_length = url_len;
            best_match = &(*it);
        }
    }
    return best_match;
}

std::ostream& operator<<(std::ostream& out, const ServerConfig& cfg) {
    out << "server {" << std::endl;
    {
        // Print Interface:Port pairs from listen
        for (std::vector<std::pair<std::string, int> >::const_iterator it = cfg.listen.begin();
             it != cfg.listen.end(); ++it) {
            out << "    listen ";
            out << it->first << ":" << it->second << ";" << std::endl;
        }
        // Print server name
        out << "    server_name " << cfg.serverName << std::endl;
        // Print error pages
        out << std::endl;
        for (std::map<int, std::string>::const_iterator it = cfg.errorPages.begin();
             it != cfg.errorPages.end(); ++it) {
            out << "    error_page ";
            out << it->first << " " << it->second << ";" << std::endl;
        }
        // Print client max body size
        out << "    client_max_body_size " << cfg.clientMaxBodySize << std::endl;
        // Print routes
        for (std::vector<RouteConfig>::const_iterator it = cfg.routes.begin();
             it != cfg.routes.end(); ++it) {
            out << std::endl;
            out << *it;
        }
        out << std::endl;
    }
    out << "}";

    return out;
}

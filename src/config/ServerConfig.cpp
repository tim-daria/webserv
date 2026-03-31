/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/26 17:06:49 by nefimov           #+#    #+#             */
/*   Updated: 2026/03/31 13:19:18 by nefimov          ###   ########.fr       */
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
    set_errorPage(404, "/errors/404.html");
    clientMaxBodySize = 1048576;

    RouteConfig r;
    r.url = "/";
    r.rootDirectory = "/var/www/html";
    r.defaultFile = "index.html";
    r.add_acceptedMethod("GET");
    r.add_acceptedMethod("POST");
    add_route(r);
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
        if (cfg.serverName != "") out << "    server_name " << cfg.serverName << std::endl;
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

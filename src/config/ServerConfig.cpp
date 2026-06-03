/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2026/06/03 11:53:51 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"

#include <iostream>

#include "Logger.hpp"

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
    rootPath = "./www";
    add_listen("127.0.0.1", 8080);
    clientMaxBodySize = 1048576;

    RouteConfig r;
    r.url = "/";
    r.rootDirectory = "./www";
    r.defaultFile = "index.html";
    r.add_acceptedMethod("GET");
    add_route(r);

    // hardcoded to make testing for 405 & 501 work correctly
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

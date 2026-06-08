/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/26 15:09:14 by nefimov           #+#    #+#             */
/*   Updated: 2026/06/03 11:43:42 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <string>
#include <vector>

#include "RouteConfig.hpp"
// class RouteConfig;

class ServerConfig {
   public:
    std::vector<std::pair<std::string, int> > listen;  // Interface:Port pairs
    std::string serverName;                            // Server name for virtual hosts
    std::string rootPath;                              // Path to server root directory
    std::map<int, std::string> errorPages;             // Default error pages Code:Path pairs
    size_t clientMaxBodySize;                          // Max allowd size for client request body
    std::vector<RouteConfig> routes;                   // List of routes for this server

    void add_listen(const std::string& interface, int port);
    void set_errorPage(int code, const std::string& path);
    void add_route(const RouteConfig& route);

    const RouteConfig* findMatchingLocation(std::string path) const;

    static ServerConfig makeDefault();
    void applyDefaults();
    // void print();
};

// std::ostream& operator<<(std::ostream& out, const ServerConfig& config);

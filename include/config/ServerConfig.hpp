/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/26 15:09:14 by nefimov           #+#    #+#             */
/*   Updated: 2026/03/26 16:32:18 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <string>
#include <vector>
#include <map>

class RouteConfig;

class ServerConfig {
public:
	std::vector<std::pair<std::string, int>> listen; // Interface:Port pairs
	std::string serverName; // Server name for virtual hosts
	std::map<int, std::string> errorPages; // Default error pages Code:Path pairs
	size_t clientMaxBodySize; // Max allowd size for client request body
	std::vector<RouteConfig> routes; // List of routes for this server
	
	void add_listen(const std::string &interface, int port) {
		listen.push_back(std::make_pair(interface, port));
	}
	
	void set_errorPage(int code, const std::string &path) {
		errorPages.insert(std::make_pair(code, path));
	}
	
	void add_route(const RouteConfig &route) {
		routes.push_back(route);	
	}
};

#endif
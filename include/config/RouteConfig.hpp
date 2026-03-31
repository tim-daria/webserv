/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteConfig.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/26 15:09:14 by nefimov           #+#    #+#             */
/*   Updated: 2026/03/31 12:37:25 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTECONFIG_HPP
#define ROUTECONFIG_HPP

#include <map>
#include <string>
#include <vector>

class RouteConfig {
   public:
    std::string url;                           // The URL/route path, e.g., /path
    std::vector<std::string> acceptedMethods;  // List of accepted HTTP methods
    // std::string redirection; // HTTP redirection URL, if any
    std::string rootDirectory;  // Directory where files for this route are located
    // bool directoryListing; // Enable/disable directory listing
    std::string defaultFile;                         // Default file to serve for directories
    std::string uploadDirectory;                     // Directory to store uploaded files
    std::map<std::string, std::string> cgiHandlers;  // File extensions to CGI handler mapping

    void add_acceptedMethod(const std::string& method) { acceptedMethods.push_back(method); }

    void set_cgiHandler(const std::string& extension, const std::string& handler) {
        cgiHandlers.insert(std::make_pair(extension, handler));
    }

    void print();
};

std::ostream& operator<<(std::ostream& out, const RouteConfig& conf);

#endif

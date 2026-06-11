/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteConfig.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/26 15:09:14 by nefimov           #+#    #+#             */
/*   Updated: 2026/06/05 17:00:08 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <string>
#include <vector>

class RouteConfig {
   public:
    std::string url;                           // The URL/route path, e.g., /path
    std::vector<std::string> acceptedMethods;  // List of accepted HTTP methods
    std::string rootDirectory;                 // Directory where files for this route are located
    bool directoryListing;                     // Enable/disable directory listing
    std::string defaultFile;                   // Default file to serve for directories
    std::string uploadDirectory;               // Directory to store uploaded files
    size_t clientMaxBodySize;                  // Max allowd size for client request body
    std::map<std::string, std::string> cgiHandlers;  // File extensions to CGI handler mapping
    bool authRequired;                               // Require auth for this route
    bool hasReturn;                                  // Has redirect rule
    int returnStatus;                                // Redirect status code
    std::string returnUri;                           // Redirect target URI

    RouteConfig();

    void add_acceptedMethod(const std::string& method);
    void set_cgiHandler(const std::string& extension, const std::string& handler);
    void applyDefaults();

    bool isMethodAllowed(const std::string& method) const;
    bool isCGI(const std::string& path) const;
};

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteConfig.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2026/06/09 15:36:18 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RouteConfig.hpp"

#include <iostream>

RouteConfig::RouteConfig()
    : url(""),
      rootDirectory(""),
      directoryListing(false),
      defaultFile(""),
      uploadDirectory(""),
      clientMaxBodySize(0),
      authRequired(false),
      hasReturn(false),
      returnStatus(0),
      returnUri("") {}

void RouteConfig::applyDefaults() {
    rootDirectory = "./www";
    defaultFile = "index.html";
    clientMaxBodySize = 1048576;
}

void RouteConfig::add_acceptedMethod(const std::string& method) {
    acceptedMethods.push_back(method);
}

void RouteConfig::set_cgiHandler(const std::string& extension, const std::string& handler) {
    cgiHandlers.insert(std::make_pair(extension, handler));
}

bool RouteConfig::isCGI(const std::string& path) const {
    for (std::map<std::string, std::string>::const_iterator it = cgiHandlers.begin();
         it != cgiHandlers.end(); ++it) {
        const std::string& ext = it->first;
        if (path.length() > ext.length() &&
            path.compare(path.length() - ext.length(), ext.length(), ext) == 0) {
            return true;
        }
    }
    return false;
}

// Made const so it can be called on const RouteConfig* returned by findMatchingLocation:
bool RouteConfig::isMethodAllowed(const std::string& method) const {
    for (std::vector<std::string>::const_iterator it = acceptedMethods.begin();
         it != acceptedMethods.end(); ++it) {
        if (*it == method) {
            return true;
        }
    }
    return false;
}

// void RouteConfig::print() { std::cout << *this << std::endl; }

// std::ostream& operator<<(std::ostream& out, const RouteConfig& conf) {
//     // Print the URL/route path
//     out << "    location " << conf.url << " {" << std::endl;
//     {
//         // Print root directory
//         out << "        root " << conf.rootDirectory << std::endl;
//         // Print default file to serve for directories
//         out << "        index " << conf.defaultFile << std::endl;
//         // Print list of accepted HTTP methods
//         out << "        methods";
//         for (std::vector<std::string>::const_iterator it = conf.acceptedMethods.begin();
//              it != conf.acceptedMethods.end(); ++it) {
//             out << " " << *it;
//         }
//         out << std::endl;
//         // Print directory to store uploaded files
//         out << "        upload_store " << conf.uploadDirectory << std::endl;
//         // Print file extensions to CGI handler mapping
//         for (std::map<std::string, std::string>::const_iterator it = conf.cgiHandlers.begin();
//              it != conf.cgiHandlers.end(); ++it) {
//             out << "        cgi ";
//             out << " " << it->first << it->second << std::endl;
//         }
//     }
//     out << "    }";

//     return out;
// }

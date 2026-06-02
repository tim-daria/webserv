/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteConfig.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2026/06/02 17:08:35 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RouteConfig.hpp"

#include <iostream>

RouteConfig::RouteConfig()
    : url(""),
      rootDirectory(""),
      directoryListing(false),
      defaultFile("index.html"),
      uploadDirectory(""),
      authRequired(false),
      hasReturn(false),
      returnStatus(0),
      returnUri("") {}

// Made const so it can be called on const RouteConfig* returned by findMatchingLocation:
bool RouteConfig::isMethodAllowed(std::string method) const {
    for (std::vector<std::string>::const_iterator it = acceptedMethods.begin();
         it != acceptedMethods.end(); ++it) {
        if (*it == method) {
            return true;
        }
    }
    return false;
}

void RouteConfig::print() { std::cout << *this << std::endl; }

std::ostream& operator<<(std::ostream& out, const RouteConfig& conf) {
    // Print the URL/route path
    out << "    location " << conf.url << " {" << std::endl;
    {
        // Print root directory
        out << "        root " << conf.rootDirectory << std::endl;
        // Print default file to serve for directories
        out << "        index " << conf.defaultFile << std::endl;
        // Print list of accepted HTTP methods
        out << "        methods";
        for (std::vector<std::string>::const_iterator it = conf.acceptedMethods.begin();
             it != conf.acceptedMethods.end(); ++it) {
            out << " " << *it;
        }
        out << std::endl;
        // Print directory to store uploaded files
        out << "        upload_store " << conf.uploadDirectory << std::endl;
        // Print file extensions to CGI handler mapping
        for (std::map<std::string, std::string>::const_iterator it = conf.cgiHandlers.begin();
             it != conf.cgiHandlers.end(); ++it) {
            out << "        cgi ";
            out << " " << it->first << it->second << std::endl;
        }
    }
    out << "    }";

    return out;
}

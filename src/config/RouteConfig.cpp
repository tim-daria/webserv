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

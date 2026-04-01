/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteConfig.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/26 17:06:49 by nefimov           #+#    #+#             */
/*   Updated: 2026/03/31 13:15:24 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RouteConfig.hpp"

#include <iostream>

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

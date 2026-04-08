/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtimofee <dtimofee@student.42berlin.de>    #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-03-20 16:42:41 by dtimofee          #+#    #+#             */
/*   Updated: 2026-03-20 16:42:41 by dtimofee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"

const std::map<int, std::string> HttpResponse::_status_messages =
    HttpResponse::initStatusMessages();

// HttpResponse::HttpResponse() : status_code_(200) {
//     headers_["Content-Type"] = "text/html";
// }

HttpResponse::HttpResponse(int status_code, const std::string& body,
                           const std::vector<std::pair<std::string, std::string> >& headers)
    : _status_code(status_code), _body(body), _headers(headers) {}

HttpResponse::HttpResponse(const HttpResponse& other)
    : _status_code(other._status_code), _body(other._body), _headers(other._headers) {};

HttpResponse::~HttpResponse() {}

std::string HttpResponse::toString() const {
    std::stringstream ss;

    // Status line
    std::map<int, std::string>::const_iterator it = _status_messages.find(_status_code);
    if (it != _status_messages.end()) {
        ss << "HTTP/1.0 " << _status_code << " " << it->second;
    } else {
        ss << "HTTP/1.0 " << _status_code << " Unknown";
    }
    ss << "\r\n";

    for (std::vector<std::pair<std::string, std::string> >::const_iterator it = _headers.begin();
         it != _headers.end(); ++it) {
        ss << it->first << ": " << it->second << "\r\n";
    }

    // Empty line before body
    ss << "\r\n";

    // Body
    ss << _body;

    return ss.str();
}

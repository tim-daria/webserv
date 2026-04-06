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

const std::map<int, std::string> HttpResponse::status_messages_ =
    HttpResponse::initStatusMessages();

// HttpResponse::HttpResponse() : status_code_(200) {
//     headers_["Content-Type"] = "text/html";
// }

HttpResponse::HttpResponse(int status_code, const std::string& body,
                           const std::vector<std::pair<std::string, std::string> >& headers)
    : status_code_(status_code), body_(body), headers_(headers) {}

HttpResponse::HttpResponse(const HttpResponse& other)
    : status_code_(other.status_code_), body_(other.body_), headers_(other.headers_) {};

HttpResponse::~HttpResponse() {}

std::string HttpResponse::toString() const {
    std::stringstream ss;

    // Status line
    std::map<int, std::string>::const_iterator it = status_messages_.find(status_code_);
    if (it != status_messages_.end()) {
        ss << "HTTP/1.0 " << status_code_ << " " << it->second;
    } else {
        ss << "HTTP/1.0 " << status_code_ << " Unknown";
    }
    ss << "\r\n";

    for (std::vector<std::pair<std::string, std::string> >::const_iterator it = headers_.begin();
         it != headers_.end(); ++it) {
        ss << it->first << ": " << it->second << "\r\n";
    }

    // Empty line before body
    ss << "\r\n";

    // Body
    ss << body_;

    return ss.str();
}

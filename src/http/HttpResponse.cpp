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

#include "Logger.hpp"

HttpResponse::HttpResponse(int status_code, const std::string& body,
                           const std::vector<std::pair<std::string, std::string> >& headers)
    : _statusCode(status_code), _body(body), _headers(headers) {}

HttpResponse::HttpResponse(const HttpResponse& other)
    : _statusCode(other._statusCode), _body(other._body), _headers(other._headers) {};

HttpResponse::~HttpResponse() {}

std::string HttpResponse::getStatusText(int statusCode) {
    struct StatusText {
        HttpStatus code;
        std::string text;
    };

    StatusText statuses[] = {{HTTP_OK, "OK"},
                             {HTTP_CREATED, "Created"},
                             {HTTP_NO_CONTENT, "No Content"},
                             {HTTP_MOVED_PERMANENTLY, "Moved Permanently"},
                             {HTTP_BAD_REQUEST, "Bad Request"},
                             {HTTP_FORBIDDEN, "Forbidden"},
                             {HTTP_NOT_FOUND, "Not Found"},
                             {HTTP_METHOD_NOT_ALLOWED, "Method Not Allowed"},
                             {HTTP_INTERNAL_ERROR, "Internal Server Error"}};

    size_t size = sizeof(statuses) / sizeof(statuses[0]);
    for (size_t i = 0; i < size; i++) {
        if (statuses[i].code == statusCode) return statuses[i].text;
    }
    return "Unknown";
}

HttpResponse HttpResponse::make(int status_code, const std::string& body,
                                const std::string& content_type) {
    std::vector<std::pair<std::string, std::string> > headers;
    headers.push_back(std::make_pair("Content-Type", content_type));
    headers.push_back(std::make_pair("Content-Length", ::toString(body.size())));
    return HttpResponse(status_code, body, headers);
}

std::string HttpResponse::toString() const {
    std::stringstream ss;

    // Status line
    ss << "HTTP/1.0 " << _statusCode << " " << getStatusText(_statusCode);
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

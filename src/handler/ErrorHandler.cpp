/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandler.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtimofee <dtimofee@student.42berlin.de>    #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-05-11 14:04:17 by dtimofee          #+#    #+#             */
/*   Updated: 2026-05-11 14:04:17 by dtimofee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ErrorHandler.hpp"

#include "HttpResponse.hpp"

ErrorHandler::ErrorHandler(const std::map<int, std::string>& errorPages)
    : _errorPages(errorPages), _fileService() {}

std::string ErrorHandler::getCustomPage(int code) const {
    std::map<int, std::string>::const_iterator it = _errorPages.find(code);
    if (it == _errorPages.end()) {
        return "";
    }

    std::string body;
    int status = _fileService.readFile(it->second, body);
    if (status != HTTP_OK) {
        return "";
    }
    return body;
}

std::string ErrorHandler::getDefaultPage(int code) const {
    std::ostringstream ss;
    ss << "<html>\n"
       << "<head><title>" << code << " " << HttpResponse::getStatusText(code) << "</title></head>\n"
       << "<body>\n"
       << "<h1>" << code << " " << HttpResponse::getStatusText(code) << "</h1>\n"
       << "<hr>\n<p>webserv</p>\n"
       << "</body>\n</html>\n";
    return ss.str();
}

HttpResponse ErrorHandler::makeError(int code) const {
    std::string body;
    std::string custom = getCustomPage(code);
    if (!custom.empty()) {
        body = custom;
    } else {
        body = getDefaultPage(code);
    }
    return HttpResponse::make(code, body, "text'html");
}

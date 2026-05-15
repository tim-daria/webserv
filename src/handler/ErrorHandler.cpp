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
#include "Logger.hpp"

ErrorHandler::ErrorHandler(const std::map<int, std::string>& errorPages)
    : _errorPages(errorPages), _fileService() {}

std::string ErrorHandler::getCustomPage(int code) const {
    std::map<int, std::string>::const_iterator it = _errorPages.find(code);
    if (it == _errorPages.end()) {
        LOG_WARNING("No custom page for error: " + toString(code));
        return "";
    }

    std::string body;
    int status = _fileService.readFile(it->second, body);
    if (status != HTTP_OK) {
        LOG_WARNING("Incorrect path for custom error page: " + it->second + toString(code));
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
    LOG_WARNING("Returning error: " + toString(code));
    std::string body;
    std::string custom = getCustomPage(code);
    if (!custom.empty()) {
        LOG_WARNING("Returning custom error page for error: " + toString(code));
        body = custom;
    } else {
        LOG_WARNING("Building default error page for error: " + toString(code));
        body = getDefaultPage(code);
    }
    return HttpResponse::make(code, body, "text/html");
}

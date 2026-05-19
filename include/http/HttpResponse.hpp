/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 16:43:12 by dtimofee          #+#    #+#             */
/*   Updated: 2026/05/20 00:09:03 by tsemenov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <cstring>
#include <map>
#include <sstream>
#include <string>
#include <vector>

enum HttpStatus {
    HTTP_OK = 200,
    HTTP_CREATED = 201,
    HTTP_NO_CONTENT = 204,
    HTTP_MOVED_PERMANENTLY = 301,
    HTTP_BAD_REQUEST = 400,
    HTTP_FORBIDDEN = 403,
    HTTP_NOT_FOUND = 404,
    HTTP_METHOD_NOT_ALLOWED = 405,
    HTTP_PAYLOAD_TOO_LARGE = 413,
    HTTP_INTERNAL_ERROR = 500,
    HTTP_METHOD_NOT_IMPLEMENTED = 501
};

class HttpResponse {
   private:
    int _statusCode;
    std::string _body;
    std::vector<std::pair<std::string, std::string> > _headers;

    HttpResponse();
    HttpResponse& operator=(const HttpResponse& other);

   public:
    HttpResponse(int status_code, const std::string& body,
                 const std::vector<std::pair<std::string, std::string> >& headers);
    HttpResponse(const HttpResponse& other);
    ~HttpResponse();

    static std::string getStatusText(int statusCode);
    static HttpResponse make(int status_code, const std::string& body,
                             const std::string& content_type);
    std::string toString() const;
};

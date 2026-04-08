/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtimofee <dtimofee@student.42berlin.de>    #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-03-20 16:43:12 by dtimofee          #+#    #+#             */
/*   Updated: 2026-03-20 16:43:12 by dtimofee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <cstring>
#include <map>
#include <sstream>
#include <string>
#include <vector>

class HttpResponse {
   private:
    int _status_code;
    std::string _body;
    std::vector<std::pair<std::string, std::string> > _headers;

    HttpResponse();
    HttpResponse& operator=(const HttpResponse& other);

    static std::map<int, std::string> initStatusMessages() {
        std::map<int, std::string> msg;
        msg[200] = "OK";
        msg[201] = "Created";
        // msg[204] = "No Content";
        // msg[301] = "Moved Permanently";
        // msg[302] = "Found";
        msg[400] = "Bad Request";
        // msg[401] = "Unauthorized";
        msg[403] = "Forbidden";
        msg[404] = "Not Found";
        msg[405] = "Method Not Allowed";
        msg[500] = "Internal Server Error";
        msg[503] = "Service Unavailable";
        return msg;
    };
    static const std::map<int, std::string> _status_messages;

   public:
    HttpResponse(int status_code, const std::string& body,
                 const std::vector<std::pair<std::string, std::string> >& headers);
    HttpResponse(const HttpResponse& other);
    ~HttpResponse();

    std::string toString() const;
};

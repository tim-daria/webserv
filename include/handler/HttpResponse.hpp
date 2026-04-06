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
    int status_code_;
    std::string body_;
    std::vector<std::pair<std::string, std::string> > headers_;

    HttpResponse();
    HttpResponse& operator=(const HttpResponse& other);

    static std::map<int, std::string> initStatusMessages() {
        std::map<int, std::string> m;
        m[200] = "OK";
        m[201] = "Created";
        // m[204] = "No Content";
        // m[301] = "Moved Permanently";
        // m[302] = "Found";
        m[400] = "Bad Request";
        // m[401] = "Unauthorized";
        m[403] = "Forbidden";
        m[404] = "Not Found";
        m[405] = "Method Not Allowed";
        m[500] = "Internal Server Error";
        m[503] = "Service Unavailable";
        return m;
    };
    static const std::map<int, std::string> status_messages_;

   public:
    HttpResponse(int status_code, const std::string& body,
                 const std::vector<std::pair<std::string, std::string> >& headers);
    HttpResponse(const HttpResponse& other);
    ~HttpResponse();

    std::string toString() const;
};

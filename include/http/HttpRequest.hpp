/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 18:22:51 by dtimofee          #+#    #+#             */
/*   Updated: 2026/05/20 11:28:24 by tsemenov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <string>

/*
1. parses the request data and stores all information units accordingly
2. checks:
        - if maxBodySize is not exceeded
        - if method in the request is from implemented ones
3. on error, returns corresponding error code:
        - 400 BAD REQUEST
        - 413 PAYLOAD TOO LARGE
        - 501 METHOD NOT IMPLEMENTED
*/

class HttpRequest {
   private:
    std::string _buf;
    std::string _method;
    std::string _path;
    std::string _query;  // part after ?
    std::string _version;
    std::map<std::string, std::string> _headers;
    std::string _body;

    enum ParsingState { READ_FIRST_LINE, READ_HEADER, READ_BODY, DONE, PARSING_ERROR };

    ParsingState _state;
    size_t _contentLen;
    size_t _maxBodySize;

    int _errorCode;

    void _parseFirstLine();
    void _parseHeaders();
    void _checkBody();
    void _parseBody();
    void _parseData();
    void _logResult(const std::string& preview) const;

    HttpRequest(const HttpRequest& other);
    HttpRequest& operator=(const HttpRequest& other);

   public:
    HttpRequest();
    ~HttpRequest();

    void processData(const char* data, size_t len);

    const std::string& getMethod() const;
    const std::string& getPath() const;
    const std::string& getQuery() const;
    const std::string& getVersion() const;
    std::string getHeader(const std::string& key) const;
    const std::map<std::string, std::string>& getHeaders() const;
    const std::string& getBody() const;

    ParsingState getState() const;
    int getErrorCode() const;

    size_t getMaxBodySize() const;

    void setMaxBodySize(size_t size);

    bool isDone() const;
    bool isError() const;
};

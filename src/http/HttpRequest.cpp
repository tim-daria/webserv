/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 16:21:17 by tsemenov          #+#    #+#             */
/*   Updated: 2026/06/05 15:55:39 by tsemenov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

#include <cctype>
#include <cstdlib>
#include <sstream>

#include "HttpResponse.hpp"
#include "Logger.hpp"
#include "utils.hpp"

HttpRequest::HttpRequest()
    : _state(READ_FIRST_LINE), _contentLen(0), _maxBodySize(0), _errorCode(0) {}
HttpRequest::~HttpRequest() {}
HttpRequest::HttpRequest(const HttpRequest& other) { *this = other; }
HttpRequest& HttpRequest::operator=(const HttpRequest& other) {
    if (this != &other) {
        _buf = other._buf;
        _method = other._method;
        _path = other._path;
        _version = other._version;
        _headers = other._headers;
        _body = other._body;
        _state = other._state;
        _contentLen = other._contentLen;
        _errorCode = other._errorCode;
        _maxBodySize = other._maxBodySize;
    }
    return *this;
}

bool HttpRequest::isDone() const { return _state == DONE; }
bool HttpRequest::isError() const { return _state == PARSING_ERROR; }

void HttpRequest::setMaxBodySize(size_t size) { _maxBodySize = size; }

size_t HttpRequest::getMaxBodySize() const { return _maxBodySize; }

HttpRequest::ParsingState HttpRequest::getState() const { return _state; }
const std::string& HttpRequest::getMethod() const { return _method; }
const std::string& HttpRequest::getPath() const { return _path; }
const std::string& HttpRequest::getVersion() const { return _version; }
const std::string& HttpRequest::getQuery() const { return _query; }
const std::string& HttpRequest::getBody() const { return _body; }
const std::map<std::string, std::string>& HttpRequest::getHeaders() const { return _headers; }
int HttpRequest::getErrorCode() const { return _errorCode; }

std::string HttpRequest::getHeader(const std::string& key) const {
    std::string low = toLowerCase(key);

    std::map<std::string, std::string>::const_iterator it = _headers.find(low);
    if (it != _headers.end()) {
        return it->second;
    }
    return "";
}

bool HttpRequest::_isImplemented(const std::string& method) {
    return method == "GET" || method == "POST" || method == "DELETE";
}

void HttpRequest::_parseFirstLine() {
    size_t end = _buf.find("\r\n");
    if (end == std::string::npos) {
        return;
    }  // if not the end of the line, wait for more

    std::string line = _buf.substr(0, end);
    _buf.erase(0, end + 2);

    // look for method, URI & version, separated by space:
    size_t pos1 = line.find(' ');
    size_t pos2 = line.find(' ', pos1 + 1);

    if (pos1 == std::string::npos || pos2 == std::string::npos) {
        _state = PARSING_ERROR;
        _errorCode = HTTP_BAD_REQUEST;  // 400
        return;
    }

    _method = line.substr(0, pos1);
    if (!_isImplemented(_method)) {
        _state = PARSING_ERROR;
        _errorCode = HTTP_METHOD_NOT_ALLOWED;  // 405
        return;
    }
    _version = line.substr(pos2 + 1);  // do we need to throw an error
    // if the version is higher than 1.1?

    std::string uri = line.substr(pos1 + 1, pos2 - pos1 - 1);
    LOG_DEBUG("URI is: " + uri);

    // Reject encoded null bytes — they are a security risk and bypass path checks:
    if (uri.find("%00") != std::string::npos) {
        _state = PARSING_ERROR;
        _errorCode = HTTP_BAD_REQUEST;
        return;
    }

    // Reject path traversal attempts (e.g. /../, /.., /foo/../../etc):
    // Check the path portion only (before '?'):
    std::string pathPart = uri.substr(0, uri.find('?'));
    bool hasTraversalSegment = pathPart.find("/../") != std::string::npos;  // is there "/../"?
    if (hasTraversalSegment) {
        LOG_DEBUG("/../ found!");
    };
    bool endsWithTraversal = pathPart.size() >= 3 && pathPart.substr(pathPart.size() - 3) ==
                                                         "/..";  // does the path end with "/.."?
    if (endsWithTraversal) {
        LOG_DEBUG("/.. found!");
    };

    if (hasTraversalSegment || endsWithTraversal) {
        _state = PARSING_ERROR;
        _errorCode = HTTP_BAD_REQUEST;
        return;
    }

    size_t q_start = uri.find('?');

    if (q_start != std::string::npos) {
        _path = uri.substr(0, q_start);
        _query = uri.substr(q_start + 1);
    } else {
        _path = uri;
    }

    _state = READ_HEADER;
}

void HttpRequest::_parseHeaders() {
    while (true) {
        size_t end = _buf.find("\r\n");
        if (end == std::string::npos) {
            return;
        }

        if (end == 0) {
            _buf.erase(0, 2);
            _checkBody();
            return;
        }

        std::string str = _buf.substr(0, end);
        _buf.erase(0, end + 2);

        size_t colon = str.find(':');
        if (colon == std::string::npos) {
            _state = PARSING_ERROR;
            _errorCode = HTTP_BAD_REQUEST;  // 400
            return;
        }

        std::string key = toLowerCase(str.substr(0, colon));
        std::string val = stripSpaces(str.substr(colon + 1));

        _headers[key] = val;
        // LOG_DEBUG("Request header:" + _headers[key]);
    }
}

void HttpRequest::_checkBody() {
    std::string temp = getHeader("content-length");

    if (!temp.empty()) {
        _contentLen = static_cast<size_t>(std::atol(temp.c_str()));
        if (_contentLen == 0) {
            _state = DONE;
        } else {
            _state = READ_BODY;
        }
    } else {
        _state = DONE;
    }  // no body
}

bool HttpRequest::checkMaxBodySize(size_t maxSize) const {
    if (maxSize > 0 && _contentLen > maxSize) {
        return false;
    }
    return true;
}

void HttpRequest::_parseBody() {
    if (_buf.size() < _contentLen) {
        return;
    }

    _body = _buf.substr(0, _contentLen);
    _buf.erase(0, _contentLen);
    _state = DONE;
}

void HttpRequest::_parseData() {
    while (_state != DONE && _state != PARSING_ERROR) {
        ParsingState old = _state;

        if (_state == READ_FIRST_LINE) {
            _parseFirstLine();
        } else if (_state == READ_HEADER) {
            _parseHeaders();
        } else if (_state == READ_BODY) {
            _parseBody();
        }

        if (_state == old) {
            break;
        }
    }
}

void HttpRequest::processData(const char* data, size_t len) {
    if (_state == DONE || _state == PARSING_ERROR) {
        return;
    }
    std::string preview = std::string(data, len < 40 ? len : 40);
    _buf.append(data, len);
    _parseData();
    _logResult(preview);
}

void HttpRequest::_logResult(const std::string& preview) const {
    if (_state == DONE) {
        std::string msg = "Parsed: " + _method + " " + _path + " " + _version +
                          " | headers: " + toString(static_cast<int>(_headers.size())) +
                          " | body: " + toString(static_cast<int>(_body.size())) + "B";
        LOG_DEBUG(msg);
    } else if (_state == PARSING_ERROR) {
        LOG_WARNING("Parse error " + toString(_errorCode) + " on: " + preview);
    }
}

std::string HttpRequest::extractMultipartFilename() {
    std::string body = _body;
    size_t pos = body.find("filename=\"");
    if (pos == std::string::npos) {
        std::ostringstream filename;
        filename << std::time(0) << "_" << std::rand() << ".bin";
        return filename.str();
    }
    pos += 10;
    size_t end = body.find("\"", pos);
    if (end == std::string::npos) {
        return "";
    }
    return body.substr(pos, end - pos);
}

std::string HttpRequest::extractMultipartBody() {
    std::string contentType = getHeader("Content-Type");
    std::string body = _body;
    LOG_DEBUG("Content type from extractMultipart " + contentType);
    // Find boundary
    size_t pos = contentType.find("boundary=");
    if (pos == std::string::npos) return body;  // no boundary — return whole body

    std::string boundary = "--" + contentType.substr(pos + 9);

    size_t boundary_start = body.find(boundary);
    if (boundary_start == std::string::npos) {
        LOG_WARNING("Boundary not found in body!");
        return "";
    }
    // Find end of headers — empty line \r\n\r\n
    size_t header_end = body.find("\r\n\r\n", boundary_start);
    if (header_end == std::string::npos) return "";

    size_t data_start = header_end + 4;

    // Find ending boundary
    size_t data_end = body.find("\r\n" + boundary, data_start);
    if (data_end == std::string::npos)
        return body.substr(data_start);  // no ending boundary — take everything

    return body.substr(data_start, data_end - data_start);
}

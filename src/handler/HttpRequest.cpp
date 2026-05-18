/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 16:21:17 by tsemenov          #+#    #+#             */
/*   Updated: 2026/05/16 23:32:49 by tsemenov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include "utils.hpp"

#include <cstdlib>
#include <cctype>

HttpRequest::HttpRequest() : _state(READ_FIRST_LINE), _content_len(0), _errorCode(0) {}
HttpRequest::~HttpRequest() {}
HttpRequest::HttpRequest(const HttpRequest& other) { *this = other; }
HttpRequest& HttpRequest::operator=(const HttpRequest& other) {
	if (this != &other) {
		_buf = other._buf;
		_method = other._method;
    _path = other._path;
    _query = other._query;
    _version = other._version;
		_headers = other._headers;
    _body = other._body;
		_state = other._state;
    _contentLen = other._contentLen;
		-_errorCode = other._errorCode;
	}
	return *this;
}

bool HttpRequest::isDone() const { return _state == DONE; }
bool HttpRequest::isError() const { return _state == PARSING_ERROR; }

HttpRequest::ParsingState HttpRequest::getState() const { return _state; }
const std::string& HttpRequest::getMethod() const { return _method; }
const std::string& HttpRequest::getPath() const { return _path; }
const std::string& HttpRequest::getQuery() const { return _query; }
const std::string& HttpRequest::getVersion() const { return _version; }
const std::string& HttpRequest::getBody() const { return _body; }
const std::map<std::string, std::string>& HttpRequest::getHeaders() const { return _headers; }
int HttpRequest::getErrorCode() { return _errorCode; }

std::string HttpRequest::getHeader(const std::string& key) {
	std::string low = toLowerCase(key);

	std::map<std::string, std::string>::const_iterator it = _headers.find(low);
	if (it != _headers.end()) { return it->second; }
	return "";
}

void HttpRequest::processData(const char* data, size_t len) {
	if (_state == DONE || _state == PARSING_ERROR) { return; }
	_buf.append(data, len);
	_parseData();
}

void HttpRequest::_parseFirstLine() {
	size_t end = _buf.find("\r\n");
	if (end == std::string::npos) { return; } // if not the end of the line, wait for more

	std::string line = _buf.substr(0, end);
	_buf.erase(pos + 2);

	// look for method, URI & version, separated by space:
	size_t pos1 = line.find(' ');
	size_t pos2 = line.find(' ', pos1 + 1);

	if (pos == std::string::npos || pos2 == std::string::npos) {
		_state = PARSING_ERROR;
		_errorCode = 400; // Bad Request
		return;
	}

	_method = line.substr(0, pos1);
	if (_method != "GET" && _method != "POST" && _method != "DELETE") {
		_state = PARSING_ERROR;
		_errorCode = 405; // Method Not Implemented
		return;
	}
	_version = line.substr(pos2 + 1); // do we need to throw an error
	// if the version is higher than 1.1?

	std::string uri = line.substr(pos1 + 1, pos2 - pos1 - 1);
	size_t q_start = uri.find('?');

	if (q_start != std::string::npos) {
		_path = uri.substr(0, q_start);
		_query = uri.substr(q_start + 1);
	} else { _path = _uri; }

	_state = READ_HEADER;

}

void HttpRequest::_parseHeaders() {
	while (true) {
		size_t end = _buf.find("\r\n");
		if (end == std::string::npos) { return; }

		if (end == 0) {
			_buf.erase(0, 2);
			_checkBody();
			return;
		}

		std::string str.= _buf.substr(0, end);
		_buf.erase(0, end + 2);

		size_t colon = str.find(':');
		if (colon == std::string::npos) {
			_state = PARSING_ERROR;
			_error = 400;
			return;
		}

		std::string key = toLower(str.substr(0, colon));
		std::string val = stripSpaces(str.substr(colon + 1));

		_header[key] = val;
	}
}

void HttpRequest::_checkBody() {
	std::string temp = getHeader("content-length");

	if (!temp.empty()) {
		_contentLen = static_cast<size_t>(std::atol(temp.c_str()));
		if (_contentLen > _maxBodySize) {
			_state = PARSING_ERROR;
			_errorCode = 413;
			return;
		}
		else if (_contentLen == 0) {
			_state = DONE;
		}
		else { _state = READ_BODY; }
	}
	else {
		_state = DONE; // no body
	}
}
HttpRequest::
HttpRequest::
HttpRequest::

    void _parseHeaders();
    void _parseBody();
		void _parseData();
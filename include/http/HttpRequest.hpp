/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 18:22:51 by dtimofee          #+#    #+#             */
/*   Updated: 2026/05/16 23:26:27 by tsemenov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <string>

class HttpRequest {
   private:
	 	std::string	_buf;
    std::string	_method;
    std::string	_path;
		std::string _query; // part after ?
		std::string _version;
    std::map<std::string, std::string>	_headers;
    std::string	_body;

		size_t _maxBodySize;

		int _errorCode;

		enum ParsingState {
			READ_FIRST_LINE,
			READ_HEADER,
			READ_BODY,
			DONE,
			PARSING_ERROR
		};
		
		ParsingState	_state;
		size_t			_contentLen;

		void _parseFirstLine();
    void _parseHeaders();
    void _checkBody();
    void _parseBody();
		void _parseData();

	public:

		HttpRequest();
		~HttpRequest();
		HttpRequest(const HttpRequest& other);
    HttpRequest& operator=(const HttpRequest& other);

		void processData(const char* data, size_t len);

		const std::string getMethod() const;
		const std::string getPath() const;
		const std::string getQuery() const;
    const std::string getVersion() const;
		const std::string getHeader(const std::string& key) const;
		const std::map<std::string, std::string> getHeaders() const;
		const std::string getBody() const;
		ParsingState getState() const;
		int getErrorCode() const;

		bool isDone() const;
		bool isError() const;

};

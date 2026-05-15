/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tests_response.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtimofee <dtimofee@student.42berlin.de>    #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-04-06 16:47:36 by dtimofee          #+#    #+#             */
/*   Updated: 2026-04-06 16:47:36 by dtimofee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"
#include "catch.hpp"

TEST_CASE("HttpResponse::make creates correct response", "[HttpResponse]") {
    HttpResponse response = HttpResponse::make(HTTP_OK, "<h1>Hello</h1>", "text/html");
    std::string raw = response.toString();

    SECTION("status line is correct") { REQUIRE(raw.find("HTTP/1.0 200 OK") != std::string::npos); }

    SECTION("Content-Type header is set") {
        REQUIRE(raw.find("Content-Type: text/html") != std::string::npos);
    }

    SECTION("Content-Length header is set") {
        REQUIRE(raw.find("Content-Length: 14") != std::string::npos);
    }

    SECTION("body is present") { REQUIRE(raw.find("<h1>Hello</h1>") != std::string::npos); }

    SECTION("headers and body separated by empty line") {
        REQUIRE(raw.find("\r\n\r\n") != std::string::npos);
    }
}

TEST_CASE("HttpResponse::make with empty body", "[HttpResponse]") {
    HttpResponse response = HttpResponse::make(HTTP_CREATED, "", "text/html");
    std::string raw = response.toString();

    REQUIRE(raw.find("201 Created") != std::string::npos);
    REQUIRE(raw.find("Content-Length: 0") != std::string::npos);
}

TEST_CASE("HttpResponse::getStatusText returns correct text", "[HttpResponse]") {
    REQUIRE(HttpResponse::getStatusText(HTTP_OK) == "OK");
    REQUIRE(HttpResponse::getStatusText(HTTP_CREATED) == "Created");
    REQUIRE(HttpResponse::getStatusText(HTTP_FORBIDDEN) == "Forbidden");
    REQUIRE(HttpResponse::getStatusText(HTTP_NOT_FOUND) == "Not Found");
    REQUIRE(HttpResponse::getStatusText(HTTP_METHOD_NOT_ALLOWED) == "Method Not Allowed");
}

TEST_CASE("HttpResponse::getStatusText returns Unknown for unknown code", "[HttpResponse]") {
    REQUIRE(HttpResponse::getStatusText(999) == "Unknown");
}

TEST_CASE("HttpResponse::toString format is correct", "[HttpResponse]") {
    std::vector<std::pair<std::string, std::string>> headers;
    headers.push_back(std::make_pair("Content-Type", "text/html"));
    headers.push_back(std::make_pair("Content-Length", "5"));

    HttpResponse response(HTTP_OK, "hello", headers);
    std::string raw = response.toString();

    SECTION("Status line - known code") { REQUIRE(raw.find("HTTP/1.0 200 OK") == 0); }

    SECTION("Headers order preserved") {
        REQUIRE(raw.find("Content-Type") < raw.find("Content-Length"));
    }

    SECTION("Empty line before body") {
        size_t separator = raw.find("\r\n\r\n");
        REQUIRE(separator != std::string::npos);
    }
    SECTION("Body comes after headers") {
        size_t separator = raw.find("\r\n\r\n");
        REQUIRE(raw.find("<h1>Hello</h1>") > separator);
    }
}

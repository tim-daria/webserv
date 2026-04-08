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

#define CATCH_CONFIG_MAIN
#include "../include/handler/HttpResponse.hpp"
#include "catch.hpp"

TEST_CASE("Status line - known code") {
    std::vector<std::pair<std::string, std::string> > headers;
    HttpResponse r(200, "hello", headers);
    REQUIRE(r.toString().find("HTTP/1.0 200 OK") == 0);
}

TEST_CASE("Status line - unknown code") {
    std::vector<std::pair<std::string, std::string> > headers;
    HttpResponse r(999, "hello", headers);
    REQUIRE(r.toString().find("HTTP/1.0 999 Unknown") == 0);
}

TEST_CASE("Headers order preserved") {
    std::vector<std::pair<std::string, std::string> > headers;
    headers.push_back(std::make_pair("Content-Type", "text/html"));
    headers.push_back(std::make_pair("Content-Length", "5"));
    HttpResponse r(200, "hello", headers);
    std::string s = r.toString();
    REQUIRE(s.find("Content-Type") < s.find("Content-Length"));
}

TEST_CASE("Empty line before body") {
    std::vector<std::pair<std::string, std::string> > headers;
    HttpResponse r(200, "hello", headers);
    REQUIRE(r.toString().find("\r\n\r\n") != std::string::npos);
}

TEST_CASE("Body comes after headers") {
    std::vector<std::pair<std::string, std::string> > headers;
    HttpResponse r(200, "<h1>Hello</h1>", headers);
    std::string s = r.toString();
    REQUIRE(s.find("<h1>Hello</h1>") > s.find("\r\n\r\n"));
}

TEST_CASE("Copy constructor") {
    std::vector<std::pair<std::string, std::string> > headers;
    headers.push_back(std::make_pair("Content-Type", "text/html"));
    HttpResponse r1(200, "hello", headers);
    HttpResponse r2(r1);
    REQUIRE(r1.toString() == r2.toString());
}

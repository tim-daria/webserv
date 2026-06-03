/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tests_error_handler.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/15 10:16:27 by dtimofee          #+#    #+#             */
/*   Updated: 2026/06/03 11:33:02 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ErrorHandler.hpp"
#include "ServerConfig.hpp"
#include "catch.hpp"

TEST_CASE("ErrorHandler::makeError returns correct status code", "[ErrorHandler]") {
    ServerConfig conf;
    ErrorHandler errorHandler(conf);

    SECTION("404 error") {
        HttpResponse response = errorHandler.makeError(HTTP_NOT_FOUND);
        std::string raw = response.toString();
        REQUIRE(raw.find("404 Not Found") != std::string::npos);
    }

    SECTION("403 error") {
        HttpResponse response = errorHandler.makeError(HTTP_FORBIDDEN);
        std::string raw = response.toString();
        REQUIRE(raw.find("403 Forbidden") != std::string::npos);
    }

    SECTION("500 error") {
        HttpResponse response = errorHandler.makeError(HTTP_INTERNAL_ERROR);
        std::string raw = response.toString();
        REQUIRE(raw.find("500 Internal Server Error") != std::string::npos);
    }
}

TEST_CASE("ErrorHandler::makeError default page contains status code in body", "[ErrorHandler]") {
    ServerConfig conf;
    ErrorHandler errorHandler(conf);

    HttpResponse response = errorHandler.makeError(HTTP_NOT_FOUND);
    std::string raw = response.toString();

    REQUIRE(raw.find("404") != std::string::npos);
    REQUIRE(raw.find("Not Found") != std::string::npos);
}

TEST_CASE("ErrorHandler::makeError has correct Content-Type", "[ErrorHandler]") {
    ServerConfig conf;
    ErrorHandler errorHandler(conf);

    HttpResponse response = errorHandler.makeError(HTTP_NOT_FOUND);
    std::string raw = response.toString();

    REQUIRE(raw.find("Content-Type: text/html") != std::string::npos);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tests_request_handler.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtimofee <dtimofee@student.42berlin.de>    #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-05-15 10:57:35 by dtimofee          #+#    #+#             */
/*   Updated: 2026-05-15 10:57:35 by dtimofee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define CATCH_CONFIG_MAIN
#include "RequestHandler.hpp"
#include "catch.hpp"
#include "io.cpp"

TEST_CASE("RequestHandler — method not allowed", "[RequestHandler]") {
    // Creates default config with location just for GET
    ServerConfig config;
    RouteConfig route;
    route.url = "/";
    route.rootDirectory = "/tmp/test_handler";
    route.add_acceptedMethod("GET");
    config.routes.push_back(route);

    createDir("/tmp/test_handler");
    createFile("/tmp/test_handler/index.html", "<h1>Hello</h1>");

    Handler handler(config);

    SECTION("GET allowed returns 200") {
        HttpRequest request;
        request.method = "GET";
        request.path = "/index.html";

        HttpResponse response = handler.handle_request(request);
        REQUIRE(response.toString().find("200 OK") != std::string::npos);
    }

    SECTION("POST not allowed returns 405") {
        HttpRequest request;
        request.method = "POST";
        request.path = "/index.html";

        HttpResponse response = handler.handle_request(request);
        REQUIRE(response.toString().find("405") != std::string::npos);
    }

    SECTION("DELETE not allowed returns 405") {
        HttpRequest request;
        request.method = "DELETE";
        request.path = "/index.html";

        HttpResponse response = handler.handle_request(request);
        REQUIRE(response.toString().find("405") != std::string::npos);
    }

    removeFile("/tmp/test_handler/index.html");
    removeDir("/tmp/test_handler");
}

TEST_CASE("RequestHandler — no matching location returns 404", "[RequestHandler]") {
    ServerConfig config;
    RouteConfig route;
    route.url = "/photos";
    route.rootDirectory = "/tmp/photos";
    route.add_acceptedMethod("GET");
    config.routes.push_back(route);

    Handler handler(config);

    HttpRequest request;
    request.method = "GET";
    request.path = "/other/page.html";  // no matching location

    HttpResponse response = handler.handle_request(request);
    REQUIRE(response.toString().find("404") != std::string::npos);
}

TEST_CASE("RequestHandler — GET nonexistent file returns 404", "[RequestHandler]") {
    ServerConfig config;
    RouteConfig route;
    route.url = "/";
    route.rootDirectory = "/tmp/test_handler2";
    route.add_acceptedMethod("GET");
    config.routes.push_back(route);

    createDir("/tmp/test_handler2");

    Handler handler(config);

    HttpRequest request;
    request.method = "GET";
    request.path = "/nonexistent.html";

    HttpResponse response = handler.handle_request(request);
    REQUIRE(response.toString().find("404") != std::string::npos);

    removeDir("/tmp/test_handler2");
}

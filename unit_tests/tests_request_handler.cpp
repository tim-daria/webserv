/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tests_request_handler.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/15 10:57:35 by dtimofee          #+#    #+#             */
/*   Updated: 2026/06/12 13:26:10 by tsemenov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define CATCH_CONFIG_MAIN
#include <sys/stat.h>

#include "RequestHandler.hpp"
#include "catch.hpp"
#include "io.hpp"

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
        std::string raw = "GET /index.html HTTP/1.1\r\n\r\n";
        request.processData(raw.c_str(), raw.size());

        HttpResponse response = handler.handle_request(request);
        REQUIRE(response.getStatusCode() == HTTP_OK);
    }

    SECTION("POST not allowed returns 405") {
        ServerConfig config;
        RouteConfig route;
        route.url = "/upload";
        route.uploadDirectory = "";
        route.rootDirectory = "./www";
        route.add_acceptedMethod("POST");
        route.clientMaxBodySize = 7;
        config.add_route(route);

        HttpRequest request;
        std::string raw = "POST /upload/index.html HTTP/1.1\r\nContent-Length: 10\r\n\r\n";
        request.processData(raw.c_str(), raw.size());

        HttpResponse response = handler.handle_request(request);
        REQUIRE(response.getStatusCode() == HTTP_METHOD_NOT_ALLOWED);
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
    config.add_route(route);

    Handler handler(config);

    HttpRequest request;
    std::string raw = "GET /other/page.html HTTP/1.1\r\n\r\n";
    request.processData(raw.c_str(), raw.size());

    HttpResponse response = handler.handle_request(request);
    REQUIRE(response.getStatusCode() == HTTP_NOT_FOUND);
}

TEST_CASE("RequestHandler — body exceeds maxBodySize returns 413", "[RequestHandler]") {
    ServerConfig config;
    RouteConfig route;
    route.url = "/upload";
    route.uploadDirectory = "";
    route.rootDirectory = "./www";
    route.add_acceptedMethod("POST");
    route.clientMaxBodySize = 1;
    config.add_route(route);

    Handler handler(config);

    HttpRequest request;
    std::string raw = "POST /upload HTTP/1.1\r\nContent-Length: 5\r\n\r\nHello";
    request.processData(raw.c_str(), raw.size());

    HttpResponse response = handler.handle_request(request);
    REQUIRE(response.getStatusCode() == HTTP_PAYLOAD_TOO_LARGE);
}

TEST_CASE("RequestHandler — GET nonexistent file returns 404", "[RequestHandler]") {
    ServerConfig config;
    RouteConfig route;
    route.url = "/";
    route.rootDirectory = "/tmp/test_handler2";
    route.add_acceptedMethod("GET");
    config.add_route(route);

    createDir("/tmp/test_handler2");

    Handler handler(config);

    HttpRequest request;
    std::string raw = "GET /nonexistent.html HTTP/1.1\r\n\r\n";
    request.processData(raw.c_str(), raw.size());

    HttpResponse response = handler.handle_request(request);
    REQUIRE(response.getStatusCode() == HTTP_NOT_FOUND);

    removeDir("/tmp/test_handler2");
}

TEST_CASE("POST returns 403 when upload directory is not configured") {
    ServerConfig config;
    RouteConfig route;

    route.url = "/upload";
    route.uploadDirectory = "";
    route.rootDirectory = "./www";
    route.add_acceptedMethod("POST");
    config.add_route(route);

    Handler handler(config);

    HttpRequest request;
    std::string raw = "POST /upload HTTP/1.1\r\n Content-Length: 5\r\n\r\n Hello";
    request.processData(raw.c_str(), raw.size());

    HttpResponse response = handler.handle_request(request);

    REQUIRE(response.getStatusCode() == HTTP_FORBIDDEN);
}

TEST_CASE("POST returns 404 when upload directory does not exist") {
    ServerConfig config;
    RouteConfig route;

    route.url = "/upload";
    route.rootDirectory = "./www";
    route.uploadDirectory = "/does_not_exist";
    route.add_acceptedMethod("POST");
    config.add_route(route);

    Handler handler(config);

    HttpRequest request;
    std::string raw = "POST /upload HTTP/1.1\r\n Content-Length: 5\r\n\r\n Hello";
    request.processData(raw.c_str(), raw.size());

    HttpResponse response = handler.handle_request(request);

    REQUIRE(response.getStatusCode() == HTTP_NOT_FOUND);
}

TEST_CASE("POST creates file and returns 201") {
    createDir("./test_uploads");

    ServerConfig config;
    RouteConfig route;

    route.url = "/upload";
    route.rootDirectory = ".";
    route.uploadDirectory = "/test_uploads";
    route.add_acceptedMethod("POST");
    config.add_route(route);
    Handler handler(config);

    HttpRequest request;
    std::string raw =
        "POST /upload HTTP/1.1\r\n"
        "Content-Length: 5\r\n"
        "\r\n"
        "Hello";
    request.processData(raw.c_str(), raw.size());

    HttpResponse response = handler.handle_request(request);

    REQUIRE(response.getStatusCode() == HTTP_CREATED);
    std::string location = response.getHeader("Location");

    REQUIRE_FALSE(location.empty());

    std::ifstream file(location.c_str());

    REQUIRE(file.good());

    std::string content;
    std::getline(file, content);

    REQUIRE(content == "Hello");

    removeFile(location);
    removeDir("./test_uploads");
}

TEST_CASE("DELETE removes existing file") {
    std::ofstream file("./delete_me.txt");
    file << "hello";
    file.close();

    ServerConfig config;
    RouteConfig route;

    route.url = "/";
    route.rootDirectory = ".";
    route.add_acceptedMethod("DELETE");
    config.add_route(route);

    Handler handler(config);

    HttpRequest request;
    std::string raw = "DELETE /delete_me.txt HTTP/1.1\r\n";
    request.processData(raw.c_str(), raw.size());

    HttpResponse response = handler.handle_request(request);

    REQUIRE(response.getStatusCode() == HTTP_NO_CONTENT);

    struct stat info;

    REQUIRE(stat("./delete_me.txt", &info) != 0);
}

TEST_CASE("DELETE returns 404 for missing file") {
    ServerConfig config;
    RouteConfig route;

    route.url = "/";
    route.rootDirectory = ".";
    route.add_acceptedMethod("DELETE");
    config.add_route(route);

    Handler handler(config);

    HttpRequest request;
    std::string raw = "DELETE /file_that_does_not_exist.txt HTTP/1.1\r\n";
    request.processData(raw.c_str(), raw.size());

    HttpResponse response = handler.handle_request(request);

    REQUIRE(response.getStatusCode() == HTTP_NOT_FOUND);
}

TEST_CASE("DELETE directory returns 403") {
    createDir("./delete_test_dir");

    ServerConfig config;
    RouteConfig route;

    route.url = "/";
    route.rootDirectory = ".";
    route.add_acceptedMethod("DELETE");
    config.add_route(route);

    Handler handler(config);

    HttpRequest request;
    std::string raw = "DELETE /delete_test_dir HTTP/1.1\r\n";
    request.processData(raw.c_str(), raw.size());

    HttpResponse response = handler.handle_request(request);

    REQUIRE(response.getStatusCode() == HTTP_FORBIDDEN);

    removeDir("./delete_test_dir");
}

TEST_CASE("GET returns 301 redirect from config") {
    ServerConfig config;

    RouteConfig route;
    route.url = "/old";
    route.hasReturn = true;
    route.returnStatus = 301;
    route.returnUri = "/new";

    config.add_route(route);

    Handler handler(config);

    HttpRequest request;

    std::string raw = "GET /old HTTP/1.1\r\n";

    request.processData(raw.c_str(), raw.size());

    HttpResponse response = handler.handle_request(request);

    std::string text = response.toString();

    REQUIRE(text.find("301") != std::string::npos);
    REQUIRE(text.find("Location: /new") != std::string::npos);
}

TEST_CASE("Directory without trailing slash returns redirect") {
    ServerConfig config;

    RouteConfig route;
    route.url = "/uploads";
    route.rootDirectory = "./www/webserv";
    route.add_acceptedMethod("GET");

    config.add_route(route);

    Handler handler(config);

    HttpRequest request;

    std::string raw = "GET /uploads HTTP/1.1\r\n";

    request.processData(raw.c_str(), raw.size());

    HttpResponse response = handler.handle_request(request);

    std::string text = response.toString();

    INFO(response.toString());
    REQUIRE(text.find("301") != std::string::npos);
    REQUIRE(text.find("Location: /uploads/") != std::string::npos);
}

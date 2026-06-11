/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tests_cgi.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtimofee <dtimofee@student.42berlin.de>    #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-06-10 10:51:15 by dtimofee          #+#    #+#             */
/*   Updated: 2026-06-10 10:51:15 by dtimofee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/stat.h>
#include <unistd.h>

#include <catch.hpp>
#include <fstream>

#include "CGIHandler.hpp"
#include "HttpRequest.hpp"
#include "RouteConfig.hpp"
#include "ServerConfig.hpp"

// ─────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────

static void createScript(const std::string& path, const std::string& content) {
    mkdir("/tmp/cgi-bin", 0755);
    std::ofstream f(path.c_str());
    REQUIRE(f.is_open());
    f << content;
    f.close();
    chmod(path.c_str(), 0755);  // to make script executable
}

static void removeFile(const std::string& path) { ::unlink(path.c_str()); }

static ServerConfig makeConfig() {
    ServerConfig config;
    config.serverName = "localhost";
    return config;
}

static RouteConfig makeRoute(const std::string& root, const std::string& url) {
    RouteConfig route;
    route.rootDirectory = root;
    route.url = url;
    route.cgiHandlers[".py"] = "/usr/bin/python3";
    route.cgiHandlers[".sh"] = "/bin/bash";
    return route;
}

// ─────────────────────────────────────────────
// CGIHandler::findInterpreter
// ─────────────────────────────────────────────

TEST_CASE("CGIHandler::findInterpreter finds correct interpreter", "[CGIHandler]") {
    ServerConfig config = makeConfig();
    CGIHandler handler(config);

    RouteConfig route = makeRoute("/tmp", "/cgi-bin");

    SECTION("finds python interpreter for .py") {
        createScript("/tmp/cgi-bin/hello.py", "#!/usr/bin/env python3\nprint('hello')");

        HttpRequest request;
        std::string raw = "GET /cgi-bin/hello.py HTTP/1.1\r\n\r\n";
        request.processData(raw.c_str(), raw.size());
        HttpResponse response = handler.execute(request, &route);

        removeFile("/tmp/cgi-bin/hello.py");
    }

    SECTION("returns 500 for unknown extension") {
        createScript("/tmp/cgi-bin/script.xyz", "some content");

        HttpRequest request;
        std::string raw = "GET /cgi-bin/script.xyz HTTP/1.1\r\n\r\n";
        request.processData(raw.c_str(), raw.size());
        HttpResponse response = handler.execute(request, &route);

        REQUIRE(response.getStatusCode() == HTTP_INTERNAL_ERROR);
        removeFile("/tmp/cgi-bin/script.xyz");
    }
}

// ─────────────────────────────────────────────
// CGIHandler::execute — File not found
// ─────────────────────────────────────────────

TEST_CASE("CGIHandler::execute returns 404 for nonexistent script", "[CGIHandler]") {
    ServerConfig config = makeConfig();
    CGIHandler handler(config);
    RouteConfig route = makeRoute("/tmp", "/cgi-bin");

    HttpRequest request;
    std::string raw = "GET /cgi-bin/nonexistent.py HTTP/1.1\r\n\r\n";
    request.processData(raw.c_str(), raw.size());
    HttpResponse response = handler.execute(request, &route);

    REQUIRE(response.getStatusCode() == HTTP_NOT_FOUND);
}

// ─────────────────────────────────────────────
// CGIHandler::execute — GET запрос
// ─────────────────────────────────────────────

TEST_CASE("CGIHandler::execute handles GET request", "[CGIHandler]") {
    ServerConfig config = makeConfig();
    CGIHandler handler(config);
    RouteConfig route = makeRoute("/tmp", "/cgi-bin");

    createScript("/tmp/cgi-bin/hello.py",
                 "#!/usr/bin/env python3\n"
                 "print('Content-Type: text/html')\n"
                 "print('')\n"
                 "print('Hello from CGI!')\n");

    HttpRequest request;
    std::string raw = "GET /cgi-bin/hello.py HTTP/1.1\r\n\r\n";
    request.processData(raw.c_str(), raw.size());
    HttpResponse response = handler.execute(request, &route);

    SECTION("returns 200") { REQUIRE(response.getStatusCode() == HTTP_OK); }

    SECTION("body contains script output") {
        REQUIRE(response.toString().find("Hello from CGI!") != std::string::npos);
    }

    removeFile("/tmp/cgi-bin/hello.py");
}

// ─────────────────────────────────────────────
// CGIHandler::execute — Environment variables
// ─────────────────────────────────────────────

TEST_CASE("CGIHandler::execute sets environment variables", "[CGIHandler]") {
    ServerConfig config = makeConfig();
    CGIHandler handler(config);
    RouteConfig route = makeRoute("/tmp", "/cgi-bin");

    // Script prints env
    createScript("/tmp/cgi-bin/env_test.py",
                 "#!/usr/bin/env python3\n"
                 "import os\n"
                 "print('Content-Type: text/plain')\n"
                 "print('')\n"
                 "print('METHOD=' + os.environ.get('REQUEST_METHOD', 'NOT_SET'))\n"
                 "print('QUERY=' + os.environ.get('QUERY_STRING', 'NOT_SET'))\n"
                 "print('GATEWAY=' + os.environ.get('GATEWAY_INTERFACE', 'NOT_SET'))\n");

    SECTION("REQUEST_METHOD is set correctly") {
        HttpRequest request;
        std::string raw = "GET /cgi-bin/env_test.py HTTP/1.1\r\n\r\n";
        request.processData(raw.c_str(), raw.size());
        HttpResponse response = handler.execute(request, &route);
        REQUIRE(response.toString().find("METHOD=GET") != std::string::npos);
    }

    SECTION("GATEWAY_INTERFACE is set") {
        HttpRequest request;
        std::string raw = "GET /cgi-bin/env_test.py HTTP/1.1\r\n\r\n";
        request.processData(raw.c_str(), raw.size());
        HttpResponse response = handler.execute(request, &route);
        REQUIRE(response.toString().find("GATEWAY=CGI/1.1") != std::string::npos);
    }

    SECTION("QUERY_STRING is set from path") {
        HttpRequest request;
        std::string raw = "GET /cgi-bin/env_test.py?name=Alice HTTP/1.1\r\n\r\n";
        request.processData(raw.c_str(), raw.size());
        HttpResponse response = handler.execute(request, &route);
        REQUIRE(response.toString().find("QUERY=name=Alice") != std::string::npos);
    }

    removeFile("/tmp/cgi-bin/env_test.py");
}

// ─────────────────────────────────────────────
// CGIHandler::execute — POST request
// ─────────────────────────────────────────────

TEST_CASE("CGIHandler::execute handles POST request with body", "[CGIHandler]") {
    ServerConfig config = makeConfig();
    CGIHandler handler(config);
    RouteConfig route = makeRoute("/tmp", "/cgi-bin");

    // Script reads from stdin
    createScript("/tmp/cgi-bin/post_test.py",
                 "#!/usr/bin/env python3\n"
                 "import sys\n"
                 "import os\n"
                 "content_length = int(os.environ.get('CONTENT_LENGTH', 0))\n"
                 "body = sys.stdin.read(content_length)\n"
                 "print('Content-Type: text/plain')\n"
                 "print('')\n"
                 "print('BODY=' + body)\n");

    HttpRequest request;
    std::string raw =
        "POST /cgi-bin/post_test.py HTTP/1.1\r\n"
        "Content-Length: 11"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "\r\n"
        "hello=world";
    request.processData(raw.c_str(), raw.size());
    HttpResponse response = handler.execute(request, &route);

    REQUIRE(response.getStatusCode() == HTTP_OK);
    REQUIRE(response.toString().find("hello=world") != std::string::npos);

    removeFile("/tmp/cgi-bin/post_test.py");
}

// ─────────────────────────────────────────────
// CGIHandler::execute — script failes
// ─────────────────────────────────────────────

TEST_CASE("CGIHandler::execute returns 500 when script fails", "[CGIHandler]") {
    ServerConfig config = makeConfig();
    CGIHandler handler(config);
    RouteConfig route = makeRoute("/tmp", "/cgi-bin");

    // Script failes with an error
    createScript("/tmp/cgi-bin/fail.py",
                 "#!/usr/bin/env python3\n"
                 "import sys\n"
                 "sys.exit(1)\n");

    HttpRequest request;
    std::string raw = "GET /cgi-bin/fail.py HTTP/1.1\r\n\r\n";
    request.processData(raw.c_str(), raw.size());
    HttpResponse response = handler.execute(request, &route);

    REQUIRE(response.getStatusCode() == HTTP_INTERNAL_ERROR);

    removeFile("/tmp/cgi-bin/fail.py");
}

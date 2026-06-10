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
    // chmod(path.c_str(), 0755);  // сделать исполняемым
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

// static HttpRequest makeRequest(const std::string& method,
//                                 const std::string& path,
//                                 const std::string& body = "") {
//     HttpRequest request;
//     request.setMethod(method);
//     request.setPath(path);
//     request.setBody(body);
//     return request;
// }

// ─────────────────────────────────────────────
// CGIHandler::findInterpreter
// ─────────────────────────────────────────────

TEST_CASE("CGIHandler::findInterpreter finds correct interpreter", "[CGIHandler]") {
    ServerConfig config = makeConfig();
    CGIHandler handler(config);

    RouteConfig route = makeRoute("/tmp", "/cgi-bin");

    SECTION("finds python interpreter for .py") {
        // Создать скрипт чтобы _scriptPath был установлен
        createScript("/tmp/hello.py", "#!/usr/bin/env python3\nprint('hello')");

        HttpRequest request;
        std::string raw = "GET /cgi-bin/hello.py HTTP/1.1\r\n\r\n";
        request.processData(raw.c_str(), raw.size());
        // HttpRequest  request = makeRequest("GET", "/cgi-bin/hello.py");
        HttpResponse response = handler.execute(request, &route);

        // Если интерпретатор найден — не должно быть 500
        // (скрипт может упасть но не из-за интерпретатора)
        removeFile("/tmp/hello.py");
    }

    SECTION("returns 500 for unknown extension") {
        createScript("/tmp/script.xyz", "some content");

        HttpRequest request;
        std::string raw = "GET /cgi-bin/script.xyz HTTP/1.1\r\n\r\n";
        request.processData(raw.c_str(), raw.size());
        // HttpRequest  request  = makeRequest("GET", "/cgi-bin/script.xyz");
        HttpResponse response = handler.execute(request, &route);
        // std::string  raw      = response.toString();

        REQUIRE(response.getStatusCode() == HTTP_INTERNAL_ERROR);
        removeFile("/tmp/script.xyz");
    }
}

// ─────────────────────────────────────────────
// CGIHandler::execute — файл не найден
// ─────────────────────────────────────────────

TEST_CASE("CGIHandler::execute returns 404 for nonexistent script", "[CGIHandler]") {
    ServerConfig config = makeConfig();
    CGIHandler handler(config);
    RouteConfig route = makeRoute("/tmp", "/cgi-bin");

    HttpRequest request;
    std::string raw = "GET /cgi-bin/nonexistent.py HTTP/1.1\r\n\r\n";
    request.processData(raw.c_str(), raw.size());
    // HttpRequest  request  = makeRequest("GET", "/cgi-bin/nonexistent.py");
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

    // Простой Python скрипт
    createScript("/tmp/hello.py",
                 "#!/usr/bin/env python3\n"
                 "print('Content-Type: text/html')\n"
                 "print('')\n"
                 "print('<h1>Hello from CGI!</h1>')\n");

    HttpRequest request;
    std::string raw = "GET /cgi-bin/hello.py HTTP/1.1\r\n\r\n";
    request.processData(raw.c_str(), raw.size());
    // HttpRequest  request  = makeRequest("GET", "/cgi-bin/hello.py");
    HttpResponse response = handler.execute(request, &route);
    // std::string  raw      = response.toString();

    SECTION("returns 200") { REQUIRE(response.getStatusCode() == HTTP_OK); }

    SECTION("body contains script output") { REQUIRE(response.getBody() == "Hello from CGI!"); }

    removeFile("/tmp/hello.py");
}

// ─────────────────────────────────────────────
// CGIHandler::execute — переменные окружения
// ─────────────────────────────────────────────

TEST_CASE("CGIHandler::execute sets environment variables", "[CGIHandler]") {
    ServerConfig config = makeConfig();
    CGIHandler handler(config);
    RouteConfig route = makeRoute("/tmp", "/cgi-bin");

    // Скрипт который выводит переменные окружения
    createScript("/tmp/env_test.py",
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
        // HttpRequest  request  = makeRequest("GET", "/cgi-bin/env_test.py");
        HttpResponse response = handler.execute(request, &route);
        REQUIRE(response.toString().find("METHOD=GET") != std::string::npos);
    }

    SECTION("GATEWAY_INTERFACE is set") {
        HttpRequest request;
        std::string raw = "GET /cgi-bin/env_test.py HTTP/1.1\r\n\r\n";
        request.processData(raw.c_str(), raw.size());
        // HttpRequest  request  = makeRequest("GET", "/cgi-bin/env_test.py");
        HttpResponse response = handler.execute(request, &route);
        REQUIRE(response.toString().find("GATEWAY=CGI/1.0") != std::string::npos);
    }

    SECTION("QUERY_STRING is set from path") {
        HttpRequest request;
        std::string raw = "GET /cgi-bin/env_test.py?name=Alice HTTP/1.1\r\n\r\n";
        request.processData(raw.c_str(), raw.size());
        // HttpRequest  request  = makeRequest("GET", "/cgi-bin/env_test.py?name=Alice");
        HttpResponse response = handler.execute(request, &route);
        REQUIRE(response.toString().find("QUERY=name=Alice") != std::string::npos);
    }

    removeFile("/tmp/env_test.py");
}

// ─────────────────────────────────────────────
// CGIHandler::execute — POST запрос
// ─────────────────────────────────────────────

// TEST_CASE("CGIHandler::execute handles POST request with body", "[CGIHandler]") {
//     ServerConfig config = makeConfig();
//     CGIHandler   handler(config);
//     RouteConfig  route = makeRoute("/tmp", "/cgi-bin");

//     // Скрипт который читает stdin
//     createScript("/tmp/post_test.py",
//         "#!/usr/bin/env python3\n"
//         "import sys\n"
//         "import os\n"
//         "content_length = int(os.environ.get('CONTENT_LENGTH', 0))\n"
//         "body = sys.stdin.read(content_length)\n"
//         "print('Content-Type: text/plain')\n"
//         "print('')\n"
//         "print('BODY=' + body)\n"
//     );

//     HttpRequest request = makeRequest("POST",
//                                        "/cgi-bin/post_test.py",
//                                        "hello=world");
//     request.setHeader("Content-Type", "application/x-www-form-urlencoded");

//     HttpResponse response = handler.execute(request, &route);
//     std::string  raw      = response.toString();

//     REQUIRE(raw.find("200")         != std::string::npos);
//     REQUIRE(raw.find("BODY=hello=world") != std::string::npos);

//     removeFile("/tmp/post_test.py");
// }

// // ─────────────────────────────────────────────
// // CGIHandler::execute — скрипт падает
// // ─────────────────────────────────────────────

// TEST_CASE("CGIHandler::execute returns 500 when script fails", "[CGIHandler]") {
//     ServerConfig config = makeConfig();
//     CGIHandler   handler(config);
//     RouteConfig  route = makeRoute("/tmp", "/cgi-bin");

//     // Скрипт который падает с ошибкой
//     createScript("/tmp/fail.py",
//         "#!/usr/bin/env python3\n"
//         "import sys\n"
//         "sys.exit(1)\n"
//     );

//     HttpRequest  request  = makeRequest("GET", "/cgi-bin/fail.py");
//     HttpResponse response = handler.execute(request, &route);
//     std::string  raw      = response.toString();

//     REQUIRE(raw.find("500") != std::string::npos);

//     removeFile("/tmp/fail.py");
// }

// // ─────────────────────────────────────────────
// // CGIHandler::toCharArray
// // ─────────────────────────────────────────────

// TEST_CASE("CGIHandler::toCharArray converts correctly", "[CGIHandler]") {
//     ServerConfig config = makeConfig();
//     CGIHandler   handler(config);

//     std::vector<std::string> env;
//     env.push_back("REQUEST_METHOD=GET");
//     env.push_back("QUERY_STRING=name=Alice");
//     env.push_back("CONTENT_LENGTH=0");

//     char** arr = handler.toCharArray(env);

//     SECTION("values are correct") {
//         REQUIRE(std::string(arr[0]) == "REQUEST_METHOD=GET");
//         REQUIRE(std::string(arr[1]) == "QUERY_STRING=name=Alice");
//         REQUIRE(std::string(arr[2]) == "CONTENT_LENGTH=0");
//     }

//     SECTION("terminated with NULL") {
//         REQUIRE(arr[3] == NULL);
//     }

//     handler.freeCharArray(arr, env.size());
// }

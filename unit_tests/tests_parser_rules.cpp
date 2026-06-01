/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tests_parser_rules.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 16:46:18 by nefimov           #+#    #+#             */
/*   Updated: 2026/06/01 17:10:02 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "catch.hpp"
#include "parser/Parser.hpp"

static std::string sConfig() {
    return "server {\n"
           "    listen 8080;\n"
           "    server_name localhost;\n"
           "    root ./www;\n"
           "    client_max_body_size 1048576;\n"
           "    error_page 404 /errors/404.html;\n"
           "    index index.html;\n"
           "    location / {\n"
           "        limit_except GET POST;\n"
           "    }\n"
           "    location /uploads {\n"
           "        limit_except GET POST DELETE;\n"
           "        autoindex on;\n"
           "    }\n"
           "}\n";
}

// config ::= ws (server_block ws)* ;
TEST_CASE("Check config rule", "[Parser]") {
    SECTION("Correct input. One server block") {
        std::string sampleConfig(" server {} \n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
    }

    SECTION("Correct input. Two server blocks") {
        std::string sampleConfig(" server {} \nserver{}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 2);
    }

    SECTION("Wrong keyword") {
        std::string sampleConfig(" location {} \n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }

    SECTION("Empty input") {
        std::string sampleConfig("");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }
}

// server_block ::= "server" ws block ;
TEST_CASE("Check server_block", "[Parser]") {
    SECTION("Correct input") {
        std::string sampleConfig("server {}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
    }

    SECTION("Wrong input without block") {
        std::string sampleConfig("server\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }
}

// block ::= "{" ws (statement ws)* "}" ;
TEST_CASE("Check block", "[Parser]") {
    SECTION("Correct input without statement") {
        std::string sampleConfig("server {}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
    }

    SECTION("Correct input with one statement") {
        std::string sampleConfig(
            "server {"
            "    listen 8080;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
    }

    SECTION("Correct input with all statement") {
        std::string sampleConfig(
            "server {"
            "    listen 8080;\n"
            "    server_name localhost;\n"
            "    root ./www;\n"
            "    client_max_body_size 1048576;\n"
            "    error_page 404 /errors/404.html;\n"
            "    index index.html;\n"
            "    location / {\n"
            "        limit_except GET POST;\n"
            "        autoindex on;\n"
            "        return 301 /redirected_here;\n"
            "        cgi_ext .py;\n"
            "        cgi_path /usr/bin/python3;\n"
            "    }\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
    }

    SECTION("Wrong input without opening curly bracket") {
        std::string sampleConfig(" server } \n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }

    SECTION("Wrong input without closing curly bracket") {
        std::string sampleConfig(" server { \n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }
}

// statement ::= directive ";" | location_block ;
TEST_CASE("Check statement", "[Parser]") {
    SECTION("Correct input with directive and location") {
        std::string sampleConfig(
            "server {"
            "    listen 8080;\n"
            "    location / {\n"
            "        limit_except GET POST;\n"
            "    }\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
    }

    SECTION("Correct input with only directive") {
        std::string sampleConfig(
            "server {"
            "    listen 8080;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
    }

    SECTION("Correct input with only location") {
        std::string sampleConfig(
            "server {"
            "    location / {\n"
            "        limit_except GET POST;\n"
            "    }\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
    }

    SECTION("Wrong input without semicolon after directive") {
        std::string sampleConfig(
            "server {"
            "    listen 8080\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }
}

// listen ::= "listen" ws port ;
TEST_CASE("Check listen directive", "[Parser]") {
    // Allowed ports 1-65535
    SECTION("Correct input") {
        std::string sampleConfig(
            "server {"
            "    listen 8080;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
    }

    SECTION("Correct input. Lowest allowed port number") {
        std::string sampleConfig(
            "server {"
            "    listen 1;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].listen[0].second == 1);
    }

    SECTION("Correct input. Highest allowed port number") {
        std::string sampleConfig(
            "server {"
            "    listen 65535;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].listen[0].second == 65535);
    }

    SECTION("Wrong input. Port numner is too low") {
        std::string sampleConfig(
            "server {"
            "    listen 0;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }

    SECTION("Wrong input. Port numner is too high") {
        std::string sampleConfig(
            "server {"
            "    listen 65536;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }
}

// server_name := "server_name" ws name ;
TEST_CASE("Check server_name directive", "[Parser]") {
    SECTION("Correct input") {
        std::string sampleConfig(
            "server {"
            "    server_name webserv;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].serverName == "webserv");
    }

    SECTION("Wrong input. Without a value") {
        std::string sampleConfig(
            "server {"
            "    server_name ;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }

    SECTION("Wrong input. Without a semicolon") {
        std::string sampleConfig(
            "server {"
            "    server_name webserv\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }
}

// root ::= "root" ws path ;
TEST_CASE("Check root directive", "[Parser]") {
    SECTION("Correct input") {
        std::string sampleConfig(
            "server {"
            "    root www/;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
    }

    SECTION("Wrong input. Without a value") {
        std::string sampleConfig(
            "server {"
            "    server_name ;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }

    SECTION("Wrong input. Without a semicolon") {
        std::string sampleConfig(
            "server {"
            "    server_name webserv\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }
}

// client_max_body_size ::= "client_max_body_size" ws number ;
TEST_CASE("Check client_max_body_size directive", "[Parser]") {
    SECTION("Correct input") {
        std::string sampleConfig(
            "server {"
            "    client_max_body_size 100000;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].clientMaxBodySize == 100000);
    }

    SECTION("Correct input. Lowest value") {
        std::string sampleConfig(
            "server {"
            "    client_max_body_size 1;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].clientMaxBodySize == 1);
    }

    SECTION("Correct input. Highest value") {
        std::string sampleConfig(
            "server {"
            "    client_max_body_size 2147483647;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].clientMaxBodySize == 2147483647);
    }

    SECTION("Wrong input. The value is too low") {
        std::string sampleConfig(
            "server {"
            "    client_max_body_size 0;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }

    SECTION("Wrong input. The value is too high") {
        std::string sampleConfig(
            "server {"
            "    client_max_body_size 2147483648;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }

    SECTION("Wrong input. Without a value") {
        std::string sampleConfig(
            "server {"
            "    client_max_body_size ;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }

    SECTION("Wrong input. Without a semicolon") {
        std::string sampleConfig(
            "server {"
            "    server_name client_max_body_size\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }
}
// error_page ::= "error_page" ws status ws+ path ;
TEST_CASE("Check error_page directive", "[Parser]") {
    SECTION("Correct input. One directive") {
        std::string sampleConfig(
            "server {"
            "    error_page 400 error400.html;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].errorPages[400] == "error400.html");
    }

    SECTION("Correct input. Two directives") {
        std::string sampleConfig(
            "server {"
            "    error_page 400 /errors/400.html;\n"
            "    error_page 501 /errors/501.html;"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].errorPages[400] == "/errors/400.html");
        REQUIRE(configs[0].errorPages[501] == "/errors/501.html");
    }

    SECTION("Wrong input. Status value is too low") {
        std::string sampleConfig(
            "server {"
            "    error_page 99 error99.html;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }

    SECTION("Wrong input. Status value is too high") {
        std::string sampleConfig(
            "server {"
            "    error_page 600 error600.html;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }

    SECTION("Wrong input. Without a semicolon") {
        std::string sampleConfig(
            "server {"
            "    error_page 200 error200.html\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }

    SECTION("Wrong input. Without status") {
        std::string sampleConfig(
            "server {"
            "    error_page error200.html;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }

    SECTION("Wrong input. Without path") {
        std::string sampleConfig(
            "server {"
            "    error_page 200;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }
}

// index ::= "index" ws filename ;
TEST_CASE("Check index directive", "[Parser]") {
    SECTION("Correct input") {
        std::string sampleConfig(
            "server {"
            "    index index.html;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].routes.size() == 1);
        REQUIRE(configs[0].routes[0].defaultFile == "index.html");
    }

    SECTION("Wrong input. Without a value") {
        std::string sampleConfig(
            "server {"
            "    index ;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }

    SECTION("Wrong input. Without a semicolon") {
        std::string sampleConfig(
            "server {"
            "    index index.html\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }
}

// limit_except ::= "limit_except" ws method (ws method)* ;
TEST_CASE("Check limit_except directive", "[Parser]") {
    SECTION("Correct input. Two methods") {
        std::string sampleConfig(
            "server {"
            "    limit_except GET POST;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].routes.size() == 1);
        const RouteConfig& route = configs[0].routes[0];
        REQUIRE(route.isMethodAllowed("GET"));
        REQUIRE(route.isMethodAllowed("POST"));
        REQUIRE(!route.isMethodAllowed("DELETE"));
    }

    SECTION("Correct input. All methods") {
        std::string sampleConfig(
            "server {"
            "    limit_except GET POST DELETE;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].routes.size() == 1);
        const RouteConfig& route = configs[0].routes[0];
        REQUIRE(route.isMethodAllowed("GET"));
        REQUIRE(route.isMethodAllowed("POST"));
        REQUIRE(route.isMethodAllowed("DELETE"));
    }

    SECTION("Wrong input. Invalid method") {
        std::string sampleConfig(
            "server {"
            "    limit_except PUT;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }

    SECTION("Wrong input. Without methods") {
        std::string sampleConfig(
            "server {"
            "    limit_except ;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }

    SECTION("Wrong input. Without a semicolon") {
        std::string sampleConfig(
            "server {"
            "    limit_except GET POST\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }
}

// autoindex ::= "autoindex" ws onoff ;
TEST_CASE("Check autoindex directive", "[Parser]") {
    SECTION("Correct input. on") {
        std::string sampleConfig(
            "server {"
            "    autoindex on;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].routes.size() == 1);
        REQUIRE(configs[0].routes[0].directoryListing == true);
    }

    SECTION("Correct input. off") {
        std::string sampleConfig(
            "server {"
            "    autoindex off;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].routes.size() == 1);
        REQUIRE(configs[0].routes[0].directoryListing == false);
    }

    SECTION("Wrong input. Invalid value") {
        std::string sampleConfig(
            "server {"
            "    autoindex maybe;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }

    SECTION("Wrong input. Without a value") {
        std::string sampleConfig(
            "server {"
            "    autoindex ;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }

    SECTION("Wrong input. Without a semicolon") {
        std::string sampleConfig(
            "server {"
            "    autoindex on\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }
}

// auth_required ::= "auth_required" ws onoff ;
TEST_CASE("Check auth_required directive", "[Parser]") {
    SECTION("Correct input. on") {
        std::string sampleConfig(
            "server {"
            "    auth_required on;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].routes.size() == 1);
        REQUIRE(configs[0].routes[0].authRequired == true);
    }

    SECTION("Correct input. off") {
        std::string sampleConfig(
            "server {"
            "    auth_required off;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].routes.size() == 1);
        REQUIRE(configs[0].routes[0].authRequired == false);
    }

    SECTION("Wrong input. Invalid value") {
        std::string sampleConfig(
            "server {"
            "    auth_required yes;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }

    SECTION("Wrong input. Without a value") {
        std::string sampleConfig(
            "server {"
            "    auth_required ;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }

    SECTION("Wrong input. Without a semicolon") {
        std::string sampleConfig(
            "server {"
            "    auth_required on\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }
}

// return ::= "return" ws status ws+ uri ;
TEST_CASE("Check return directive", "[Parser]") {
    SECTION("Correct input") {
        std::string sampleConfig(
            "server {"
            "    return 301 /redirected_here;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].routes.size() == 1);
        const RouteConfig& route = configs[0].routes[0];
        REQUIRE(route.hasReturn == true);
        REQUIRE(route.returnStatus == 301);
        REQUIRE(route.returnUri == "/redirected_here");
    }

    SECTION("Wrong input. Status value is too low") {
        std::string sampleConfig(
            "server {"
            "    return 99 /redirect;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }

    SECTION("Wrong input. Status value is too high") {
        std::string sampleConfig(
            "server {"
            "    return 600 /redirect;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }

    SECTION("Wrong input. Without uri") {
        std::string sampleConfig(
            "server {"
            "    return 301;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }

    SECTION("Wrong input. Without a semicolon") {
        std::string sampleConfig(
            "server {"
            "    return 301 /redirect\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }
}

// upload_store ::= "upload_store" ws path ;
TEST_CASE("Check upload_store directive", "[Parser]") {
    SECTION("Correct input") {
        std::string sampleConfig(
            "server {"
            "    upload_store /uploads;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].routes.size() == 1);
        const RouteConfig& route = configs[0].routes[0];
        REQUIRE(route.uploadDirectory == "/uploads");
    }

    SECTION("Wrong input. Without a value") {
        std::string sampleConfig(
            "server {"
            "    upload_store ;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }

    SECTION("Wrong input. Without a semicolon") {
        std::string sampleConfig(
            "server {"
            "    upload_store /uploads\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }
}

// cgi_ext ::= "cgi_ext" ws ext ;
TEST_CASE("Check cgi_ext directive", "[Parser]") {
    SECTION("Correct input") {
        std::string sampleConfig(
            "server {"
            "    cgi_ext .py;\n"
            "    cgi_path /usr/bin/python3;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].routes.size() == 1);
        const RouteConfig& route = configs[0].routes[0];
        std::map<std::string, std::string>::const_iterator it = route.cgiHandlers.find(".py");
        REQUIRE(it != route.cgiHandlers.end());
        REQUIRE(it->second == "/usr/bin/python3");
    }

    SECTION("Wrong input. Without cgi_path") {
        std::string sampleConfig(
            "server {"
            "    cgi_ext .py;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }

    SECTION("Wrong input. Without a semicolon") {
        std::string sampleConfig(
            "server {"
            "    cgi_ext .py\n"
            "    cgi_path /usr/bin/python3;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }
}

// cgi_path ::= "cgi_path" ws path ;
TEST_CASE("Check cgi_path directive", "[Parser]") {
    SECTION("Correct input") {
        std::string sampleConfig(
            "server {"
            "    cgi_ext .pl;\n"
            "    cgi_path /usr/bin/perl;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].routes.size() == 1);
        const RouteConfig& route = configs[0].routes[0];
        std::map<std::string, std::string>::const_iterator it = route.cgiHandlers.find(".pl");
        REQUIRE(it != route.cgiHandlers.end());
        REQUIRE(it->second == "/usr/bin/perl");
    }

    SECTION("Wrong input. Without cgi_ext") {
        std::string sampleConfig(
            "server {"
            "    cgi_path /usr/bin/python3;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }

    SECTION("Wrong input. Without a value") {
        std::string sampleConfig(
            "server {"
            "    cgi_ext .py;\n"
            "    cgi_path ;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }

    SECTION("Wrong input. Without a semicolon") {
        std::string sampleConfig(
            "server {"
            "    cgi_ext .py;\n"
            "    cgi_path /usr/bin/python3\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }
}

// method ::= "GET" | "POST" | "DELETE" ;
TEST_CASE("Check method rule", "[Parser]") {
    SECTION("Correct input. All methods") {
        std::string sampleConfig(
            "server {"
            "    limit_except GET POST DELETE;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].routes.size() == 1);
        const RouteConfig& route = configs[0].routes[0];
        REQUIRE(route.isMethodAllowed("GET"));
        REQUIRE(route.isMethodAllowed("POST"));
        REQUIRE(route.isMethodAllowed("DELETE"));
    }

    SECTION("Wrong input. Lowercase method") {
        std::string sampleConfig(
            "server {"
            "    limit_except get;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }

    SECTION("Wrong input. Unknown method") {
        std::string sampleConfig(
            "server {"
            "    limit_except PATCH;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }
}

// onoff ::= "on" | "off" ;
TEST_CASE("Check onoff rule", "[Parser]") {
    SECTION("Correct input. on") {
        std::string sampleConfig(
            "server {"
            "    autoindex on;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].routes.size() == 1);
        REQUIRE(configs[0].routes[0].directoryListing == true);
    }

    SECTION("Correct input. off") {
        std::string sampleConfig(
            "server {"
            "    auth_required off;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].routes.size() == 1);
        REQUIRE(configs[0].routes[0].authRequired == false);
    }

    SECTION("Wrong input. Invalid value") {
        std::string sampleConfig(
            "server {"
            "    autoindex maybe;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }
}

// status ::= digit digit digit ;
TEST_CASE("Check status rule", "[Parser]") {
    SECTION("Correct input. Lowest status") {
        std::string sampleConfig(
            "server {"
            "    return 100 /ok;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].routes.size() == 1);
        const RouteConfig& route = configs[0].routes[0];
        REQUIRE(route.returnStatus == 100);
    }

    SECTION("Wrong input. Non-digit status") {
        std::string sampleConfig(
            "server {"
            "    return 2a0 /ok;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }
}

// port ::= digit+ ;
TEST_CASE("Check port rule", "[Parser]") {
    SECTION("Correct input. Digits only") {
        std::string sampleConfig(
            "server {"
            "    listen 8080;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].listen[0].second == 8080);
    }

    SECTION("Wrong input. Non-digit port") {
        std::string sampleConfig(
            "server {"
            "    listen 80a;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }
}

// number ::= digit+ ;
TEST_CASE("Check number rule", "[Parser]") {
    SECTION("Correct input") {
        std::string sampleConfig(
            "server {"
            "    client_max_body_size 1024;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].clientMaxBodySize == 1024);
    }

    SECTION("Wrong input. Non-digit value") {
        std::string sampleConfig(
            "server {"
            "    client_max_body_size 10kb;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }
}

// name ::= token ;
TEST_CASE("Check name rule", "[Parser]") {
    SECTION("Correct input. Dots and dashes") {
        std::string sampleConfig(
            "server {"
            "    server_name example-site.local;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].serverName == "example-site.local");
    }

    SECTION("Correct input. Colon in token") {
        std::string sampleConfig(
            "server {"
            "    server_name local:8080;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].serverName == "local:8080");
    }

    SECTION("Wrong input. Whitespace in name") {
        std::string sampleConfig(
            "server {"
            "    server_name example .com;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }
}

// path ::= token ;
TEST_CASE("Check path rule", "[Parser]") {
    SECTION("Correct input. Absolute path") {
        std::string sampleConfig(
            "server {"
            "    root /var/www/site;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
    }

    SECTION("Correct input. Relative path") {
        std::string sampleConfig(
            "server {"
            "    root ./www;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
    }

    SECTION("Wrong input. Path with space") {
        std::string sampleConfig(
            "server {"
            "    root /var/www site;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }
}

// uri ::= token ;
TEST_CASE("Check uri rule", "[Parser]") {
    SECTION("Correct input. Query string") {
        std::string sampleConfig(
            "server {"
            "    return 302 /redirect?x=1&y=2;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].routes.size() == 1);
        const RouteConfig& route = configs[0].routes[0];
        REQUIRE(route.returnUri == "/redirect?x=1&y=2");
    }

    SECTION("Correct input. Full URL") {
        std::string sampleConfig(
            "server {"
            "    return 301 http://example.com/path;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].routes.size() == 1);
        const RouteConfig& route = configs[0].routes[0];
        REQUIRE(route.returnUri == "http://example.com/path");
    }

    SECTION("Wrong input. Missing uri") {
        std::string sampleConfig(
            "server {"
            "    return 302;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }
}

// filename ::= token ;
TEST_CASE("Check filename rule", "[Parser]") {
    SECTION("Correct input. With extension") {
        std::string sampleConfig(
            "server {"
            "    index index.html;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].routes.size() == 1);
        REQUIRE(configs[0].routes[0].defaultFile == "index.html");
    }

    SECTION("Correct input. With dash") {
        std::string sampleConfig(
            "server {"
            "    index default-file.htm;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].routes.size() == 1);
        REQUIRE(configs[0].routes[0].defaultFile == "default-file.htm");
    }

    SECTION("Wrong input. Filename with space") {
        std::string sampleConfig(
            "server {"
            "    index index file.html;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }
}

// ext ::= token ;
TEST_CASE("Check ext rule", "[Parser]") {
    SECTION("Correct input. With dot") {
        std::string sampleConfig(
            "server {"
            "    cgi_ext .py;\n"
            "    cgi_path /usr/bin/python3;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].routes.size() == 1);
        const RouteConfig& route = configs[0].routes[0];
        REQUIRE(route.cgiHandlers.find(".py") != route.cgiHandlers.end());
    }

    SECTION("Correct input. Without dot") {
        std::string sampleConfig(
            "server {"
            "    cgi_ext py;\n"
            "    cgi_path /usr/bin/python3;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].routes.size() == 1);
        const RouteConfig& route = configs[0].routes[0];
        REQUIRE(route.cgiHandlers.find("py") != route.cgiHandlers.end());
    }
}

// token ::= (unquoted_char)+ ;
TEST_CASE("Check token rule", "[Parser]") {
    SECTION("Correct input. Punctuation allowed") {
        std::string sampleConfig(
            "server {"
            "    server_name name.with-dash_and:port;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].serverName == "name.with-dash_and:port");
    }
}

// unquoted_char ::= ? any non-whitespace, non-';', non-'{' , non-'}' ? ;
TEST_CASE("Check unquoted_char rule", "[Parser]") {
    SECTION("Correct input. Slash and dot allowed") {
        std::string sampleConfig(
            "server {"
            "    root /var/www.site;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
    }

    SECTION("Wrong input. Brace in token") {
        std::string sampleConfig(
            "server {"
            "    server_name name{bad};\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }
}

// digit ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" ;
TEST_CASE("Check digit rule", "[Parser]") {
    SECTION("Correct input. Digits only") {
        std::string sampleConfig(
            "server {"
            "    client_max_body_size 123;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
        REQUIRE(configs[0].clientMaxBodySize == 123);
    }

    SECTION("Wrong input. Contains letter") {
        std::string sampleConfig(
            "server {"
            "    client_max_body_size 12a;\n"
            "}\n");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }
}

// ws ::= (space | tab | newline | comment)* ;
TEST_CASE("Check ws rule", "[Parser]") {
    SECTION("Correct input. Mixed whitespace") {
        std::string sampleConfig(
            "\n\t  server\t{\n"
            "\tlisten\t8080;\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
    }
}

// comment ::= "#" ? any char except newline ?* newline ;
TEST_CASE("Check comment rule", "[Parser]") {
    SECTION("Correct input. Full line and inline comments") {
        std::string sampleConfig(
            "server {\n"
            "    # full line comment\n"
            "    listen 8080; # inline comment\n"
            "}\n");
        std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE(configs.size() == 1);
    }
}

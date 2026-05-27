/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tests_parser_rules.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 16:46:18 by nefimov           #+#    #+#             */
/*   Updated: 2026/05/27 11:55:12 by nefimov          ###   ########.fr       */
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
        // std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }

    SECTION("Wrong input. Without a semicolon") {
        std::string sampleConfig(
            "server {"
            "    server_name webserv\n"
            "}\n");
        // std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
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
        // REQUIRE(configs[0].ro == "webserv");
    }

    SECTION("Wrong input. Without a value") {
        std::string sampleConfig(
            "server {"
            "    server_name ;\n"
            "}\n");
        // std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }

    SECTION("Wrong input. Without a semicolon") {
        std::string sampleConfig(
            "server {"
            "    server_name webserv\n"
            "}\n");
        // std::vector<ServerConfig> configs = Parser::parseString(sampleConfig, "sample");
        REQUIRE_THROWS(Parser::parseString(sampleConfig, "sample"));
    }
}

// client_max_body_size ::= "client_max_body_size" ws number ;
TEST_CASE("Check client_max_body_size directive", "[Parser]") {}

// error_page ::= "error_page" ws status ws+ path ;
TEST_CASE("Check error_page directive", "[Parser]") {}

// index ::= "index" ws filename ;
TEST_CASE("Check index directive", "[Parser]") {}

// limit_except ::= "limit_except" ws method (ws method)* ;
TEST_CASE("Check limit_except directive", "[Parser]") {}

// autoindex ::= "autoindex" ws onoff ;
TEST_CASE("Check autoindex directive", "[Parser]") {}

// auth_required ::= "auth_required" ws onoff ;
TEST_CASE("Check auth_required directive", "[Parser]") {}

// return ::= "return" ws status ws+ uri ;
TEST_CASE("Check return directive", "[Parser]") {}

// cgi_ext ::= "cgi_ext" ws ext ;
TEST_CASE("Check cgi_ext directive", "[Parser]") {}

// cgi_path ::= "cgi_path" ws path ;
TEST_CASE("Check cgi_path directive", "[Parser]") {}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tests_parserImpl.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 16:46:18 by nefimov           #+#    #+#             */
/*   Updated: 2026/06/11 18:43:44 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"
#include "catch.hpp"
#include "parser/ParserImpl.hpp"

TEST_CASE("Parse listen directive. Correct config, one listener", "[ParserImpl]") {
    std::string sampleConfig(
        "server {"
        "    listen 127.0.0.1:8080;\n"
        "}\n");

    ParserImpl parser(sampleConfig, "test");
    std::vector<ServerConfig> configs = parser.parseConfig();
    REQUIRE(configs.size() == 1);
    REQUIRE(configs[0].listen.size() == 1);
    REQUIRE(configs[0].listen[0].first == "127.0.0.1");
    REQUIRE(configs[0].listen[0].second == 8080);
    LOG_DEBUG(configs[0].listen[0].first + ":" + toString(configs[0].listen[0].second));
}

TEST_CASE("Parse listen directive. Correct config, one listener all zerows", "[ParserImpl]") {
    std::string sampleConfig(
        "server {"
        "    listen 0.0.0.0:8080;\n"
        "}\n");

    ParserImpl parser(sampleConfig, "test");
    std::vector<ServerConfig> configs = parser.parseConfig();
    REQUIRE(configs.size() == 1);
    REQUIRE(configs[0].listen.size() == 1);
    REQUIRE(configs[0].listen[0].first == "0.0.0.0");
    REQUIRE(configs[0].listen[0].second == 8080);
    LOG_DEBUG(configs[0].listen[0].first + ":" + toString(configs[0].listen[0].second));
}

TEST_CASE("Parse listen directive. Correct config, one listener all 255", "[ParserImpl]") {
    std::string sampleConfig(
        "server {"
        "    listen 255.255.255.255:8080;\n"
        "}\n");

    ParserImpl parser(sampleConfig, "test");
    std::vector<ServerConfig> configs = parser.parseConfig();
    REQUIRE(configs.size() == 1);
    REQUIRE(configs[0].listen.size() == 1);
    REQUIRE(configs[0].listen[0].first == "255.255.255.255");
    REQUIRE(configs[0].listen[0].second == 8080);
    LOG_DEBUG(configs[0].listen[0].first + ":" + toString(configs[0].listen[0].second));
}

TEST_CASE("Parse listen directive. Correct config, localhost", "[ParserImpl]") {
    std::string sampleConfig(
        "server {"
        "    listen localhost:8080;\n"
        "}\n");

    ParserImpl parser(sampleConfig, "test");
    std::vector<ServerConfig> configs = parser.parseConfig();
    REQUIRE(configs.size() == 1);
    REQUIRE(configs[0].listen.size() == 1);
    REQUIRE(configs[0].listen[0].first == "127.0.0.1");
    REQUIRE(configs[0].listen[0].second == 8080);
    LOG_DEBUG(configs[0].listen[0].first + ":" + toString(configs[0].listen[0].second));
}

TEST_CASE("Parse listen directive. Wrong config, other word", "[ParserImpl]") {
    std::string sampleConfig(
        "server {"
        "    listen host:8080;\n"
        "}\n");

    ParserImpl parser(sampleConfig, "test");
    REQUIRE_THROWS(parser.parseConfig());
}

TEST_CASE("Parse listen directive. Correct config, two listeners", "[ParserImpl]") {
    std::string sampleConfig(
        "server {"
        "    listen 127.0.0.1:8080;\n"
        "    listen 126.0.0.1:8080;\n"
        "}\n");

    ParserImpl parser(sampleConfig, "test");
    std::vector<ServerConfig> configs = parser.parseConfig();
    REQUIRE(configs.size() == 1);
    REQUIRE(configs[0].listen.size() == 2);
    REQUIRE(configs[0].listen[0].first == "127.0.0.1");
    REQUIRE(configs[0].listen[0].second == 8080);
    REQUIRE(configs[0].listen[1].first == "126.0.0.1");
    REQUIRE(configs[0].listen[1].second == 8080);
    LOG_DEBUG(configs[0].listen[0].first + ":" + toString(configs[0].listen[0].second));
    LOG_DEBUG(configs[0].listen[1].first + ":" + toString(configs[0].listen[1].second));
}

TEST_CASE("Parse listen directive. Wrong config, negative value", "[ParserImpl]") {
    std::string sampleConfig(
        "server {"
        "    listen -127.0.0.1:8080;\n"
        "}\n");

    ParserImpl parser(sampleConfig, "test");
    REQUIRE_THROWS(parser.parseConfig());
}

TEST_CASE("Parse listen directive. Wrong config, value too high", "[ParserImpl]") {
    std::string sampleConfig(
        "server {"
        "    listen 256.0.0.1:8080;\n"
        "}\n");

    ParserImpl parser(sampleConfig, "test");
    REQUIRE_THROWS(parser.parseConfig());
}

TEST_CASE("Parse listen directive. Wrong config, only two dots", "[ParserImpl]") {
    std::string sampleConfig(
        "server {"
        "    listen 127.0.01:8080;\n"
        "}\n");

    ParserImpl parser(sampleConfig, "test");
    REQUIRE_THROWS(parser.parseConfig());
}

TEST_CASE("Parse listen directive. Wrong config, duplicate addresses in one server block",
          "[ParserImpl]") {
    std::string sampleConfig(
        "server {"
        "    listen 127.0.0.1:8080;\n"
        "    listen 127.0.0.1:8080;\n"
        "}\n");

    ParserImpl parser(sampleConfig, "test");
    REQUIRE_THROWS(parser.parseConfig());
}

TEST_CASE("Parse listen directive. Wrong config, duplicate localhost addresses in one server block",
          "[ParserImpl]") {
    std::string sampleConfig(
        "server {"
        "    listen localhost:8080;\n"
        "    listen 127.0.0.1:8080;\n"
        "}\n");

    ParserImpl parser(sampleConfig, "test");
    REQUIRE_THROWS(parser.parseConfig());
}

TEST_CASE(
    "Parse listen directive. Wrong config, duplicate addresses in two different server blocks",
    "[ParserImpl]") {
    std::string sampleConfig(
        "server {"
        "    listen 127.0.0.1:8080;\n"
        "}\n"
        "server {"
        "    listen 127.0.0.1:8080;\n"
        "}\n");

    ParserImpl parser(sampleConfig, "test");
    REQUIRE_THROWS(parser.parseConfig());
}

TEST_CASE("Parse listen directive. Wrong config, duplicate port in one server block",
          "[ParserImpl]") {
    std::string sampleConfig(
        "server {"
        "    listen 8080;\n"
        "    listen 127.0.0.1:8080;\n"
        "}\n");

    ParserImpl parser(sampleConfig, "test");
    REQUIRE_THROWS(parser.parseConfig());
}

TEST_CASE("Parse listen directive. Wrong config, duplicate port in two different server blocks",
          "[ParserImpl]") {
    std::string sampleConfig(
        "server {"
        "    listen 127.0.0.1:8080;\n"
        "}\n"
        "server {"
        "    listen 8080;\n"
        "}\n");

    ParserImpl parser(sampleConfig, "test");
    REQUIRE_THROWS(parser.parseConfig());
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tests_parser_read_file.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 16:46:18 by nefimov           #+#    #+#             */
/*   Updated: 2026/05/22 19:20:06 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/stat.h>

#include <cstdio>
#include <fstream>

#include "catch.hpp"
#include "parser/Parser.hpp"

TEST_CASE("Parser parses correct_basic.conf config file", "[Parser]") {
    std::vector<ServerConfig> configs =
        Parser::parseFile("unit_tests/test_configs/correct_basic.conf");
    REQUIRE(configs.size() == 1);
}

TEST_CASE("Parser parses file_not_exist.conf config file", "[Parser]") {
    REQUIRE_THROWS(Parser::parseFile("file_not_exist.conf"));
}

TEST_CASE("Parser parses .conf file with wrong access permissions", "[Parser]") {
    const std::string path = "unit_tests/test_configs/tmp_unreadable.conf";

    {
        std::ofstream out(path.c_str());
        out << "server { listen 8080; }";
    }

    if (chmod(path.c_str(), 0) != 0) {
        perror("chmod");
    }

    REQUIRE_THROWS(Parser::parseFile(path));

    chmod(path.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    std::remove(path.c_str());
}

TEST_CASE("Parser parses directory.conf directory", "[Parser]") {
    REQUIRE_THROWS(Parser::parseFile("unit_tests/test_configs/directory.conf"));
}

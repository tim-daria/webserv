/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 16:45:56 by nefimov           #+#    #+#             */
/*   Updated: 2026/06/01 16:38:30 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser/Parser.hpp"

#include <sys/stat.h>

#include <fstream>
#include <sstream>
#include <stdexcept>

#include "Logger.hpp"
#include "parser/ParserImpl.hpp"

std::vector<ServerConfig> Parser::parseFile(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
        LOG_ERROR("Config path is a directory: " + path);
        throw std::runtime_error("Config path is a directory: " + path);
    }

    std::ifstream input(path.c_str());
    if (!input) {
        LOG_ERROR("Failed to open config file: " + path);
        throw std::runtime_error("Failed to open config file: " + path);
    }
    std::stringstream buffer;
    buffer << input.rdbuf();
    ParserImpl parser(buffer.str(), path);
    LOG_INFO("Parsing file " + path);
    return parser.parseConfig();
}

std::vector<ServerConfig> Parser::parseString(const std::string& content,
                                              const std::string& sourceName) {
    ParserImpl parser(content, sourceName);
    return parser.parseConfig();
}

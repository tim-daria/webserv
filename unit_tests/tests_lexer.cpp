/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tests_lexer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 16:46:18 by nefimov           #+#    #+#             */
/*   Updated: 2026/05/22 21:38:57 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <vector>

#include "catch.hpp"
#include "parser/Lexer.hpp"

TEST_CASE("Lexer get an empty input", "[Lexer]") {
    Lexer lexer("");
    Token token = lexer.nextToken();
    REQUIRE(token.type == TOKEN_END);
}

TEST_CASE("Lexer input with all kinds of tokens", "[Lexer]") {
    std::string content("server{};");
    Lexer lexer(content);
    std::vector<Token> tokens;
    while (true) {
        Token current_token = lexer.nextToken();
        tokens.push_back(current_token);
        if (current_token.type == TOKEN_END) break;
    }
    REQUIRE(tokens[0].type == TOKEN_WORD);
    REQUIRE(tokens[1].type == TOKEN_LBRACE);
    REQUIRE(tokens[2].type == TOKEN_RBRACE);
    REQUIRE(tokens[3].type == TOKEN_SEMICOLON);
    REQUIRE(tokens[4].type == TOKEN_END);
}

TEST_CASE("Lexer input with all kinds of tokens with a comment", "[Lexer]") {
    std::string content("server{};#comment");
    Lexer lexer(content);
    std::vector<Token> tokens;
    while (true) {
        Token current_token = lexer.nextToken();
        tokens.push_back(current_token);
        if (current_token.type == TOKEN_END) break;
    }
    REQUIRE(tokens[0].type == TOKEN_WORD);
    REQUIRE(tokens[1].type == TOKEN_LBRACE);
    REQUIRE(tokens[2].type == TOKEN_RBRACE);
    REQUIRE(tokens[3].type == TOKEN_SEMICOLON);
    REQUIRE(tokens[4].type == TOKEN_END);
}

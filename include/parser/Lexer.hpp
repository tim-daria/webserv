/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 16:44:55 by nefimov           #+#    #+#             */
/*   Updated: 2026/06/03 11:34:13 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

#include "Logger.hpp"
#include "parser/Token.hpp"

class Lexer {
   public:
    Lexer(const std::string& content);
    Token nextToken();

   private:
    const std::string& _content;
    std::string::size_type _pos;
    int _line;
    int _col;

    Token makeToken(TokenType type, const std::string& text);
    Token makeTokenWithPosition(TokenType type, const std::string& text, int line, int col);
    void advance();
    void skipWhitespaceAndComments();
    Token readWord();
};

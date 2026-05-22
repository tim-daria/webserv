/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 16:45:39 by nefimov           #+#    #+#             */
/*   Updated: 2026/05/22 16:45:40 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser/Lexer.hpp"

#include <cctype>

Lexer::Lexer(const std::string& content) : _content(content), _pos(0), _line(1), _col(1) {}

Token Lexer::nextToken() {
    skipWhitespaceAndComments();
    if (_pos >= _content.size()) return makeToken(TOKEN_END, "");

    char c = _content[_pos];
    if (c == '{') {
        int line = _line;
        int col = _col;
        advance();
        return makeTokenWithPosition(TOKEN_LBRACE, "{", line, col);
    }
    if (c == '}') {
        int line = _line;
        int col = _col;
        advance();
        return makeTokenWithPosition(TOKEN_RBRACE, "}", line, col);
    }
    if (c == ';') {
        int line = _line;
        int col = _col;
        advance();
        return makeTokenWithPosition(TOKEN_SEMICOLON, ";", line, col);
    }

    return readWord();
}

Token Lexer::makeToken(TokenType type, const std::string& text) {
    return makeTokenWithPosition(type, text, _line, _col);
}

Token Lexer::makeTokenWithPosition(TokenType type, const std::string& text, int line, int col) {
    Token token;
    token.type = type;
    token.text = text;
    token.line = line;
    token.column = col;
    LOG_DEBUG("Token \"" + toString(token.type) + " \'" + token.text + "\'\" created");
    return token;
}

void Lexer::advance() {
    if (_content[_pos] == '\n') {
        _line++;
        _col = 1;
    } else {
        _col++;
    }
    _pos++;
}

void Lexer::skipWhitespaceAndComments() {
    while (_pos < _content.size()) {
        char c = _content[_pos];
        if (c == '#') {
            while (_pos < _content.size() && _content[_pos] != '\n') advance();
            continue;
        }
        if (std::isspace(static_cast<unsigned char>(c))) {
            advance();
            continue;
        }
        break;
    }
}

Token Lexer::readWord() {
    int startLine = _line;
    int startCol = _col;
    std::string text;
    while (_pos < _content.size()) {
        char c = _content[_pos];
        if (c == '{' || c == '}' || c == ';' || c == '#' ||
            std::isspace(static_cast<unsigned char>(c))) {
            break;
        }
        text.push_back(c);
        advance();
    }

    Token token;
    token.type = TOKEN_WORD;
    token.text = text;
    token.line = startLine;
    token.column = startCol;
    LOG_DEBUG("Token \"" + toString(token.type) + " \'" + token.text + "\'\" created");
    return token;
}

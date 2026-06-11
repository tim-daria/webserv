/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserImpl.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 16:45:23 by nefimov           #+#    #+#             */
/*   Updated: 2026/06/11 18:57:37 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>

#include "config/ServerConfig.hpp"
#include "parser/Lexer.hpp"
#include "parser/ParsedRoute.hpp"
#include "parser/Token.hpp"

class ParserImpl {
   public:
    ParserImpl(const std::string& content, const std::string& sourceName);
    std::vector<ServerConfig> parseConfig();

   private:
    std::string _sourceName;
    std::vector<Token> _tokens;
    std::string::size_type _pos;
    std::vector<std::pair<std::string, int> > _addresses;

    const Token& peek() const;
    const Token& consume();
    bool isEnd() const;
    void expectType(TokenType type, const std::string& message);
    Token expectWordToken(const std::string& message);
    std::string expectWord(const std::string& message);
    void expectKeyword(const std::string& keyword);
    void throwError(const Token& token, const std::string& message);

    static bool isNumber(const std::string& text);
    static int parseInt(const std::string& text, int minValue, int maxValue);
    static bool isValidMethod(const std::string& method);
    static bool isOnOff(const std::string& value);
    static bool isIpAddress(const std::string& value);

    ServerConfig parseServerBlock();
    ParsedRoute parseLocationBlock(std::string& pendingCgiExt);
    void applyServerDefaults(ParsedRoute& parsed, const RouteConfig& serverDefaults);
    void parseServerDirective(ServerConfig& cfg, RouteConfig& serverDefaults,
                              bool& hasExplicitListen, bool& hasExplicitErrorPages,
                              std::string& pendingCgiExt);
    void parseLocationDirective(ParsedRoute& parsed, std::string& pendingCgiExt);
    void checkAddress(std::pair<std::string, int> new_addr, const Token& valueToken);
};

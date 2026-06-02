/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Token.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 16:45:30 by nefimov           #+#    #+#             */
/*   Updated: 2026/05/22 16:45:31 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

enum TokenType { TOKEN_WORD, TOKEN_LBRACE, TOKEN_RBRACE, TOKEN_SEMICOLON, TOKEN_END };

struct Token {
    TokenType type;
    std::string text;
    int line;
    int column;
};

#endif

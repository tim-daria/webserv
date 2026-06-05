/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 16:45:15 by nefimov           #+#    #+#             */
/*   Updated: 2026/06/03 11:34:31 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>

#include "config/ServerConfig.hpp"

class Parser {
   public:
    static std::vector<ServerConfig> parseFile(const std::string& path);
    static std::vector<ServerConfig> parseString(const std::string& content,
                                                 const std::string& sourceName);
};

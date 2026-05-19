/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 14:34:40 by tsemenov          #+#    #+#             */
/*   Updated: 2026/05/19 23:17:07 by tsemenov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

std::string toLowerCase(std::string str) {
    for (size_t i = 0; i < str.length(); ++i) str[i] = std::tolower((unsigned char)str[i]);
    return str;
}

std::string stripSpaces(const std::string& str) {
    size_t start = str.find_first_not_of(" \t");
    if (start == std::string::npos) {
        return "";
    }

    size_t end = str.find_last_not_of(" \t");
    return str.substr(start, end - start + 1);
}

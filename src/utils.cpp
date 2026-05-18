/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 14:34:40 by tsemenov          #+#    #+#             */
/*   Updated: 2026/05/14 22:51:59 by tsemenov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

std::string toLowerCase(std::string str) {
    for (size_t i = 0; i < str.length(); ++i) str[i] = std::tolower((unsigned char)str[i]);
    return str;
}

std::string stripSpaces(const std::string& str) {
	size_t start = str.find_first_not_of(" \t");
	if (start == std::string::npos) { return ""; }

	size_t end = str.find_last_not_of(" \t");
	return s.substr(start, end - start + 1);

}
// std::map<int, std::string> initStatusMessages() {
//     std::map<int, std::string> msg;
//     msg[200] = "OK";
//     msg[201] = "Created";
//     msg[400] = "Bad Request";
// 		msg[401] = "Unauthorised";
//     msg[403] = "Forbidden";
//     msg[404] = "Not Found";
//     msg[405] = "Method Not Allowed";
//     msg[411] = "Length Required";
//     msg[413] = "Content Too Large";
//     msg[500] = "Internal Server Error";
//     msg[501] = "Not Implemented";
//     msg[503] = "Service Unavailable";
//     return msg;
// }

// const std::map<int, std::string> STATUS_MESSAGES = initStatusMessages();
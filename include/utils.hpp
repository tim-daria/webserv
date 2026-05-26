/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 14:35:11 by tsemenov          #+#    #+#             */
/*   Updated: 2026/05/14 22:52:08 by tsemenov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <string>

std::string toLowerCase(std::string str);
std::string stripSpaces(const std::string& str);

std::map<int, std::string> initStatusMessages();
extern const std::map<int, std::string> STATUS_MESSAGES;

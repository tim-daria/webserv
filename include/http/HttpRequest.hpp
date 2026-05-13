/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HtppRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtimofee <dtimofee@student.42berlin.de>    #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-03-20 18:22:51 by dtimofee          #+#    #+#             */
/*   Updated: 2026-03-20 18:22:51 by dtimofee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <cstring>
#include <map>
#include <string>

class HttpRequest {
   public:
    std::string method;
    std::string path;
    std::map<std::string, std::string> headers;
    std::string body;
};

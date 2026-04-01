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

#include <cstring>
#include <map>

class HttpRequest {
    std::string method;
    std::string path;
    std::map<std::string, std::string> headers;
    std::string body;
};

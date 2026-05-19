/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestValidator.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/17 00:03:24 by tsemenov          #+#    #+#             */
/*   Updated: 2026/05/20 11:28:30 by tsemenov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

class HttpRequest;
class ServerConfig;

class RequestValidator {
   private:
    static int _checkRouteAndMethod(const HttpRequest& request, const ServerConfig& config);
    static void _logResult(const HttpRequest& request, int code);

   public:
    static int validate(const HttpRequest& request, const ServerConfig& config);
};

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestValidator.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/18 14:23:40 by tsemenov          #+#    #+#             */
/*   Updated: 2026/05/20 11:28:21 by tsemenov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestValidator.hpp"

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Logger.hpp"
#include "RouteConfig.hpp"
#include "ServerConfig.hpp"

// Finds the matching route for the request path, then checks the method against it.
// Single lookup covers both checks — 404 if no route matches, 405 if method not allowed:
void RequestValidator::_logResult(const HttpRequest& request, int code) {
    if (code == 0)
        LOG_DEBUG("Validator: " + request.getMethod() + " " + request.getPath() + " -> OK");
    else if (code == HTTP_NOT_FOUND)
        LOG_WARNING("Validator: no route for " + request.getPath());
    else if (code == HTTP_METHOD_NOT_ALLOWED)
        LOG_WARNING("Validator: " + request.getMethod() + " not allowed on " + request.getPath());
}

int RequestValidator::_checkRouteAndMethod(const HttpRequest& request, const ServerConfig& config) {
    const RouteConfig* route = config.findMatchingLocation(request.getPath());
    if (!route) return HTTP_NOT_FOUND;
    if (!route->isMethodAllowed(request.getMethod())) return HTTP_METHOD_NOT_ALLOWED;
    return 0;
}

// Entry point called after HttpRequest parsing succeeds.
// Returns the first error code encountered, or 0 if the request is valid:
int RequestValidator::validate(const HttpRequest& request, const ServerConfig& config) {
    int code = _checkRouteAndMethod(request, config);
    _logResult(request, code);
    return code;
}

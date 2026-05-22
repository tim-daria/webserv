/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsedRoute.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 16:45:04 by nefimov           #+#    #+#             */
/*   Updated: 2026/05/22 16:45:09 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSEDROUTE_HPP
#define PARSEDROUTE_HPP

#include "config/RouteConfig.hpp"

struct ParsedRoute {
    RouteConfig route;
    bool hasRoot;
    bool hasIndex;
    bool hasMethods;
    bool hasAutoindex;
    bool hasAuth;
    bool hasReturn;
    bool hasCgi;
    bool hasUpload;
};

#endif

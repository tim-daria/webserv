/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsedRoute.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 16:45:04 by nefimov           #+#    #+#             */
/*   Updated: 2026/06/03 11:34:23 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

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

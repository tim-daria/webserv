/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtimofee <dtimofee@student.42berlin.de>    #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-03-26 13:30:10 by dtimofee          #+#    #+#             */
/*   Updated: 2026-03-26 13:30:10 by dtimofee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "RouteConfig.hpp"

class Handler {
   private:
    RouteConfig config;

    Handler();
    Handler(const Handler& other);
    Handler& operator=(const Handler& other);

    HttpResponse get_default_response(HttpRequest request);
    HttpResponse handleGet(HttpRequest request);
    HttpResponse handlePost(HttpRequest request);
    HttpResponse handleDelete(HttpRequest request);

   public:
    Handler(RouteConfig config);
    ~Handler();

    HttpResponse handle(HttpRequest request);
};

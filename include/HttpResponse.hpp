/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtimofee <dtimofee@student.42berlin.de>    #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-03-20 16:43:12 by dtimofee          #+#    #+#             */
/*   Updated: 2026-03-20 16:43:12 by dtimofee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstring>
#include <map>

class HttpResponse {
   private:
    int status_code_;
    std::map<std::string, std::string> headers_;
    std::string body_;

   public:
    HttpResponse();
    ~HttpResponse();
};

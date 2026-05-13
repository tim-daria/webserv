/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandler.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtimofee <dtimofee@student.42berlin.de>    #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-05-11 14:04:26 by dtimofee          #+#    #+#             */
/*   Updated: 2026-05-11 14:04:26 by dtimofee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "HttpResponse.hpp"

class ErrorHandler {
   private:
    std::string _error_pages_path;  // путь к кастомным страницам ошибок

    // std::string  getStatusText(int code) const;
    std::string defaultPage(int code) const;

   public:
    ErrorHandler(const std::string& error_pages_path = "");

    HttpResponse makeError(int code) const;
};

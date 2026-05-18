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
#include "FileService.hpp"
#include "HttpResponse.hpp"

class ErrorHandler {
   private:
    std::map<int, std::string> _errorPages;
    FileService _fileService;

    std::string getCustomPage(int code) const;
    std::string getDefaultPage(int code) const;

   public:
    ErrorHandler(const std::map<int, std::string>& errorPages);

    HttpResponse makeError(int code) const;
};

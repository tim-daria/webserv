/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandler.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 14:04:26 by dtimofee          #+#    #+#             */
/*   Updated: 2026/06/03 11:33:32 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "FileService.hpp"
#include "HttpResponse.hpp"
#include "ServerConfig.hpp"

class ErrorHandler {
   private:
    std::map<int, std::string> _errorPages;
    FileService _fileService;
    std::string _rootPath;

    std::string getCustomPage(int code) const;
    std::string getDefaultPage(int code) const;

   public:
    ErrorHandler(const ServerConfig& conf);

    HttpResponse makeError(int code) const;
};

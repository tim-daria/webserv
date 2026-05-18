/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileService.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtimofee <dtimofee@student.42berlin.de>    #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-05-11 16:25:44 by dtimofee          #+#    #+#             */
/*   Updated: 2026-05-11 16:25:44 by dtimofee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

class FileService {
   public:
    // Чтение
    int readFile(const std::string& path, std::string& content) const;

    // // Запись
    // bool writeFile(const std::string& path,
    //                const std::string& content) const;

    // // Удаление
    // bool deleteFile(const std::string& path) const;

    // stat одним вызовом
    int checkPath(const std::string& path, struct stat& info) const;
};

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PathUtils.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 16:26:54 by dtimofee          #+#    #+#             */
/*   Updated: 2026/06/02 18:21:21 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

class PathUtils {
   public:
    // На будущее, точно использовать для редирекшенов Собрать путь к файлу
    // static std::string buildPath(const std::string& root,
    //                              const std::string& location,
    //                              const std::string& request_path);

    // static std::string bildPathForDirectory(const std::string& path,
    //                                         const std::string& defaultFile);
    static std::string concatenatePath(const std::string& root, const std::string& path);

    static std::string getContentType(const std::string& path);

   private:
    struct MimeType {
        std::string extension;
        std::string type;
    };
};

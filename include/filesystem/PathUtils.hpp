/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PathUtils.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtimofee <dtimofee@student.42berlin.de>    #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-05-11 16:26:54 by dtimofee          #+#    #+#             */
/*   Updated: 2026-05-11 16:26:54 by dtimofee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

class PathUtils {
   public:
    // static std::string buildFullPath(const std::string& root,
    //                                  const std::string& request_path);

    // Content-Type по расширению
    static std::string getContentType(const std::string& path);

   private:
    struct MimeType {
        std::string extension;
        std::string type;
    };
};

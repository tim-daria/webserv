/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PathUtils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtimofee <dtimofee@student.42berlin.de>    #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-05-12 09:34:15 by dtimofee          #+#    #+#             */
/*   Updated: 2026-05-12 09:34:15 by dtimofee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PathUtils.hpp"

std::string PathUtils::getContentType(const std::string& path) {
    size_t dot_pos = path.rfind('.');
    if (dot_pos == std::string::npos) {
        return "application/octet-stream";
    }
    std::string ext = path.substr(dot_pos);
    MimeType mimeTypes[] = {{".html", "text/html"},
                            {".css", "text/css"},
                            {".js", "application/javascript"},
                            {".jpg", "image/jpeg"},
                            {".jpeg", "image/jpeg"},
                            {".png", "image/png"},
                            {".gif", "image/gif"},
                            {".pdf", "application/pdf"},
                            {".txt", "text/plain"},
                            {".json", "application/json"},
                            {".ico", "image/x-icon"}};
    size_t size = sizeof(mimeTypes) / sizeof(mimeTypes[0]);
    for (size_t i = 0; i < size; i++) {
        if (mimeTypes[i].extension == ext) {
            return mimeTypes[i].type;
        }
    }
    return "application/octet-stream";
}

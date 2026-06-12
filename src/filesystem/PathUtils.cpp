/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PathUtils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 09:34:15 by dtimofee          #+#    #+#             */
/*   Updated: 2026/06/03 10:10:43 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PathUtils.hpp"

#include <ctime>
#include <sstream>

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

std::string PathUtils::generateFilename(const std::string& contentType) {
    std::ostringstream filename;

    filename << std::time(0) << "_" << std::rand();
    if (contentType.find("text/plain") != std::string::npos) {
        return filename.str() + ".txt";
    }
    if (contentType.find("text/html") != std::string::npos) {
        return filename.str() + ".html";
    }
    if (contentType.find("image/jpeg") != std::string::npos) {
        return filename.str() + ".jpg";
    }
    if (contentType.find("image/png") != std::string::npos) {
        return filename.str() + ".png";
    }
    if (contentType.find("application/json") != std::string::npos) {
        return filename.str() + ".json";
    }

    return filename.str() + ".bin";
}

bool PathUtils::endsWithSlash(const std::string& fullPath) {
    if (!fullPath.empty() && fullPath[fullPath.length() - 1] == '/') {
        return true;
    }
    return false;
}

std::string PathUtils::concatenatePath(const std::string& root_path, const std::string& file_path) {
    std::string result_path(root_path);

    if (!result_path.empty() && result_path[result_path.length() - 1] == '/') {
        result_path.resize(result_path.length() - 1);
    }
    if (!file_path.empty() && file_path[0] != '/') {
        result_path.append("/");
    }
    result_path.append(file_path);

    return result_path;
}

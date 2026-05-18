/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoIndex.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtimofee <dtimofee@student.42berlin.de>    #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-05-11 16:23:52 by dtimofee          #+#    #+#             */
/*   Updated: 2026-05-11 16:23:52 by dtimofee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AutoIndex.hpp"

#include <dirent.h>
#include <sys/stat.h>

#include <sstream>

#include "ErrorHandler.hpp"

std::string AutoIndex::generateRow(std::string name, const struct stat& info) {
    std::ostringstream ss;
    if (S_ISDIR(info.st_mode)) {
        name += "/";
    }
    ss << "<a href='" + name + "'>" + name + "</a>" << "          ";
    if (S_ISDIR(info.st_mode)) {
        ss << "-" << "\n";
    } else {
        ss << info.st_size << "\n";
    }
    return ss.str();
}

int AutoIndex::generate(const std::string& path, const std::string& uri, std::string& body) {
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        return HTTP_FORBIDDEN;
    }
    std::ostringstream ss;

    ss << "<html>\n"
       << "<head><title>Index of " << uri << "</title></head>\n"
       << "<body>\n"
       << "<h1>Index of " << uri << "</h1>\n"
       << "<hr>\n<pre>\n"
       << "<a href='../'>../</a>\n";

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;
        if (name == "." || name == "..") {
            continue;
        }
        std::string fullPath = path + "/" + name;
        struct stat info;
        if (stat(fullPath.c_str(), &info) != 0) {
            continue;
        }
        ss << generateRow(name, info);
    }
    closedir(dir);
    ss << "</pre>\n<hr>\n</body>\n</html>\n";
    body = ss.str();
    return HTTP_OK;
}

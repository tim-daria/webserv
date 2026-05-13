/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileService.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtimofee <dtimofee@student.42berlin.de>    #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-05-12 09:22:17 by dtimofee          #+#    #+#             */
/*   Updated: 2026-05-12 09:22:17 by dtimofee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileService.hpp"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "HttpResponse.hpp"

int FileService::checkPath(const std::string& path, struct stat& info) const {
    if (stat(path.c_str(), &info) != 0) {
        return HTTP_NOT_FOUND;
    }
    if (S_ISDIR(info.st_mode) && access(path.c_str(), X_OK) != 0) {
        return HTTP_FORBIDDEN;
    }
    if (S_ISREG(info.st_mode) && access(path.c_str(), R_OK) != 0) {
        return HTTP_FORBIDDEN;
    }
    return HTTP_OK;
}

int FileService::readFile(const std::string& path, std::string& content) const {
    int fd = open(path.c_str(), O_RDONLY);
    if (fd == -1) {
        return HTTP_NOT_FOUND;
    }
    char buf[1024];
    int bytes;
    while ((bytes = read(fd, buf, sizeof(buf))) > 0) {
        content.append(buf, bytes);
    }
    close(fd);
    return HTTP_OK;
}

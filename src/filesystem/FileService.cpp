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

#include <fstream>

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

int FileService::checkUploadDirectory(const std::string& path, struct stat& info) const {
    if (stat(path.c_str(), &info) != 0) {
        return HTTP_NOT_FOUND;
    }
    if (!S_ISDIR(info.st_mode)) {
        return HTTP_FORBIDDEN;
    }
    if (access(path.c_str(), X_OK) != 0 || access(path.c_str(), W_OK) != 0) {
        return HTTP_FORBIDDEN;
    }
    return HTTP_OK;
}

int FileService::checkCGI(const std::string& path, struct stat& info) const {
    if (stat(path.c_str(), &info) != 0) return HTTP_NOT_FOUND;

    if (!S_ISREG(info.st_mode)) return HTTP_FORBIDDEN;

    // if (access(path.c_str(), F_OK) != 0) return HTTP_FORBIDDEN;

    return HTTP_OK;
}

int FileService::readFile(const std::string& path, std::string& content) const {
    std::ifstream file(path.c_str());
    if (!file.is_open()) {
        return HTTP_NOT_FOUND;
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    content = ss.str();
    return HTTP_OK;
}

bool FileService::writeFile(const std::string& path, const std::string& content) const {
    std::ofstream file(path.c_str());
    if (!file.is_open()) {
        return false;
    }
    file << content;
    return file.good();
}
bool FileService::deleteFile(const std::string& path) const { return unlink(path.c_str()) == 0; }

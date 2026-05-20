/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tests_filesystem.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/15 10:42:44 by dtimofee          #+#    #+#             */
/*   Updated: 2026/05/20 11:12:05 by tsemenov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/stat.h>

#include "FileService.hpp"
#include "HttpResponse.hpp"
#include "PathUtils.hpp"
#include "catch.hpp"
#include "io.hpp"

TEST_CASE("FileService::readFile", "[FileService]") {
    FileService fs;

    SECTION("reads existing file correctly") {
        createFile("/tmp/test_read.txt", "Hello webserv!");
        std::string body;
        int status = fs.readFile("/tmp/test_read.txt", body);
        REQUIRE(status == HTTP_OK);
        REQUIRE(body == "Hello webserv!");
        removeFile("/tmp/test_read.txt");
    }

    SECTION("returns error for nonexistent file") {
        std::string body;
        int status = fs.readFile("/tmp/nonexistent_xyz.txt", body);
        REQUIRE(status == HTTP_NOT_FOUND);
        REQUIRE(body.empty());
    }

    SECTION("reads empty file correctly") {
        createFile("/tmp/test_empty.txt", "");
        std::string body;
        int status = fs.readFile("/tmp/test_empty.txt", body);
        REQUIRE(status == HTTP_OK);
        REQUIRE(body.empty());
        removeFile("/tmp/test_empty.txt");
    }
}

TEST_CASE("FileService::checkPath", "[FileService]") {
    FileService fs;
    struct stat info;

    SECTION("existing file returns HTTP_OK") {
        createFile("/tmp/test_check.txt", "hello");
        int status = fs.checkPath("/tmp/test_check.txt", info);
        REQUIRE(status == HTTP_OK);
        REQUIRE(S_ISREG(info.st_mode));
        removeFile("/tmp/test_check.txt");
    }

    SECTION("nonexistent path returns HTTP_NOT_FOUND") {
        int status = fs.checkPath("/tmp/nonexistent_xyz.txt", info);
        REQUIRE(status == HTTP_NOT_FOUND);
    }

    SECTION("directory returns HTTP_OK") {
        createDir("/tmp/test_check_dir");
        int status = fs.checkPath("/tmp/test_check_dir", info);
        REQUIRE(status == HTTP_OK);
        REQUIRE(S_ISDIR(info.st_mode));
        removeDir("/tmp/test_check_dir");
    }
}

TEST_CASE("PathUtils::getContentType returns correct type", "[PathUtils]") {
    REQUIRE(PathUtils::getContentType("index.html") == "text/html");
    REQUIRE(PathUtils::getContentType("style.css") == "text/css");
    REQUIRE(PathUtils::getContentType("script.js") == "application/javascript");
    REQUIRE(PathUtils::getContentType("photo.jpg") == "image/jpeg");
    REQUIRE(PathUtils::getContentType("photo.jpeg") == "image/jpeg");
    REQUIRE(PathUtils::getContentType("image.png") == "image/png");
    REQUIRE(PathUtils::getContentType("anim.gif") == "image/gif");
    REQUIRE(PathUtils::getContentType("doc.pdf") == "application/pdf");
    REQUIRE(PathUtils::getContentType("data.json") == "application/json");
    REQUIRE(PathUtils::getContentType("favicon.ico") == "image/x-icon");
}

TEST_CASE("PathUtils::getContentType handles unknown extension", "[PathUtils]") {
    REQUIRE(PathUtils::getContentType("file.xyz") == "application/octet-stream");
    REQUIRE(PathUtils::getContentType("no_extension") == "application/octet-stream");
}

TEST_CASE("PathUtils::getContentType handles path with dots", "[PathUtils]") {
    // rfind should take last extension
    REQUIRE(PathUtils::getContentType("./my.site/index.html") == "text/html");
    REQUIRE(PathUtils::getContentType("./my.site/photo.jpg") == "image/jpeg");
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tests_filesystem.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/15 10:42:44 by dtimofee          #+#    #+#             */
/*   Updated: 2026/06/02 18:52:48 by nefimov          ###   ########.fr       */
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

TEST_CASE("PathUtils::buildPathForDirectory returns correct path", "[PathUtils]") {
    SECTION("Path ending without /") {
        std::string path = "./www/upload";
        std::string defaultFile = "index.html";
        REQUIRE(PathUtils::bildPathForDirectory(path, defaultFile) == "./www/upload/index.html");
    }

    SECTION("Path ending with /") {
        std::string path = "./www/upload/";
        std::string defaultFile = "index.html";
        REQUIRE(PathUtils::bildPathForDirectory(path, defaultFile) == "./www/upload/index.html");
    }
}

TEST_CASE("PathUtils::buildPathForFile returns correct path", "[PathUtils]") {
    SECTION("File path starting without '/', root path ending without '/'") {
        std::string root_path = "./www/upload";
        std::string file_path = "index.html";
        REQUIRE(PathUtils::bildPathForFile(root_path, file_path) == "./www/upload/index.html");
    }

    SECTION("File path starting without '/', root path ending with '/'") {
        std::string root_path = "./www/upload/";
        std::string file_path = "index.html";
        REQUIRE(PathUtils::bildPathForFile(root_path, file_path) == "./www/upload/index.html");
    }

    SECTION("File path starting with '/', root path ending without '/'") {
        std::string root_path = "./www/upload";
        std::string file_path = "/index.html";
        REQUIRE(PathUtils::bildPathForFile(root_path, file_path) == "./www/upload/index.html");
    }

    SECTION("File path starting with '/', root path ending with '/'") {
        std::string root_path = "./www/upload/";
        std::string file_path = "/index.html";
        REQUIRE(PathUtils::bildPathForFile(root_path, file_path) == "./www/upload/index.html");
    }

    SECTION("File path starting with '/', root path is '/'") {
        std::string root_path = "/";
        std::string file_path = "/index.html";
        REQUIRE(PathUtils::bildPathForFile(root_path, file_path) == "/index.html");
    }

    SECTION("File path is empty string") {
        std::string root_path = "./www/upload";
        std::string file_path = "";
        REQUIRE(PathUtils::bildPathForFile(root_path, file_path) == "./www/upload");
    }

    SECTION("Root path is empty string") {
        std::string root_path = "";
        std::string file_path = "/index.html";
        REQUIRE(PathUtils::bildPathForFile(root_path, file_path) == "/index.html");
    }
}

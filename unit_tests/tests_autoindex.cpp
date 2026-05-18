/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tests_autoindex.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtimofee <dtimofee@student.42berlin.de>    #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-05-15 11:00:27 by dtimofee          #+#    #+#             */
/*   Updated: 2026-05-15 11:00:27 by dtimofee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AutoIndex.hpp"
#include "catch.hpp"
#include "io.cpp"

TEST_CASE("AutoIndex::generate", "[AutoIndex]") {
    AutoIndex autoIndex;

    SECTION("generates HTML for existing directory") {
        createDir("/tmp/test_autoindex");
        createFile("/tmp/test_autoindex/file1.txt", "content1");
        createFile("/tmp/test_autoindex/file2.html", "content2");

        std::string body;
        int status = autoIndex.generate("/tmp/test_autoindex", "/test_autoindex", body);

        REQUIRE(status == HTTP_OK);
        REQUIRE(body.find("file1.txt") != std::string::npos);
        REQUIRE(body.find("file2.html") != std::string::npos);

        removeFile("/tmp/test_autoindex/file1.txt");
        removeFile("/tmp/test_autoindex/file2.html");
        removeDir("/tmp/test_autoindex");
    }

    SECTION("returns HTTP_FORBIDDEN for nonexistent directory") {
        std::string body;
        int status = autoIndex.generate("/tmp/nonexistent_xyz", "/xyz", body);
        REQUIRE(status == HTTP_FORBIDDEN);
        REQUIRE(body.empty());
    }

    SECTION("generated HTML contains title with uri") {
        createDir("/tmp/test_autoindex_title");

        std::string body;
        autoIndex.generate("/tmp/test_autoindex_title", "/test_autoindex_title", body);

        REQUIRE(body.find("Index of /test_autoindex_title") != std::string::npos);

        removeDir("/tmp/test_autoindex_title");
    }

    SECTION("generated HTML contains parent link") {
        createDir("/tmp/test_autoindex_parent");

        std::string body;
        autoIndex.generate("/tmp/test_autoindex_parent", "/sub/dir", body);

        REQUIRE(body.find("../") != std::string::npos);

        removeDir("/tmp/test_autoindex_parent");
    }

    SECTION("generated HTML is valid HTML") {
        createDir("/tmp/test_autoindex_html");

        std::string body;
        autoIndex.generate("/tmp/test_autoindex_html", "/dir", body);

        REQUIRE(body.find("<html>") != std::string::npos);
        REQUIRE(body.find("</html>") != std::string::npos);
        REQUIRE(body.find("<body>") != std::string::npos);
        REQUIRE(body.find("</body>") != std::string::npos);

        removeDir("/tmp/test_autoindex_html");
    }

    SECTION("skips . entry") {
        createDir("/tmp/test_autoindex_dot");

        std::string body;
        autoIndex.generate("/tmp/test_autoindex_dot", "/dir", body);

        // . shouldn't be in a list
        // .. should be in a list as a parent link
        size_t dot_pos = body.find(">.<");
        REQUIRE(dot_pos == std::string::npos);

        removeDir("/tmp/test_autoindex_dot");
    }
}

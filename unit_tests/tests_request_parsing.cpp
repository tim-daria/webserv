#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "catch.hpp"

// Helper: feed a raw string into an existing HttpRequest
static void feedRequest(HttpRequest& req, const std::string& raw, size_t maxBodySize = 0) {
    if (maxBodySize > 0)
        req.setMaxBodySize(maxBodySize);
    req.processData(raw.c_str(), raw.size());
}

// ──────────────────────────────────────────────
// Happy-path parsing
// ──────────────────────────────────────────────

TEST_CASE("HttpRequest: minimal GET request", "[parsing]") {
    HttpRequest req;
    feedRequest(req, "GET / HTTP/1.1\r\n\r\n");

    REQUIRE(req.isDone());
    REQUIRE_FALSE(req.isError());
    REQUIRE(req.getMethod()  == "GET");
    REQUIRE(req.getPath()    == "/");
    REQUIRE(req.getVersion() == "HTTP/1.1");
    REQUIRE(req.getBody()    == "");
}

TEST_CASE("HttpRequest: GET with path and headers", "[parsing]") {
    std::string raw =
        "GET /index.html HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Connection: close\r\n"
        "\r\n";
    HttpRequest req;
    feedRequest(req, raw);

    REQUIRE(req.isDone());
    REQUIRE(req.getPath()               == "/index.html");
    REQUIRE(req.getHeader("host")       == "localhost");
    REQUIRE(req.getHeader("connection") == "close");
}

TEST_CASE("HttpRequest: GET with query string", "[parsing]") {
    HttpRequest req;
    feedRequest(req, "GET /search?q=hello&page=1 HTTP/1.1\r\n\r\n");

    REQUIRE(req.isDone());
    REQUIRE(req.getPath()  == "/search");
    REQUIRE(req.getQuery() == "q=hello&page=1");
}

TEST_CASE("HttpRequest: POST with body", "[parsing]") {
    std::string body = "name=Alice&age=30";
    std::string raw =
        "POST /submit HTTP/1.1\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n"
        "\r\n" + body;
    HttpRequest req;
    feedRequest(req, raw);

    REQUIRE(req.isDone());
    REQUIRE(req.getMethod() == "POST");
    REQUIRE(req.getBody()   == body);
}

TEST_CASE("HttpRequest: DELETE request", "[parsing]") {
    HttpRequest req;
    feedRequest(req, "DELETE /resource HTTP/1.1\r\n\r\n");

    REQUIRE(req.isDone());
    REQUIRE(req.getMethod() == "DELETE");
    REQUIRE(req.getPath()   == "/resource");
}

TEST_CASE("HttpRequest: headers are stored lowercase", "[parsing]") {
    std::string raw =
        "GET / HTTP/1.1\r\n"
        "X-Custom-Header: Value123\r\n"
        "\r\n";
    HttpRequest req;
    feedRequest(req, raw);

    // Keys are stored lowercase; value is preserved
    REQUIRE(req.getHeader("x-custom-header") == "Value123");
}

TEST_CASE("HttpRequest: Content-Length 0 means no body, state is DONE", "[parsing]") {
    std::string raw =
        "POST /empty HTTP/1.1\r\n"
        "Content-Length: 0\r\n"
        "\r\n";
    HttpRequest req;
    feedRequest(req, raw);

    REQUIRE(req.isDone());
    REQUIRE(req.getBody() == "");
}

// ──────────────────────────────────────────────
// Incremental / partial reads
// ──────────────────────────────────────────────

TEST_CASE("HttpRequest: incremental feed — headers split across two chunks", "[parsing]") {
    // Simulates two recv() calls delivering the request in parts
    std::string part1 = "GET /page HTTP/1.1\r\nHost: loc";
    std::string part2 = "alhost\r\n\r\n";

    HttpRequest req;
    req.processData(part1.c_str(), part1.size());
    REQUIRE_FALSE(req.isDone()); // still waiting for end of headers

    req.processData(part2.c_str(), part2.size());
    REQUIRE(req.isDone());
    REQUIRE(req.getHeader("host") == "localhost");
}

TEST_CASE("HttpRequest: incremental feed — body split across chunks", "[parsing]") {
    std::string body = "hello=world";
    std::string headers =
        "POST /data HTTP/1.1\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";

    HttpRequest req;
    req.processData(headers.c_str(), headers.size());
    REQUIRE_FALSE(req.isDone()); // body not yet received

    req.processData(body.c_str(), body.size());
    REQUIRE(req.isDone());
    REQUIRE(req.getBody() == body);
}

// ──────────────────────────────────────────────
// Error cases
// ──────────────────────────────────────────────

TEST_CASE("HttpRequest: unknown method returns 501", "[parsing][errors]") {
    HttpRequest req;
    feedRequest(req, "PATCH /resource HTTP/1.1\r\n\r\n");

    REQUIRE(req.isError());
    REQUIRE(req.getErrorCode() == HTTP_METHOD_NOT_IMPLEMENTED);
}

TEST_CASE("HttpRequest: malformed first line returns 400", "[parsing][errors]") {
    // No spaces — can't extract method/path/version
    HttpRequest req;
    feedRequest(req, "BADREQUEST\r\n\r\n");

    REQUIRE(req.isError());
    REQUIRE(req.getErrorCode() == HTTP_BAD_REQUEST);
}

TEST_CASE("HttpRequest: body exceeds maxBodySize returns 413", "[parsing][errors]") {
    std::string body(100, 'x');
    std::string raw =
        "POST /upload HTTP/1.1\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n"
        "\r\n" + body;

    // Set maxBodySize smaller than the declared Content-Length
    HttpRequest req;
    feedRequest(req, raw, 50);

    REQUIRE(req.isError());
    REQUIRE(req.getErrorCode() == HTTP_PAYLOAD_TOO_LARGE);
}

TEST_CASE("HttpRequest: malformed header (no colon) returns 400", "[parsing][errors]") {
    std::string raw =
        "GET / HTTP/1.1\r\n"
        "BadHeaderWithoutColon\r\n"
        "\r\n";
    HttpRequest req;
    feedRequest(req, raw);

    REQUIRE(req.isError());
    REQUIRE(req.getErrorCode() == HTTP_BAD_REQUEST);
}

TEST_CASE("HttpRequest: processData after DONE is a no-op", "[parsing]") {
    HttpRequest req;
    feedRequest(req, "GET / HTTP/1.1\r\n\r\n");
    REQUIRE(req.isDone());

    // Feeding more data after completion must not change staet
    std::string extra = "EXTRA GARBAGE";
    req.processData(extra.c_str(), extra.size());
    REQUIRE(req.isDone());
    REQUIRE(req.getMethod() == "GET");
}

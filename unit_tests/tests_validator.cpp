/*
 * NOTE: Signal handler unit testing
 * ----------------------------------
 * The actual signal handlers (signalHandler, sigchldHandler) are declared
 * static in main.cpp and are therefore not accessible from unit tests.
 * Their effect — setting g_running = 0 — is a one-liner that doesn't warrant
 * mocking. What CAN be tested at unit level is the mechanism: that a
 * sigaction-registered handler fires and modifies a global flag. That test
 * is included below under [signals].
 */

#include <csignal>
#include <unistd.h>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "RequestValidator.hpp"
#include "RouteConfig.hpp"
#include "ServerConfig.hpp"
#include "catch.hpp"

// ──────────────────────────────────────────────
// Helpers
// ──────────────────────────────────────────────

// Build a parsed HttpRequest from a raw string
static void feedRequest(HttpRequest& req, const std::string& raw) {
    req.processData(raw.c_str(), raw.size());
}

// Build a minimal ServerConfig with one route
static ServerConfig makeConfig(const std::string& url,
                               const std::string& method1,
                               const std::string& method2 = "") {
    ServerConfig cfg;
    RouteConfig route;
    route.url = url;
    route.rootDirectory = "www";
    route.add_acceptedMethod(method1);
    if (!method2.empty())
        route.add_acceptedMethod(method2);
    cfg.add_route(route);
    return cfg;
}

// ──────────────────────────────────────────────
// RequestValidator tests
// ──────────────────────────────────────────────

TEST_CASE("RequestValidator: valid GET on matching route returns 0", "[validator]") {
    ServerConfig cfg = makeConfig("/", "GET");
    HttpRequest req;
    feedRequest(req, "GET / HTTP/1.1\r\n\r\n");

    REQUIRE(req.isDone());
    REQUIRE(RequestValidator::validate(req, cfg) == 0);
}

TEST_CASE("RequestValidator: path with no matching route returns 404", "[validator]") {
    ServerConfig cfg = makeConfig("/api", "GET");
    HttpRequest req;
    feedRequest(req, "GET /missing HTTP/1.1\r\n\r\n");

    REQUIRE(req.isDone());
    REQUIRE(RequestValidator::validate(req, cfg) == HTTP_NOT_FOUND);
}

TEST_CASE("RequestValidator: method not in route returns 405", "[validator]") {
    ServerConfig cfg = makeConfig("/", "GET");
    HttpRequest req;
    feedRequest(req, "POST / HTTP/1.1\r\nContent-Length: 0\r\n\r\n");

    REQUIRE(req.isDone());
    REQUIRE(RequestValidator::validate(req, cfg) == HTTP_METHOD_NOT_ALLOWED);
}

TEST_CASE("RequestValidator: POST allowed when listed in route", "[validator]") {
    ServerConfig cfg = makeConfig("/submit", "GET", "POST");
    HttpRequest req;
    feedRequest(req, "POST /submit HTTP/1.1\r\nContent-Length: 0\r\n\r\n");

    REQUIRE(req.isDone());
    REQUIRE(RequestValidator::validate(req, cfg) == 0);
}

TEST_CASE("RequestValidator: sub-path matches parent route prefix", "[validator]") {
    // Route /static should match /static/style.css
    ServerConfig cfg = makeConfig("/static", "GET");
    HttpRequest req;
    feedRequest(req, "GET /static/style.css HTTP/1.1\r\n\r\n");

    REQUIRE(req.isDone());
    REQUIRE(RequestValidator::validate(req, cfg) == 0);
}

TEST_CASE("RequestValidator: route prefix must end at word boundary", "[validator]") {
    // Route /photo must NOT match /photos
    ServerConfig cfg = makeConfig("/photo", "GET");
    HttpRequest req;
    feedRequest(req, "GET /photos HTTP/1.1\r\n\r\n");

    REQUIRE(req.isDone());
    REQUIRE(RequestValidator::validate(req, cfg) == HTTP_NOT_FOUND);
}

TEST_CASE("RequestValidator: DELETE on route that allows it returns 0", "[validator]") {
    ServerConfig cfg = makeConfig("/resource", "DELETE");
    HttpRequest req;
    feedRequest(req, "DELETE /resource HTTP/1.1\r\n\r\n");

    REQUIRE(req.isDone());
    REQUIRE(RequestValidator::validate(req, cfg) == 0);
}

TEST_CASE("RequestValidator: most specific route wins (longest prefix)", "[validator]") {
    // Two routes: / allows GET, /upload allows POST
    ServerConfig cfg;
    RouteConfig root;
    root.url = "/";
    root.rootDirectory = "www";
    root.add_acceptedMethod("GET");
    cfg.add_route(root);

    RouteConfig upload;
    upload.url = "/upload";
    upload.rootDirectory = "www/upload";
    upload.add_acceptedMethod("POST");
    cfg.add_route(upload);

    HttpRequest req;
    feedRequest(req, "POST /upload HTTP/1.1\r\nContent-Length: 0\r\n\r\n");
    REQUIRE(req.isDone());
    // Should match /upload (longer), so POST is allowed
    REQUIRE(RequestValidator::validate(req, cfg) == 0);

    HttpRequest req2;
    feedRequest(req2, "POST / HTTP/1.1\r\nContent-Length: 0\r\n\r\n");
    REQUIRE(req2.isDone());
    // Should match / (shorter), POST not allowed there
    REQUIRE(RequestValidator::validate(req2, cfg) == HTTP_METHOD_NOT_ALLOWED);
}

TEST_CASE("RequestValidator: empty server config returns 404 for any path", "[validator]") {
    ServerConfig cfg; // no routes
    HttpRequest req;
    feedRequest(req, "GET / HTTP/1.1\r\n\r\n");

    REQUIRE(req.isDone());
    REQUIRE(RequestValidator::validate(req, cfg) == HTTP_NOT_FOUND);
}

// ──────────────────────────────────────────────
// Signal handling — observable behaviour test
// ──────────────────────────────────────────────

// The actual handlers in main.cpp are static and inaccessible.
// We verify the pattern: a sigaction handler setting a volatile flag works correctly.

namespace {
    volatile sig_atomic_t test_flag = 1;
    void testSignalHandler(int) { test_flag = 0; }
}

TEST_CASE("Signal: SIGINT handler sets flag to 0", "[signals]") {
    test_flag = 1;

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = testSignalHandler;
    sa.sa_flags   = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);

    raise(SIGINT);

    REQUIRE(test_flag == 0);

    // Restore default so subsequent tests/processes aren't affected
    struct sigaction sa_dfl;
    sigemptyset(&sa_dfl.sa_mask);
    sa_dfl.sa_handler = SIG_DFL;
    sa_dfl.sa_flags   = 0;
    sigaction(SIGINT, &sa_dfl, NULL);
}

TEST_CASE("Signal: SIGTERM handler sets flag to 0", "[signals]") {
    test_flag = 1;

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = testSignalHandler;
    sa.sa_flags   = SA_RESTART;
    sigaction(SIGTERM, &sa, NULL);

    raise(SIGTERM);

    REQUIRE(test_flag == 0);

    struct sigaction sa_dfl;
    sigemptyset(&sa_dfl.sa_mask);
    sa_dfl.sa_handler = SIG_DFL;
    sa_dfl.sa_flags   = 0;
    sigaction(SIGTERM, &sa_dfl, NULL);
}

TEST_CASE("Signal: SIGTSTP is ignored (SIG_IGN)", "[signals]") {
    // After setting SIG_IGN, raise(SIGTSTP) must not terminate/suspend the process
    struct sigaction sa_ign;
    sigemptyset(&sa_ign.sa_mask);
    sa_ign.sa_handler = SIG_IGN;
    sa_ign.sa_flags   = 0;
    sigaction(SIGTSTP, &sa_ign, NULL);

    // If this line is reached, the process was not suspended
    raise(SIGTSTP);

    REQUIRE(true); // process survived

    struct sigaction sa_dfl;
    sigemptyset(&sa_dfl.sa_mask);
    sa_dfl.sa_handler = SIG_DFL;
    sa_dfl.sa_flags   = 0;
    sigaction(SIGTSTP, &sa_dfl, NULL);
}

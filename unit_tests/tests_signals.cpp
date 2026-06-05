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

#include <unistd.h>

#include <csignal>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "RouteConfig.hpp"
#include "ServerConfig.hpp"
#include "catch.hpp"

// ──────────────────────────────────────────────
// Signal handling — observable behaviour test
// ──────────────────────────────────────────────

// The actual handlers in main.cpp are static and inaccessible.
// We verify the pattern: a sigaction handler setting a volatile flag works correctly.

namespace {
volatile sig_atomic_t test_flag = 1;
void testSignalHandler(int) { test_flag = 0; }
}  // namespace

TEST_CASE("Signal: SIGINT handler sets flag to 0", "[signals]") {
    test_flag = 1;

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = testSignalHandler;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);

    raise(SIGINT);

    REQUIRE(test_flag == 0);

    // Restore default so subsequent tests/processes aren't affected
    struct sigaction sa_dfl;
    sigemptyset(&sa_dfl.sa_mask);
    sa_dfl.sa_handler = SIG_DFL;
    sa_dfl.sa_flags = 0;
    sigaction(SIGINT, &sa_dfl, NULL);
}

TEST_CASE("Signal: SIGTERM handler sets flag to 0", "[signals]") {
    test_flag = 1;

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = testSignalHandler;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGTERM, &sa, NULL);

    raise(SIGTERM);

    REQUIRE(test_flag == 0);

    struct sigaction sa_dfl;
    sigemptyset(&sa_dfl.sa_mask);
    sa_dfl.sa_handler = SIG_DFL;
    sa_dfl.sa_flags = 0;
    sigaction(SIGTERM, &sa_dfl, NULL);
}

TEST_CASE("Signal: SIGTSTP is ignored (SIG_IGN)", "[signals]") {
    // After setting SIG_IGN, raise(SIGTSTP) must not terminate/suspend the process
    struct sigaction sa_ign;
    sigemptyset(&sa_ign.sa_mask);
    sa_ign.sa_handler = SIG_IGN;
    sa_ign.sa_flags = 0;
    sigaction(SIGTSTP, &sa_ign, NULL);

    // If this line is reached, the process was not suspended
    raise(SIGTSTP);

    REQUIRE(true);  // process survived

    struct sigaction sa_dfl;
    sigemptyset(&sa_dfl.sa_mask);
    sa_dfl.sa_handler = SIG_DFL;
    sa_dfl.sa_flags = 0;
    sigaction(SIGTSTP, &sa_dfl, NULL);
}

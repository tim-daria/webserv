#include <csignal>

// Global flag checked by the server loop to know when to shut down.
// Declared extern in ServerHub.cpp and set to 0 by signal handlers in main.cpp.
volatile sig_atomic_t g_running = 1;

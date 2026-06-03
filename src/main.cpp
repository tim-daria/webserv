/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2026/06/02 17:10:45 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/wait.h>

#include <csignal>
#include <exception>
#include <iostream>
#include <vector>

#include "Logger.hpp"
#include "Parser.hpp"
#include "ServerConfig.hpp"
#include "ServerHub.hpp"

extern volatile sig_atomic_t g_running;  // defined in globals.cpp

static void signalHandler(int) { g_running = 0; }

static void sigChldHandler(int) {
    pid_t pid;

    // WNOHANG - if no zombies, return 0
    while (true) {
        pid = waitpid(-1, NULL, WNOHANG);
        if (pid <= 0) {
            break;
        }  // no more zombies or no children at all
    }
}

int main(int ac, char** av) {
    (void)av;
    (void)ac;

    // ignore SIGPIPE (if writing to a pipe fd failed - it's closed)
    // ignore SIGTSTP (Ctrl-Z) — a server should not be suspendable
    struct sigaction sa_ign;
    sigemptyset(&sa_ign.sa_mask);
    sa_ign.sa_handler = SIG_IGN;
    sa_ign.sa_flags = 0;
    sigaction(SIGPIPE, &sa_ign, NULL);
    sigaction(SIGTSTP, &sa_ign, NULL);

    // quit on Ctrl-C / kill
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = signalHandler;  // g_running = 0
    sa.sa_flags = SA_RESTART;       // if a syscall interrupted, restart it

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    // kill zombie children
    struct sigaction sa_chld;
    sigemptyset(&sa_chld.sa_mask);
    sa_chld.sa_handler = sigChldHandler;
    sigemptyset(&sa_chld.sa_mask);
    sa_chld.sa_flags =
        SA_RESTART | SA_NOCLDSTOP;  // ignore SIGSTOP (Ctrl-Z) & SIGCONT from children
    sigaction(SIGCHLD, &sa_chld, NULL);

    Logger::setLevel(DEBUG);

    try {
        std::vector<ServerConfig> configs;
        if (ac == 1) {
            configs = Parser::parseFile("webserv.conf");
        } else if (ac == 2) {
            configs = Parser::parseFile(av[1]);
        } else {
            throw std::runtime_error("too many arguments");
        }

        ServerHub hub(configs);
        hub.runServers();
        LOG_INFO("Server is shutting down... Byee!");
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("Error: ") + e.what());
        return 1;
    }
    return 0;
}

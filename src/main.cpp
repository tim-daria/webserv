/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 22:30:06 by dtimofee          #+#    #+#             */
/*   Updated: 2026/03/31 21:36:04 by tsemenov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>
#include <exception>
#include <iostream>
#include <vector>

#include "ServerConfig.hpp"
#include "ServerHub.hpp"

extern volatile bool g_running;

static void signalHandler(int) { g_running = false; }

int main() {
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    try {
        std::vector<ServerConfig> configs;
        configs.push_back(ServerConfig::makeDefault());

        ServerHub hub(configs);
        hub.runServers();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

// 	}
// }

// int main() {
//     int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

//     sockaddr_in serverAddress = {};
//     serverAddress.sin_family = AF_INET;
//     serverAddress.sin_port = htons(8080);
//     serverAddress.sin_addr.s_addr = INADDR_ANY;

//     while (1) {
//         bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress));
//         listen(serverSocket, 5);
//         int client = accept(serverSocket, NULL, NULL);
//         char buffer[1024] = {0};
//         read(client, buffer, 1024);
//         const char* response =
//             "HTTP/1.1 200 OK\r\n"
//             "Content-Type: text/plain\r\n"
//             "\r\n"
//             "Hello from mini webserv!\n";
//         write(client, response, std::strlen(response));
//         close(client);
//     }
// }

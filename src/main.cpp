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

#include <exception>
#include <iostream>

#include "Server.hpp"
#include "config/ServerConfig.hpp"

int main() {
    try {
        // Use default config for testing/demo
        ServerConfig config = ServerConfig::makeDefault();
        config.print();  // Print config for debugging

        Server serv(config);
        serv.initServ();
        serv.runServ();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
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

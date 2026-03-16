/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 22:30:06 by dtimofee          #+#    #+#             */
/*   Updated: 2026/03/16 17:06:43 by tsemenov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
// #include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include "Server.hpp"

int main() {
<<<<<<< New base: fix: add clang-tidy checks to CI (#2)
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress = {};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    while (true) {
        bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress));
        listen(serverSocket, 5);
        int client = accept(serverSocket, NULL, NULL);
        char buffer[1024] = {0};
        read(client, buffer, 1024);
        const char* response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "Hello from mini webserv!\n";
        write(client, response, std::strlen(response));
        close(client);
||||||| Common ancestor
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress = {};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    while (1) {
        bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress));
        listen(serverSocket, 5);
        int client = accept(serverSocket, NULL, NULL);
        char buffer[1024] = {0};
        read(client, buffer, 1024);
        const char* response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "Hello from mini webserv!\n";
        write(client, response, std::strlen(response));
        close(client);
=======
    try {
        Server serv(8080);
        serv.init_serv();

        while (true) {
            struct sockaddr_in clientAddr;
            socklen_t clientLen = sizeof(clientAddr);

            int clientFd = accept(serv.get_fd(), (struct sockaddr*)&clientAddr, &clientLen);

            if (clientFd >= 0) {
                std::cout << "New client connected at fd: " << clientFd << std::endl;
                close(clientFd);
            } else
                usleep(100000);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
>>>>>>> Current commit: Feat: min server setup
    }
<<<<<<< New base: fix: add clang-tidy checks to CI (#2)
    return 0;
||||||| Common ancestor
=======

    return 0;
>>>>>>> Current commit: Feat: min server setup
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

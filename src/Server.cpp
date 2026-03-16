/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 16:07:29 by tsemenov          #+#    #+#             */
/*   Updated: 2026/03/16 17:01:35 by tsemenov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int port) : _sockfd(-1), _port(port) {}

Server::~Server() {
    if (_sockfd != -1) close(_sockfd);
}

int Server::get_fd() const { return _sockfd; };

void Server::init_serv() {
    // fill in the sockaddr_in struct through getaddrinfo:
    struct addrinfo hints = {};  // set all to 0 upon creation

    hints.ai_family = AF_INET;        // IPv4 only
    hints.ai_socktype = SOCK_STREAM;  // TCP (Transmission Control Protocol) is used by stream
                                      // sockets (two-way connected communication streams)
    hints.ai_flags = AI_PASSIVE;      // if no addr, set INADDR_ANY yourself

    struct addrinfo* result = NULL;
    std::stringstream ss;
    ss << _port;
    std::string portStr = ss.str();

    if ((getaddrinfo(NULL, portStr.c_str(), &hints, &result)) != 0)
        throw std::runtime_error(std::string("getaddrinfo failed: ") + strerror(errno));

    // will call freeaddrinfo on every (incl. throw) exit
    // RAII (Resource Acquisition Is Initialization)
    struct AddrInfoGuard {
        struct addrinfo* result;

        AddrInfoGuard(struct addrinfo* res) : result(res) {}
        ~AddrInfoGuard() {
            if (result) freeaddrinfo(result);
        }
    } guard(result);

    // create socket with a filled setup:
    _sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (_sockfd < 0) throw std::runtime_error("Failed to create socket");

    // make the port reusable (if Ctrl+C, don't wait until the port is freed):
    int opt = 1;  // yes flag
    if ((setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) != 0)
        throw std::runtime_error("Failed to set non-blocking socket");

    // make the socket non-blocking:
    if (fcntl(_sockfd, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("Failed to set non-blocking socket");

    // bind socket to the port (don't use < 1024):
    if (bind(_sockfd, result->ai_addr, result->ai_addrlen) < 0)
        throw std::runtime_error(std::string("Failed to bind: ") + strerror(errno));

    if (listen(_sockfd, 10) < 0) throw std::runtime_error("Filed to listen");

    std::cout << "Server started at http://localhost:" << _port << "/" << std::endl;
    std::cout << "Fd: " << _sockfd << std::endl;
}

// we fill the struct via getsockaddr:
// 		struct addrinfo {
//     int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
//     int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
//     int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
//     int              ai_protocol;  // use 0 for "any"
//     size_t           ai_addrlen;   // size of ai_addr in bytes
//     struct sockaddr *ai_addr;      // struct sockaddr_in or _in6
//     char            *ai_canonname; // full canonical hostname

//     struct addrinfo *ai_next;      // linked list, next node
// };

// sockaddr points to this struct (for IPv4):
// struct sockaddr_in {
//     short int          sin_family;  // Address family, AF_INET
//     unsigned short int sin_port;    // Port number
//     struct in_addr     sin_addr;    // Internet address
//     unsigned char      sin_zero[8]; // Same size as struct sockaddr
// };

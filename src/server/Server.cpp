/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 16:07:29 by tsemenov          #+#    #+#             */
/*   Updated: 2026/04/07 23:44:58 by tsemenov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

#include <fcntl.h>
#include <netinet/in.h>  // sockaddr_in
#include <sys/socket.h>  // socket, bind, listen
#include <unistd.h>      // close

#include <cerrno>
#include <cstring>  // strerror
#include <iostream>
#include <sstream>
#include <stdexcept>  // runtime_error

Server::Server(const ServerConfig& config) : _sockfd(-1), _config(config) {}
Server::Server(const Server& other) : _sockfd(-1), _config(other._config) {}
Server& Server::operator=(const Server& other) {
    (void)other;
    return *this;
}

Server::~Server() {
    if (_sockfd != -1) close(_sockfd);
}

int Server::get_fd() const { return _sockfd; }

const ServerConfig& Server::getConfig() const { return _config; }

// Resolves the port to an addrinfo struct. Caller must freeaddrinfo
struct addrinfo* Server::createAddress(int port) {
    struct addrinfo hints = {};       // set all to 0 upon creation
    hints.ai_family = AF_INET;        // IPv4
    hints.ai_socktype = SOCK_STREAM;  // TCP
    hints.ai_flags = AI_PASSIVE;      // if no addr, set INADDR_ANY

    std::stringstream ss;
    ss << port;

    struct addrinfo* result = NULL;
    if (getaddrinfo(NULL, ss.str().c_str(), &hints, &result) != 0) {
        throw std::runtime_error(std::string("getaddrinfo failed: ") + strerror(errno));
    }
    return result;
}

void Server::createSocket(struct addrinfo* addr) {
    _sockfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (_sockfd < 0) {
        throw std::runtime_error("Failed to create socket");
    }
}

void Server::configureSocket() {
    int yes = 1;
    if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) != 0) {
        throw std::runtime_error("Failed to set SO_REUSEADDR");
    }
    if (fcntl(_sockfd, F_SETFL, O_NONBLOCK) < 0) {
        throw std::runtime_error("Failed to set O_NONBLOCK");
    }
}

void Server::bindAndListen(struct addrinfo* addr) {
    if (bind(_sockfd, addr->ai_addr, addr->ai_addrlen) != 0) {
        throw std::runtime_error(std::string("Failed to bind: ") + strerror(errno));
    }
    if (listen(_sockfd, 10) != 0) {
        throw std::runtime_error("Failed to listen");
    }
}

void Server::initServ(size_t index) {
    if (index >= _config.listen.size()) throw std::runtime_error("listen index out of range");

    int port = _config.listen[index].second;
    struct addrinfo* addr = createAddress(port);

    // RAII guard — freeaddrinfo on any exit including throws
    struct AddrInfoGuard {
        struct addrinfo* ptr;
        AddrInfoGuard(struct addrinfo* ptr) : ptr(ptr) {}
        ~AddrInfoGuard() { freeaddrinfo(ptr); }
    } guard(addr);

    createSocket(addr);
    configureSocket();
    bindAndListen(addr);

    std::cout << "Server listening on port " << port << " (fd " << _sockfd << ")" << std::endl;
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

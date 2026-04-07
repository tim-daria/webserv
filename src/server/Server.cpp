/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 16:07:29 by tsemenov          #+#    #+#             */
/*   Updated: 2026/03/31 22:35:07 by tsemenov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

#include <fcntl.h>
#include <netdb.h>       // addrinfo, getaddrinfo
#include <netinet/in.h>  // sockaddr_in
#include <sys/socket.h>  // socket, bind, listen
#include <unistd.h>      // close

#include <cerrno>
#include <cstring>  // strerror
#include <iostream>
#include <sstream>
#include <stdexcept>  // runtime_error
#include <string>

#include "Handler.hpp"

Server::Server(const ServerConfig& config) : _sockfd(-1), _config(config) {}

Server::~Server() {
    if (_sockfd != -1) close(_sockfd);
}

Server::Server(const Server& other) : _sockfd(-1), _config(other._config) {
    // Do not copy socket fd, only config
}

Server& Server::operator=(const Server& other) {
    if (this != &other) {
        if (_sockfd != -1) close(_sockfd);
        _sockfd = -1;
        _config = other._config;
    }
    return *this;
}

int Server::get_fd() const { return _sockfd; };

void Server::initServ() {
    if (_config.listen.empty()) {
        throw std::runtime_error("No listen directive in config");
    }
    // For now, use the first listen entry, second member (port number)
    int port = _config.listen[0].second;
    // fill in the sockaddr_in struct through getaddrinfo:
    struct addrinfo hints = {};  // set all to 0 upon creation

    hints.ai_family = AF_INET;        // IPv4 only
    hints.ai_socktype = SOCK_STREAM;  // TCP (Transmission Control Protocol) is used by stream
                                      // sockets (two-way connected communication streams)
    hints.ai_flags = AI_PASSIVE;      // if no addr, set INADDR_ANY yourself

    struct addrinfo* result = NULL;
    std::stringstream ss;
    ss << port;
    std::string port_str = ss.str();

    if ((getaddrinfo(NULL, port_str.c_str(), &hints, &result)) != 0) {
        throw std::runtime_error(std::string("getaddrinfo failed: ") + strerror(errno));
    }

    // with this sctruct, we will call freeaddrinfo on every (incl. throw) exit
    // RAII (Resource Acquisition Is Initialization)
    struct AddrInfoGuard {
        struct addrinfo* result;

        AddrInfoGuard(struct addrinfo* res) : result(res) {}
        ~AddrInfoGuard() {
            if (result) {
                freeaddrinfo(result);
            }
        }
    } guard(result);

    // create socket with a filled setup:
    _sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (_sockfd < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    // make the port reusable (if Ctrl+C, don't wait until the port is freed):
    int yes = 1;  // yes flag
    if ((setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))) != 0) {
        throw std::runtime_error("Failed to set non-blocking socket");
    }

    // make the socket non-blocking:
    if (fcntl(_sockfd, F_SETFL, O_NONBLOCK) < 0) {
        throw std::runtime_error("Failed to set non-blocking socket");
    }

    // bind socket to the port (don't use < 1024):
    if (bind(_sockfd, result->ai_addr, result->ai_addrlen) != 0) {
        throw std::runtime_error(std::string("Failed to bind: ") + strerror(errno));
    }

    if (listen(_sockfd, 10) != 0) {
        throw std::runtime_error("Filed to listen");
    }

    std::cout << "Server started at http://localhost:" << port << "/" << std::endl;
    std::cout << "Fd: " << _sockfd << std::endl;
}

void Server::runServ() {
    std::vector<struct pollfd> fds;
    struct pollfd server_pfd;

    server_pfd.fd = _sockfd;
    server_pfd.events = POLLIN;  // requested events: incoming
    server_pfd.revents = 0;      // returned events
    fds.push_back(server_pfd);

    std::cout << "Waiting for connections..." << std::endl;

    while (true) {
        // int poll(struct pollfd *fds, nfds_t nfds, int timeout);
        // returns a number of connections that have events
        int ret = poll(&fds[0], fds.size(),
                       -1);  // wait for any of fds to become ready for I/O (to "send" anything)
        if (ret < 0) {
            throw std::runtime_error("Failed to poll");
        }

        for (size_t i = 0; i < fds.size(); ++i) {
            // do I need to scan them all?
            if (fds[i].revents & POLLIN) {   // if sometihng returned and there's something to read
                if (fds[i].fd == _sockfd) {  // if there's a new connection request for _sockfd
                    acceptNew(fds);
                } else {  // if a request from existing connection
                    handleClient(fds, i);
                    i--;
                }
            }
        }
    }
}

void Server::acceptNew(std::vector<struct pollfd>& fds) {
    struct sockaddr_in client_addr = {};
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(_sockfd, (struct sockaddr*)&client_addr, &client_len);

    // errno not on read/write/recv/send:
    if (client_fd < 0) {
        // if (errno == EAGAIN || errno == EWOULDBLOCK) return;
        std::cerr << "Accept failed: " << strerror(errno) << std::endl;
        return;
    }

    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) < 0) {
        std::cerr << "Client fcntl failed: " << strerror(errno) << std::endl;
        return;
    }

    struct pollfd client_pfd;
    client_pfd.fd = client_fd;
    client_pfd.events = POLLIN;  // requested events: incoming
    client_pfd.revents = 0;
    fds.push_back(client_pfd);

    std::cout << "New client connected on fd: " << client_fd << std::endl;
}

void Server::handleClient(std::vector<struct pollfd>& fds, size_t index) {
    char buf[4096];
    int client_fd = fds[index].fd;

    // ssize_t recv(int socket, void *buffer, size_t length, int flags);
    int bytes_read = recv(client_fd, buf, sizeof(buf) - 1, 0);  // - 1 on failure

    // error:
    if (bytes_read < 0) {
        std::cerr << "recv failed for client_fd " << client_fd << std::endl;
        close(client_fd);
        fds.erase(fds.begin() + index);
        return;
    }

    // client disconnected first:
    if (bytes_read == 0) {
        std::cout << "Client_fd " << client_fd << " disconnected" << std::endl;
        close(client_fd);
        fds.erase(fds.begin() + index);
        return;
    }

    buf[bytes_read] = '\0';

    // print request from browser:
    std::cout << "Request from client_fd " << client_fd << ":\n" << buf << std::endl;

    // Parse HTTP request
    HttpRequest request;
    std::string request_str(buf);

    // Parse first line (METHOD PATH HTTP/VERSION)
    size_t first_space = request_str.find(' ');
    size_t second_space = request_str.find(' ', first_space + 1);

    if (first_space != std::string::npos && second_space != std::string::npos) {
        request.method = request_str.substr(0, first_space);
        request.path = request_str.substr(first_space + 1, second_space - first_space - 1);
    }

    // Handle favicon request
    if (request.path == "/favicon.ico") {
        std::cout << "--> This is just a favicon request, sending 404" << std::endl;
        std::string response =
            "HTTP/1.0 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
        send(client_fd, response.c_str(), response.size(), 0);
    } else {
        // Use Handler to process the request
        Handler handler;

        std::string response_str = handler.handle_request(request).toString();

        std::cout << "--> Sending response!" << std::endl;
        std::cout << "Response sent to client_fd " << client_fd << ":\n"
                  << response_str << std::endl;
        send(client_fd, response_str.c_str(), response_str.size(), 0);
    }

    close(client_fd);
    fds.erase(fds.begin() + index);
}

// TBD:
// big requests
// timeouts for clients
// signal handling
// SIGPIPE
// give html files

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

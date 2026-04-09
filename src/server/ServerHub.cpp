/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerHub.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/06 16:23:17 by tsemenov          #+#    #+#             */
/*   Updated: 2026/04/08 14:41:30 by tsemenov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerHub.hpp"

#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "Handler.hpp"
#include "HttpRequest.hpp"
#include "ServerConfig.hpp"

extern volatile bool g_running;

static const int TIMEOUT = 30;  // max seconds before disconnecting inactive client

ServerHub::ServerHub() {}
ServerHub::ServerHub(const ServerHub& other) { (void)other; }
ServerHub& ServerHub::operator=(const ServerHub& other) {
    (void)other;
    return *this;
}

ServerHub::~ServerHub() {
    // server fds closed by Server destructor
    for (size_t i = _servers.size(); i < _fds.size(); ++i)
        close(_fds[i].fd);  // close remaining client fds
}

// one Server per listen entry across all configs:
ServerHub::ServerHub(const std::vector<ServerConfig>& configs) {
    // create and initialize servers, store them in arr:
    for (size_t i = 0; i < configs.size(); ++i) {
        for (size_t j = 0; j < configs[i].listen.size(); ++j) {
            _servers.push_back(Server(configs[i]));
            _servers.back().initServ(j);

            // create fds & store them in arr:
            struct pollfd pfd = {};
            pfd.fd = _servers.back().get_fd();
            pfd.events = POLLIN;
            _fds.push_back(pfd);
        }
    }
}

void ServerHub::runServers() {
    while (g_running) {
        int res = poll(&_fds[0], _fds.size(), 5000);  // wake up every 5 sec or if a signal comes
        if (res < 0) {
            if (errno == EINTR) continue;  // interrupted by signal, check g_running
            throw std::runtime_error("Failed to poll");
        }

        for (size_t i = 0; i < _fds.size(); ++i) {
            // if the other end is closed or disconnected | is a error on fd occured:
            if (_fds[i].revents & (POLLHUP | POLLERR)) {
                disconnectClient(i);
                if (i > 0) {
                    --i;
                }
                // read and write ONLY after poll() check:
                // if there's data to read:
            } else if (_fds[i].revents & POLLIN) {
                // if POLLIN on a server fd, it's a new connection request
                if (isServerFd(_fds[i].fd))
                    acceptNewClient(_fds[i].fd);
                else  // if POLLIN on a client_fd, it's a data from an existing connection
                    handleRead(i);
                // if the client fd is ready to send responce to the browser:
            } else if (_fds[i].revents & POLLOUT) {
                handleWrite(i);
            }
        }
        checkTimeouts();
    }
    // if shut down:
    std::cout << "\nServer shutting down" << std::endl;
}

bool ServerHub::isServerFd(int fd) {
    for (size_t i = 0; i < _servers.size(); ++i)
        if (_servers[i].get_fd() == fd) {
            return true;
        }
    return false;
}

void ServerHub::acceptNewClient(int server_fd) {
    // create a struct that will be filled by OS on accept:
    struct sockaddr_in client_addr = {};
    socklen_t client_len = sizeof(client_addr);

    // accept the first connection in a row, open an fd for it:
    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        std::cerr << "Accept failed" << std::endl;
        return;
    }

    // make it non-blocking:
    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) < 0) {
        std::cerr << "fcntl failed for client fd " << client_fd << std::endl;
        close(client_fd);
        return;
    }

    // save fd and a new Client object into the map:
    _clients.insert(std::make_pair(client_fd, Client(client_fd)));

    // save client_fd in the arr of fds, used by poll()
    // poll() needs a pollfd, not fd:
    struct pollfd pfd = {};
    pfd.fd = client_fd;
    pfd.events = POLLIN;
    _fds.push_back(pfd);

    std::cout << "New client connected on fd: " << client_fd << std::endl;
}

void ServerHub::handleRead(size_t index) {
    int client_fd = _fds[index].fd;
    Client& client = _clients.at(client_fd);

    // if failed to read the whole message
    if (!client.isMsgReceived()) {
        disconnectClient(index);
        return;
    }

    // if the received message is missing header/body
    if (!client.isRequestComplete()) return;  // wait for more data

    std::cout << "Request complete from fd " << client_fd << std::endl;

    // Parse raw buffer into HttpRequest
    HttpRequest req;
    const std::string& raw = client.get_readBuffer();
    size_t first_space = raw.find(' ');
    size_t second_space = raw.find(' ', first_space + 1);
    if (first_space != std::string::npos && second_space != std::string::npos) {
        req.method = raw.substr(0, first_space);
        req.path = raw.substr(first_space + 1, second_space - first_space - 1);
    }

    // Dispatch to Handler
    Handler handler;
    client.set_writeBuffer(handler.handle_request(req).toString());

    // Stop watching for reads, start watching for writes
    _fds[index].events = POLLOUT;
}

void ServerHub::handleWrite(size_t index) {
    int client_fd = _fds[index].fd;
    Client& client = _clients.at(client_fd);

    const std::string& buf = client.get_writeBuffer();
    ssize_t bytes_sent = send(client_fd, buf.c_str(), buf.size(), 0);

    if (bytes_sent < 0) {
        // if send error — disconnect
        disconnectClient(index);
        return;
    }
    if (bytes_sent > 0) client.clear_writeBuffer(bytes_sent);  // clear what was sent
    // bytes_sent == 0: send buffer full, try again next POLLOUT

    if (client.get_writeBuffer().empty()) disconnectClient(index);  // response fully sent
}

void ServerHub::disconnectClient(size_t index) {
    int client_fd = _fds[index].fd;
		std::cout << "Client on fd " << client_fd << " timed out" << std::endl;
    close(client_fd);  // ServerHub owns the fd lifecycle
    _clients.erase(client_fd);
    _fds.erase(_fds.begin() + index);
}

void ServerHub::checkTimeouts() {
    time_t now = time(NULL);

    for (size_t i = _servers.size(); i < _fds.size();) {
        int client_fd = _fds[i].fd;
        Client& client = _clients.at(client_fd);

        if (now - client.get_lastActive() > TIMEOUT) {
            std::cout << "Client fd " << client_fd << " timed out" << std::endl;
            disconnectClient(i);
        } else {
            ++i;
        }
    }
}

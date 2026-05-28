/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerHub.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/06 16:23:17 by tsemenov          #+#    #+#             */
/*   Updated: 2026/05/20 21:07:49 by tsemenov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerHub.hpp"

#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h> 	
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "Logger.hpp"
// Added to allow constructing error responses directly in handleRead,
// without going through Handler (which requires a fully valid request):
#include "ErrorHandler.hpp"
#include "HttpRequest.hpp"
#include "RequestHandler.hpp"
// Added to validate the parsed request against the server config
// (checks route existence and method allowance) before dispatching:
#include "RequestValidator.hpp"
#include "ServerConfig.hpp"

extern volatile sig_atomic_t g_running;

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
ServerHub::ServerHub(std::vector<ServerConfig>& configs) {
    // create and initialize servers, store them in arr:
    for (size_t i = 0; i < configs.size(); ++i) {
        for (size_t j = 0; j < configs[i].listen.size(); ++j) {
            _servers.push_back(Server(configs[i]));
            _servers.back().initServ(j);

            // create fds & store them in arr:
            struct pollfd pfd = {};
            pfd.fd = _servers.back().getFd();
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
                int sIdx = getServerIndex(_fds[i].fd);  // if >= 0, it's server
                if (sIdx >= 0)
                    acceptNewClient(_fds[i].fd, static_cast<size_t>(sIdx));
                else  // if POLLIN on a client_fd, it's a data from an existing connection
                    handleRead(i);
                // if the client fd is ready to send response to the browser:
            } else if (_fds[i].revents & POLLOUT) {
                handleWrite(i);
            }
        }
        checkTimeouts();
    }
}

// Returns index into _servers if fd belongs to a server, -1 otherwise
int ServerHub::getServerIndex(int fd) {
    for (size_t i = 0; i < _servers.size(); ++i)
        if (_servers[i].getFd() == fd) return static_cast<int>(i);
    return -1;
}

void ServerHub::acceptNewClient(int server_fd, size_t serverIndex) {
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
        std::ostringstream oss_fcntl;
        oss_fcntl << "fcntl failed for client fd " << client_fd;
        LOG_DEBUG(oss_fcntl.str());
        close(client_fd);
        return;
    }

    // save fd and a new Client object into the map (Client knows which server it belongs to):
    _clients.insert(std::make_pair(client_fd, Client(client_fd, serverIndex)));

    // save client_fd in the arr of fds, used by poll()
    // poll() needs a pollfd, not fd:
    struct pollfd pfd = {};
    pfd.fd = client_fd;
    pfd.events = POLLIN;
    _fds.push_back(pfd);

    std::ostringstream oss_accept;
    oss_accept << "New client connected on fd: " << client_fd;
    LOG_DEBUG(oss_accept.str());
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

    std::ostringstream oss_req;
    oss_req << "Request received from fd " << client_fd;
    LOG_DEBUG(oss_req.str());

    ServerConfig& config = _servers[client.getServerIndex()].getConfig();

    // config.print();

    // Feed the raw read buffer into HttpRequest's incremental parser.
    // setMaxBodySize must be called first so the parser can reject oversized
    // bodies with 413 before allocating them. processData handles method
    // extraction, header parsing, and body accumulation, setting error codes
    // (400, 413, 501) internally if anything is malformed:
    HttpRequest request;
    request.setMaxBodySize(config.clientMaxBodySize);
    const std::string& raw = client.getReadBuffer();
    request.processData(raw.c_str(), raw.size());

    if (!request.isError()) {
        std::ostringstream oss_req_log;
        oss_req_log << request.getMethod() << " " << request.getPath();
        if (!request.getQuery().empty()) oss_req_log << "?" << request.getQuery();
        LOG_INFO(oss_req_log.str());
    }

    // ErrorHandler is constructed here (outside the branches) so it can be
    // used for both parse errors and validation errors without duplicating
    // the config.errorPages lookup:
    ErrorHandler eh(config.errorPages);
    Handler handler(config);

    std::string responseStr;
    if (request.isError()) {
        // Parsing failed — send the appropriate HTTP error response (400/413/501)
        // that HttpRequest set during parsing. No point validating further:
        responseStr = eh.makeError(request.getErrorCode()).toString();
    } else {
        // Parsing succeeded. Now check the request against the server config:
        // - 404 if no route matches the path
        // - 405 if the matched route doesn't allow the method
        int validationError = RequestValidator::validate(request, config);
        if (validationError != 0) {
            responseStr = eh.makeError(validationError).toString();
        } else {
            // Request is structurally valid and matches a configured route —
            // dispatch to the handler to build the actual response:
            responseStr = handler.handle_request(request).toString();
        }
    }

    // Log the status line of the response (first line before \r\n):
    size_t status_end = responseStr.find("\r\n");
    LOG_INFO(status_end != std::string::npos ? responseStr.substr(0, status_end) : responseStr);

    client.setWriteBuffer(responseStr);

    // Stop watching for reads, start watching for writes
    _fds[index].events = POLLOUT;
}

void ServerHub::handleWrite(size_t index) {
    int client_fd = _fds[index].fd;
    Client& client = _clients.at(client_fd);

    const std::string& buf = client.getWriteBuffer();
    ssize_t bytes_sent = send(client_fd, buf.c_str(), buf.size(), 0);

    if (bytes_sent < 0) {
        // if send error — disconnect
        disconnectClient(index);
        return;
    }
    if (bytes_sent > 0) client.clearWriteBuffer(bytes_sent);  // clear what was sent
    // bytes_sent == 0: send buffer full, try again next POLLOUT

    if (client.getWriteBuffer().empty()) disconnectClient(index);  // response fully sent
}

void ServerHub::disconnectClient(size_t index) {
    int client_fd = _fds[index].fd;
    std::ostringstream oss_disc;
    oss_disc << "Client on fd " << client_fd << " disconnected by the server";
    LOG_DEBUG(oss_disc.str());
    close(client_fd);  // ServerHub owns the fd lifecycle
    _clients.erase(client_fd);
    _fds.erase(_fds.begin() + index);
}

void ServerHub::checkTimeouts() {
    time_t now = time(NULL);

    for (size_t i = _servers.size(); i < _fds.size();) {
        int client_fd = _fds[i].fd;
        Client& client = _clients.at(client_fd);

        if (now - client.getLastActive() > TIMEOUT) {
            std::ostringstream oss_timeout;
            oss_timeout << "Client on fd " << client_fd << " timed out";
            LOG_DEBUG(oss_timeout.str());
            disconnectClient(i);
        } else {
            ++i;
        }
    }
}

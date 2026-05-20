/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/05 21:22:39 by tsemenov          #+#    #+#             */
/*   Updated: 2026/05/19 23:09:09 by tsemenov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

#include <sys/socket.h>  // recv
#include <unistd.h>      // close

#include <cstdlib>  // strtol
#include <iostream>

#include "utils.hpp"

int Client::_nextIndex = 0;

Client::Client() : _index(++_nextIndex), _fd(-1), _serverIndex(0), _lastActive(0) {}
Client::Client(const Client& other)
    : _index(other._index),
      _fd(other._fd),
      _serverIndex(other._serverIndex),
      _readBuffer(other._readBuffer),
      _writeBuffer(other._writeBuffer),
      _lastActive(other._lastActive) {}
Client& Client::operator=(const Client& other) {
    (void)other;
    return *this;
}

Client::Client(int fd, size_t serverIndex)
    : _index(++_nextIndex), _fd(fd), _serverIndex(serverIndex), _lastActive(time(NULL)) {}

Client::~Client() {}

// reads and appends the message
// returns true if data was received; false on disconnect or error
bool Client::isMsgReceived() {
    char buf[4096];
    ssize_t bytes_read = recv(_fd, buf, sizeof(buf) - 1, 0);

    if (bytes_read < 0) {
        std::cerr << "recv failed on fd " << _fd << std::endl;
        return false;
    }
    if (bytes_read == 0) {
        std::cout << "Client fd " << _fd << " disconnected" << std::endl;
        return false;
    }
    _readBuffer.append(buf, bytes_read);
    // update time so the client_fd is not disconnected by the hub
    _lastActive = time(NULL);
    return true;
}

bool Client::isRequestComplete() {
    // is header received?
    size_t header_end = _readBuffer.find("\r\n\r\n");
    if (header_end == std::string::npos)  // if no ending
        return false;

    // if yes, skip 4 ending symbols, find where the body starts
    size_t body_start = header_end + 4;
    size_t content_len = 0;
    // lowercase the headers section for case-insensitive search
    std::string header = toLowerCase(_readBuffer.substr(0, header_end));

    // look for content-length; read value from original buffer at same position
    static const std::string to_find = "content-length: ";
    size_t len_pos = header.find(to_find);

    if (len_pos != std::string::npos)  // if len found, convert it into long
        content_len = static_cast<size_t>(
            std::strtol(_readBuffer.c_str() + len_pos + to_find.size(), NULL, 10));

    // >= because there can be consecutive requests
    return _readBuffer.size() >= body_start + content_len;
}

int Client::getIndex() const { return _index; }
size_t Client::getServerIndex() const { return _serverIndex; }
const std::string& Client::getReadBuffer() const { return _readBuffer; }
const std::string& Client::getWriteBuffer() const { return _writeBuffer; }
void Client::setWriteBuffer(const std::string& response) { _writeBuffer = response; }
void Client::clearWriteBuffer(size_t bytes) { _writeBuffer.erase(0, bytes); }
time_t Client::getLastActive() const { return _lastActive; }

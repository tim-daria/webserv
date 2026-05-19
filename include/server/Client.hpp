/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/05 21:14:15 by tsemenov          #+#    #+#             */
/*   Updated: 2026/05/19 22:46:32 by tsemenov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <ctime>
#include <string>

class Client {
   private:
    static int _nextIndex;  // shared counter
    int _index;             // unique index for every object
    int _fd;
    size_t _serverIndex;
    std::string _readBuffer;
    std::string _writeBuffer;
    time_t _lastActive;

    Client();

   public:
    Client(int fd, size_t serverIndex);
    Client(const Client& other);
    Client& operator=(const Client& other);
    ~Client();

    bool isMsgReceived();
    bool isRequestComplete();

    int getIndex() const;
    size_t getServerIndex() const;
    time_t getLastActive() const;
    const std::string& getReadBuffer() const;
    const std::string& getWriteBuffer() const;
    void setWriteBuffer(const std::string& response);

    void clearWriteBuffer(size_t bytes);
};

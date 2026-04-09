/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/05 21:14:15 by tsemenov          #+#    #+#             */
/*   Updated: 2026/04/07 23:25:18 by tsemenov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <ctime>
#include <string>

class Client {
   private:
    int _fd;
    std::string _readBuffer;
    std::string _writeBuffer;
    time_t _lastActive;

    Client();

   public:
    Client(int fd);
    Client(const Client& other);
    Client& operator=(const Client& other);
    ~Client();

    bool isMsgReceived();
    bool isRequestComplete();

    time_t get_lastActive() const;
    const std::string& get_readBuffer() const;
    const std::string& get_writeBuffer() const;
    void set_writeBuffer(const std::string& response);

    void clear_writeBuffer(size_t bytes);
};

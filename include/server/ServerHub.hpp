/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerHub.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/06 16:12:53 by tsemenov          #+#    #+#             */
/*   Updated: 2026/04/06 23:23:58 by tsemenov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <vector>

#include "Client.hpp"
#include "Server.hpp"

class ServerHub {
   private:
    std::vector<Server> _servers;
    std::vector<struct pollfd> _fds;
    std::map<int, Client> _clients;

    ServerHub();
    ServerHub(const ServerHub& other);
    ServerHub& operator=(const ServerHub& other);

   public:
    ServerHub(const std::vector<ServerConfig>& configs);
    ~ServerHub();

    void runServers();
    void acceptNewClient(int server_fd);
    void handleRead(size_t index);
    void handleWrite(size_t index);
    void disconnectClient(size_t index);
    void checkTimeouts();
    bool isServerFd(int fd);
};

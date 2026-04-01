/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 16:12:38 by tsemenov          #+#    #+#             */
/*   Updated: 2026/03/31 22:37:19 by tsemenov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <poll.h>

#include <vector>

#include "ServerConfig.hpp"

class Server {
   private:
    int _sockfd;
    ServerConfig _config;
    Server();
    Server(const Server& other);
    Server& operator=(const Server& other);

   public:
    Server(const ServerConfig& config);
    ~Server();

    int get_fd() const;

    void initServ();
    void runServ();
    void acceptNew(std::vector<struct pollfd>& fds);
    void handleClient(std::vector<struct pollfd>& fds, size_t index);
};

// nc localhost 8080

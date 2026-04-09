/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 16:12:38 by tsemenov          #+#    #+#             */
/*   Updated: 2026/04/07 23:42:47 by tsemenov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <netdb.h>  // addrinfo, getaddrinfo

#include <cstddef>

#include "ServerConfig.hpp"

class Server {
   private:
    int _sockfd;
    ServerConfig _config;

    Server();

    // init helpers:
    struct addrinfo* createAddress(int port);
    void createSocket(struct addrinfo* addr);
    void configureSocket();
    void bindAndListen(struct addrinfo* addr);

   public:
    Server(const ServerConfig& config);
    Server(const Server& other);
    Server& operator=(const Server& other);
    ~Server();

    int get_fd() const;
    const ServerConfig& getConfig() const;

    void initServ(size_t index);
};

// nc localhost 8080

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 16:12:38 by tsemenov          #+#    #+#             */
/*   Updated: 2026/03/16 16:55:16 by tsemenov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>       // addrinfo, getaddrinfo
#include <netinet/in.h>  // sockaddr_in
#include <poll.h>
#include <sys/socket.h>  // socket, bind, listen
#include <unistd.h>      // close

#include <cstring>  // strerror
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>  // runtime_error
#include <string>
#include <vector>

class Server {
   private:
    int _sockfd;
    int _port;

   public:
    Server(int port);
    ~Server();

    int get_fd() const;

    void init_serv();
    // void run_serv();
};

// nc localhost 8080

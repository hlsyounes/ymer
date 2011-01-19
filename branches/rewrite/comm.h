/* -*-C++-*- */
/*
 * Client/Server communication.
 *
 * Copyright (C) 2003--2005 Carnegie Mellon University
 * Copyright (C) 2011 Google Inc
 *
 * This file is part of Ymer.
 *
 * Ymer is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Ymer is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
 * License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ymer; if not, write to the Free Software Foundation,
 * Inc., #59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
#ifndef COMM_H
#define COMM_H

#include <config.h>
#include <stdexcept>


/* Exception raised when server is unavailable. */
struct server_unavailable : public std::runtime_error {
  server_unavailable() : std::runtime_error("server unavailable") {}
};


/* Establishes a server. */
void establish_server(int port);

/* Connects to a server. */
int connect_to_server(const std::string& hostname, int port);
/* Receives a register message from a server. */
void recv_register(int& client_id, unsigned long& seed, int sockfd);
/* Receives an init message from a server. */
void recv_init(int sockfd);
/* Receives a start message from a server. */
void recv_start(double& alpha, double& beta, double& gamma, int sockfd);
/* Receives a stop message from a server. */
bool recv_stop(int sockfd);
/* Sends a sample message to a server. */
void send_sample(int sockfd, int sample);


#endif /* COMM_H */

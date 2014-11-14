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
#include "comm.h"
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <map>
#include <netdb.h>
//#include <cygwin/in.h>
#include <sys/socket.h>


/* Parses a model. */
extern int parse_model();
/* Parses a path formula. */
extern int parse_path_formula();
/* File to parse. */
extern FILE* yyin;

/* Message identifiers. */
enum { REGISTER_MSG, INIT_MSG, START_MSG, STOP_MSG, SAMPLE_MSG };

/* Server socket. */
static int server_socket_ = -1;
/* Registered clients. */
static std::map<int, short> registered_clients_;
/* Next client id. */
static short next_client_id_;


/* Receives a server message of the given length. */
static void recv_from_server(int sockfd, void* buf, int n) {
  int bytes_left = n;
  while (bytes_left > 0) {
    int nbytes = recv(sockfd, buf, bytes_left, 0);
    if (nbytes == -1) {
      throw std::runtime_error(strerror(errno));
    } else if (nbytes == 0) {
      throw server_unavailable();
    } else if (nbytes > bytes_left) {
      throw std::logic_error("received unexpectedly long message");
    } else {
      bytes_left -= nbytes;
    }
  }
}


/* Sends a client message of the given length. */
static void send_to_server(int sockfd, const void* buf, int n) {
  int bytes_left = n;
  while (bytes_left > 0) {
    int nbytes = send(sockfd, buf, bytes_left, 0);
    if (nbytes == -1) {
      throw std::runtime_error(strerror(errno));
    } else if (nbytes == 0) {
      throw server_unavailable();
    } else {
      bytes_left -= nbytes;
    }
  }
}


/* Establishes a server. */
void establish_server(int port) {
  server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket_ == -1) {
    throw std::runtime_error(strerror(errno));
  }
  int yes = 1;
  if (-1 == setsockopt(server_socket_, SOL_SOCKET,
		       SO_REUSEADDR, &yes, sizeof yes)) {
    throw std::runtime_error(strerror(errno));
  }
  sockaddr_in srvaddr;
  srvaddr.sin_family = AF_INET;
  srvaddr.sin_port = htons(port);
  srvaddr.sin_addr.s_addr = INADDR_ANY;
  if (-1 == bind(server_socket_, (sockaddr*) &srvaddr, sizeof srvaddr)) {
    throw std::runtime_error(strerror(errno));
  }
  if (-1 == listen(server_socket_, 100)) {
    throw std::runtime_error(strerror(errno));
  }
  next_client_id_ = 1;
}


/* Connects to a server. */
int connect_to_server(const std::string& hostname, int port) {
  hostent* host = gethostbyname(hostname.c_str());
  if (host == 0) {
      throw std::runtime_error(hstrerror(h_errno));
  }
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    throw std::runtime_error(strerror(errno));
  }
  sockaddr_in srvaddr;
  srvaddr.sin_family = AF_INET;
  srvaddr.sin_port = htons(port);
  srvaddr.sin_addr = *(in_addr*) host->h_addr;
  if (-1 == connect(sockfd, (sockaddr*) &srvaddr, sizeof srvaddr)) {
    throw std::runtime_error(strerror(errno));
  }
  return sockfd;
}


/* Receives a register message from a server. */
void recv_register(int& client_id, unsigned long& seed, int sockfd) {
  int msg_id;
  recv_from_server(sockfd, &msg_id, sizeof msg_id);
  if (msg_id != REGISTER_MSG) {
    throw std::logic_error("expecting register message");
  }
  recv_from_server(sockfd, &client_id, sizeof client_id);
  recv_from_server(sockfd, &seed, sizeof seed);
}


/* Receives an init message from a server. */
void recv_init(int sockfd) {
  int msg_id;
  recv_from_server(sockfd, &msg_id, sizeof msg_id);
  if (msg_id != INIT_MSG) {
    throw std::logic_error("expecting init message");
  }
  int size;
  recv_from_server(sockfd, &size, sizeof size);
  char* buffer = new char[size];
  recv_from_server(sockfd, buffer, size*sizeof *buffer);
#if 0
  yyin = fmemopen(buffer, size, "r");
  if (parse_model() != 0) {
    throw std::logic_error("bad model in init message");
  }
  fclose(yyin);
#endif
  delete buffer;
}


/* Receives a start message from a server. */
void recv_start(double& alpha, double& beta, double& gamma, int sockfd) {
  int msg_id;
  recv_from_server(sockfd, &msg_id, sizeof msg_id);
  if (msg_id != START_MSG) {
    throw std::logic_error("expecting start message");
  }
  recv_from_server(sockfd, &alpha, sizeof alpha);
  recv_from_server(sockfd, &beta, sizeof beta);
  recv_from_server(sockfd, &gamma, sizeof gamma);
  int size;
  recv_from_server(sockfd, &size, sizeof size);
  if (size > 0) {
    /*
     * Get new property.
     */
    char* buffer = new char[size];
    recv_from_server(sockfd, buffer, size*sizeof *buffer);
#if 0
    yyin = fmemopen(buffer, size, "r");
    if (parse_path_formula() != 0) {
      throw std::logic_error("bad property in start message");
    }
    fclose(yyin);
#endif
    delete buffer;
  }
}


/* Receives a stop message from a server. */
bool recv_stop(int sockfd) {
  fd_set read_fds;
  FD_ZERO(&read_fds);
  FD_SET(sockfd, &read_fds);
  timeval timeout = { 0, 0 };
  int result = select(sockfd + 1, &read_fds, 0, 0, &timeout);
  if (result == -1) {
    throw std::runtime_error(strerror(errno));
  } else if (result == 0) {
    return false;
  } else {
    int msg_id;
    recv_from_server(sockfd, &msg_id, sizeof msg_id);
    if (msg_id != STOP_MSG) {
      throw std::logic_error("expecting stop message");
    }
    return true;
  }
}


/* Sends a sample message to a server. */
void send_sample(int sockfd, int sample) {
  int msg_id = SAMPLE_MSG;
  send_to_server(sockfd, &msg_id, sizeof msg_id);
  send_to_server(sockfd, &sample, sizeof sample);
}

/* -*-C++-*- */
/*
 * Client/Server communication.
 *
 * Copyright (C) 2003, 2004 Carnegie Mellon University
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
 *
 * $Id: comm.h,v 2.1 2004-01-25 12:17:37 lorens Exp $
 */
#ifndef COMM_H
#define COMM_H

#include <netinet/in.h>
#include <netdb.h>
#include <map>


/*
 * A client message.
 */
struct ClientMsg {
  typedef enum { REGISTER, SAMPLE } MsgId;

  MsgId id;
  short client_id;
  unsigned long value;
};


/*
 * A server message.
 */
struct ServerMsg {
  typedef enum { START, STOP, QUIT } MsgId;

  MsgId id;
  int value;
};


/*
 * A client table.
 */
struct ClientTable : public std::map<short, sockaddr_in> {
};


/* Sockets for communication. */
extern int client_socket, server_socket;
/* Base port. */
extern int port;
/* Registered clients. */
extern ClientTable registered_clients;
/* Current property. */
extern size_t current_property;


#endif /* COMM_H */

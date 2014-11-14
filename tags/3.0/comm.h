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
 * $Id: comm.h,v 3.1 2004-03-11 20:19:01 lorens Exp $
 */
#ifndef COMM_H
#define COMM_H

#include <cstdlib>


/*
 * A client message.
 */
struct ClientMsg {
  typedef enum { SAMPLE } MsgId;

  MsgId id;
  bool value;
};


/*
 * A server message.
 */
struct ServerMsg {
  typedef enum { REGISTER, START, STOP } MsgId;

  MsgId id;
  int value;
};


/* Sockets for communication. */
extern int server_socket;
/* Current property. */
extern size_t current_property;


#endif /* COMM_H */

/* -*-C++-*- */
/*
 * Reference counters.
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
#ifndef REFCOUNT_H
#define REFCOUNT_H

#include <config.h>


/* ====================================================================== */
/* RCObject */

/*
 * An object with a reference counter.
 */
struct RCObject {
  /* Increases the reference count for the given object. */
  static void ref(const RCObject* o) {
    if (o != 0) {
      o->ref_count_++;
    }
  }

  /* Decreases the reference count for the given object and deletes it
     if the reference count becomes zero. */
  static void deref(const RCObject* o) {
    if (o != 0) {
      o->ref_count_--;
      if (o->ref_count_ == 0) {
	delete o;
      }
    }
  }

  /* Deletes this object. */
  virtual ~RCObject() {}

protected:
  /* Constructs an object with a reference counter. */
  RCObject() : ref_count_(0) {}

  /* Copy constructor. */
  RCObject(const RCObject& o) : ref_count_(0) {}

private:
  /* Reference counter. */
  mutable unsigned long ref_count_;
};


#endif /* REFCOUNT_H */

// Copyright (C) 2003--2005 Carnegie Mellon University
// Copyright (C) 2011--2013 Google Inc
//
// This file is part of Ymer.
//
// Ymer is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Ymer is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Ymer; if not, write to the Free Software Foundation,
// Inc., #59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// A class hierarchy for representing compiled properties that supports the
// visitor pattern.

#ifndef COMPILED_PROPERTY_H_
#define COMPILED_PROPERTY_H_

class CompiledPropertyVisitor;

// Abstract base class for compiled properties that supports the visitor
// pattern.
class CompiledProperty {
 public:
  // Disallow copy and assign.
  CompiledProperty(const CompiledProperty&) = delete;
  CompiledProperty& operator=(const CompiledProperty&) = delete;

  virtual ~CompiledProperty();

  void Accept(CompiledPropertyVisitor* visitor) const;

 protected:
  CompiledProperty();

 private:
  virtual void DoAccept(CompiledPropertyVisitor* visitor) const = 0;
};

// Abstract base class for compiled property visitors.
class CompiledPropertyVisitor {
 protected:
  CompiledPropertyVisitor();
  ~CompiledPropertyVisitor();
};

#endif  // COMPILED_PROPERTY_H_

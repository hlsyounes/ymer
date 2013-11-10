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
// A vector class that takes ownership of its elements.

#ifndef POINTER_VECTOR_H_
#define POINTER_VECTOR_H_

#include <memory>
#include <vector>

template <typename T>
class PointerVector {
 public:
  typedef typename std::vector<std::unique_ptr<const T>>::size_type size_type;

  size_type size() const { return elements_.size(); }
  const T& operator[](int i) const { return *elements_[i]; }
  void push_back(std::unique_ptr<const T>&& element) {
    elements_.push_back(std::move(element));
  }

 private:
  std::vector<std::unique_ptr<const T>> elements_;
};

#endif  // POINTER_VECTOR_H_

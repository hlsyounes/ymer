// Copyright (C) 2011 Google Inc
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
// A class for representing an optional value.

#ifndef OPTIONAL_H_
#define OPTIONAL_H_

#include <type_traits>

#include "glog/logging.h"

// Class for representing an optional value of type T.
template <typename T>
class Optional {
 public:
  // Constructs an optional object with no value.
  Optional();

  // Constructs an optional object with the value constructed from the given
  // arguments.
  template <typename Arg, typename... Args>
  Optional(Arg&& arg, Args&&... args, typename std::enable_if<!std::is_same<Arg, const Optional<T> >::value>::type* = nullptr);

  // Copy constructor.
  Optional(const Optional<T>& optional);
  // Move constructor.
  Optional(Optional<T>&& optional);

  // Copy assignment.
  Optional<T>& operator=(const Optional<T>& optional);
  // Move assignment.
  Optional<T>& operator=(Optional<T>&& optional);

  // Returns true if this optional object has a value.
  explicit operator bool() const;

  // Returns the value of this optional object.  Requires that the object has
  // a value.
  const T& get() const;

 private:
  bool has_value_;
  T value_;
};

template <typename T>
Optional<T>::Optional()
    : has_value_(false) {
}

template <typename T>
template <typename Arg, typename... Args>
Optional<T>::Optional(Arg&& arg, Args&&... args, typename std::enable_if<!std::is_same<Arg, const Optional<T> >::value>::type*)
    : has_value_(true), value_(std::forward<Arg>(arg), std::forward<Args>(args)...) {
}

template <typename T>
Optional<T>::Optional(const Optional<T>& optional)
    : has_value_(optional.has_value_), value_(optional.value_) {
}

template <typename T>
Optional<T>::Optional(Optional<T>&& optional)
    : has_value_(optional.has_value_), value_(std::move(optional.value_)) {
}

template <typename T>
Optional<T>& Optional<T>::operator=(const Optional<T>& optional) {
  has_value_ = optional.has_value_;
  value_ = optional.value_;
  return *this;
}

template <typename T>
Optional<T>& Optional<T>::operator=(Optional<T>&& optional) {
  has_value_ = optional.has_value_;
  value_ = std::move(optional.value_);
  return *this;
}

template <typename T>
Optional<T>::operator bool() const {
  return has_value_;
}

template <typename T>
const T& Optional<T>::get() const {
  CHECK(has_value_);
  return value_;
}

template <typename T>
bool operator<(const Optional<T>& optional1, const Optional<T>& optional2) {
  if (optional1 && optional2) {
    return optional1.get() < optional2.get();
  } else {
    return !optional1 && optional2;
  }
}

#endif  // OPTIONAL_H_

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
// A class for representing an optional value.

#ifndef OPTIONAL_H_
#define OPTIONAL_H_

#include "glog/logging.h"

template <typename T>
class Optional {
 public:
  // Constructs an optional object with no value.
  Optional();

  // Constructs an optional object with the given value.
  Optional(const T& value);
  template <typename U,
            typename = typename std::enable_if<
                !std::is_same<typename std::remove_reference<U>::type,
                              Optional<T>>::value>::type>
  Optional(U&& value);

  Optional(const Optional<T>& optional);
  Optional(Optional<T>&& optional);

  Optional<T>& operator=(const Optional<T>& optional);
  Optional<T>& operator=(Optional<T>&& optional);
  template <typename U,
            typename = typename std::enable_if<
                !std::is_same<typename std::remove_reference<U>::type,
                              Optional<T>>::value>::type>
  Optional<T>& operator=(U&& value);

  ~Optional();

  // Returns true if this optional object has a value.
  bool has_value() const { return has_value_; }

  // Returns the value of this optional object.  Requires that the object has a
  // value.
  const T& value() const;
  T& value();

 private:
  bool has_value_;
  union OptionalValue {
    OptionalValue() {}
    ~OptionalValue() {}
    T value;
  } optional_value_;
};

template <typename T>
Optional<T>::Optional()
    : has_value_(false) {}

template <typename T>
Optional<T>::Optional(const T& value)
    : has_value_(true) {
  new (&optional_value_.value) T(value);
}

template <typename T>
template <typename U, typename>
Optional<T>::Optional(U&& value)
    : has_value_(true) {
  new (&optional_value_.value) T(std::move(value));
}

template <typename T>
Optional<T>::Optional(const Optional<T>& optional)
    : has_value_(optional.has_value_) {
  if (has_value_) {
    new (&optional_value_.value) T(optional.optional_value_.value);
  }
}

template <typename T>
Optional<T>::Optional(Optional<T>&& optional)
    : has_value_(optional.has_value_) {
  if (has_value_) {
    new (&optional_value_.value) T(std::move(optional.optional_value_.value));
  }
}

template <typename T>
Optional<T>& Optional<T>::operator=(const Optional<T>& optional) {
  if (this != &optional) {
    if (has_value_) {
      optional_value_.value.~T();
    }
    has_value_ = optional.has_value_;
    if (has_value_) {
      new (&optional_value_.value) T(optional.optional_value_.value);
    }
  }
  return *this;
}

template <typename T>
Optional<T>& Optional<T>::operator=(Optional<T>&& optional) {
  if (has_value_) {
    optional_value_.value.~T();
  }
  has_value_ = optional.has_value_;
  if (has_value_) {
    new (&optional_value_.value) T(std::move(optional.optional_value_.value));
  }
  return *this;
}

template <typename T>
template <typename U, typename>
Optional<T>& Optional<T>::operator=(U&& value) {
  if (has_value_) {
    optional_value_.value.~T();
  }
  has_value_ = true;
  new (&optional_value_.value) T(std::forward<U>(value));
  return *this;
}

template <typename T>
Optional<T>::~Optional() {
  if (has_value_) {
    optional_value_.value.~T();
  }
}

template <typename T>
const T& Optional<T>::value() const {
  CHECK(has_value_);
  return optional_value_.value;
}

template <typename T>
T& Optional<T>::value() {
  CHECK(has_value_);
  return optional_value_.value;
}

#endif  // OPTIONAL_H_

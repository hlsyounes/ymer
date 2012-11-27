// Copyright (C) 2003--2005 Carnegie Mellon University
// Copyright (C) 2011--2012 Google Inc
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
// Typed values.

#ifndef TYPED_VALUE_H_
#define TYPED_VALUE_H_

#include <ostream>

#include "glog/logging.h"

// Supported value types.
enum class Type { INT, DOUBLE, BOOL };

// A typed value.
class TypedValue {
 public:
  // Constructs a typed value.
  TypedValue(int i) : type_(Type::INT) { value_.i = i; }
  TypedValue(double d) : type_(Type::DOUBLE) { value_.d = d; }
  TypedValue(bool b) : type_(Type::BOOL) { value_.b = b; }

  // Avoid implicit conversion from pointer through TypedValue(bool).
  template <typename T> TypedValue(T*) = delete;

  // Returns the type.
  Type type() const { return type_; }

  // Returns the value, converted to the given type.
  template <typename T> T value() const;

 private:
  Type type_;
  union {
    int i;
    double d;
    bool b;
  } value_;
};

template <typename T>
T TypedValue::value() const {
  switch (type_) {
    case Type::INT:
      return value_.i;
    case Type::DOUBLE:
      return value_.d;
    case Type::BOOL:
      return value_.b;
  }
  LOG(FATAL) << "bad type";
}

// Comparison operators for typed values.
bool operator==(const TypedValue& v1, const TypedValue& v2);
bool operator!=(const TypedValue& v1, const TypedValue& v2);
bool operator<(const TypedValue& v1, const TypedValue& v2);
bool operator<=(const TypedValue& v1, const TypedValue& v2);
bool operator>=(const TypedValue& v1, const TypedValue& v2);
bool operator>(const TypedValue& v1, const TypedValue& v2);

// Arithmetic operators for typed values.
TypedValue operator-(const TypedValue& v);
TypedValue operator+(const TypedValue& v1, const TypedValue& v2);
TypedValue operator-(const TypedValue& v1, const TypedValue& v2);
TypedValue operator*(const TypedValue& v1, const TypedValue& v2);
double operator/(const TypedValue& v1, const TypedValue& v2);
int operator%(const TypedValue& v1, const TypedValue& v2);

// Logical not for typed values.
bool operator!(const TypedValue& v);

// Mathematical functions for typed values.
int floor(const TypedValue& v);
int ceil(const TypedValue& v);
double pow(const TypedValue& v1, const TypedValue& v2);
double log(const TypedValue& v);

// Output operator for typed values.
std::ostream& operator<<(std::ostream& os, const TypedValue& v);

#endif  // TYPED_VALUE_H_

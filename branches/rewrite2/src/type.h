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
// Type information for values, model constants and model variables.

#ifndef TYPE_H_
#define TYPE_H_

#include <cmath>
#include <ostream>
#include <string>

#include "glog/logging.h"

// Supported value and expression types.
enum class Type { INT, DOUBLE, BOOL };

std::ostream& operator<<(std::ostream& os, Type type);

// Returns true if actual_type silently converts to expected_type.  If error
// is non-null, then it is populated with an error message in case of failure.
bool ConvertsToType(Type actual_type, Type expected_type, std::string* error);

// A typed value.
class TypedValue {
 public:
  // Constructs a typed value.
  TypedValue() {}
  TypedValue(int i) : type_(Type::INT) { value_.i = i; }
  TypedValue(double d) : type_(Type::DOUBLE) { value_.d = d; }
  TypedValue(bool b) : type_(Type::BOOL) { value_.b = b; }

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
  LOG(FATAL) << "bad value type";
}

inline bool operator==(const TypedValue& v1, const TypedValue& v2) {
  if (v1.type() == Type::DOUBLE || v2.type() == Type::DOUBLE) {
    return v1.value<double>() == v2.value<double>();
  } else {
    return v1.value<int>() == v2.value<int>();
  }
}

inline bool operator!=(const TypedValue& v1, const TypedValue& v2) {
  return !(v1 == v2);
}

inline bool operator<(const TypedValue& v1, const TypedValue& v2) {
  if (v1.type() == Type::DOUBLE || v2.type() == Type::DOUBLE) {
    return v1.value<double>() < v2.value<double>();
  } else {
    return v1.value<int>() < v2.value<int>();
  }
}

inline bool operator<=(const TypedValue& v1, const TypedValue& v2) {
  if (v1.type() == Type::DOUBLE || v2.type() == Type::DOUBLE) {
    return v1.value<double>() <= v2.value<double>();
  } else {
    return v1.value<int>() <= v2.value<int>();
  }
}

inline bool operator>=(const TypedValue& v1, const TypedValue& v2) {
  return !(v1 < v2);
}

inline bool operator>(const TypedValue& v1, const TypedValue& v2) {
  return !(v1 <= v2);
}

inline TypedValue operator-(const TypedValue& v) {
  if (v.type() == Type::DOUBLE) {
    return -v.value<double>();
  } else {
    return -v.value<int>();
  }
}

inline TypedValue operator+(const TypedValue& v1, const TypedValue& v2) {
  if (v1.type() == Type::DOUBLE || v2.type() == Type::DOUBLE) {
    return v1.value<double>() + v2.value<double>();
  } else {
    return v1.value<int>() + v2.value<int>();
  }
}

inline TypedValue operator-(const TypedValue& v1, const TypedValue& v2) {
  if (v1.type() == Type::DOUBLE || v2.type() == Type::DOUBLE) {
    return v1.value<double>() - v2.value<double>();
  } else {
    return v1.value<int>() - v2.value<int>();
  }
}

inline TypedValue operator*(const TypedValue& v1, const TypedValue& v2) {
  if (v1.type() == Type::DOUBLE || v2.type() == Type::DOUBLE) {
    return v1.value<double>() * v2.value<double>();
  } else {
    return v1.value<int>() * v2.value<int>();
  }
}

inline TypedValue operator/(const TypedValue& v1, const TypedValue& v2) {
  return v1.value<double>() / v2.value<double>();
}

inline TypedValue operator%(const TypedValue& v1, const TypedValue& v2) {
  return v1.value<int>() % v2.value<int>();
}

inline TypedValue operator!(const TypedValue& v) {
  return !v.value<bool>();
}

inline TypedValue floor(const TypedValue& v) {
  return static_cast<int>(floor(v.value<double>()));
}

inline TypedValue ceil(const TypedValue& v) {
  return static_cast<int>(ceil(v.value<double>()));
}

inline TypedValue pow(const TypedValue& v1, const TypedValue& v2) {
  return pow(v1.value<double>(), v2.value<double>());
}

inline TypedValue log(const TypedValue& v) {
  return log(v.value<double>());
}

std::ostream& operator<<(std::ostream& os, const TypedValue& v);

#endif  // TYPE_H_

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

#include "typed-value.h"

#include <cmath>
#include <ostream>

bool operator==(const TypedValue& v1, const TypedValue& v2) {
  return v1.value<double>() == v2.value<double>();
}

bool operator!=(const TypedValue& v1, const TypedValue& v2) {
  return !(v1 == v2);
}

bool operator<(const TypedValue& v1, const TypedValue& v2) {
  return v1.value<double>() < v2.value<double>();
}

bool operator<=(const TypedValue& v1, const TypedValue& v2) {
  return v1.value<double>() <= v2.value<double>();
}

bool operator>=(const TypedValue& v1, const TypedValue& v2) {
  return !(v1 < v2);
}

bool operator>(const TypedValue& v1, const TypedValue& v2) {
  return !(v1 <= v2);
}

TypedValue operator-(const TypedValue& v) {
  if (v.type() == Type::DOUBLE) {
    return -v.value<double>();
  } else {
    return -v.value<int>();
  }
}

TypedValue operator+(const TypedValue& v1, const TypedValue& v2) {
  if (v1.type() == Type::DOUBLE || v2.type() == Type::DOUBLE) {
    return v1.value<double>() + v2.value<double>();
  } else {
    return v1.value<int>() + v2.value<int>();
  }
}

TypedValue operator-(const TypedValue& v1, const TypedValue& v2) {
  if (v1.type() == Type::DOUBLE || v2.type() == Type::DOUBLE) {
    return v1.value<double>() - v2.value<double>();
  } else {
    return v1.value<int>() - v2.value<int>();
  }
}

TypedValue operator*(const TypedValue& v1, const TypedValue& v2) {
  if (v1.type() == Type::DOUBLE || v2.type() == Type::DOUBLE) {
    return v1.value<double>() * v2.value<double>();
  } else {
    return v1.value<int>() * v2.value<int>();
  }
}

double operator/(const TypedValue& v1, const TypedValue& v2) {
  return v1.value<double>() / v2.value<double>();
}

int operator%(const TypedValue& v1, const TypedValue& v2) {
  return v1.value<int>() % v2.value<int>();
}

bool operator!(const TypedValue& v) {
  return !v.value<bool>();
}

int floor(const TypedValue& v) {
  return static_cast<int>(floor(v.value<double>()));
}

int ceil(const TypedValue& v) {
  return static_cast<int>(ceil(v.value<double>()));
}

double pow(const TypedValue& v1, const TypedValue& v2) {
  return pow(v1.value<double>(), v2.value<double>());
}

double log(const TypedValue& v) {
  return log(v.value<double>());
}

std::ostream& operator<<(std::ostream& os, const TypedValue& v) {
  switch (v.type()) {
    case Type::INT:
      return os << v.value<int>();
    case Type::DOUBLE:
      return os << v.value<double>();
    case Type::BOOL:
      return os << (v.value<bool>() ? "true" : "false");
  }
}

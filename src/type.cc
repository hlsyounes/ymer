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

#include "type.h"

#include <ostream>
#include <sstream>
#include <string>

#include "glog/logging.h"

std::ostream& operator<<(std::ostream& os, Type type) {
  switch (type) {
    case Type::INT:
      return os << "int";
    case Type::DOUBLE:
      return os << "double";
    case Type::BOOL:
      return os << "bool";
  }
  LOG(FATAL) << "bad type";
}

namespace {

void SetTypeMismatchError(Type actual, Type expected, std::string* error) {
  if (error) {
    std::ostringstream out;
    out << "type mismatch; expecting " << expected << ", found " << actual;
    *error = out.str();
  }
}

}  // namespace

bool ConvertsToType(Type actual_type, Type expected_type, std::string* error) {
  // We allow implicit conversion from int to double.  Any other implicit
  // conversion is disallowed.
  if (actual_type != expected_type &&
      (actual_type != Type::INT || expected_type != Type::DOUBLE)) {
    SetTypeMismatchError(actual_type, expected_type, error);
    return false;
  }
  return true;
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
  LOG(FATAL) << "bad value type";
}

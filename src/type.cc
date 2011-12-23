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

#include <string>

#include "glog/logging.h"

std::string Type_Name(Type type) {
  switch (type) {
    case Type::INT:
      return "int";
    case Type::DOUBLE:
      return "double";
    case Type::BOOL:
      return "bool";
  }
  LOG(FATAL) << "bad type";
}

bool ConvertsToType(Type actual_type, Type expected_type, std::string* error) {
  // We allow implicit conversion from int to double (just int to bool is
  // disallowed).  For the other types, all implicit conversion is disallowed.
  if ((actual_type == Type::INT && expected_type == Type::BOOL) ||
      (actual_type != Type::INT && expected_type != actual_type)) {
    if (error) {
      *error = "type mismatch; expecting ";
      *error += Type_Name(expected_type);
      *error += ", found ";
      *error += Type_Name(actual_type);
    }
    return false;
  }
  return true;
}

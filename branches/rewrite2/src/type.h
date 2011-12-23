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
// Type information for model constants and variables.

#ifndef TYPE_H_
#define TYPE_H_

#include <string>

// Supported expression types.
enum class Type { INT, DOUBLE, BOOL };

// Returns the name of the given expression type.
std::string Type_Name(Type type);

// Returns true if actual_type silently converts to expected_type.  If error
// is non-null, then it is populated with an error message in case of failure.
bool ConvertsToType(Type actual_type, Type expected_type, std::string* error);

// Class for mapping native type to expression type.
template <typename NativeType> class TypeInfo {
 public:
  TypeInfo() = delete;

  // Returns the expression type associated with NativeType.
  static Type type();
};

template <> inline Type TypeInfo<int>::type() { return Type::INT; }

template <> inline Type TypeInfo<double>::type() { return Type::DOUBLE; }

template <> inline Type TypeInfo<bool>::type() { return Type::BOOL; }

#endif  // TYPE_H_

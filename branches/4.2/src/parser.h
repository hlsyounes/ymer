// Copyright (C) 2003--2005 Carnegie Mellon University
// Copyright (C) 2011--2015 Google Inc
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
// Functions for parsing the PRISM language.

#ifndef PARSER_H_
#define PARSER_H_

#include <string>
#include <vector>

#include "expression.h"
#include "model.h"
#include "optional.h"
#include "unique-ptr-vector.h"

// Struct for storing the result of successfully parsing a file of string.
struct ModelAndProperties {
  Optional<Model> model;
  UniquePtrVector<const Expression> properties;
};

// Parses the file with the given name.  If filename is "-", then input is read
// from stdin.  Returns true on success and populates result.  Returns false on
// failure and adds error messages to errors.
bool ParseFile(const std::string& filename, ModelAndProperties* result,
               std::vector<std::string>* errors);

// Parses the given string buffer.  Returns true on success and populates
// result.  Returns false on failure and adds error messages to errors.
bool ParseString(const std::string& buffer, ModelAndProperties* result,
                 std::vector<std::string>* errors);

#endif  // PARSER_H_

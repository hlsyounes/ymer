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
// Functions for parsing the PRISM language.

#ifndef PARSE_H_
#define PARSE_H_

#include <string>

class Model;

// Parses the file with the given name.  If filename is "-", then input is
// read from stdin.  Returns true on success and populates model.  If message
// is non-null, then it is populated with any errors or warnings that occur
// when parsing the file.
bool ParseFile(const std::string& filename, Model* model, std::string* message);

// Parses the given string buffer.  Returns true on success and populates
// model.  If message is non-null, then it is populated with any errors or
// warnings that occur when parsing the buffer.
bool ParseString(const std::string& buffer, Model* model, std::string* message);

#endif  // PARSE_H_

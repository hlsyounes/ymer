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

#include "parse.h"

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <functional>
#include <sstream>
#include <string>

#include "glog/logging.h"

#include "model.h"
#include "parser-state.h"

struct yy_buffer_state;

// Init function for scanner.
extern int yylex_init(void** scanner_ptr);
// Destroy function for scanner.
extern int yylex_destroy(void* scanner);
// Creates a scanner buffer for the given file.
extern yy_buffer_state* yy_create_buffer(FILE* file, int size, void* scanner);
// Switches to the given scanner buffer.
extern void yy_switch_to_buffer(yy_buffer_state* buf, void* scanner);
// Sets up scanner to read from a string buffer.
extern yy_buffer_state* yy_scan_string(const char* str, void* scanner);
// Deletes a scanner buffer.
extern void yy_delete_buffer(yy_buffer_state* buf, void* scanner);
// Parse function.
extern int yyparse(void* scanner, ParserState* state);

namespace {

// Shared implementation for ParseFile() and ParseString().
bool ParseImpl(const std::function<yy_buffer_state*(void*)>& create_buffer,
               const std::string* filename,
               Model* model,
               std::string* message) {
  CHECK(model);

  void* scanner;
  yylex_init(&scanner);
  yy_buffer_state* buffer_state = create_buffer(scanner);
  ParserState state = { filename, model, true, message };
  bool success = (yyparse(scanner, &state) == 0);
  yy_delete_buffer(buffer_state, scanner);
  yylex_destroy(scanner);
  if (success && model->type() == ModelType::NONE) {
    CHECK(model->SetType(ModelType::MDP));
  }
  return success;
}

// Prepares a scanner to scan the given file.
yy_buffer_state* ScanFile(FILE* file, void* scanner) {
  yy_buffer_state* buffer_state = yy_create_buffer(file, 32768, scanner);
  yy_switch_to_buffer(buffer_state, scanner);
  return buffer_state;
}

void SetCannotOpenFileError(const std::string& filename, std::string* error) {
  if (error) {
    std::stringstream out;
    out << "Cannot open " << filename << ": " << strerror(errno);
    *error = out.str();
  }
}

}  // namespace

bool ParseFile(const std::string& filename,
               Model* model, std::string* message) {
  FILE* file;
  if (filename == "-") {
    file = stdin;
  } else {
    file = fopen(filename.c_str(), "r");
    if (file == nullptr) {
      SetCannotOpenFileError(filename, message);
      return false;
    }
  }

  bool success = ParseImpl(
      std::bind(ScanFile, file, std::placeholders::_1),
      &filename, model, message);

  if (file != stdin) {
    fclose(file);
  }

  return success;
}

bool ParseString(const std::string& buffer,
                 Model* model, std::string* message) {
  return ParseImpl(
      std::bind(yy_scan_string, buffer.c_str(), std::placeholders::_1),
      nullptr, model, message);
}

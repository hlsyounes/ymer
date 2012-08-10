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
// A compiled model.

#ifndef COMPILED_MODEL_H_
#define COMPILED_MODEL_H_

#include <string>
#include <vector>

// A compiled variable.
class CompiledVariable {
 public:
  // Constructs a compiled variable with the given name, value range, and
  // initial value.
  CompiledVariable(
      const std::string& name, int min_value, int max_value, int init_value);

  // Returns the name for this compiled variable.
  const std::string& name() const { return name_; }

  // Returns the minimum value for this compiled variable.
  int min_value() const { return min_value_; }

  // Returns the maximum value for this compiled variable.
  int max_value() const { return max_value_; }

  // Returns the initial value for this compiled variable.
  int init_value() const { return init_value_; }

 private:
  std::string name_;
  int min_value_;
  int max_value_;
  int init_value_;
};

// A compiled model.
class CompiledModel {
 public:
  // Adds a variable to this compiled model.
  void AddVariable(
      const std::string& name, int min_value, int max_value, int init_value);

  // Returns the variables for this compiled model.
  const std::vector<CompiledVariable>& variables() const { return variables_; }

 private:
  std::vector<CompiledVariable> variables_;
};

#endif  // COMPILED_MODEL_H_

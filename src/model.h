// Copyright (C) 2003--2005 Carnegie Mellon University
// Copyright (C) 2011--2014 Google Inc
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
// Classes to represent a PRISM model.

#ifndef MODEL_H_
#define MODEL_H_

#include <memory>
#include <ostream>
#include <string>

#include "expression.h"

// A variable update.
class Update {
 public:
  // Constructs a variable update.
  Update(const std::string& variable, std::unique_ptr<const Expression>&& expr);

  // Returns the variable for this update.
  const std::string& variable() const { return variable_; }

  // Returns the expression for this update.
  const Expression& expr() const { return *expr_; }

 private:
  std::string variable_;
  std::unique_ptr<const Expression> expr_;
};

// Output operator for updates.
std::ostream& operator<<(std::ostream& os, const Update& update);

// Supported parsed model types.
enum class ModelType { DEFAULT, MDP, DTMC, CTMC, GSMP };

// Output operator for model types.
std::ostream& operator<<(std::ostream& os, ModelType model_type);

#endif  // MODEL_H_

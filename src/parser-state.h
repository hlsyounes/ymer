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
// Internal state of parser.

#ifndef PARSER_STATE_H_
#define PARSER_STATE_H_

#include <optional>
#include <string>
#include <vector>

#include "model.h"
#include "strutil.h"

class ParserState {
 public:
  ParserState(const std::optional<std::string>& filename,
              std::vector<std::string>* errors)
      : filename_(filename), errors_(errors), success_(true) {}

  Model* mutable_model() {
    if (!model_.has_value()) {
      model_ = Model();
    }
    return &model_.value();
  }

  void add_property(std::unique_ptr<const Expression>&& property) {
    properties_.push_back(std::move(property));
  }

  void add_error(const std::string& error) {
    if (!filename_.has_value() || filename_.value() == "-") {
      errors_->push_back(error);
    } else {
      errors_->push_back(StrCat(filename_.value(), ":", error));
    }
    success_ = false;
  }

  bool has_model() const { return model_.has_value(); }

  Model release_model() { return std::move(model_.value()); }

  UniquePtrVector<const Expression> release_properties() {
    return std::move(properties_);
  }

  bool success() const { return success_; }

 private:
  const std::optional<std::string> filename_;
  std::vector<std::string>* const errors_;
  std::optional<Model> model_;
  UniquePtrVector<const Expression> properties_;
  bool success_;
};

#endif  // PARSER_STATE_H_

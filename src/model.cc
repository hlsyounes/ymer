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

#include "model.h"

#include "glog/logging.h"

Update::Update(const std::string& variable,
               std::unique_ptr<const Expression>&& expr)
    : variable_(variable), expr_(std::move(expr)) {}

std::ostream& operator<<(std::ostream& os, const Update& update) {
  return os << update.variable() << "' = " << update.expr();
}

std::ostream& operator<<(std::ostream& os, ModelType model_type) {
  switch (model_type) {
    case ModelType::DEFAULT:
    case ModelType::MDP:
      return os << "mdp";
    case ModelType::DTMC:
      return os << "dtmc";
    case ModelType::CTMC:
      return os << "ctmc";
    case ModelType::GSMP:
      return os << "gsmp";
  }
  LOG(FATAL) << "bad model type";
}

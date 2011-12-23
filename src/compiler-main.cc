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
// A simple PRISM compiler.

#include <algorithm>
#include <cstdio>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include "gflags/gflags.h"
#include "glog/logging.h"

#include "compiled-model.h"
#include "expression.h"
#include "model.h"
#include "parse.h"

DEFINE_string(const, "", "Value assignments for constants.");

namespace {

// Parses spec for const overrides.  Returns true on success.
bool ParseConstOverrides(
    const std::string& spec,
    std::map<std::string,
             std::unique_ptr<const ParsedExpression> >* const_overrides) {
  if (spec.empty()) {
    return true;
  }
  auto comma = spec.begin() - 1;
  while (comma != spec.end()) {
    auto next_comma = find(comma + 1, spec.end(), ',');
    auto assignment = find(comma + 1, next_comma, '=');
    if (assignment == next_comma) {
      return false;
    }
    const std::string name(comma + 1, assignment);
    const std::string value(assignment + 1, next_comma);
    std::unique_ptr<const ParsedExpression> expr;
    if (value == "true" || value == "false") {
      expr = ParsedLiteral::Create(value == "true");
    } else {
      char* end;
      int int_value = strtol(value.c_str(), &end, 10);
      if (*end == '\0' && end != value.c_str()) {
        expr = ParsedLiteral::Create(int_value);
      } else {
        double double_value = strtod(value.c_str(), &end);
        if (*end == '\0' && end != value.c_str()) {
          expr = ParsedLiteral::Create(double_value);
        } else {
          return false;
        }
      }
    }
    CHECK(expr);
    if (!const_overrides->insert(
            std::make_pair(name, std::move(expr))).second) {
      return false;
    }
    comma = next_comma;
  }
  return true;
}

int NumCommands(const ActionToCommandsMap& commands) {
  int n = 0;
  for (const auto& p: commands) {
    n += p.second.size();
  }
  return n;
}

std::string ModuleSizes(const CompiledModel& compiled_model) {
  std::ostringstream out;
  if (compiled_model.num_modules() > 0) {
    out << NumCommands(compiled_model.commands(0));
    for (int i = 1; i < compiled_model.num_modules(); ++i) {
      out << "x" << NumCommands(compiled_model.commands(i));
    }
  }
  return out.str();
}

}  // namespace

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true /* remove_flags */);
  google::InstallFailureSignalHandler();
  google::InitGoogleLogging(argv[0]);
  std::string filename = (argc > 1) ? argv[1] : "-";
  Model parsed_model;
  std::string message;
  bool success = ParseFile(filename, &parsed_model, &message);
  if (!message.empty()) {
    fprintf(stderr, "%s\n", message.c_str());
  }
  if (success) {
    std::map<std::string,
             std::unique_ptr<const ParsedExpression> > owned_const_overrides;
    if (!ParseConstOverrides(FLAGS_const, &owned_const_overrides)) {
      fprintf(stderr, "bad --const specification '%s'\n", FLAGS_const.c_str());
      return 1;
    }
    std::map<std::string, const ParsedExpression*> const_overrides;
    for (const auto& p: owned_const_overrides) {
      const_overrides.insert({ p.first, p.second.get() });
    }
    std::string error;
    CompiledModel compiled_model =
        CompiledModel::Make(parsed_model, const_overrides, &error);
    if (compiled_model.is_valid()) {
      if (VLOG_IS_ON(1)) {
        printf("%s:\ntype: %s; variables: %d; modules %d (%s)\n",
               filename.c_str(),
               ModelType_Name(compiled_model.model_type()).c_str(),
               compiled_model.num_variables(),
               compiled_model.num_modules(),
               ModuleSizes(compiled_model).c_str());
      }
    } else {
      fprintf(stderr, "%s\n", error.c_str());
      success = false;
    }
  }
  return success ? 0 : 1;
}

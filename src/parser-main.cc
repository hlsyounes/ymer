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
// A simple PRISM parser.

#include <cstdio>
#include <string>

#include "gflags/gflags.h"
#include "glog/logging.h"

#include "model.h"
#include "parse.h"

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true /* remove_flags */);
  google::InitGoogleLogging(argv[0]);
  Model model;
  std::string message;
  bool success = ParseFile((argc > 1) ? argv[1] : "-", &model, &message);
  if (!message.empty()) {
    fprintf(stderr, "%s\n", message.c_str());
  }
  return success ? 0 : 1;
}

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
// Random number generator engine.

#ifndef RNG_H_
#define RNG_H_

#include <memory>
#include <random>

#include "dc.h"

// A random number generator engine that supports dynamic creation of Mersenne
// Twister engines.
class DCEngine {
 public:
  typedef uint32_t result_type;

  DCEngine();
  DCEngine(int id, result_type seed);

  DCEngine(const DCEngine&) = delete;
  DCEngine& operator=(const DCEngine&) = delete;

  static result_type min();
  static result_type max();

  void seed(result_type value);
  result_type operator()();

 private:
  std::unique_ptr<mt_struct> mts_;
  std::mt19937 default_engine_;
};

template <typename Engine>
double StandardUniform(Engine& engine) {
  return (engine() - engine.min()) / (engine.max() - engine.min() + 1.0);
}

#endif  // RNG_H_

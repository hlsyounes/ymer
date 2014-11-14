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

#include "rng.h"

#include "dc.h"

DCEngine::DCEngine() = default;

DCEngine::DCEngine(int id, result_type seed)
    : mts_(get_mt_parameter_id_st(32, 521, id, seed)) {
}

DCEngine::result_type DCEngine::min() {
  return 0;
}

DCEngine::result_type DCEngine::max() {
  return 0xffffffff;
}

void DCEngine::seed(result_type value) {
  if (mts_) {
    sgenrand_mt(value, mts_.get());
  } else {
    default_engine_.seed(value);
  }
}

DCEngine::result_type DCEngine::operator()() {
  if (mts_) {
    return genrand_mt(mts_.get());
  } else {
    return default_engine_();
  }
}

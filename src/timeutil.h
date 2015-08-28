// Copyright (C) 2003--2005 Carnegie Mellon University
// Copyright (C) 2011--2015 Google Inc
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
// Utility functions for measuring time.

#ifndef TIMEUTIL_H_
#define TIMEUTIL_H_

#include <chrono>

template <typename Clock = std::chrono::steady_clock>
class Timer {
 public:
  Timer() : start_(Clock::now()) {}

  double GetElapsedSeconds() const {
    return std::chrono::duration<double>(Clock::now() - start_).count();
  }

 private:
  typename Clock::time_point start_;
};

#endif  // TIMEUTIL_H_

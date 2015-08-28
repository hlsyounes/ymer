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

#include "timeutil.h"

#include <chrono>
#include <ratio>

#include "gtest/gtest.h"

namespace {

class FakeClock {
 public:
  using rep = int;
  using period = std::centi;
  using duration = std::chrono::duration<rep, period>;
  using time_point = std::chrono::time_point<FakeClock>;

  static time_point now() { return time_point(duration(value)); }

  static int value;
};

int FakeClock::value = 0;

TEST(TimerTest, FakeClock) {
  Timer<FakeClock> timer1;
  EXPECT_EQ(0, timer1.GetElapsedSeconds());
  FakeClock::value = 50;
  Timer<FakeClock> timer2;
  EXPECT_EQ(0.5, timer1.GetElapsedSeconds());
  EXPECT_EQ(0, timer2.GetElapsedSeconds());
  FakeClock::value = 75;
  EXPECT_EQ(0.75, timer1.GetElapsedSeconds());
  EXPECT_EQ(0.25, timer2.GetElapsedSeconds());
  FakeClock::value = 200;
  EXPECT_EQ(2, timer1.GetElapsedSeconds());
  EXPECT_EQ(1.5, timer2.GetElapsedSeconds());
}

TEST(TimerTest, RealClock) {
  Timer<> timer;
  EXPECT_LE(0, timer.GetElapsedSeconds());
}

}  // namespace

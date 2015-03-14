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
// Utility functions for string manipulation.

#ifndef STRUTIL_H_
#define STRUTIL_H_

#include <sstream>
#include <string>
#include <utility>

namespace internal {

inline void StrCatImpl(std::ostream& out) {}

template <typename Arg, typename... Args>
void StrCatImpl(std::ostream& out, Arg&& arg, Args&&... args) {
  out << arg;
  StrCatImpl(out, std::forward<Args>(args)...);
}

}  // namespace internal

template <typename... Args>
std::string StrCat(Args&&... args) {
  std::ostringstream out;
  internal::StrCatImpl(out, std::forward<Args>(args)...);
  return out.str();
}

#endif  // STRUTIL_H_

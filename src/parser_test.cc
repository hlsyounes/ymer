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

#include "parser.h"

#include <cstdlib>
#include <regex>

#include "strutil.h"

#include "gtest/gtest.h"

namespace {

bool MatchesRegex(const std::string& r, const std::string& s) {
  return regex_match(s, std::regex(r));
}

std::string GetTestPath(const std::string& path) {
  return StrCat(CHECK_NOTNULL(getenv("TEST_SRCDIR")), '/', path);
}

TEST(ParseStringTest, ParsesValidModel) {
  const std::string kModelSource =
      "// A comment.\n"
      "dtmc\n"
      "const int c1 = 17;\n"
      "const int c2 = 42;\n"
      "const bool k;\n"
      "const double K = 0.5;\n"
      "global i : [17..42] init 17;\n"
      "global b : bool init true;\n"
      "global j : [0..1];\n"
      "formula f1 = 17;\n"
      "formula f2 = 42;\n"
      "formula h = true;\n"
      "formula g = 3.14159;\n"
      "module M1\n"
      "  i1 : [0..c1] init f1;\n"
      "  b1 : bool;\n"
      "  [a1] i1<c1 -> 1/i : (i1'=i1+1);\n"
      "endmodule\n"
      "module M2 = M1 [ c1=c2, i1=i2, b1=b2, a1=a2 ]\n"
      "endmodule\n"
      "label \"l\" = f1;\n"
      "rewards\n"
      "  i1=c1 : 1.0;\n"
      "endrewards\n"
      "rewards \"r\"\n"
      "  [a2] true : 0.25;\n"
      "endrewards\n"
      "system\n"
      "  M1 {a1<-a3} ||| M2\n"
      "endsystem\n";

  ModelAndProperties result;
  std::vector<std::string> errors;
  EXPECT_TRUE(ParseString(kModelSource, &result, &errors));
  EXPECT_TRUE(errors.empty());
  ASSERT_TRUE(result.model.has_value());
  EXPECT_EQ(
      "dtmc\n\n"
      "const int c1 = 17;\n"
      "const int c2 = 42;\n"
      "const bool k;\n"
      "const double K = 0.5;\n\n"
      "global i : [17..42] init 17;\n"
      "global b : bool init true;\n"
      "global j : [0..1];\n\n"
      "formula f1 = 17;\n"
      "formula f2 = 42;\n"
      "formula h = true;\n"
      "formula g = 3.14159;\n\n"
      "module M1\n"
      "  i1 : [0..c1] init f1;\n"
      "  b1 : bool;\n\n"
      "  [a1] i1 < c1 -> 1 / i : (i1' = i1 + 1);\n"
      "endmodule\n\n"
      "module M2\n"
      "  i2 : [0..c2] init 17;\n"
      "  b2 : bool;\n\n"
      "  [a2] i2 < c2 -> 1 / i : (i2' = i2 + 1);\n"
      "endmodule\n\n"
      "label \"l\" = f1;\n\n"
      "rewards\n"
      "  i1 = c1 : 1;\n"
      "endrewards\n\n"
      "rewards \"r\"\n"
      "  [a2] true : 0.25;\n"
      "endrewards",
      StrCat(result.model.value()));
}

TEST(ParseStringTest, ReportsErrorForInvalidModel) {
  const std::string kModelSource =
      "module M\n"
      "  true -> (b'=!b);\n"  // Missing action specification.
      "endmodule\n";
  ModelAndProperties result;
  std::vector<std::string> errors;
  EXPECT_FALSE(ParseString(kModelSource, &result, &errors));
  ASSERT_EQ(1U, errors.size());
  EXPECT_PRED2(MatchesRegex, "2:syntax error.*", errors[0]);
}

TEST(ParseFileTest, ParsesValidModel) {
  ModelAndProperties result;
  std::vector<std::string> errors;
  EXPECT_TRUE(
      ParseFile(GetTestPath("src/testdata/example.sm"), &result, &errors));
  EXPECT_TRUE(errors.empty());
  ASSERT_TRUE(result.model.has_value());
  EXPECT_EQ(
      "ctmc\n\n"
      "module M\n"
      "  b : bool;\n\n"
      "  [] true -> 1 : (b' = !b);\n"
      "endmodule",
      StrCat(result.model.value()));
}

TEST(ParseFileTest, MissingFile) {
  ModelAndProperties result;
  std::vector<std::string> errors;
  EXPECT_FALSE(
      ParseFile(GetTestPath("src/testdata/missing.sm"), &result, &errors));
  EXPECT_EQ(
      std::vector<std::string>({StrCat(GetTestPath("src/testdata/missing.sm"),
                                       ":No such file or directory")}),
      errors);
}

TEST(ParseFileTest, ReportsErrorForInvalidModel) {
  ModelAndProperties result;
  std::vector<std::string> errors;
  EXPECT_FALSE(
      ParseFile(GetTestPath("src/testdata/invalid.pm"), &result, &errors));
  ASSERT_EQ(1U, errors.size());
  EXPECT_PRED2(MatchesRegex, StrCat(GetTestPath("src/testdata/invalid.pm"),
                                    ":4:syntax error.*"),
               errors[0]);
}

}  // namespace

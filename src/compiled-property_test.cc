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

#include "compiled-property.h"

#include "strutil.h"

#include "gtest/gtest.h"

namespace {

std::unique_ptr<const CompiledExpressionProperty>
NewCompiledExpressionProperty(int variable) {
  return CompiledExpressionProperty::New(
      CompiledExpression({Operation::MakeILOAD(variable, 0)}));
}

CompiledExpression UnoptimizedCompiledExpression() {
  return CompiledExpression({
      Operation::MakeICONST(1, 0),
      Operation::MakeICONST(0, 0)
 });
}

std::unique_ptr<const CompiledProbabilityThresholdProperty>
NewCompiledProbabilityThresholdProperty(int base_variable) {
  return CompiledProbabilityThresholdProperty::New(
      CompiledProbabilityThresholdOperator::GREATER, 0.25,
      CompiledUntilProperty::New(
          17, 42,
          NewCompiledExpressionProperty(base_variable),
          NewCompiledExpressionProperty(base_variable + 1),
          0, "don't care", nullptr));
}

UniquePtrVector<const CompiledProperty> MakeConjuncts(
    std::unique_ptr<const CompiledProperty> operand1,
    std::unique_ptr<const CompiledProperty> operand2) {
  return UniquePtrVector<const CompiledProperty>(std::move(operand1),
                                                 std::move(operand2));
}

#if 0
TEST(OptimizePropertyTest, OptimizesEmptyAnd) {
  const CompiledNaryProperty property = CompiledNaryProperty(
      CompiledNaryOperator::AND,
      CompiledExpression({}), UniquePtrVector<const CompiledProperty>());
  const std::string expected = "0: ICONST 1 0";
  EXPECT_EQ(expected, StrCat(*OptimizeProperty(property)));
}

TEST(OptimizePropertyTest, OptimizesExpressionConjunct) {
  const CompiledNaryProperty property = CompiledNaryProperty(
      CompiledNaryOperator::AND,
      UnoptimizedCompiledExpression(),
      MakeConjuncts(NewCompiledProbabilityThresholdProperty(0),
                    NewCompiledProbabilityThresholdProperty(2)));
  const std::string expected =
      "AND of 3 operands\n"
      "operand 0:\n"
      "0: ICONST 0 0\n"
      "operand 1:\n"
      "P > 0.25\n"
      "UNTIL [17, 42]\n"
      "pre:\n"
      "0: ILOAD 0 0\n"
      "post:\n"
      "0: ILOAD 1 0\n"
      "operand 2:\n"
      "P > 0.25\n"
      "UNTIL [17, 42]\n"
      "pre:\n"
      "0: ILOAD 2 0\n"
      "post:\n"
      "0: ILOAD 3 0";
  EXPECT_EQ(expected, StrCat(*OptimizeProperty(property)));
}

TEST(OptimizePropertyTest, OptimizesLogicOperators) {
  const CompiledNotProperty property1 = CompiledNotProperty(
      NewCompiledExpressionProperty(0));
  const std::string expected1 =
      "0: ILOAD 0 0\n"
      "1: NOT 0";
  EXPECT_EQ(expected1, StrCat(*OptimizeProperty(property1)));
  const CompiledNotProperty property2 = CompiledNotProperty(
      CompiledNotProperty::New(
          NewCompiledProbabilityThresholdProperty(0)));
  const std::string expected2 =
      "P > 0.25\n"
      "UNTIL [17, 42]\n"
      "pre:\n"
      "0: ILOAD 0 0\n"
      "post:\n"
      "0: ILOAD 1 0";
  EXPECT_EQ(expected2, StrCat(*OptimizeProperty(property2)));
  const CompiledNaryProperty property3 = CompiledNaryProperty(
      CompiledNaryOperator::AND,
      CompiledExpression({}),
      MakeConjuncts(
          NewCompiledExpressionProperty(0),
          CompiledNotProperty::New(
              CompiledNotProperty::New(
                  CompiledNaryProperty::New(
                      CompiledNaryOperator::AND,
                      CompiledExpression({}),
                      MakeConjuncts(
                          CompiledNotProperty::New(
                              NewCompiledProbabilityThresholdProperty(1)),
                          NewCompiledExpressionProperty(3)))))));
  const std::string expected3 =
      "AND of 2 operands\n"
      "operand 0:\n"
      "0: ILOAD 0 0\n"
      "1: IFFALSE 0 3\n"
      "2: ILOAD 3 0\n"
      "operand 1:\n"
      "NOT of:\n"
      "P > 0.25\n"
      "UNTIL [17, 42]\n"
      "pre:\n"
      "0: ILOAD 1 0\n"
      "post:\n"
      "0: ILOAD 2 0";
  EXPECT_EQ(expected3, StrCat(*OptimizeProperty(property3)));
}
#endif

}  // namespace

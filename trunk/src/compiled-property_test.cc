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

#if 0
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
#endif

TEST(CompilePropertyTest, Literal) {
  const DecisionDiagramManager dd_manager(0);
  const CompilePropertyResult result1 =
      CompileProperty(Literal(true), {}, dd_manager);
  const std::string expected1 = "0: ICONST 1 0";
  EXPECT_EQ(expected1, StrCat(*result1.property));
  const CompilePropertyResult result2 =
      CompileProperty(Literal(17), {}, dd_manager);
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found int"}),
      result2.errors);
  const CompilePropertyResult result3 =
      CompileProperty(Literal(0.5), {}, dd_manager);
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found double"}),
      result3.errors);
}

TEST(CompilePropertyTest, Identifier) {
  const DecisionDiagramManager dd_manager(4);
  std::map<std::string, IdentifierInfo> identifiers_by_name = {
      {"a", IdentifierInfo::Variable(Type::INT, 0)},
      {"b", IdentifierInfo::Variable(Type::BOOL, 1)},
      {"c", IdentifierInfo::Constant(17)},
      {"d", IdentifierInfo::Constant(false)},
      {"e", IdentifierInfo::Variable(Type::DOUBLE, 2)},
      {"f", IdentifierInfo::Constant(0.5)}};

  const CompilePropertyResult result1 =
      CompileProperty(Identifier("a"), identifiers_by_name, dd_manager);
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found int"}),
      result1.errors);
  const CompilePropertyResult result2 =
      CompileProperty(Identifier("b"), identifiers_by_name, dd_manager);
  const std::string expected2 = "0: ILOAD 1 0";
  EXPECT_EQ(expected2, StrCat(*result2.property));
  const CompilePropertyResult result3 =
      CompileProperty(Identifier("c"), identifiers_by_name, dd_manager);
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found int"}),
      result3.errors);
  const CompilePropertyResult result4 =
      CompileProperty(Identifier("d"), identifiers_by_name, dd_manager);
  const std::string expected4 = "0: ICONST 0 0";
  EXPECT_EQ(expected4, StrCat(*result4.property));
  const CompilePropertyResult result5 =
      CompileProperty(Identifier("e"), identifiers_by_name, dd_manager);
  EXPECT_EQ(std::vector<std::string>({"double variables not supported"}),
            result5.errors);
  const CompilePropertyResult result6 =
      CompileProperty(Identifier("f"), identifiers_by_name, dd_manager);
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found double"}),
      result6.errors);
  const CompilePropertyResult result7 =
      CompileProperty(Identifier("g"), identifiers_by_name, dd_manager);
  EXPECT_EQ(
      std::vector<std::string>({"undefined identifier 'g' in expression"}),
      result7.errors);
}

TEST(CompilePropertyTest, FunctionCall) {
  const DecisionDiagramManager dd_manager(0);
  const CompilePropertyResult result1 = CompileProperty(
      FunctionCall(Function::MAX,
                   UniquePtrVector<const Expression>(Literal::New(false))),
      {}, dd_manager);
  const std::string expected1 = "0: ICONST 0 0";
  EXPECT_EQ(expected1, StrCat(*result1.property));
  const CompilePropertyResult result2 = CompileProperty(
      FunctionCall(Function::FLOOR,
                   UniquePtrVector<const Expression>(Literal::New(0.5))),
      {}, dd_manager);
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found int"}),
      result2.errors);
  const CompilePropertyResult result3 = CompileProperty(
      FunctionCall(Function::MIN,
                   UniquePtrVector<const Expression>(Literal::New(0.5))),
      {}, dd_manager);
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found double"}),
      result3.errors);
}

TEST(CompilePropertyTest, UnaryOperation) {
  const DecisionDiagramManager dd_manager(2);
  const CompilePropertyResult result1 = CompileProperty(
      UnaryOperation(UnaryOperator::NOT, Literal::New(false)), {}, dd_manager);
  const std::string expected1 =
      "0: ICONST 0 0\n"
      "1: NOT 0";
  EXPECT_EQ(expected1, StrCat(*result1.property));
  const CompilePropertyResult result2 = CompileProperty(
      UnaryOperation(UnaryOperator::NOT,
                     ProbabilityThresholdOperation::New(
                         ProbabilityThresholdOperator::GREATER, 0.25,
                         UntilProperty::New(17, 42, Literal::New(true),
                                            Identifier::New("a")))),
      {{"a", IdentifierInfo::Variable(Type::BOOL, 0)}}, dd_manager);
  const std::string expected2 =
      "NOT of:\n"
      "P > 0.25\n"
      "0: UNTIL [17, 42]\n"
      "pre:\n"
      "0: ICONST 1 0\n"
      "post:\n"
      "0: ILOAD 0 0";
  EXPECT_EQ(expected2, StrCat(*result2.property));
  const CompilePropertyResult result3 = CompileProperty(
      UnaryOperation(UnaryOperator::NEGATE, Literal::New(17)), {}, dd_manager);
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found int"}),
      result3.errors);
  const CompilePropertyResult result4 = CompileProperty(
      UnaryOperation(UnaryOperator::NOT, Literal::New(0.5)), {}, dd_manager);
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; unary operator ! applied to double"}),
            result4.errors);
}

TEST(CompilePropertyTest, ProbabilityThresholdOperator) {
  const DecisionDiagramManager dd_manager(2);
  const CompilePropertyResult result1 = CompileProperty(
      ProbabilityThresholdOperation(
          ProbabilityThresholdOperator::LESS, 0.25,
          UntilProperty::New(17, 42, Literal::New(true), Identifier::New("a"))),
      {{"a", IdentifierInfo::Variable(Type::BOOL, 0)}}, dd_manager);
  const std::string expected1 =
      "NOT of:\n"
      "P >= 0.25\n"
      "0: UNTIL [17, 42]\n"
      "pre:\n"
      "0: ICONST 1 0\n"
      "post:\n"
      "0: ILOAD 0 0";
  EXPECT_EQ(expected1, StrCat(*result1.property));
  const CompilePropertyResult result2 = CompileProperty(
      ProbabilityThresholdOperation(
          ProbabilityThresholdOperator::LESS_EQUAL, 0.5,
          UntilProperty::New(17, 42, Literal::New(true), Identifier::New("a"))),
      {{"a", IdentifierInfo::Variable(Type::BOOL, 0)}}, dd_manager);
  const std::string expected2 =
      "NOT of:\n"
      "P > 0.5\n"
      "0: UNTIL [17, 42]\n"
      "pre:\n"
      "0: ICONST 1 0\n"
      "post:\n"
      "0: ILOAD 0 0";
  EXPECT_EQ(expected2, StrCat(*result2.property));
  const CompilePropertyResult result3 = CompileProperty(
      ProbabilityThresholdOperation(
          ProbabilityThresholdOperator::GREATER_EQUAL, 0.75,
          UntilProperty::New(17, 42, Literal::New(true), Identifier::New("a"))),
      {{"a", IdentifierInfo::Variable(Type::BOOL, 0)}}, dd_manager);
  const std::string expected3 =
      "P >= 0.75\n"
      "0: UNTIL [17, 42]\n"
      "pre:\n"
      "0: ICONST 1 0\n"
      "post:\n"
      "0: ILOAD 0 0";
  EXPECT_EQ(expected3, StrCat(*result3.property));
  const CompilePropertyResult result4 = CompileProperty(
      ProbabilityThresholdOperation(
          ProbabilityThresholdOperator::GREATER, 0.125,
          UntilProperty::New(17, 42, Literal::New(true), Identifier::New("a"))),
      {{"a", IdentifierInfo::Variable(Type::BOOL, 0)}}, dd_manager);
  const std::string expected4 =
      "P > 0.125\n"
      "0: UNTIL [17, 42]\n"
      "pre:\n"
      "0: ICONST 1 0\n"
      "post:\n"
      "0: ILOAD 0 0";
  EXPECT_EQ(expected4, StrCat(*result4.property));
  const CompilePropertyResult result5 = CompileProperty(
      ProbabilityThresholdOperation(
          ProbabilityThresholdOperator::GREATER, 0.125,
          UntilProperty::New(
              17, 42, ProbabilityThresholdOperation::New(
                          ProbabilityThresholdOperator::GREATER_EQUAL, 0.25,
                          UntilProperty::New(0, 1, Literal::New(true),
                                             Identifier::New("a"))),
              ProbabilityThresholdOperation::New(
                  ProbabilityThresholdOperator::GREATER, 0.5,
                  UntilProperty::New(0.5, 17, Literal::New(false),
                                     Identifier::New("a"))))),
      {{"a", IdentifierInfo::Variable(Type::BOOL, 0)}}, dd_manager);
  const std::string expected5 =
      "P > 0.125\n"
      "0: UNTIL [17, 42]\n"
      "pre:\n"
      "P >= 0.25\n"
      "1: UNTIL [0, 1]\n"
      "pre:\n"
      "0: ICONST 1 0\n"
      "post:\n"
      "0: ILOAD 0 0\n"
      "post:\n"
      "P > 0.5\n"
      "2: UNTIL [0.5, 17]\n"
      "pre:\n"
      "0: ICONST 0 0\n"
      "post:\n"
      "0: ILOAD 0 0";
  EXPECT_EQ(expected5, StrCat(*result5.property));
  const CompilePropertyResult result6 = CompileProperty(
      ProbabilityThresholdOperation(
          ProbabilityThresholdOperator::GREATER, -2,
          UntilProperty::New(17, 42, Literal::New(true), Identifier::New("a"))),
      {{"a", IdentifierInfo::Variable(Type::BOOL, 0)}}, dd_manager);
  EXPECT_EQ(std::vector<std::string>({"threshold -2 is not a probability"}),
            result6.errors);
  const CompilePropertyResult result7 = CompileProperty(
      ProbabilityThresholdOperation(
          ProbabilityThresholdOperator::GREATER, 1.5,
          UntilProperty::New(17, 42, Literal::New(true), Identifier::New("a"))),
      {{"a", IdentifierInfo::Variable(Type::BOOL, 0)}}, dd_manager);
  EXPECT_EQ(std::vector<std::string>({"threshold 1.5 is not a probability"}),
            result7.errors);
  const CompilePropertyResult result8 = CompileProperty(
      ProbabilityThresholdOperation(
          ProbabilityThresholdOperator::GREATER, 0.125,
          UntilProperty::New(42, 17, Literal::New(true), Identifier::New("a"))),
      {{"a", IdentifierInfo::Variable(Type::BOOL, 0)}}, dd_manager);
  EXPECT_EQ(std::vector<std::string>({"bad time range; 42 > 17"}),
            result8.errors);
  const CompilePropertyResult result9 = CompileProperty(
      ProbabilityThresholdOperation(
          ProbabilityThresholdOperator::GREATER, 0.125,
          UntilProperty::New(17, 42, Literal::New(0.5), Identifier::New("a"))),
      {{"a", IdentifierInfo::Variable(Type::BOOL, 0)}}, dd_manager);
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found double"}),
      result9.errors);
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
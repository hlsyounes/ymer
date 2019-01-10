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

#include <memory>
#include <optional>

#include "strutil.h"

#include "gtest/gtest.h"

namespace {

TEST(CompilePropertyTest, Literal) {
  const std::optional<DecisionDiagramManager> dd_manager(0);
  const CompilePropertyResult result1 =
      CompileProperty(Literal(true), {}, {}, {}, dd_manager);
  const std::string expected1 = "0: ICONST 1 0";
  EXPECT_EQ(expected1, StrCat(*result1.property));
  const CompilePropertyResult result2 =
      CompileProperty(Literal(17), {}, {}, {}, dd_manager);
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found int: 17"}),
      result2.errors);
  const CompilePropertyResult result3 =
      CompileProperty(Literal(0.5), {}, {}, {}, dd_manager);
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type bool; found "
                 "double: 0.5"}),
            result3.errors);
}

TEST(CompilePropertyTest, Identifier) {
  const std::optional<DecisionDiagramManager> dd_manager(4);
  const Identifier e("e");
  const Identifier b("b");
  const std::map<std::string, const Expression*> formulas_by_name = {{"h", &e},
                                                                     {"i", &b}};
  const std::map<std::string, IdentifierInfo> identifiers_by_name = {
      {"a", IdentifierInfo::Variable(Type::INT, 0, 0, 1, 0)},
      {"b", IdentifierInfo::Variable(Type::BOOL, 1, 2, 2, false)},
      {"c", IdentifierInfo::Constant(17)},
      {"d", IdentifierInfo::Constant(false)},
      {"e", IdentifierInfo::Variable(Type::DOUBLE, 2, 3, 10, 0.5)},
      {"f", IdentifierInfo::Constant(0.5)}};

  const CompilePropertyResult result1 = CompileProperty(
      Identifier("a"), formulas_by_name, {}, identifiers_by_name, dd_manager);
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found int: a"}),
      result1.errors);
  const CompilePropertyResult result2 = CompileProperty(
      Identifier("b"), formulas_by_name, {}, identifiers_by_name, dd_manager);
  const std::string expected2 = "0: ILOAD 1 0";
  EXPECT_EQ(expected2, StrCat(*result2.property));
  const CompilePropertyResult result3 = CompileProperty(
      Identifier("c"), formulas_by_name, {}, identifiers_by_name, dd_manager);
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found int: c"}),
      result3.errors);
  const CompilePropertyResult result4 = CompileProperty(
      Identifier("d"), formulas_by_name, {}, identifiers_by_name, dd_manager);
  const std::string expected4 = "0: ICONST 0 0";
  EXPECT_EQ(expected4, StrCat(*result4.property));
  const CompilePropertyResult result5 = CompileProperty(
      Identifier("e"), formulas_by_name, {}, identifiers_by_name, dd_manager);
  EXPECT_EQ(std::vector<std::string>({"double variables not supported"}),
            result5.errors);
  const CompilePropertyResult result6 = CompileProperty(
      Identifier("f"), formulas_by_name, {}, identifiers_by_name, dd_manager);
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type bool; found "
                 "double: f"}),
            result6.errors);
  const CompilePropertyResult result7 = CompileProperty(
      Identifier("g"), formulas_by_name, {}, identifiers_by_name, dd_manager);
  EXPECT_EQ(
      std::vector<std::string>({"undefined identifier 'g' in expression"}),
      result7.errors);
  const CompilePropertyResult result8 = CompileProperty(
      Identifier("h"), formulas_by_name, {}, identifiers_by_name, dd_manager);
  EXPECT_EQ(std::vector<std::string>({"double variables not supported"}),
            result8.errors);
  const CompilePropertyResult result9 = CompileProperty(
      Identifier("i"), formulas_by_name, {}, identifiers_by_name, dd_manager);
  const std::string expected9 = "0: ILOAD 1 0";
  EXPECT_EQ(expected9, StrCat(*result9.property));
}

TEST(CompilePropertyTest, Label) {
  const std::optional<DecisionDiagramManager> dd_manager(2);
  const Literal t(true);
  const Identifier b("b");
  const std::map<std::string, const Expression*> labels_by_name = {
      {"\"t\"", &t}, {"\"b\"", &b}};
  const std::map<std::string, IdentifierInfo> identifiers_by_name = {
      {"b", IdentifierInfo::Variable(Type::BOOL, 0, 0, 0, false)}};

  const CompilePropertyResult result1 = CompileProperty(
      Label("\"t\""), {}, labels_by_name, identifiers_by_name, dd_manager);
  const std::string expected1 = "0: ICONST 1 0";
  EXPECT_EQ(expected1, StrCat(*result1.property));
  const CompilePropertyResult result2 = CompileProperty(
      Label("\"b\""), {}, labels_by_name, identifiers_by_name, dd_manager);
  const std::string expected2 = "0: ILOAD 0 0";
  EXPECT_EQ(expected2, StrCat(*result2.property));
  const CompilePropertyResult result3 = CompileProperty(
      Label("\"a\""), {}, labels_by_name, identifiers_by_name, dd_manager);
  EXPECT_EQ(std::vector<std::string>({"undefined label \"a\" in property"}),
            result3.errors);
}

TEST(CompilePropertyTest, FunctionCall) {
  const std::optional<DecisionDiagramManager> dd_manager(0);
  const CompilePropertyResult result1 = CompileProperty(
      FunctionCall(Function::MAX, UniquePtrVector<const Expression>(
                                      std::make_unique<Literal>(false))),
      {}, {}, {}, dd_manager);
  const std::string expected1 = "0: ICONST 0 0";
  EXPECT_EQ(expected1, StrCat(*result1.property));
  const CompilePropertyResult result2 = CompileProperty(
      FunctionCall(Function::FLOOR, UniquePtrVector<const Expression>(
                                        std::make_unique<Literal>(0.5))),
      {}, {}, {}, dd_manager);
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type bool; found int: "
                 "floor(0.5)"}),
            result2.errors);
  const CompilePropertyResult result3 = CompileProperty(
      FunctionCall(Function::MIN, UniquePtrVector<const Expression>(
                                      std::make_unique<Literal>(0.5))),
      {}, {}, {}, dd_manager);
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type bool; found "
                 "double: min(0.5)"}),
            result3.errors);
}

TEST(CompilePropertyTest, UnaryOperation) {
  const std::optional<DecisionDiagramManager> dd_manager(2);
  const CompilePropertyResult result1 = CompileProperty(
      UnaryOperation(UnaryOperator::NOT, std::make_unique<Literal>(false)), {},
      {}, {}, dd_manager);
  const std::string expected1 =
      "0: ICONST 0 0\n"
      "1: NOT 0";
  EXPECT_EQ(expected1, StrCat(*result1.property));
  const CompilePropertyResult result2 = CompileProperty(
      UnaryOperation(UnaryOperator::NOT,
                     std::make_unique<ProbabilityThresholdOperation>(
                         ProbabilityThresholdOperator::GREATER, 0.25,
                         std::make_unique<UntilProperty>(
                             TimeRange(17, 42), std::make_unique<Literal>(true),
                             std::make_unique<Identifier>("a")))),
      {}, {}, {{"a", IdentifierInfo::Variable(Type::BOOL, 0, 0, 0, false)}},
      dd_manager);
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
      UnaryOperation(UnaryOperator::NEGATE, std::make_unique<Literal>(17)), {},
      {}, {}, dd_manager);
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found int: -17"}),
      result3.errors);
  const CompilePropertyResult result4 = CompileProperty(
      UnaryOperation(UnaryOperator::NOT, std::make_unique<Literal>(0.5)), {},
      {}, {}, dd_manager);
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; unary operator ! applied to double"}),
            result4.errors);
}

TEST(CompilePropertyTest, BinaryOperation) {
  const std::optional<DecisionDiagramManager> dd_manager(2);
  const CompilePropertyResult result1 = CompileProperty(
      BinaryOperation(BinaryOperator::AND, std::make_unique<Literal>(false),
                      std::make_unique<Identifier>("a")),
      {}, {}, {{"a", IdentifierInfo::Variable(Type::BOOL, 0, 0, 0, false)}},
      dd_manager);
  const std::string expected1 =
      "0: ICONST 0 0\n"
      "1: IFFALSE 0 3\n"
      "2: ILOAD 0 0";
  EXPECT_EQ(expected1, StrCat(*result1.property));
  const CompilePropertyResult result2 = CompileProperty(
      BinaryOperation(
          BinaryOperator::AND, std::make_unique<Literal>(false),
          std::make_unique<ProbabilityThresholdOperation>(
              ProbabilityThresholdOperator::GREATER, 0.25,
              std::make_unique<EventuallyProperty>(
                  TimeRange(17, 42), std::make_unique<Identifier>("a")))),
      {}, {}, {{"a", IdentifierInfo::Variable(Type::BOOL, 0, 0, 0, false)}},
      dd_manager);
  const std::string expected2 =
      "AND of 2 operands:\n"
      "operand 0:\n"
      "0: ICONST 0 0\n"
      "operand 1:\n"
      "P > 0.25\n"
      "0: UNTIL [17, 42]\n"
      "pre:\n"
      "0: ICONST 1 0\n"
      "post:\n"
      "0: ILOAD 0 0";
  EXPECT_EQ(expected2, StrCat(*result2.property));
  const CompilePropertyResult result3 = CompileProperty(
      BinaryOperation(
          BinaryOperator::OR, std::make_unique<Literal>(false),
          std::make_unique<ProbabilityThresholdOperation>(
              ProbabilityThresholdOperator::GREATER, 0.25,
              std::make_unique<UntilProperty>(
                  TimeRange(17, 42), std::make_unique<Literal>(true),
                  std::make_unique<Identifier>("a")))),
      {}, {}, {{"a", IdentifierInfo::Variable(Type::BOOL, 0, 0, 0, false)}},
      dd_manager);
  const std::string expected3 =
      "OR of 2 operands:\n"
      "operand 0:\n"
      "0: ICONST 0 0\n"
      "operand 1:\n"
      "P > 0.25\n"
      "0: UNTIL [17, 42]\n"
      "pre:\n"
      "0: ICONST 1 0\n"
      "post:\n"
      "0: ILOAD 0 0";
  EXPECT_EQ(expected3, StrCat(*result3.property));
  const CompilePropertyResult result4 = CompileProperty(
      BinaryOperation(
          BinaryOperator::IMPLY, std::make_unique<Literal>(false),
          std::make_unique<ProbabilityThresholdOperation>(
              ProbabilityThresholdOperator::GREATER, 0.25,
              std::make_unique<EventuallyProperty>(
                  TimeRange(17, 42), std::make_unique<Identifier>("a")))),
      {}, {}, {{"a", IdentifierInfo::Variable(Type::BOOL, 0, 0, 0, false)}},
      dd_manager);
  const std::string expected4 =
      "OR of 2 operands:\n"
      "operand 0:\n"
      "NOT of:\n"
      "0: ICONST 0 0\n"
      "operand 1:\n"
      "P > 0.25\n"
      "0: UNTIL [17, 42]\n"
      "pre:\n"
      "0: ICONST 1 0\n"
      "post:\n"
      "0: ILOAD 0 0";
  EXPECT_EQ(expected4, StrCat(*result4.property));
  const CompilePropertyResult result5 = CompileProperty(
      BinaryOperation(
          BinaryOperator::IFF, std::make_unique<Literal>(false),
          std::make_unique<ProbabilityThresholdOperation>(
              ProbabilityThresholdOperator::GREATER, 0.25,
              std::make_unique<UntilProperty>(
                  TimeRange(17, 42), std::make_unique<Literal>(true),
                  std::make_unique<Identifier>("a")))),
      {}, {}, {{"a", IdentifierInfo::Variable(Type::BOOL, 0, 0, 0, false)}},
      dd_manager);
  const std::string expected5 =
      "IFF of 2 operands:\n"
      "operand 0:\n"
      "0: ICONST 0 0\n"
      "operand 1:\n"
      "P > 0.25\n"
      "0: UNTIL [17, 42]\n"
      "pre:\n"
      "0: ICONST 1 0\n"
      "post:\n"
      "0: ILOAD 0 0";
  EXPECT_EQ(expected5, StrCat(*result5.property));
  const CompilePropertyResult result6 = CompileProperty(
      BinaryOperation(
          BinaryOperator::LESS, std::make_unique<Literal>(false),
          std::make_unique<ProbabilityThresholdOperation>(
              ProbabilityThresholdOperator::GREATER, 0.25,
              std::make_unique<EventuallyProperty>(
                  TimeRange(17, 42), std::make_unique<Identifier>("a")))),
      {}, {}, {{"a", IdentifierInfo::Variable(Type::BOOL, 0, 0, 0, false)}},
      dd_manager);
  const std::string expected6 =
      "AND of 2 operands:\n"
      "operand 0:\n"
      "NOT of:\n"
      "0: ICONST 0 0\n"
      "operand 1:\n"
      "P > 0.25\n"
      "0: UNTIL [17, 42]\n"
      "pre:\n"
      "0: ICONST 1 0\n"
      "post:\n"
      "0: ILOAD 0 0";
  EXPECT_EQ(expected6, StrCat(*result6.property));
  const CompilePropertyResult result7 = CompileProperty(
      BinaryOperation(
          BinaryOperator::LESS_EQUAL, std::make_unique<Literal>(false),
          std::make_unique<ProbabilityThresholdOperation>(
              ProbabilityThresholdOperator::GREATER, 0.25,
              std::make_unique<UntilProperty>(
                  TimeRange(17, 42), std::make_unique<Literal>(true),
                  std::make_unique<Identifier>("a")))),
      {}, {}, {{"a", IdentifierInfo::Variable(Type::BOOL, 0, 0, 0, false)}},
      dd_manager);
  EXPECT_EQ(expected4, StrCat(*result7.property));
  const CompilePropertyResult result8 = CompileProperty(
      BinaryOperation(
          BinaryOperator::GREATER_EQUAL, std::make_unique<Literal>(false),
          std::make_unique<ProbabilityThresholdOperation>(
              ProbabilityThresholdOperator::GREATER, 0.25,
              std::make_unique<EventuallyProperty>(
                  TimeRange(17, 42), std::make_unique<Identifier>("a")))),
      {}, {}, {{"a", IdentifierInfo::Variable(Type::BOOL, 0, 0, 0, false)}},
      dd_manager);
  const std::string expected8 =
      "OR of 2 operands:\n"
      "operand 0:\n"
      "0: ICONST 0 0\n"
      "operand 1:\n"
      "NOT of:\n"
      "P > 0.25\n"
      "0: UNTIL [17, 42]\n"
      "pre:\n"
      "0: ICONST 1 0\n"
      "post:\n"
      "0: ILOAD 0 0";
  EXPECT_EQ(expected8, StrCat(*result8.property));
  const CompilePropertyResult result9 = CompileProperty(
      BinaryOperation(
          BinaryOperator::GREATER, std::make_unique<Literal>(false),
          std::make_unique<ProbabilityThresholdOperation>(
              ProbabilityThresholdOperator::GREATER, 0.25,
              std::make_unique<UntilProperty>(
                  TimeRange(17, 42), std::make_unique<Literal>(true),
                  std::make_unique<Identifier>("a")))),
      {}, {}, {{"a", IdentifierInfo::Variable(Type::BOOL, 0, 0, 0, false)}},
      dd_manager);
  const std::string expected9 =
      "AND of 2 operands:\n"
      "operand 0:\n"
      "0: ICONST 0 0\n"
      "operand 1:\n"
      "NOT of:\n"
      "P > 0.25\n"
      "0: UNTIL [17, 42]\n"
      "pre:\n"
      "0: ICONST 1 0\n"
      "post:\n"
      "0: ILOAD 0 0";
  EXPECT_EQ(expected9, StrCat(*result9.property));
  const CompilePropertyResult result10 = CompileProperty(
      BinaryOperation(
          BinaryOperator::EQUAL, std::make_unique<Literal>(false),
          std::make_unique<ProbabilityThresholdOperation>(
              ProbabilityThresholdOperator::GREATER, 0.25,
              std::make_unique<EventuallyProperty>(
                  TimeRange(17, 42), std::make_unique<Identifier>("a")))),
      {}, {}, {{"a", IdentifierInfo::Variable(Type::BOOL, 0, 0, 0, false)}},
      dd_manager);
  EXPECT_EQ(expected5, StrCat(*result10.property));
  const CompilePropertyResult result11 = CompileProperty(
      BinaryOperation(
          BinaryOperator::NOT_EQUAL, std::make_unique<Literal>(false),
          std::make_unique<ProbabilityThresholdOperation>(
              ProbabilityThresholdOperator::GREATER, 0.25,
              std::make_unique<UntilProperty>(
                  TimeRange(17, 42), std::make_unique<Literal>(true),
                  std::make_unique<Identifier>("a")))),
      {}, {}, {{"a", IdentifierInfo::Variable(Type::BOOL, 0, 0, 0, false)}},
      dd_manager);
  const std::string expected11 =
      "NOT of:\n"
      "IFF of 2 operands:\n"
      "operand 0:\n"
      "0: ICONST 0 0\n"
      "operand 1:\n"
      "P > 0.25\n"
      "0: UNTIL [17, 42]\n"
      "pre:\n"
      "0: ICONST 1 0\n"
      "post:\n"
      "0: ILOAD 0 0";
  EXPECT_EQ(expected11, StrCat(*result11.property));
  const CompilePropertyResult result12 = CompileProperty(
      BinaryOperation(BinaryOperator::PLUS, std::make_unique<Literal>(17),
                      std::make_unique<Literal>(42)),
      {}, {}, {}, dd_manager);
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type bool; found int: "
                 "17 + 42"}),
            result12.errors);
  const CompilePropertyResult result13 = CompileProperty(
      BinaryOperation(
          BinaryOperator::PLUS, std::make_unique<Literal>(false),
          std::make_unique<ProbabilityThresholdOperation>(
              ProbabilityThresholdOperator::GREATER, 0.25,
              std::make_unique<EventuallyProperty>(
                  TimeRange(17, 42), std::make_unique<Identifier>("a")))),
      {}, {}, {{"a", IdentifierInfo::Variable(Type::BOOL, 0, 0, 0, false)}},
      dd_manager);
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator + applied to bool"}),
            result13.errors);
  const CompilePropertyResult result14 = CompileProperty(
      BinaryOperation(
          BinaryOperator::AND, std::make_unique<Literal>(17),
          std::make_unique<ProbabilityThresholdOperation>(
              ProbabilityThresholdOperator::GREATER, 0.25,
              std::make_unique<UntilProperty>(
                  TimeRange(17, 42), std::make_unique<Literal>(true),
                  std::make_unique<Identifier>("a")))),
      {}, {}, {{"a", IdentifierInfo::Variable(Type::BOOL, 0, 0, 0, false)}},
      dd_manager);
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found int: 17"}),
      result14.errors);
}

TEST(CompilePropertyTest, Conditional) {
  const std::optional<DecisionDiagramManager> dd_manager(0);
  const CompilePropertyResult result1 =
      CompileProperty(Conditional(std::make_unique<Literal>(true),
                                  std::make_unique<Literal>(false),
                                  std::make_unique<Literal>(true)),
                      {}, {}, {}, dd_manager);
  const std::string expected1 =
      "0: ICONST 1 0\n"
      "1: IFFALSE 0 4\n"
      "2: ICONST 0 0\n"
      "3: GOTO 5\n"
      "4: ICONST 1 0";
  EXPECT_EQ(expected1, StrCat(*result1.property));
  const CompilePropertyResult result2 = CompileProperty(
      Conditional(std::make_unique<Literal>(true),
                  std::make_unique<Literal>(17), std::make_unique<Literal>(42)),
      {}, {}, {}, dd_manager);
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type bool; found int: "
                 "true ? 17 : 42"}),
            result2.errors);
  const CompilePropertyResult result3 = CompileProperty(
      Conditional(
          std::make_unique<ProbabilityThresholdOperation>(
              ProbabilityThresholdOperator::GREATER, 0.25,
              std::make_unique<EventuallyProperty>(
                  TimeRange(17, 42), std::make_unique<Identifier>("a"))),
          std::make_unique<Literal>(false), std::make_unique<Literal>(true)),
      {}, {}, {}, dd_manager);
  EXPECT_EQ(std::vector<std::string>(
                {"unexpected probability threshold operation in expression"}),
            result3.errors);
}

TEST(CompilePropertyTest, ProbabilityThresholdOperation) {
  const std::optional<DecisionDiagramManager> dd_manager(2);
  const CompilePropertyResult result1 = CompileProperty(
      ProbabilityThresholdOperation(
          ProbabilityThresholdOperator::LESS, 0.25,
          std::make_unique<UntilProperty>(TimeRange(17, 42),
                                          std::make_unique<Literal>(true),
                                          std::make_unique<Identifier>("a"))),
      {}, {}, {{"a", IdentifierInfo::Variable(Type::BOOL, 0, 0, 0, false)}},
      dd_manager);
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
          std::make_unique<EventuallyProperty>(
              TimeRange(17, 42), std::make_unique<Identifier>("a"))),
      {}, {}, {{"a", IdentifierInfo::Variable(Type::BOOL, 0, 0, 0, false)}},
      dd_manager);
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
          std::make_unique<UntilProperty>(TimeRange(17, 42),
                                          std::make_unique<Literal>(true),
                                          std::make_unique<Identifier>("a"))),
      {}, {}, {{"a", IdentifierInfo::Variable(Type::BOOL, 0, 0, 0, false)}},
      dd_manager);
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
          std::make_unique<EventuallyProperty>(
              TimeRange(17, 42), std::make_unique<Identifier>("a"))),
      {}, {}, {{"a", IdentifierInfo::Variable(Type::BOOL, 0, 0, 0, false)}},
      dd_manager);
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
          std::make_unique<UntilProperty>(
              TimeRange(17, 42),
              std::make_unique<ProbabilityThresholdOperation>(
                  ProbabilityThresholdOperator::GREATER_EQUAL, 0.25,
                  std::make_unique<EventuallyProperty>(
                      TimeRange(0, 1), std::make_unique<Identifier>("a"))),
              std::make_unique<ProbabilityThresholdOperation>(
                  ProbabilityThresholdOperator::GREATER, 0.5,
                  std::make_unique<UntilProperty>(
                      TimeRange(0.5, 17), std::make_unique<Literal>(false),
                      std::make_unique<Identifier>("a"))))),
      {}, {}, {{"a", IdentifierInfo::Variable(Type::BOOL, 0, 0, 0, false)}},
      dd_manager);
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
          std::make_unique<UntilProperty>(TimeRange(17, 42),
                                          std::make_unique<Literal>(true),
                                          std::make_unique<Identifier>("a"))),
      {}, {}, {{"a", IdentifierInfo::Variable(Type::BOOL, 0, 0, 0, false)}},
      dd_manager);
  EXPECT_EQ(std::vector<std::string>({"threshold -2 is not a probability"}),
            result6.errors);
  const CompilePropertyResult result7 = CompileProperty(
      ProbabilityThresholdOperation(
          ProbabilityThresholdOperator::GREATER, 1.5,
          std::make_unique<EventuallyProperty>(
              TimeRange(17, 42), std::make_unique<Identifier>("a"))),
      {}, {}, {{"a", IdentifierInfo::Variable(Type::BOOL, 0, 0, 0, false)}},
      dd_manager);
  EXPECT_EQ(std::vector<std::string>({"threshold 1.5 is not a probability"}),
            result7.errors);
  const CompilePropertyResult result8 = CompileProperty(
      ProbabilityThresholdOperation(
          ProbabilityThresholdOperator::GREATER, 0.125,
          std::make_unique<UntilProperty>(TimeRange(42, 17),
                                          std::make_unique<Literal>(true),
                                          std::make_unique<Identifier>("a"))),
      {}, {}, {{"a", IdentifierInfo::Variable(Type::BOOL, 0, 0, 0, false)}},
      dd_manager);
  EXPECT_EQ(std::vector<std::string>({"bad time range; 42 > 17"}),
            result8.errors);
  const CompilePropertyResult result9 = CompileProperty(
      ProbabilityThresholdOperation(
          ProbabilityThresholdOperator::GREATER, 0.125,
          std::make_unique<EventuallyProperty>(TimeRange(17, 42),
                                               std::make_unique<Literal>(0.5))),
      {}, {}, {{"a", IdentifierInfo::Variable(Type::BOOL, 0, 0, 0, false)}},
      dd_manager);
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type bool; found "
                 "double: 0.5"}),
            result9.errors);
}

TEST(OptimizePropertyTest, NotProperty) {
  const std::optional<DecisionDiagramManager> dd_manager(2);
  const Identifier a("a");
  const std::map<std::string, const Expression*> labels_by_name = {
      {"\"b\"", &a}};
  const std::map<std::string, IdentifierInfo> identifiers_by_name = {
      {"a", IdentifierInfo::Variable(Type::BOOL, 0, 0, 0, false)}};
  auto property1 = OptimizeProperty(
      *CompileProperty(
           UnaryOperation(UnaryOperator::NOT, std::make_unique<Literal>(false)),
           {}, {}, {}, dd_manager)
           .property,
      dd_manager);
  const std::string expected1 = "0: ICONST 1 0";
  EXPECT_EQ(expected1, StrCat(*property1));
  auto property2 = OptimizeProperty(
      CompiledNotProperty(CompileProperty(Identifier("a"), {}, {},
                                          identifiers_by_name, dd_manager)
                              .property),
      dd_manager);
  const std::string expected2 =
      "0: ILOAD 0 0\n"
      "1: NOT 0";
  EXPECT_EQ(expected2, StrCat(*property2));
  auto property3 = OptimizeProperty(
      CompiledNotProperty(
          CompileProperty(UnaryOperation(UnaryOperator::NOT,
                                         std::make_unique<Identifier>("a")),
                          {}, {}, identifiers_by_name, dd_manager)
              .property),
      dd_manager);
  const std::string expected3 = "0: ILOAD 0 0";
  EXPECT_EQ(expected3, StrCat(*property3));
  auto property4 = OptimizeProperty(
      *CompileProperty(
           UnaryOperation(UnaryOperator::NOT,
                          std::make_unique<ProbabilityThresholdOperation>(
                              ProbabilityThresholdOperator::LESS, 0.25,
                              std::make_unique<UntilProperty>(
                                  TimeRange(17, 42),
                                  std::make_unique<UnaryOperation>(
                                      UnaryOperator::NOT,
                                      std::make_unique<Literal>(false)),
                                  std::make_unique<UnaryOperation>(
                                      UnaryOperator::NOT,
                                      std::make_unique<UnaryOperation>(
                                          UnaryOperator::NOT,
                                          std::make_unique<Label>("\"b\"")))))),
           {}, labels_by_name, identifiers_by_name, dd_manager)
           .property,
      dd_manager);
  const std::string expected4 =
      "P >= 0.25\n"
      "0: UNTIL [17, 42]\n"
      "pre:\n"
      "0: ICONST 1 0\n"
      "post:\n"
      "0: ILOAD 0 0";
  EXPECT_EQ(expected4, StrCat(*property4));
}

TEST(OptimizePropertyTest, NaryProperty) {
  const std::optional<DecisionDiagramManager> dd_manager(2);
  std::map<std::string, IdentifierInfo> identifiers_by_name = {
      {"a", IdentifierInfo::Variable(Type::BOOL, 0, 0, 0, false)}};
  auto property1 = OptimizeProperty(
      *CompileProperty(BinaryOperation(BinaryOperator::AND,
                                       std::make_unique<Literal>(false),
                                       std::make_unique<Identifier>("a")),
                       {}, {}, identifiers_by_name, dd_manager)
           .property,
      dd_manager);
  const std::string expected1 = "0: ICONST 0 0";
  EXPECT_EQ(expected1, StrCat(*property1));
  auto property2 = OptimizeProperty(
      *CompileProperty(
           BinaryOperation(BinaryOperator::OR, std::make_unique<Literal>(true),
                           std::make_unique<Identifier>("a")),
           {}, {}, identifiers_by_name, dd_manager)
           .property,
      dd_manager);
  const std::string expected2 = "0: ICONST 1 0";
  EXPECT_EQ(expected2, StrCat(*property2));
  auto property3 = OptimizeProperty(
      *CompileProperty(
           BinaryOperation(BinaryOperator::IFF, std::make_unique<Literal>(true),
                           std::make_unique<Identifier>("a")),
           {}, {}, identifiers_by_name, dd_manager)
           .property,
      dd_manager);
  const std::string expected3 = "0: IVEQ 0 1 0";
  EXPECT_EQ(expected3, StrCat(*property3));
  auto property4 = OptimizeProperty(
      CompiledNaryProperty(CompiledNaryOperator::AND, nullptr,
                           UniquePtrVector<const CompiledProperty>(
                               CompileProperty(Identifier("a"), {}, {},
                                               identifiers_by_name, dd_manager)
                                   .property,
                               CompileProperty(Identifier("a"), {}, {},
                                               identifiers_by_name, dd_manager)
                                   .property)),
      dd_manager);
  const std::string expected4 =
      "0: ILOAD 0 0\n"
      "1: IFFALSE 0 3\n"
      "2: ILOAD 0 0";
  EXPECT_EQ(expected4, StrCat(*property4));
}

}  // namespace

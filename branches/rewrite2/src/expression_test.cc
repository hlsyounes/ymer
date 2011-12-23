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

#include "expression.h"

#include <memory>
#include <sstream>
#include <utility>

#include <gtest/gtest.h>

#include "type.h"

namespace {

enum class ExpressionType {
  NO_TYPE, LITERAL,
  IDENTIFIER, FUNCTION_CALL, UNARY_OPERATION, BINARY_OPERATION, CONDITIONAL
};

class TestExpressionVisitor : public ParsedExpressionVisitor {
 public:
  TestExpressionVisitor() : type_(ExpressionType::NO_TYPE) {}

  ExpressionType type() const { return type_; }

  virtual void VisitLiteral(const ParsedLiteral& expr) {
    type_ = ExpressionType::LITERAL;
  }

  virtual void VisitIdentifier(const ParsedIdentifier& expr) {
    type_ = ExpressionType::IDENTIFIER;
  }

  virtual void VisitFunctionCall(const ParsedFunctionCall& expr) {
    type_ = ExpressionType::FUNCTION_CALL;
  }

  virtual void VisitUnaryOperation(const ParsedUnaryOperation& expr) {
    type_ = ExpressionType::UNARY_OPERATION;
  }

  virtual void VisitBinaryOperation(const ParsedBinaryOperation& expr) {
    type_ = ExpressionType::BINARY_OPERATION;
  }

  virtual void VisitConditional(const ParsedConditional& expr) {
    type_ = ExpressionType::CONDITIONAL;
  }

 private:
  ExpressionType type_;
};

TEST(ExpressionTest, ConstructsIntLiteral) {
  const ParsedLiteral expr(17);
  EXPECT_EQ(Type::INT, expr.value().type());
  EXPECT_EQ(17, expr.value());
}

TEST(ExpressionTest, ConstructsDoubleLiteral) {
  const ParsedLiteral expr(3.14159);
  EXPECT_EQ(Type::DOUBLE, expr.value().type());
  EXPECT_EQ(3.14159, expr.value());
}

TEST(ExpressionTest, ConstructsBoolLiteral) {
  const ParsedLiteral expr(true);
  EXPECT_EQ(Type::BOOL, expr.value().type());
  EXPECT_EQ(true, expr.value());
}

TEST(ExpressionTest, ConstructsParsedIdentifier) {
  const ParsedIdentifier expr("foo");
  EXPECT_EQ("foo", expr.id());
}

TEST(ExpressionTest, ConstructsCompiledIdentifier) {
  const CompiledIdentifier<TypedValue> expr(17);
  EXPECT_EQ(17, expr.id());
}

TEST(ArgumentListTest, ConstructsWithZeroArguments) {
  const ParsedArgumentList arguments;
  EXPECT_EQ(0, arguments.size());
}

TEST(ArgumentListTest, ConstructsWithOneArgument) {
  std::unique_ptr<const ParsedExpression> argument0(ParsedLiteral::Create(17));
  const ParsedExpression* const raw_arguments[] = { argument0.get() };
  const ParsedArgumentList arguments(std::move(argument0));
  ASSERT_EQ(1, arguments.size());
  EXPECT_EQ(raw_arguments[0], &arguments[0]);
}

TEST(ArgumentListTest, ConstructsWithTwoArguments) {
  std::unique_ptr<const ParsedExpression> argument0(ParsedLiteral::Create(17));
  std::unique_ptr<const ParsedExpression> argument1(
      ParsedLiteral::Create(3.14159));
  const ParsedExpression* const raw_arguments[] = {
    argument0.get(), argument1.get()
  };
  const ParsedArgumentList arguments(std::move(argument0),
                                     std::move(argument1));
  ASSERT_EQ(2, arguments.size());
  EXPECT_EQ(raw_arguments[0], &arguments[0]);
  EXPECT_EQ(raw_arguments[1], &arguments[1]);
}

TEST(ArgumentListTest, ConstructsWithThreeArguments) {
  std::unique_ptr<const ParsedExpression> argument0(ParsedLiteral::Create(17));
  std::unique_ptr<const ParsedExpression> argument1(
      ParsedLiteral::Create(3.14159));
  std::unique_ptr<const ParsedExpression> argument2(
      ParsedLiteral::Create(true));
  const ParsedExpression* const raw_arguments[] = {
    argument0.get(), argument1.get(), argument2.get()
  };
  const ParsedArgumentList arguments(
      std::move(argument0), std::move(argument1), std::move(argument2));
  ASSERT_EQ(3, arguments.size());
  EXPECT_EQ(raw_arguments[0], &arguments[0]);
  EXPECT_EQ(raw_arguments[1], &arguments[1]);
  EXPECT_EQ(raw_arguments[2], &arguments[2]);
}

TEST(ArgumentListTest, PushBackAddsArgument) {
  std::unique_ptr<const ParsedExpression> argument0(ParsedLiteral::Create(17));
  std::unique_ptr<const ParsedExpression> argument1(
      ParsedLiteral::Create(3.14159));
  std::unique_ptr<const ParsedExpression> argument2(
      ParsedLiteral::Create(true));
  const ParsedExpression* const raw_arguments[] = {
    argument0.get(), argument1.get(), argument2.get()
  };
  ParsedArgumentList arguments;
  arguments.push_back(std::move(argument0));
  ASSERT_EQ(1, arguments.size());
  EXPECT_EQ(raw_arguments[0], &arguments[0]);
  arguments.push_back(std::move(argument1));
  ASSERT_EQ(2, arguments.size());
  EXPECT_EQ(raw_arguments[0], &arguments[0]);
  EXPECT_EQ(raw_arguments[1], &arguments[1]);
  arguments.push_back(std::move(argument2));
  ASSERT_EQ(3, arguments.size());
  EXPECT_EQ(raw_arguments[0], &arguments[0]);
  EXPECT_EQ(raw_arguments[1], &arguments[1]);
  EXPECT_EQ(raw_arguments[2], &arguments[2]);
}

TEST(ArgumentListTest, Iterator) {
  std::unique_ptr<const ParsedExpression> argument0(
      ParsedIdentifier::Create("i"));
  std::unique_ptr<const ParsedExpression> argument1(
      ParsedLiteral::Create(3.14159));
  const ParsedExpression* const raw_arguments[] = {
    argument0.get(), argument1.get()
  };
  const ParsedArgumentList arguments(std::move(argument0),
                                     std::move(argument1));
  ParsedArgumentList::Iterator i = arguments.begin();
  EXPECT_EQ(raw_arguments[0], &(*i));
  ++i;
  EXPECT_EQ(raw_arguments[1], &(*i));
  ParsedArgumentList::Iterator j = i++;
  EXPECT_EQ(raw_arguments[1], &(*j));
  EXPECT_EQ(arguments.end(), i);
  TestExpressionVisitor visitor;
  j->Accept(&visitor);
  EXPECT_EQ(ExpressionType::LITERAL, visitor.type());
}

TEST(FunctionTest, Output) {
  std::stringstream out;
  out << Function::MIN << ';' << Function::MAX << ';'
      << Function::FLOOR << ';' << Function::CEIL << ';'
      << Function::POW << ';' << Function::LOG << ';' << Function::MOD;
  EXPECT_EQ("min;max;floor;ceil;pow;log;mod", out.str());
}

TEST(ExpressionTest, ConstructsFunctionCall) {
  std::unique_ptr<const ParsedExpression> argument0(ParsedLiteral::Create(17));
  const ParsedExpression* const raw_arguments[] = { argument0.get() };
  const ParsedFunctionCall expr(Function::FLOOR, std::move(argument0));
  EXPECT_EQ(Function::FLOOR, expr.function());
  ASSERT_EQ(1, expr.arguments().size());
  EXPECT_EQ(raw_arguments[0], &expr.arguments()[0]);
}

TEST(UnaryOperatorTest, Output) {
  std::stringstream out;
  out << UnaryOperator::NEGATE << ';' << UnaryOperator::NOT;
  EXPECT_EQ("-;!", out.str());
}

TEST(ExpressionTest, ConstructsUnaryOperation) {
  std::unique_ptr<const ParsedExpression> operand(ParsedLiteral::Create(17));
  const ParsedExpression* const raw_operand = operand.get();
  const ParsedUnaryOperation expr(UnaryOperator::NEGATE, std::move(operand));
  EXPECT_EQ(UnaryOperator::NEGATE, expr.op());
  EXPECT_EQ(raw_operand, &expr.operand());
}

TEST(BinaryOperatorTest, Output) {
  std::ostringstream out;
  out << BinaryOperator::MULTIPLY << ';' << BinaryOperator::DIVIDE << ';'
      << BinaryOperator::PLUS << ';' << BinaryOperator::MINUS << ';'
      << BinaryOperator::LESS << ';' << BinaryOperator::LESS_EQUAL << ';'
      << BinaryOperator::GREATER_EQUAL << ';' << BinaryOperator::GREATER << ';'
      << BinaryOperator::EQUAL << ';' << BinaryOperator::NOT_EQUAL << ';'
      << BinaryOperator::AND << ';' << BinaryOperator::OR << ';'
      << BinaryOperator::IMPLY;
  EXPECT_EQ("*;/;+;-;<;<=;>=;>;=;!=;&;|;=>", out.str());
}

TEST(ExpressionTest, ConstructsBinaryOperation) {
  std::unique_ptr<const ParsedExpression> operand1(ParsedLiteral::Create(17));
  const ParsedExpression* const raw_operand1 = operand1.get();
  std::unique_ptr<const ParsedExpression> operand2(ParsedLiteral::Create(42));
  const ParsedExpression* const raw_operand2 = operand2.get();
  const ParsedBinaryOperation expr(BinaryOperator::PLUS,
                                   std::move(operand1), std::move(operand2));
  EXPECT_EQ(BinaryOperator::PLUS, expr.op());
  EXPECT_EQ(raw_operand1, &expr.operand1());
  EXPECT_EQ(raw_operand2, &expr.operand2());
}

TEST(ExpressionTest, ConstructsConditional) {
  std::unique_ptr<const ParsedExpression> condition(
      ParsedLiteral::Create(true));
  const ParsedExpression* const raw_condition = condition.get();
  std::unique_ptr<const ParsedExpression> if_expr(ParsedLiteral::Create(17));
  const ParsedExpression* const raw_if_expr = if_expr.get();
  std::unique_ptr<const ParsedExpression> else_expr(ParsedLiteral::Create(42));
  const ParsedExpression* const raw_else_expr = else_expr.get();
  const ParsedConditional expr(std::move(condition),
                               std::move(if_expr), std::move(else_expr));
  EXPECT_EQ(raw_condition, &expr.condition());
  EXPECT_EQ(raw_if_expr, &expr.if_expr());
  EXPECT_EQ(raw_else_expr, &expr.else_expr());
}

TEST(ExpressionTest, EvaluatesIntLiteral) {
  EXPECT_EQ(17, CompiledLiteral<TypedValue>(17).ValueInState({}));
  EXPECT_EQ(-42, CompiledLiteral<TypedValue>(-42).ValueInState({}));
}

TEST(ExpressionTest, EvaluatesBoolLiteral) {
  EXPECT_EQ(true, CompiledLiteral<TypedValue>(true).ValueInState({}));
  EXPECT_EQ(TypedValue(false),
            CompiledLiteral<TypedValue>(false).ValueInState({}));
}

TEST(ExpressionTest, EvaluatesDoubleLiteral) {
  EXPECT_EQ(3.14159, CompiledLiteral<TypedValue>(3.14159).ValueInState({}));
}

TEST(ExpressionTest, EvaluatesParsedIdentifier) {
  ParsedState state = { { "i", 17 }, { "b", true }, { "x", 3.14159 } };
  EXPECT_EQ(17, ParsedIdentifier("i").ValueInState(state));
  EXPECT_EQ(true, ParsedIdentifier("b").ValueInState(state));
  EXPECT_EQ(3.14159, ParsedIdentifier("x").ValueInState(state));
}

TEST(ExpressionTest, EvaluatesCompiledIdentifier) {
  CompiledState state = { 17, 42 };
  EXPECT_EQ(17, CompiledIdentifier<TypedValue>(0).ValueInState(state));
  EXPECT_EQ(42, CompiledIdentifier<TypedValue>(1).ValueInState(state));
}

TEST(ExpressionTest, EvaluatesFunctionCall) {
  EXPECT_EQ(-42,
            ParsedFunctionCall(Function::MIN,
                               ParsedLiteral::Create(17),
                               ParsedLiteral::Create(-42),
                               ParsedLiteral::Create(3.14159),
                               ParsedLiteral::Create(4711)).ValueInState({}));
  EXPECT_EQ(4711,
            ParsedFunctionCall(Function::MAX,
                               ParsedLiteral::Create(17),
                               ParsedLiteral::Create(-42),
                               ParsedLiteral::Create(3.14159),
                               ParsedLiteral::Create(4711)).ValueInState({}));
  EXPECT_EQ(1,
            ParsedFunctionCall(Function::FLOOR,
                               ParsedLiteral::Create(1.1)).ValueInState({}));
  EXPECT_EQ(-2,
            ParsedFunctionCall(Function::CEIL,
                               ParsedLiteral::Create(-2.3)).ValueInState({}));
  EXPECT_EQ(32,
            ParsedFunctionCall(Function::POW,
                               ParsedLiteral::Create(2),
                               ParsedLiteral::Create(5)).ValueInState({}));
  EXPECT_EQ(5,
            ParsedFunctionCall(Function::LOG,
                               ParsedLiteral::Create(32),
                               ParsedLiteral::Create(2)).ValueInState({}));
  EXPECT_EQ(1,
            ParsedFunctionCall(Function::MOD,
                               ParsedLiteral::Create(4),
                               ParsedLiteral::Create(3)).ValueInState({}));
}

TEST(ExpressionTest, EvaluatesUnaryOperation) {
  EXPECT_EQ(-17,
            ParsedUnaryOperation(UnaryOperator::NEGATE,
                                 ParsedLiteral::Create(17)).ValueInState({}));
  EXPECT_EQ(0.5,
            ParsedUnaryOperation(UnaryOperator::NEGATE,
                                 ParsedLiteral::Create(-0.5)).ValueInState({}));
  EXPECT_EQ(true,
            ParsedUnaryOperation(
                UnaryOperator::NOT,
                ParsedLiteral::Create(false)).ValueInState({}));
  EXPECT_EQ(TypedValue(false),
            ParsedUnaryOperation(
                UnaryOperator::NOT,
                ParsedLiteral::Create(true)).ValueInState({}));
}

TEST(ExpressionTest, EvaluatesBinaryOperation) {
  EXPECT_EQ(17 * 42,
            ParsedBinaryOperation(BinaryOperator::MULTIPLY,
                                  ParsedLiteral::Create(17),
                                  ParsedLiteral::Create(42)).ValueInState({}));
  EXPECT_EQ(17.0 / 42.0,
            ParsedBinaryOperation(BinaryOperator::DIVIDE,
                                  ParsedLiteral::Create(17),
                                  ParsedLiteral::Create(42)).ValueInState({}));
  EXPECT_EQ(17 + 42,
            ParsedBinaryOperation(BinaryOperator::PLUS,
                                  ParsedLiteral::Create(17),
                                  ParsedLiteral::Create(42)).ValueInState({}));
  EXPECT_EQ(17 - 42,
            ParsedBinaryOperation(BinaryOperator::MINUS,
                                  ParsedLiteral::Create(17),
                                  ParsedLiteral::Create(42)).ValueInState({}));
  EXPECT_EQ(true,
            ParsedBinaryOperation(BinaryOperator::LESS,
                                  ParsedLiteral::Create(17),
                                  ParsedLiteral::Create(42)).ValueInState({}));
  EXPECT_EQ(true,
            ParsedBinaryOperation(BinaryOperator::LESS_EQUAL,
                                  ParsedLiteral::Create(17),
                                  ParsedLiteral::Create(42)).ValueInState({}));
  EXPECT_EQ(TypedValue(false),
            ParsedBinaryOperation(BinaryOperator::GREATER_EQUAL,
                                  ParsedLiteral::Create(17),
                                  ParsedLiteral::Create(42)).ValueInState({}));
  EXPECT_EQ(TypedValue(false),
            ParsedBinaryOperation(BinaryOperator::GREATER,
                                  ParsedLiteral::Create(17),
                                  ParsedLiteral::Create(42)).ValueInState({}));
  EXPECT_EQ(TypedValue(false),
            ParsedBinaryOperation(BinaryOperator::EQUAL,
                                  ParsedLiteral::Create(17),
                                  ParsedLiteral::Create(42)).ValueInState({}));
  EXPECT_EQ(true,
            ParsedBinaryOperation(BinaryOperator::NOT_EQUAL,
                                  ParsedLiteral::Create(17),
                                  ParsedLiteral::Create(42)).ValueInState({}));
  EXPECT_EQ(true,
            ParsedBinaryOperation(
                BinaryOperator::AND,
                ParsedLiteral::Create(true),
                ParsedLiteral::Create(true)).ValueInState({}));
  EXPECT_EQ(true,
            ParsedBinaryOperation(
                BinaryOperator::OR,
                ParsedLiteral::Create(false),
                ParsedLiteral::Create(true)).ValueInState({}));
  EXPECT_EQ(true,
            ParsedBinaryOperation(
                BinaryOperator::IMPLY,
                ParsedLiteral::Create(true),
                ParsedLiteral::Create(true)).ValueInState({}));
  EXPECT_EQ(TypedValue(false),
            ParsedBinaryOperation(
                BinaryOperator::IMPLY,
                ParsedLiteral::Create(true),
                ParsedLiteral::Create(false)).ValueInState({}));
  EXPECT_EQ(true,
            ParsedBinaryOperation(
                BinaryOperator::IMPLY,
                ParsedLiteral::Create(false),
                ParsedLiteral::Create(true)).ValueInState({}));
  EXPECT_EQ(true,
            ParsedBinaryOperation(
                BinaryOperator::IMPLY,
                ParsedLiteral::Create(false),
                ParsedLiteral::Create(false)).ValueInState({}));
}

TEST(ExpressionTest, EvaluatesConditional) {
  EXPECT_EQ(17,
            ParsedConditional(ParsedLiteral::Create(true),
                              ParsedLiteral::Create(17),
                              ParsedLiteral::Create(3.14159)).ValueInState({}));
  EXPECT_EQ(3.14159,
            ParsedConditional(ParsedLiteral::Create(false),
                              ParsedLiteral::Create(17),
                              ParsedLiteral::Create(3.14159)).ValueInState({}));
}

TEST(ExpressionTest, AcceptVisitsLiteral) {
  TestExpressionVisitor visitor;
  ParsedLiteral(17).Accept(&visitor);
  EXPECT_EQ(ExpressionType::LITERAL, visitor.type());
}

TEST(ExpressionTest, AcceptVisitsIdentifier) {
  TestExpressionVisitor visitor;
  ParsedIdentifier("foo").Accept(&visitor);
  EXPECT_EQ(ExpressionType::IDENTIFIER, visitor.type());
}

TEST(ExpressionTest, AcceptVisitsFunctionCall) {
  TestExpressionVisitor visitor;
  ParsedFunctionCall(Function::FLOOR, ParsedLiteral::Create(17))
      .Accept(&visitor);
  EXPECT_EQ(ExpressionType::FUNCTION_CALL, visitor.type());
}

TEST(ExpressionTest, AcceptVisitsUnaryOperation) {
  TestExpressionVisitor visitor;
  ParsedUnaryOperation(UnaryOperator::NEGATE, ParsedLiteral::Create(17))
      .Accept(&visitor);
  EXPECT_EQ(ExpressionType::UNARY_OPERATION, visitor.type());
}

TEST(ExpressionTest, AcceptVisitsBinaryOperation) {
  TestExpressionVisitor visitor;
  ParsedBinaryOperation(BinaryOperator::PLUS,
                        ParsedLiteral::Create(17), ParsedLiteral::Create(42))
      .Accept(&visitor);
  EXPECT_EQ(ExpressionType::BINARY_OPERATION, visitor.type());
}

TEST(ExpressionTest, AcceptVisitsCondition) {
  TestExpressionVisitor visitor;
  ParsedConditional(ParsedLiteral::Create(true),
                    ParsedLiteral::Create(17), ParsedLiteral::Create(42))
      .Accept(&visitor);
  EXPECT_EQ(ExpressionType::CONDITIONAL, visitor.type());
}

}  // namespace

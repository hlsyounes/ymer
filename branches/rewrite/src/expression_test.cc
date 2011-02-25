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
#include <utility>

#include <gtest/gtest.h>

namespace {

enum class ExpressionType {
  NO_TYPE,
  INT_LITERAL, DOUBLE_LITERAL, BOOL_LITERAL,
  IDENTIFIER, FUNCTION_CALL, UNARY_OPERATION, BINARY_OPERATION, CONDITIONAL
};

class TestExpressionVisitor : public ExpressionVisitor {
 public:
  TestExpressionVisitor() : type_(ExpressionType::NO_TYPE) {}

  ExpressionType type() const { return type_; }

  virtual void VisitIntLiteral(const IntLiteral& expr) {
    type_ = ExpressionType::INT_LITERAL;
  }

  virtual void VisitDoubleLiteral(const DoubleLiteral& expr) {
    type_ = ExpressionType::DOUBLE_LITERAL;
  }

  virtual void VisitBoolLiteral(const BoolLiteral& expr) {
    type_ = ExpressionType::BOOL_LITERAL;
  }

  virtual void VisitIdentifier(const Identifier& expr) {
    type_ = ExpressionType::IDENTIFIER;
  }

  virtual void VisitFunctionCall(const FunctionCall& expr) {
    type_ = ExpressionType::FUNCTION_CALL;
  }

  virtual void VisitUnaryOperation(const UnaryOperation& expr) {
    type_ = ExpressionType::UNARY_OPERATION;
  }

  virtual void VisitBinaryOperation(const BinaryOperation& expr) {
    type_ = ExpressionType::BINARY_OPERATION;
  }

  virtual void VisitConditional(const Conditional& expr) {
    type_ = ExpressionType::CONDITIONAL;
  }

 private:
  ExpressionType type_;
};

TEST(ExpressionTest, ConstructsIntLiteral) {
  const IntLiteral expr(17);
  EXPECT_EQ(17, expr.value());
}

TEST(ExpressionTest, ConstructsDoubleLiteral) {
  const DoubleLiteral expr(3.14159);
  EXPECT_DOUBLE_EQ(3.14159, expr.value());
}

TEST(ExpressionTest, ConstructsBoolLiteral) {
  const BoolLiteral expr(true);
  EXPECT_TRUE(expr.value());
}

TEST(ExpressionTest, ConstructsIdentifier) {
  const Identifier expr("foo");
  EXPECT_EQ("foo", expr.name());
}

TEST(ArgumentListTest, ConstructsWithZeroArguments) {
  const ArgumentList arguments;
  EXPECT_EQ(0, arguments.size());
}

TEST(ArgumentListTest, ConstructsWithOneArgument) {
  std::unique_ptr<const Expression> argument0(IntLiteral::Create(17));
  const Expression* const raw_arguments[] = { argument0.get() };
  const ArgumentList arguments(std::move(argument0));
  ASSERT_EQ(1, arguments.size());
  EXPECT_EQ(raw_arguments[0], &arguments[0]);
}

TEST(ArgumentListTest, ConstructsWithTwoArguments) {
  std::unique_ptr<const Expression> argument0(IntLiteral::Create(17));
  std::unique_ptr<const Expression> argument1(DoubleLiteral::Create(3.14159));
  const Expression* const raw_arguments[] = {
    argument0.get(), argument1.get()
  };
  const ArgumentList arguments(std::move(argument0), std::move(argument1));
  ASSERT_EQ(2, arguments.size());
  EXPECT_EQ(raw_arguments[0], &arguments[0]);
  EXPECT_EQ(raw_arguments[1], &arguments[1]);
}

TEST(ArgumentListTest, ConstructsWithThreeArguments) {
  std::unique_ptr<const Expression> argument0(IntLiteral::Create(17));
  std::unique_ptr<const Expression> argument1(DoubleLiteral::Create(3.14159));
  std::unique_ptr<const Expression> argument2(BoolLiteral::Create(true));
  const Expression* const raw_arguments[] = {
    argument0.get(), argument1.get(), argument2.get()
  };
  const ArgumentList arguments(
      std::move(argument0), std::move(argument1), std::move(argument2));
  ASSERT_EQ(3, arguments.size());
  EXPECT_EQ(raw_arguments[0], &arguments[0]);
  EXPECT_EQ(raw_arguments[1], &arguments[1]);
  EXPECT_EQ(raw_arguments[2], &arguments[2]);
}

TEST(ArgumentListTest, PushBackAddsArgument) {
  std::unique_ptr<const Expression> argument0(IntLiteral::Create(17));
  std::unique_ptr<const Expression> argument1(DoubleLiteral::Create(3.14159));
  std::unique_ptr<const Expression> argument2(BoolLiteral::Create(true));
  const Expression* const raw_arguments[] = {
    argument0.get(), argument1.get(), argument2.get()
  };
  ArgumentList arguments;
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

TEST(ExpressionTest, FunctionName) {
  EXPECT_EQ("min", Function_Name(Function::MIN));
  EXPECT_EQ("max", Function_Name(Function::MAX));
  EXPECT_EQ("floor", Function_Name(Function::FLOOR));
  EXPECT_EQ("ceil", Function_Name(Function::CEIL));
  EXPECT_EQ("pow", Function_Name(Function::POW));
  EXPECT_EQ("mod", Function_Name(Function::MOD));
  EXPECT_EQ("log", Function_Name(Function::LOG));
}

TEST(ExpressionTest, ConstructsFunctionCall) {
  std::unique_ptr<const Expression> argument0(IntLiteral::Create(17));
  const Expression* const raw_arguments[] = { argument0.get() };
  ArgumentList arguments(std::move(argument0));
  const FunctionCall expr(Function::FLOOR, std::move(arguments));
  EXPECT_EQ(Function::FLOOR, expr.function());
  ASSERT_EQ(1, expr.arguments().size());
  EXPECT_EQ(raw_arguments[0], &expr.arguments()[0]);
}

TEST(ExpressionTest, ConstructsUnaryOperation) {
  std::unique_ptr<const Expression> operand(IntLiteral::Create(17));
  const Expression* const raw_operand = operand.get();
  const UnaryOperation expr(UnaryOperator::NEGATE, std::move(operand));
  EXPECT_EQ(UnaryOperator::NEGATE, expr.op());
  EXPECT_EQ(raw_operand, &expr.operand());
}

TEST(ExpressionTest, ConstructsBinaryOperation) {
  std::unique_ptr<const Expression> operand1(IntLiteral::Create(17));
  const Expression* const raw_operand1 = operand1.get();
  std::unique_ptr<const Expression> operand2(IntLiteral::Create(42));
  const Expression* const raw_operand2 = operand2.get();
  const BinaryOperation expr(BinaryOperator::ADD,
                             std::move(operand1), std::move(operand2));
  EXPECT_EQ(BinaryOperator::ADD, expr.op());
  EXPECT_EQ(raw_operand1, &expr.operand1());
  EXPECT_EQ(raw_operand2, &expr.operand2());
}

TEST(ExpressionTest, ConstructsConditional) {
  std::unique_ptr<const Expression> condition(BoolLiteral::Create(true));
  const Expression* const raw_condition = condition.get();
  std::unique_ptr<const Expression> if_expr(IntLiteral::Create(17));
  const Expression* const raw_if_expr = if_expr.get();
  std::unique_ptr<const Expression> else_expr(IntLiteral::Create(42));
  const Expression* const raw_else_expr = else_expr.get();
  const Conditional expr(std::move(condition),
                         std::move(if_expr), std::move(else_expr));
  EXPECT_EQ(raw_condition, &expr.condition());
  EXPECT_EQ(raw_if_expr, &expr.if_expr());
  EXPECT_EQ(raw_else_expr, &expr.else_expr());
}

TEST(ExpressionTest, AcceptVisitsIntLiteral) {
  TestExpressionVisitor visitor;
  IntLiteral(17).Accept(&visitor);
  EXPECT_EQ(ExpressionType::INT_LITERAL, visitor.type());
}

TEST(ExpressionTest, AcceptVisitsDoubleLiteral) {
  TestExpressionVisitor visitor;
  DoubleLiteral(3.14159).Accept(&visitor);
  EXPECT_EQ(ExpressionType::DOUBLE_LITERAL, visitor.type());
}

TEST(ExpressionTest, AcceptVisitsBoolLiteral) {
  TestExpressionVisitor visitor;
  BoolLiteral(true).Accept(&visitor);
  EXPECT_EQ(ExpressionType::BOOL_LITERAL, visitor.type());
}

TEST(ExpressionTest, AcceptVisitsIdentifier) {
  TestExpressionVisitor visitor;
  Identifier("foo").Accept(&visitor);
  EXPECT_EQ(ExpressionType::IDENTIFIER, visitor.type());
}

TEST(ExpressionTest, AcceptVisitsFunctionCall) {
  TestExpressionVisitor visitor;
  FunctionCall(Function::FLOOR, ArgumentList(IntLiteral::Create(17)))
      .Accept(&visitor);
  EXPECT_EQ(ExpressionType::FUNCTION_CALL, visitor.type());
}

TEST(ExpressionTest, AcceptVisitsUnaryOperation) {
  TestExpressionVisitor visitor;
  UnaryOperation(UnaryOperator::NEGATE, IntLiteral::Create(17))
      .Accept(&visitor);
  EXPECT_EQ(ExpressionType::UNARY_OPERATION, visitor.type());
}

TEST(ExpressionTest, AcceptVisitsBinaryOperation) {
  TestExpressionVisitor visitor;
  BinaryOperation(BinaryOperator::ADD,
                  IntLiteral::Create(17), IntLiteral::Create(42))
      .Accept(&visitor);
  EXPECT_EQ(ExpressionType::BINARY_OPERATION, visitor.type());
}

TEST(ExpressionTest, AcceptVisitsCondition) {
  TestExpressionVisitor visitor;
  Conditional(BoolLiteral::Create(true),
              IntLiteral::Create(17), IntLiteral::Create(42))
      .Accept(&visitor);
  EXPECT_EQ(ExpressionType::CONDITIONAL, visitor.type());
}

}  // namespace

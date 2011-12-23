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

#include "parse.h"

#include <set>
#include <string>

#include <gtest/gtest.h>

#include "expression.h"
#include "model.h"
#include "process-algebra.h"

namespace {

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
      "module M2 = M1 [ c1=c2, f1=f2, i1=i2, b1=b2, a1=a2 ]\n"
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

  Model model;
  std::string message;
  EXPECT_TRUE(ParseString(kModelSource, &model, &message));
  EXPECT_TRUE(message.empty()) << message;

  EXPECT_EQ(ModelType::DTMC, model.type());

  ASSERT_EQ(4, model.num_constants());
  EXPECT_EQ(Type::INT, model.constant_type(0));
  const ParsedLiteral* const const_init0 =
      dynamic_cast<const ParsedLiteral*>(model.GetConstantInit(0));
  ASSERT_NE(nullptr, const_init0);
  EXPECT_EQ(Type::INT, const_init0->value().type());
  EXPECT_EQ(17, const_init0->value());
  EXPECT_EQ("c1", model.GetConstantName(0));
  EXPECT_EQ(Type::INT, model.constant_type(1));
  const ParsedLiteral* const const_init1 =
      dynamic_cast<const ParsedLiteral*>(model.GetConstantInit(1));
  ASSERT_NE(nullptr, const_init1);
  EXPECT_EQ(Type::INT, const_init1->value().type());
  EXPECT_EQ(42, const_init1->value());
  EXPECT_EQ("c2", model.GetConstantName(1));
  EXPECT_EQ(Type::BOOL, model.constant_type(2));
  EXPECT_EQ(nullptr, model.GetConstantInit(2));
  EXPECT_EQ("k", model.GetConstantName(2));
  EXPECT_EQ(Type::DOUBLE, model.constant_type(3));
  const ParsedLiteral* const const_init3 =
      dynamic_cast<const ParsedLiteral*>(model.GetConstantInit(3));
  ASSERT_NE(nullptr, const_init3);
  EXPECT_EQ(Type::DOUBLE, const_init3->value().type());
  EXPECT_EQ(0.5, const_init3->value());
  EXPECT_EQ("K", model.GetConstantName(3));

  ASSERT_EQ(7, model.num_variables());
  EXPECT_EQ(Type::INT, model.variable_type(0));
  ASSERT_NE(nullptr, model.GetVariableRange(0));
  const ParsedLiteral* const var_min0 =
      dynamic_cast<const ParsedLiteral*>(&model.GetVariableRange(0)->min());
  ASSERT_NE(nullptr, var_min0);
  EXPECT_EQ(Type::INT, var_min0->value().type());
  EXPECT_EQ(17, var_min0->value());
  const ParsedLiteral* const var_max0 =
      dynamic_cast<const ParsedLiteral*>(&model.GetVariableRange(0)->max());
  ASSERT_NE(nullptr, var_max0);
  EXPECT_EQ(Type::INT, var_max0->value().type());
  EXPECT_EQ(42, var_max0->value());
  const ParsedLiteral* const var_init0 =
      dynamic_cast<const ParsedLiteral*>(model.GetVariableInit(0));
  ASSERT_NE(nullptr, var_init0);
  EXPECT_EQ(Type::INT, var_init0->value().type());
  EXPECT_EQ(17, var_init0->value());
  EXPECT_EQ("i", model.GetVariableName(0));
  EXPECT_EQ(Type::BOOL, model.variable_type(1));
  EXPECT_EQ(nullptr, model.GetVariableRange(1));
  const ParsedLiteral* const var_init1 =
      dynamic_cast<const ParsedLiteral*>(model.GetVariableInit(1));
  ASSERT_NE(nullptr, var_init1);
  EXPECT_EQ(Type::BOOL, var_init1->value().type());
  EXPECT_EQ(true, var_init1->value());
  EXPECT_EQ("b", model.GetVariableName(1));
  EXPECT_EQ(Type::INT, model.variable_type(2));
  ASSERT_NE(nullptr, model.GetVariableRange(2));
  const ParsedLiteral* const var_min2 =
      dynamic_cast<const ParsedLiteral*>(&model.GetVariableRange(2)->min());
  ASSERT_NE(nullptr, var_min2);
  EXPECT_EQ(Type::INT, var_min2->value().type());
  EXPECT_EQ(0, var_min2->value());
  const ParsedLiteral* const var_max2 =
      dynamic_cast<const ParsedLiteral*>(&model.GetVariableRange(2)->max());
  ASSERT_NE(nullptr, var_max2);
  EXPECT_EQ(Type::INT, var_max2->value().type());
  EXPECT_EQ(1, var_max2->value());
  EXPECT_EQ(nullptr, model.GetVariableInit(2));
  EXPECT_EQ("j", model.GetVariableName(2));
  EXPECT_EQ(Type::INT, model.variable_type(3));
  ASSERT_NE(nullptr, model.GetVariableRange(3));
  const ParsedLiteral* const var_min3 =
      dynamic_cast<const ParsedLiteral*>(&model.GetVariableRange(3)->min());
  ASSERT_NE(nullptr, var_min3);
  EXPECT_EQ(Type::INT, var_min3->value().type());
  EXPECT_EQ(0, var_min3->value());
  const ParsedIdentifier* const var_max3 =
      dynamic_cast<const ParsedIdentifier*>(&model.GetVariableRange(3)->max());
  ASSERT_NE(nullptr, var_max3);
  EXPECT_EQ("c1", var_max3->id());
  const ParsedIdentifier* const var_init3 =
      dynamic_cast<const ParsedIdentifier*>(model.GetVariableInit(3));
  ASSERT_NE(nullptr, var_init3);
  EXPECT_EQ("f1", var_init3->id());
  EXPECT_EQ("i1", model.GetVariableName(3));
  EXPECT_EQ(Type::BOOL, model.variable_type(4));
  EXPECT_EQ(nullptr, model.GetVariableRange(4));
  EXPECT_EQ(nullptr, model.GetVariableInit(4));
  EXPECT_EQ("b1", model.GetVariableName(4));
  EXPECT_EQ(Type::INT, model.variable_type(5));
  ASSERT_NE(nullptr, model.GetVariableRange(5));
  const ParsedLiteral* const var_min5 =
      dynamic_cast<const ParsedLiteral*>(&model.GetVariableRange(5)->min());
  ASSERT_NE(nullptr, var_min5);
  EXPECT_EQ(Type::INT, var_min5->value().type());
  EXPECT_EQ(0, var_min5->value());
  const ParsedIdentifier* const var_max5 =
      dynamic_cast<const ParsedIdentifier*>(&model.GetVariableRange(5)->max());
  ASSERT_NE(nullptr, var_max5);
  EXPECT_EQ("c2", var_max5->id());
  const ParsedIdentifier* const var_init5 =
      dynamic_cast<const ParsedIdentifier*>(model.GetVariableInit(5));
  ASSERT_NE(nullptr, var_init5);
  EXPECT_EQ("f2", var_init5->id());
  EXPECT_EQ("i2", model.GetVariableName(5));
  EXPECT_EQ(Type::BOOL, model.variable_type(6));
  EXPECT_EQ(nullptr, model.GetVariableRange(6));
  EXPECT_EQ(nullptr, model.GetVariableInit(6));
  EXPECT_EQ("b2", model.GetVariableName(6));

  ASSERT_EQ(4, model.num_formulas());
  const ParsedLiteral* const formula_expr0 =
      dynamic_cast<const ParsedLiteral*>(&model.formula_expr(0));
  ASSERT_NE(nullptr, formula_expr0);
  EXPECT_EQ(Type::INT, formula_expr0->value().type());
  EXPECT_EQ(17, formula_expr0->value());
  EXPECT_EQ("f1", model.GetFormulaName(0));
  const ParsedLiteral* const formula_expr1 =
      dynamic_cast<const ParsedLiteral*>(&model.formula_expr(1));
  ASSERT_NE(nullptr, formula_expr1);
  EXPECT_EQ(Type::INT, formula_expr1->value().type());
  EXPECT_EQ(42, formula_expr1->value());
  EXPECT_EQ("f2", model.GetFormulaName(1));
  const ParsedLiteral* const formula_expr2 =
      dynamic_cast<const ParsedLiteral*>(&model.formula_expr(2));
  ASSERT_NE(nullptr, formula_expr2);
  EXPECT_EQ(Type::BOOL, formula_expr2->value().type());
  EXPECT_EQ(true, formula_expr2->value());
  EXPECT_EQ("h", model.GetFormulaName(2));
  const ParsedLiteral* const formula_expr3 =
      dynamic_cast<const ParsedLiteral*>(&model.formula_expr(3));
  ASSERT_NE(nullptr, formula_expr3);
  EXPECT_EQ(Type::DOUBLE, formula_expr3->value().type());
  EXPECT_EQ(3.14159, formula_expr3->value());
  EXPECT_EQ("g", model.GetFormulaName(3));

  ASSERT_EQ(2, model.num_modules());
  EXPECT_EQ("M1", model.GetModuleName(0));
  EXPECT_EQ(std::set<int>({ 3, 4 }), model.module_variables(0));
  ASSERT_EQ(1, model.module_commands(0).size());
  ASSERT_TRUE(model.module_commands(0)[0].action() ? true : false);
  EXPECT_EQ("a1", model.module_commands(0)[0].action().get());
  const ParsedBinaryOperation* const guard0 =
      dynamic_cast<const ParsedBinaryOperation*>(
          &model.module_commands(0)[0].guard());
  ASSERT_NE(nullptr, guard0);
  EXPECT_EQ(BinaryOperator::LESS, guard0->op());
  const ParsedIdentifier* const guard0_operand1 =
      dynamic_cast<const ParsedIdentifier*>(&guard0->operand1());
  ASSERT_NE(nullptr, guard0_operand1);
  EXPECT_EQ("i1", guard0_operand1->id());
  const ParsedIdentifier* const guard0_operand2 =
      dynamic_cast<const ParsedIdentifier*>(&guard0->operand2());
  ASSERT_NE(nullptr, guard0_operand2);
  EXPECT_EQ("c1", guard0_operand2->id());
  ASSERT_EQ(1, model.module_commands(0)[0].outcomes().size());
  const ParsedBinaryOperation* const probability0 =
      dynamic_cast<const ParsedBinaryOperation*>(
          &model.module_commands(0)[0].outcomes()[0].probability());
  ASSERT_NE(nullptr, probability0);
  EXPECT_EQ(BinaryOperator::DIVIDE, probability0->op());
  const ParsedLiteral* const probability0_operand1 =
      dynamic_cast<const ParsedLiteral*>(&probability0->operand1());
  ASSERT_NE(nullptr, probability0_operand1);
  EXPECT_EQ(Type::INT, probability0_operand1->value().type());
  EXPECT_EQ(1, probability0_operand1->value());
  const ParsedIdentifier* const probability0_operand2 =
      dynamic_cast<const ParsedIdentifier*>(&probability0->operand2());
  ASSERT_NE(nullptr, probability0_operand2);
  EXPECT_EQ("i", probability0_operand2->id());
  ASSERT_EQ(1, model.module_commands(0)[0].outcomes()[0].updates().size());
  EXPECT_EQ("i1",
            model.module_commands(0)[0].outcomes()[0].updates()[0].variable());
  const ParsedBinaryOperation* const update_expr0 =
      dynamic_cast<const ParsedBinaryOperation*>(
          &model.module_commands(0)[0].outcomes()[0].updates()[0].expr());
  ASSERT_NE(nullptr, update_expr0);
  EXPECT_EQ(BinaryOperator::PLUS, update_expr0->op());
  const ParsedIdentifier* const update_expr0_operand1 =
      dynamic_cast<const ParsedIdentifier*>(&update_expr0->operand1());
  ASSERT_NE(nullptr, update_expr0_operand1);
  EXPECT_EQ("i1", update_expr0_operand1->id());
  const ParsedLiteral* const update_expr0_operand2 =
      dynamic_cast<const ParsedLiteral*>(&update_expr0->operand2());
  ASSERT_NE(nullptr, update_expr0_operand2);
  EXPECT_EQ(Type::INT, update_expr0_operand2->value().type());
  EXPECT_EQ(1, update_expr0_operand2->value());
  EXPECT_EQ("M2", model.GetModuleName(1));
  EXPECT_EQ(std::set<int>({ 5, 6 }), model.module_variables(1));
  ASSERT_EQ(1, model.module_commands(1).size());
  ASSERT_TRUE(model.module_commands(1)[0].action() ? true : false);
  EXPECT_EQ("a2", model.module_commands(1)[0].action().get());
  const ParsedBinaryOperation* const guard1 =
      dynamic_cast<const ParsedBinaryOperation*>(
          &model.module_commands(1)[0].guard());
  ASSERT_NE(nullptr, guard1);
  EXPECT_EQ(BinaryOperator::LESS, guard1->op());
  const ParsedIdentifier* const guard1_operand1 =
      dynamic_cast<const ParsedIdentifier*>(&guard1->operand1());
  ASSERT_NE(nullptr, guard1_operand1);
  EXPECT_EQ("i2", guard1_operand1->id());
  const ParsedIdentifier* const guard1_operand2 =
      dynamic_cast<const ParsedIdentifier*>(&guard1->operand2());
  ASSERT_NE(nullptr, guard1_operand2);
  EXPECT_EQ("c2", guard1_operand2->id());
  ASSERT_EQ(1, model.module_commands(1)[0].outcomes().size());
  const ParsedBinaryOperation* const probability1 =
      dynamic_cast<const ParsedBinaryOperation*>(
          &model.module_commands(1)[0].outcomes()[0].probability());
  ASSERT_NE(nullptr, probability1);
  EXPECT_EQ(BinaryOperator::DIVIDE, probability1->op());
  const ParsedLiteral* const probability1_operand1 =
      dynamic_cast<const ParsedLiteral*>(&probability1->operand1());
  ASSERT_NE(nullptr, probability1_operand1);
  EXPECT_EQ(Type::INT, probability1_operand1->value().type());
  EXPECT_EQ(1, probability1_operand1->value());
  const ParsedIdentifier* const probability1_operand2 =
      dynamic_cast<const ParsedIdentifier*>(&probability1->operand2());
  ASSERT_NE(nullptr, probability1_operand2);
  EXPECT_EQ("i", probability1_operand2->id());
  ASSERT_EQ(1, model.module_commands(1)[0].outcomes()[0].updates().size());
  EXPECT_EQ("i2",
            model.module_commands(1)[0].outcomes()[0].updates()[0].variable());
  const ParsedBinaryOperation* const update_expr1 =
      dynamic_cast<const ParsedBinaryOperation*>(
          &model.module_commands(1)[0].outcomes()[0].updates()[0].expr());
  ASSERT_NE(nullptr, update_expr1);
  EXPECT_EQ(BinaryOperator::PLUS, update_expr1->op());
  const ParsedIdentifier* const update_expr1_operand1 =
      dynamic_cast<const ParsedIdentifier*>(&update_expr1->operand1());
  ASSERT_NE(nullptr, update_expr1_operand1);
  EXPECT_EQ("i2", update_expr1_operand1->id());
  const ParsedLiteral* const update_expr1_operand2 =
      dynamic_cast<const ParsedLiteral*>(&update_expr1->operand2());
  ASSERT_NE(nullptr, update_expr1_operand2);
  EXPECT_EQ(Type::INT, update_expr1_operand2->value().type());
  EXPECT_EQ(1, update_expr1_operand2->value());

  const ParsedIdentifier* const label_expr =
      dynamic_cast<const ParsedIdentifier*>(model.GetLabelExpr("\"l\""));
  ASSERT_NE(nullptr, label_expr);
  EXPECT_EQ("f1", label_expr->id());

  ASSERT_EQ(2, model.num_rewards());
  ASSERT_EQ(1, model.state_rewards(0).size());
  const ParsedBinaryOperation* const reward_guard0 =
      dynamic_cast<const ParsedBinaryOperation*>(
          &model.state_rewards(0)[0].guard());
  ASSERT_NE(nullptr, reward_guard0);
  EXPECT_EQ(BinaryOperator::EQUAL, reward_guard0->op());
  const ParsedIdentifier* const reward_guard0_operand1 =
      dynamic_cast<const ParsedIdentifier*>(&reward_guard0->operand1());
  ASSERT_NE(nullptr, reward_guard0_operand1);
  EXPECT_EQ("i1", reward_guard0_operand1->id());
  const ParsedIdentifier* const reward_guard0_operand2 =
      dynamic_cast<const ParsedIdentifier*>(&reward_guard0->operand2());
  ASSERT_NE(nullptr, reward_guard0_operand2);
  EXPECT_EQ("c1", reward_guard0_operand2->id());
  const ParsedLiteral* const reward_expr0 =
      dynamic_cast<const ParsedLiteral*>(&model.state_rewards(0)[0].reward());
  ASSERT_NE(nullptr, reward_expr0);
  EXPECT_EQ(Type::DOUBLE, reward_expr0->value().type());
  EXPECT_EQ(1.0, reward_expr0->value());
  EXPECT_EQ(0, model.transition_rewards(0).size());
  EXPECT_EQ(0, model.state_rewards(1).size());
  ASSERT_EQ(1, model.transition_rewards(1).size());
  const ParsedLiteral* const reward_guard1 =
      dynamic_cast<const ParsedLiteral*>(
          &model.transition_rewards(1)[0].guard());
  ASSERT_NE(nullptr, reward_guard1);
  EXPECT_EQ(Type::BOOL, reward_guard1->value().type());
  EXPECT_EQ(true, reward_guard1->value());
  const ParsedLiteral* const reward_expr1 =
      dynamic_cast<const ParsedLiteral*>(
          &model.transition_rewards(1)[0].reward());
  ASSERT_NE(nullptr, reward_expr1);
  EXPECT_EQ(Type::DOUBLE, reward_expr1->value().type());
  EXPECT_EQ(0.25, reward_expr1->value());
  int index = -1;
  EXPECT_TRUE(model.GetLabeledRewardsIndex("\"r\"", &index));
  EXPECT_EQ(1, index);

  const ParallelComposition* const system =
      dynamic_cast<const ParallelComposition*>(model.system());
  ASSERT_NE(nullptr, system);
  EXPECT_EQ(ParallelCompositionOperator::ASYNCHRONOUS, system->op());
  const ActionRenaming* const system_operand1 =
      dynamic_cast<const ActionRenaming*>(&system->operand1());
  ASSERT_NE(nullptr, system_operand1);
  EXPECT_EQ((std::map<std::string, std::string>({ { "a1", "a3" } })),
            system_operand1->substitutions());
  const ModuleIdentifier* const system_operand11 =
      dynamic_cast<const ModuleIdentifier*>(&system_operand1->operand());
  ASSERT_NE(nullptr, system_operand11);
  EXPECT_EQ("M1", system_operand11->name());
  const ModuleIdentifier* const system_operand2 =
      dynamic_cast<const ModuleIdentifier*>(&system->operand2());
  ASSERT_NE(nullptr, system_operand2);
  EXPECT_EQ("M2", system_operand2->name());
}

TEST(ParseStringTest, DefaultsToMDP) {
  const std::string kModelSource =
      "module M\n"
      "  b : bool;\n"
      "  [] b -> 1 : (b'=!b);\n"
      "endmodule\n";

  Model model;
  std::string message;
  EXPECT_TRUE(ParseString(kModelSource, &model, &message));
  EXPECT_TRUE(message.empty()) << message;

  EXPECT_EQ(ModelType::MDP, model.type());
}

TEST(ParseStringTest, ReportsErrorForInvalidModel) {
  const std::string kModelSource =
      "module M\n"
      "  true -> (b'=!b);\n"  // Missing action specification.
      "endmodule\n";
  Model model;
  std::string message;
  EXPECT_FALSE(ParseString(kModelSource, &model, &message));
  EXPECT_EQ("2:syntax error", message.substr(0, 14));
}

TEST(ParseFileTest, ParsesValidModel) {
  Model model;
  std::string message;
  EXPECT_TRUE(ParseFile("src/testdata/example.sm", &model, &message));
  EXPECT_TRUE(message.empty()) << message;

  EXPECT_EQ(ModelType::CTMC, model.type());

  EXPECT_EQ(0, model.num_constants());

  ASSERT_EQ(1, model.num_variables());
  EXPECT_EQ(Type::BOOL, model.variable_type(0));
  EXPECT_EQ(nullptr, model.GetVariableRange(0));
  EXPECT_EQ(nullptr, model.GetVariableInit(0));
  EXPECT_EQ("b", model.GetVariableName(0));

  EXPECT_EQ(0, model.num_formulas());

  ASSERT_EQ(1, model.num_modules());
  EXPECT_EQ("M", model.GetModuleName(0));
  EXPECT_EQ(std::set<int>({ 0 }), model.module_variables(0));
  ASSERT_EQ(1, model.module_commands(0).size());
  EXPECT_FALSE(model.module_commands(0)[0].action() ? true : false);
  const ParsedLiteral* const guard0 =
      dynamic_cast<const ParsedLiteral*>(&model.module_commands(0)[0].guard());
  ASSERT_NE(nullptr, guard0);
  EXPECT_EQ(Type::BOOL, guard0->value().type());
  EXPECT_EQ(true, guard0->value());
  ASSERT_EQ(1, model.module_commands(0)[0].outcomes().size());
  const ParsedLiteral* const probability0 =
      dynamic_cast<const ParsedLiteral*>(
          &model.module_commands(0)[0].outcomes()[0].probability());
  ASSERT_NE(nullptr, probability0);
  EXPECT_EQ(Type::DOUBLE, probability0->value().type());
  EXPECT_EQ(1.0, probability0->value());
  ASSERT_EQ(1, model.module_commands(0)[0].outcomes()[0].updates().size());
  EXPECT_EQ("b",
            model.module_commands(0)[0].outcomes()[0].updates()[0].variable());
  const ParsedUnaryOperation* const update_expr0 =
      dynamic_cast<const ParsedUnaryOperation*>(
          &model.module_commands(0)[0].outcomes()[0].updates()[0].expr());
  ASSERT_NE(nullptr, update_expr0);
  EXPECT_EQ(UnaryOperator::NOT, update_expr0->op());
  const ParsedIdentifier* const update_expr0_operand =
      dynamic_cast<const ParsedIdentifier*>(&update_expr0->operand());
  ASSERT_NE(nullptr, update_expr0_operand);
  EXPECT_EQ("b", update_expr0_operand->id());

  EXPECT_EQ(0, model.num_rewards());

  EXPECT_EQ(nullptr, model.system());
}

TEST(ParseFileTest, MissingFile) {
  Model model;
  std::string message;
  EXPECT_FALSE(ParseFile("src/testdata/missing.sm", &model, &message));
  EXPECT_EQ("Cannot open src/testdata/missing.sm: No such file or directory",
            message);
}

TEST(ParseFileTest, ReportsErrorForInvalidModel) {
  Model model;
  std::string message;
  EXPECT_FALSE(ParseFile("src/testdata/invalid.pm", &model, &message));
  EXPECT_EQ("src/testdata/invalid.pm:4:syntax error", message.substr(0, 38));
}

}  // namespace

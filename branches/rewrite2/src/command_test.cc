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

#include "command.h"

#include <memory>

#include "gtest/gtest.h"

#include "expression.h"

namespace {

TEST(UpdateTest, ConstructsUpdate) {
  std::unique_ptr<const ParsedExpression> expr(ParsedLiteral::Create(17));
  const ParsedExpression* const raw_expr = expr.get();
  const ParsedUpdate update("foo", std::move(expr));
  EXPECT_EQ("foo", update.variable());
  EXPECT_EQ(raw_expr, &update.expr());
}

TEST(OutcomeTest, ConstructsOutcome) {
  std::unique_ptr<const ParsedExpression> probability(
      ParsedLiteral::Create(0.5));
  const ParsedExpression* const raw_probability = probability.get();
  std::unique_ptr<const ParsedExpression> expr0(ParsedLiteral::Create(17));
  const ParsedExpression* const raw_expr0 = expr0.get();
  std::unique_ptr<const ParsedExpression> expr1(ParsedLiteral::Create(42));
  const ParsedExpression* const raw_expr1 = expr1.get();
  std::vector<ParsedUpdate> updates;
  updates.emplace_back("foo", std::move(expr0));
  updates.emplace_back("bar", std::move(expr1));
  const ParsedOutcome outcome(std::move(probability), std::move(updates));
  EXPECT_EQ(raw_probability, &outcome.probability());
  ASSERT_EQ(2, outcome.updates().size());
  EXPECT_EQ("foo", outcome.updates()[0].variable());
  EXPECT_EQ(raw_expr0, &outcome.updates()[0].expr());
  EXPECT_EQ("bar", outcome.updates()[1].variable());
  EXPECT_EQ(raw_expr1, &outcome.updates()[1].expr());
}

TEST(CommandTest, ConstructsCommand) {
  std::unique_ptr<const ParsedExpression> guard(ParsedLiteral::Create(true));
  const ParsedExpression* const raw_guard = guard.get();
  std::vector<ParsedOutcome> outcomes;
  std::unique_ptr<const ParsedExpression> probability(
      ParsedLiteral::Create(0.5));
  const ParsedExpression* const raw_probability = probability.get();
  outcomes.emplace_back(std::move(probability), std::vector<ParsedUpdate>());
  const ParsedCommand command("foo", std::move(guard), std::move(outcomes));
  ASSERT_TRUE(command.action() ? true : false);
  EXPECT_EQ("foo", command.action().get());
  EXPECT_EQ(raw_guard, &command.guard());
  ASSERT_EQ(1, command.outcomes().size());
  EXPECT_EQ(raw_probability, &command.outcomes()[0].probability());
  EXPECT_EQ(0, command.outcomes()[0].updates().size());
}

TEST(CommandTest, ConstructsCommandWithoutAction) {
  std::unique_ptr<const ParsedExpression> guard(ParsedLiteral::Create(true));
  const ParsedExpression* const raw_guard = guard.get();
  std::vector<ParsedOutcome> outcomes;
  std::unique_ptr<const ParsedExpression> probability(
      ParsedLiteral::Create(0.5));
  const ParsedExpression* const raw_probability = probability.get();
  outcomes.emplace_back(std::move(probability), std::vector<ParsedUpdate>());
  const ParsedCommand command(
      Optional<std::string>(), std::move(guard), std::move(outcomes));
  EXPECT_FALSE(command.action() ? true : false);
  EXPECT_EQ(raw_guard, &command.guard());
  ASSERT_EQ(1, command.outcomes().size());
  EXPECT_EQ(raw_probability, &command.outcomes()[0].probability());
  EXPECT_EQ(0, command.outcomes()[0].updates().size());
}

}  // namespace

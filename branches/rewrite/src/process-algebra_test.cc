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

#include "process-algebra.h"

#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>

#include <gtest/gtest.h>

namespace {

enum class ProcessAlgebraType {
  NO_TYPE,
  MODULE_IDENTIFIER, PARALLEL_COMPOSITION, RESTRICTED_PARALLEL_COMPOSITION,
  ACTION_HIDING, ACTION_RENAMING
};

class TestProcessAlgebraVisitor : public ProcessAlgebraVisitor {
 public:
  TestProcessAlgebraVisitor() : type_(ProcessAlgebraType::NO_TYPE) {}

  ProcessAlgebraType type() const { return type_; }

  virtual void VisitModuleIdentifier(const ModuleIdentifier& proc_algebra) {
    type_ = ProcessAlgebraType::MODULE_IDENTIFIER;
  }

  virtual void VisitParallelComposition(
      const ParallelComposition& proc_algebra) {
    type_ = ProcessAlgebraType::PARALLEL_COMPOSITION;
  }

  virtual void VisitRestrictedParallelComposition(
      const RestrictedParallelComposition& proc_algebra) {
    type_ = ProcessAlgebraType::RESTRICTED_PARALLEL_COMPOSITION;
  }

  virtual void VisitActionHiding(const ActionHiding& proc_algebra) {
    type_ = ProcessAlgebraType::ACTION_HIDING;
  }

  virtual void VisitActionRenaming(const ActionRenaming& proc_algebra) {
    type_ = ProcessAlgebraType::ACTION_RENAMING;
  }

 private:
  ProcessAlgebraType type_;
};

TEST(ProcessAlgebraTest, ConstructsModuleIdentifier) {
  const ModuleIdentifier proc_algebra("foo");
  EXPECT_EQ("foo", proc_algebra.name());
}

TEST(ProcessAlgebraTest, ConstructsParallelComposition) {
  std::unique_ptr<const ProcessAlgebra> operand1(
      ModuleIdentifier::Create("foo"));
  const ProcessAlgebra* const raw_operand1 = operand1.get();
  std::unique_ptr<const ProcessAlgebra> operand2(
      ModuleIdentifier::Create("bar"));
  const ProcessAlgebra* const raw_operand2 = operand2.get();
  const ParallelComposition proc_algebra(
      ParallelCompositionOperator::ASYNCHRONOUS,
      std::move(operand1), std::move(operand2));
  EXPECT_EQ(ParallelCompositionOperator::ASYNCHRONOUS, proc_algebra.op());
  EXPECT_EQ(raw_operand1, &proc_algebra.operand1());
  EXPECT_EQ(raw_operand2, &proc_algebra.operand2());
}

TEST(ProcessAlgebraTest, ConstructsRestrictedParallelComposition) {
  std::unique_ptr<const ProcessAlgebra> operand1(
      ModuleIdentifier::Create("foo"));
  const ProcessAlgebra* const raw_operand1 = operand1.get();
  std::unique_ptr<const ProcessAlgebra> operand2(
      ModuleIdentifier::Create("bar"));
  const ProcessAlgebra* const raw_operand2 = operand2.get();
  const std::set<std::string> actions({ "a", "b" });
  const RestrictedParallelComposition proc_algebra(
      actions, std::move(operand1), std::move(operand2));
  EXPECT_EQ(actions, proc_algebra.actions());
  EXPECT_EQ(raw_operand1, &proc_algebra.operand1());
  EXPECT_EQ(raw_operand2, &proc_algebra.operand2());
}

TEST(ProcessAlgebraTest, ConstructsActionHiding) {
  std::unique_ptr<const ProcessAlgebra> operand(
      ModuleIdentifier::Create("foo"));
  const ProcessAlgebra* const raw_operand = operand.get();
  const std::set<std::string> actions({ "a" });
  const ActionHiding proc_algebra(actions, std::move(operand));
  EXPECT_EQ(actions, proc_algebra.actions());
  EXPECT_EQ(raw_operand, &proc_algebra.operand());
}

TEST(ProcessAlgebraTest, ConstructsActionRenaming) {
  std::unique_ptr<const ProcessAlgebra> operand(
      ModuleIdentifier::Create("foo"));
  const ProcessAlgebra* const raw_operand = operand.get();
  const std::map<std::string, std::string> substitutions(
      { std::make_pair("a", "b") });
  const ActionRenaming proc_algebra(substitutions, std::move(operand));
  EXPECT_EQ(substitutions, proc_algebra.substitutions());
  EXPECT_EQ(raw_operand, &proc_algebra.operand());
}

TEST(ProcessAlgebraTest, AcceptVisitsModuleIdentifier) {
  TestProcessAlgebraVisitor visitor;
  ModuleIdentifier("foo").Accept(&visitor);
  EXPECT_EQ(ProcessAlgebraType::MODULE_IDENTIFIER, visitor.type());
}

TEST(ProcessAlgebraTest, AcceptParallelComposition) {
  TestProcessAlgebraVisitor visitor;
  ParallelComposition(ParallelCompositionOperator::ALPHABETIZED,
                      ModuleIdentifier::Create("foo"),
                      ModuleIdentifier::Create("bar"))
      .Accept(&visitor);
  EXPECT_EQ(ProcessAlgebraType::PARALLEL_COMPOSITION, visitor.type());
}

TEST(ProcessAlgebraTest, AcceptRestrictedParallelComposition) {
  TestProcessAlgebraVisitor visitor;
  RestrictedParallelComposition({ "a" },
                                ModuleIdentifier::Create("foo"),
                                ModuleIdentifier::Create("bar"))
      .Accept(&visitor);
  EXPECT_EQ(ProcessAlgebraType::RESTRICTED_PARALLEL_COMPOSITION,
            visitor.type());
}

TEST(ProcessAlgebraTest, AcceptVisitsActionHiding) {
  TestProcessAlgebraVisitor visitor;
  ActionHiding({ "a" }, ModuleIdentifier::Create("foo")).Accept(&visitor);
  EXPECT_EQ(ProcessAlgebraType::ACTION_HIDING, visitor.type());
}

TEST(ProcessAlgebraTest, AcceptVisitsActionRenaming) {
  TestProcessAlgebraVisitor visitor;
  ActionRenaming({ std::make_pair("a", "b") }, ModuleIdentifier::Create("foo"))
      .Accept(&visitor);
  EXPECT_EQ(ProcessAlgebraType::ACTION_RENAMING, visitor.type());
}

}  // namespace

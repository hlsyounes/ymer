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
//
// A class hierarchy for representing process algebra expressions that
// supports the visitor pattern.

#ifndef PROCESS_ALGEBRA_H_
#define PROCESS_ALGEBRA_H_

#include <map>
#include <memory>
#include <set>
#include <string>

class ProcessAlgebraVisitor;

// Abstract base class for process algebra expressions.
//
// This class supports the visitor pattern.  Example usage:
//
//   class ConcreteProcessAlgebraVisitor : public ProcessAlgebraVisitor {
//     ...
//   };
//
//   ProcessAlgebra* proc_algebra = ...;
//   ConcreteProcessAlgebraVisitor visitor;
//   proc_algebra->Accept(&visitor);
//
class ProcessAlgebra {
 public:
  // Disallow copy and assign.
  ProcessAlgebra(const ProcessAlgebra&) = delete;
  ProcessAlgebra& operator=(const ProcessAlgebra&) = delete;

  virtual ~ProcessAlgebra();

  virtual void Accept(ProcessAlgebraVisitor* visitor) const = 0;

 protected:
  ProcessAlgebra();
};

// A module identifier.
class ModuleIdentifier : public ProcessAlgebra {
 public:
  // Constucts a module identifier with the given name.
  explicit ModuleIdentifier(const std::string& name);

  virtual ~ModuleIdentifier();

  // Factory method for module identifier.
  static std::unique_ptr<const ModuleIdentifier> Create(
      const std::string& name);

  // Returns the name of this module identifier.
  const std::string& name() const { return name_; }

  virtual void Accept(ProcessAlgebraVisitor* visitor) const;

 private:
  // The name of this module identifier.
  const std::string name_;
};

// An abstact base class for binary process algebra operations.
class BinaryProcessAlgebraOperation : public ProcessAlgebra {
 public:
  virtual ~BinaryProcessAlgebraOperation();

  // Returns the first operand for this binary process algebra operation.
  const ProcessAlgebra& operand1() const { return *operand1_; }

  // Returns the second operand for this binary process algebra operation.
  const ProcessAlgebra& operand2() const { return *operand2_; }

 protected:
  // Constructs a binary process algebra operation with the given operands.
  BinaryProcessAlgebraOperation(
      std::unique_ptr<const ProcessAlgebra>&& operand1,
      std::unique_ptr<const ProcessAlgebra>&& operand2);

 private:
  // The first operand for this binary process algebra operation.
  const std::unique_ptr<const ProcessAlgebra> operand1_;
  // The second operand for this binary process algebra operation.
  const std::unique_ptr<const ProcessAlgebra> operand2_;
};

// Supported parallel composition operators.
enum class ParallelCompositionOperator {
  ALPHABETIZED, ASYNCHRONOUS
};

// A parallel composition operation.
class ParallelComposition : public BinaryProcessAlgebraOperation {
 public:
  // Constructs a parallel composition operation with the given operator and
  // operands.
  ParallelComposition(ParallelCompositionOperator op,
                      std::unique_ptr<const ProcessAlgebra>&& operand1,
                      std::unique_ptr<const ProcessAlgebra>&& operand2);

  virtual ~ParallelComposition();

  // Factory method for parallel composition operation.
  static std::unique_ptr<const ParallelComposition> Create(
      ParallelCompositionOperator op,
      std::unique_ptr<const ProcessAlgebra>&& operand1,
      std::unique_ptr<const ProcessAlgebra>&& operand2);

  // Returns the operator for this parallel composition operation.
  ParallelCompositionOperator op() const { return op_; }

  virtual void Accept(ProcessAlgebraVisitor* visitor) const;

 private:
  // The operator for this parallel composition operation.
  const ParallelCompositionOperator op_;
};

// A restricted parallel composition operation.
class RestrictedParallelComposition : public BinaryProcessAlgebraOperation {
 public:
  // Constructs a restricted parallel composition operation with the given
  // actions and operands.
  RestrictedParallelComposition(
      const std::set<std::string>& actions,
      std::unique_ptr<const ProcessAlgebra>&& operand1,
      std::unique_ptr<const ProcessAlgebra>&& operand2);

  virtual ~RestrictedParallelComposition();

  // Factory method for restricted parallel composition operation.
  static std::unique_ptr<const RestrictedParallelComposition> Create(
      const std::set<std::string>& actions,
      std::unique_ptr<const ProcessAlgebra>&& operand1,
      std::unique_ptr<const ProcessAlgebra>&& operand2);

  // Returns the actions for this restricted parallel composition operation.
  const std::set<std::string>& actions() const { return actions_; }

  virtual void Accept(ProcessAlgebraVisitor* visitor) const;

 private:
  // The actions for this restricted parallel composition operation.
  std::set<std::string> actions_;
};

// An abstact base class for unary process algebra operations.
class UnaryProcessAlgebraOperation : public ProcessAlgebra {
 public:
  virtual ~UnaryProcessAlgebraOperation();

  // Returns the operand for this unary process algebra operation.
  const ProcessAlgebra& operand() const { return *operand_; }

 protected:
  // Constructs a unary process algebra operation with the given operand.
  UnaryProcessAlgebraOperation(std::unique_ptr<const ProcessAlgebra>&& operand);

 private:
  // The operand for this binary process algebra operation.
  const std::unique_ptr<const ProcessAlgebra> operand_;
};

// An action hiding operation.
class ActionHiding : public UnaryProcessAlgebraOperation {
 public:
  // Constructs an action hiding operation with the given actions and operand.
  ActionHiding(const std::set<std::string>& actions,
               std::unique_ptr<const ProcessAlgebra>&& operand);

  virtual ~ActionHiding();

  // Factory method for action hiding operation.
  static std::unique_ptr<const ActionHiding> Create(
      const std::set<std::string>& actions,
      std::unique_ptr<const ProcessAlgebra>&& operand);

  // Returns the actions for this action hiding operation.
  const std::set<std::string>& actions() const { return actions_; }

  virtual void Accept(ProcessAlgebraVisitor* visitor) const;

 private:
  // The actions for this action hiding operation.
  std::set<std::string> actions_;
};

// An action renaming operation.
class ActionRenaming : public UnaryProcessAlgebraOperation {
 public:
  // Constructs an action renaming operation with the given substitutions and
  // operand.
  ActionRenaming(const std::map<std::string, std::string>& substitutions,
                 std::unique_ptr<const ProcessAlgebra>&& operand);

  virtual ~ActionRenaming();

  // Factory method for action renamic operation.
  static std::unique_ptr<const ActionRenaming> Create(
      const std::map<std::string, std::string>& substitutions,
      std::unique_ptr<const ProcessAlgebra>&& operand);

  // Returns the substitutions for this action renaming operation.
  const std::map<std::string, std::string>& substitutions() const {
    return substitutions_;
  }

  virtual void Accept(ProcessAlgebraVisitor* visitor) const;

 private:
  // The substitutions for this action renaming operation.
  const std::map<std::string, std::string> substitutions_;
};

// Abstract base class for process algebra visitors.
class ProcessAlgebraVisitor {
 public:
  // Disallow copy and assign.
  ProcessAlgebraVisitor(const ProcessAlgebraVisitor&) = delete;
  ProcessAlgebraVisitor& operator=(const ProcessAlgebraVisitor&) = delete;

  virtual ~ProcessAlgebraVisitor();

  // Called by the Accept() method of the respective process algebra classes.
  virtual void VisitModuleIdentifier(const ModuleIdentifier& proc_algebra) = 0;
  virtual void VisitParallelComposition(
      const ParallelComposition& proc_algebra) = 0;
  virtual void VisitRestrictedParallelComposition(
      const RestrictedParallelComposition& proc_algebra) = 0;
  virtual void VisitActionHiding(const ActionHiding& proc_algebra) = 0;
  virtual void VisitActionRenaming(const ActionRenaming& proc_algebra) = 0;

 protected:
  ProcessAlgebraVisitor();
};

#endif  // PROCESS_ALGEBRA_H_

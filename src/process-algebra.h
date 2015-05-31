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
//
// A class hierarchy for representing process algebra expressions that supports
// the visitor pattern.

#ifndef PROCESS_ALGEBRA_H_
#define PROCESS_ALGEBRA_H_

#include <map>
#include <memory>
#include <ostream>
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
//   ProcessAlgebra* process_algebra = ...;
//   ConcreteProcessAlgebraVisitor visitor;
//   process_algebra->Accept(&visitor);
//
class ProcessAlgebra {
 public:
  virtual ~ProcessAlgebra();

  void Accept(ProcessAlgebraVisitor* visitor) const;

 private:
  virtual void DoAccept(ProcessAlgebraVisitor* visitor) const = 0;
};

// Output operator for process algebra expressions.
std::ostream& operator<<(std::ostream& os, const ProcessAlgebra& a);

// A module identifier.
class ModuleIdentifier : public ProcessAlgebra {
 public:
  // Constructs a module identifier with the given name.
  explicit ModuleIdentifier(const std::string& name);

  // Returns the name of this module identifier.
  const std::string& name() const { return name_; }

 private:
  void DoAccept(ProcessAlgebraVisitor* visitor) const override;

  std::string name_;
};

// Supported parallel composition operators.
enum class ParallelCompositionOperator { ALPHABETIZED, ASYNCHRONOUS };

// Output operator for parallel composition operators.
std::ostream& operator<<(std::ostream& os, ParallelCompositionOperator op);

// A parallel composition operation.
class ParallelComposition : public ProcessAlgebra {
 public:
  // Constructs a parallel composition operation with the given operator and
  // operands.
  explicit ParallelComposition(
      ParallelCompositionOperator op,
      std::unique_ptr<const ProcessAlgebra>&& operand1,
      std::unique_ptr<const ProcessAlgebra>&& operand2);

  // Returns the operator for this parallel composition operation.
  const ParallelCompositionOperator op() const { return op_; }

  // Returns the first operand for this parallel composition operation.
  const ProcessAlgebra& operand1() const { return *operand1_; }

  // Returns the second operand for this parallel composition operation.
  const ProcessAlgebra& operand2() const { return *operand2_; }

 private:
  void DoAccept(ProcessAlgebraVisitor* visitor) const override;

  ParallelCompositionOperator op_;
  std::unique_ptr<const ProcessAlgebra> operand1_;
  std::unique_ptr<const ProcessAlgebra> operand2_;
};

// A restricted parallel composition operation.
class RestrictedParallelComposition : public ProcessAlgebra {
 public:
  // Constructs a restricted parallel composition operation with the given
  // actions and operands.
  RestrictedParallelComposition(
      const std::set<std::string>& actions,
      std::unique_ptr<const ProcessAlgebra>&& operand1,
      std::unique_ptr<const ProcessAlgebra>&& operand2);

  // Returns the actions for this restricted parallel composition operation.
  const std::set<std::string>& actions() const { return actions_; }

  // Returns the first operand for this restricted parallel composition
  // operation.
  const ProcessAlgebra& operand1() const { return *operand1_; }

  // Returns the second operand for this restricted parallel composition
  // operation.
  const ProcessAlgebra& operand2() const { return *operand2_; }

 private:
  void DoAccept(ProcessAlgebraVisitor* visitor) const override;

  std::set<std::string> actions_;
  std::unique_ptr<const ProcessAlgebra> operand1_;
  std::unique_ptr<const ProcessAlgebra> operand2_;
};

// An action hiding operation.
class ActionHiding : public ProcessAlgebra {
 public:
  // Constructs an action hiding operation with the given actions and operand.
  explicit ActionHiding(const std::set<std::string>& actions,
                        std::unique_ptr<const ProcessAlgebra>&& operand);

  // Returns the actions for this action hiding operation.
  const std::set<std::string>& actions() const { return actions_; }

  // Returns the operand for this action hiding operation.
  const ProcessAlgebra& operand() const { return *operand_; }

 private:
  void DoAccept(ProcessAlgebraVisitor* visitor) const override;

  std::set<std::string> actions_;
  std::unique_ptr<const ProcessAlgebra> operand_;
};

// An action renaming operation.
class ActionRenaming : public ProcessAlgebra {
 public:
  // Constructs an action renaming operation with the given substitutions and
  // operand.
  explicit ActionRenaming(
      const std::map<std::string, std::string>& substitutions,
      std::unique_ptr<const ProcessAlgebra>&& operand);

  // Returns the substitutions for this action renaming operation.
  const std::map<std::string, std::string>& substitutions() const {
    return substitutions_;
  }

  const ProcessAlgebra& operand() const { return *operand_; }

 private:
  void DoAccept(ProcessAlgebraVisitor* visitor) const override;

  std::map<std::string, std::string> substitutions_;
  std::unique_ptr<const ProcessAlgebra> operand_;
};

// Abstract base class for process algebra expression visitors.
class ProcessAlgebraVisitor {
 public:
  void VisitModuleIdentifier(const ModuleIdentifier& process_algebra);
  void VisitParallelComposition(const ParallelComposition& process_algebra);
  void VisitRestrictedParallelComposition(
      const RestrictedParallelComposition& process_algebra);
  void VisitActionHiding(const ActionHiding& process_algebra);
  void VisitActionRenaming(const ActionRenaming& process_algebra);

 protected:
  ~ProcessAlgebraVisitor();

 private:
  virtual void DoVisitModuleIdentifier(
      const ModuleIdentifier& process_algebra) = 0;
  virtual void DoVisitParallelComposition(
      const ParallelComposition& process_algebra) = 0;
  virtual void DoVisitRestrictedParallelComposition(
      const RestrictedParallelComposition& process_algebra) = 0;
  virtual void DoVisitActionHiding(const ActionHiding& process_algebra) = 0;
  virtual void DoVisitActionRenaming(const ActionRenaming& process_algebra) = 0;
};

#endif  // PROCESS_ALGEBRA_H_

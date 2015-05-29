// Copyright (C) 2003--2005 Carnegie Mellon University
// Copyright (C) 2011--2013 Google Inc
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
// A class hierarchy for representing compiled properties that supports the
// visitor pattern.

#ifndef COMPILED_PROPERTY_H_
#define COMPILED_PROPERTY_H_

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "compiled-expression.h"
#include "ddutil.h"
#include "optional.h"
#include "unique-ptr-vector.h"

class CompiledPropertyVisitor;

// Abstract base class for compiled properties that supports the visitor
// pattern.
class CompiledProperty {
 public:
  virtual ~CompiledProperty();

  bool is_probabilistic() const { return is_probabilistic_; }

  void Accept(CompiledPropertyVisitor* visitor) const;

 protected:
  explicit CompiledProperty(bool is_probabilistic);

 private:
  virtual void DoAccept(CompiledPropertyVisitor* visitor) const = 0;

  bool is_probabilistic_;
};

// Output operator for compiled properties.
std::ostream& operator<<(std::ostream& os, const CompiledProperty& p);

class CompiledPathPropertyVisitor;

// Abstract base class for compiled path properties that supports the visitor
// pattern.
class CompiledPathProperty {
 public:
  virtual ~CompiledPathProperty();

  int index() const { return index_; }

  bool is_probabilistic() const { return is_probabilistic_; }

  bool is_unbounded() const { return is_unbounded_; }

  const std::string& string() const { return string_; }

  void Accept(CompiledPathPropertyVisitor* visitor) const;

 protected:
  explicit CompiledPathProperty(int index, bool is_probabilistic,
                                bool is_unbounded, const std::string& string);

 private:
  virtual void DoAccept(CompiledPathPropertyVisitor* visitor) const = 0;

  int index_;
  bool is_probabilistic_;
  bool is_unbounded_;
  std::string string_;
};

// Output operator for compiled path properties.
std::ostream& operator<<(std::ostream& os, const CompiledPathProperty& p);

// A compiled expression property.
class CompiledExpressionProperty : public CompiledProperty {
 public:
  explicit CompiledExpressionProperty(const CompiledExpression& expr);

  const CompiledExpression& expr() const { return expr_; }

 private:
  void DoAccept(CompiledPropertyVisitor* visitor) const override;

  CompiledExpression expr_;
};

// Supported operators for compiled n-ary logic operator property.
enum class CompiledNaryOperator { AND, OR, IFF };

// Output operator for compiled n-ary logic operators.
std::ostream& operator<<(std::ostream& os, CompiledNaryOperator op);

// A compiled n-ary logic operator property.
class CompiledNaryProperty : public CompiledProperty {
 public:
  explicit CompiledNaryProperty(
      CompiledNaryOperator op,
      std::unique_ptr<const CompiledExpressionProperty>&& optional_expr_operand,
      UniquePtrVector<const CompiledProperty>&& other_operands);

  CompiledNaryOperator op() const { return op_; }

  bool has_expr_operand() const { return optional_expr_operand_ != nullptr; }

  const CompiledExpressionProperty& expr_operand() const {
    return *optional_expr_operand_;
  }

  const UniquePtrVector<const CompiledProperty>& other_operands() const {
    return other_operands_;
  }

 private:
  void DoAccept(CompiledPropertyVisitor* visitor) const override;

  CompiledNaryOperator op_;
  std::unique_ptr<const CompiledExpressionProperty> optional_expr_operand_;
  UniquePtrVector<const CompiledProperty> other_operands_;
};

// A compiled logic negation property.
class CompiledNotProperty : public CompiledProperty {
 public:
  explicit CompiledNotProperty(
      std::unique_ptr<const CompiledProperty>&& operand);

  const CompiledProperty& operand() const { return *operand_; }

 private:
  void DoAccept(CompiledPropertyVisitor* visitor) const override;

  std::unique_ptr<const CompiledProperty> operand_;
};

// Supported operators for compiled probabability threshold operations.
enum class CompiledProbabilityThresholdOperator { GREATER_EQUAL, GREATER };

// Output operator for compiled probability threshold operators.
std::ostream& operator<<(std::ostream& os,
                         CompiledProbabilityThresholdOperator op);

// A compiled probability threshold property
class CompiledProbabilityThresholdProperty : public CompiledProperty {
 public:
  explicit CompiledProbabilityThresholdProperty(
      CompiledProbabilityThresholdOperator op, double threshold,
      std::unique_ptr<const CompiledPathProperty>&& path_property);

  CompiledProbabilityThresholdOperator op() const { return op_; }

  double threshold() const { return threshold_; }

  const CompiledPathProperty& path_property() const { return *path_property_; }

 private:
  void DoAccept(CompiledPropertyVisitor* visitor) const override;

  CompiledProbabilityThresholdOperator op_;
  double threshold_;
  std::unique_ptr<const CompiledPathProperty> path_property_;
};

// A compiled probability estimation property.
class CompiledProbabilityEstimationProperty : public CompiledProperty {
 public:
  explicit CompiledProbabilityEstimationProperty(
      std::unique_ptr<const CompiledPathProperty>&& path_property);

  const CompiledPathProperty& path_property() const { return *path_property_; }

 private:
  void DoAccept(CompiledPropertyVisitor* visitor) const override;

  std::unique_ptr<const CompiledPathProperty> path_property_;
};

// A compiled until property.
class CompiledUntilProperty : public CompiledPathProperty {
 public:
  explicit CompiledUntilProperty(double min_time, double max_time,
                                 std::unique_ptr<const CompiledProperty>&& pre,
                                 std::unique_ptr<const CompiledProperty>&& post,
                                 int index, const std::string& string);

  double min_time() const { return min_time_; }

  double max_time() const { return max_time_; }

  const CompiledProperty& pre_property() const { return *pre_property_; }

  const CompiledProperty& post_property() const { return *post_property_; }

 private:
  void DoAccept(CompiledPathPropertyVisitor* visitor) const override;

  double min_time_;
  double max_time_;
  std::unique_ptr<const CompiledProperty> pre_property_;
  std::unique_ptr<const CompiledProperty> post_property_;
};

// Abstract base class for compiled property visitors.
class CompiledPropertyVisitor {
 public:
  void VisitCompiledNaryProperty(const CompiledNaryProperty& property);
  void VisitCompiledNotProperty(const CompiledNotProperty& property);
  void VisitCompiledProbabilityThresholdProperty(
      const CompiledProbabilityThresholdProperty& property);
  void VisitCompiledProbabilityEstimationProperty(
      const CompiledProbabilityEstimationProperty& property);
  void VisitCompiledExpressionProperty(
      const CompiledExpressionProperty& property);

 protected:
  ~CompiledPropertyVisitor();

 private:
  virtual void DoVisitCompiledNaryProperty(
      const CompiledNaryProperty& property) = 0;
  virtual void DoVisitCompiledNotProperty(
      const CompiledNotProperty& property) = 0;
  virtual void DoVisitCompiledProbabilityThresholdProperty(
      const CompiledProbabilityThresholdProperty& property) = 0;
  virtual void DoVisitCompiledProbabilityEstimationProperty(
      const CompiledProbabilityEstimationProperty& property) = 0;
  virtual void DoVisitCompiledExpressionProperty(
      const CompiledExpressionProperty& property) = 0;
};

// Abstract base class for compiled path property visitors.
class CompiledPathPropertyVisitor {
 public:
  void VisitCompiledUntilProperty(const CompiledUntilProperty& path_property);

 protected:
  ~CompiledPathPropertyVisitor();

 private:
  virtual void DoVisitCompiledUntilProperty(
      const CompiledUntilProperty& path_property) = 0;
};

// The result of a property expression compilation.  On success, property will
// hold the compiled property.  On error, property will be null and errors will
// be populated with error messages.
struct CompilePropertyResult {
  std::unique_ptr<const CompiledProperty> property;
  std::vector<std::string> errors;
};

// Compiles the given expression property, expecting it to be of type bool, and
// using the given formula and label name to expression maps and identifier name
// to info map to compile identifiers.  On error, the result contains a null
// compiled property and the errors vector will be populated with error
// messages.
CompilePropertyResult CompileProperty(
    const Expression& expr,
    const std::map<std::string, const Expression*>& formulas_by_name,
    const std::map<std::string, const Expression*>& labels_by_name,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    const Optional<DecisionDiagramManager>& dd_manager);

// Optimizes the given property.
std::unique_ptr<const CompiledProperty> OptimizeProperty(
    const CompiledProperty& property,
    const Optional<DecisionDiagramManager>& dd_manager);

// Returns the number of integer and double registers referenced by the given
// compiled expression.
std::pair<int, int> GetPropertyRegisterCounts(const CompiledProperty& property);

#endif  // COMPILED_PROPERTY_H_

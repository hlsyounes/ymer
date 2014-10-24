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

#include "compiled-expression.h"
#include "ddutil.h"
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

  const std::string& string() const { return string_; }

  void Accept(CompiledPathPropertyVisitor* visitor) const;

 protected:
  explicit CompiledPathProperty(int index, bool is_probabilistic,
                                const std::string& string);

 private:
  virtual void DoAccept(CompiledPathPropertyVisitor* visitor) const = 0;

  int index_;
  bool is_probabilistic_;
  std::string string_;
};

// Output operator for compiled path properties.
std::ostream& operator<<(std::ostream& os, const CompiledPathProperty& p);

// Supported operators for compiled n-ary logic operator property.
enum class CompiledNaryOperator {
  AND, OR, IFF
};

// Output operator for compiled n-ary logic operators.
std::ostream& operator<<(std::ostream& os, CompiledNaryOperator op);

// A compiled n-ary logic operator property.
class CompiledNaryProperty : public CompiledProperty {
 public:
  explicit CompiledNaryProperty(
      CompiledNaryOperator op,
      const CompiledExpression& optional_expr_operand,
      const BDD& expr_operand_bdd,
      UniquePtrVector<const CompiledProperty>&& other_operands);

  static std::unique_ptr<const CompiledNaryProperty> New(
      CompiledNaryOperator op,
      const CompiledExpression& optional_expr_operand,
      const BDD& expr_operand_bdd,
      UniquePtrVector<const CompiledProperty>&& other_operands);

  CompiledNaryOperator op() const { return op_; }

  bool has_expr_operand() const {
    return !optional_expr_operand_.operations().empty();
  }

  const CompiledExpression& expr_operand() const {
    return optional_expr_operand_;
  }

  const BDD& expr_operand_bdd() const { return expr_operand_bdd_; }

  const UniquePtrVector<const CompiledProperty>& other_operands() const {
    return other_operands_;
  }

 private:
  virtual void DoAccept(CompiledPropertyVisitor* visitor) const;

  CompiledNaryOperator op_;
  CompiledExpression optional_expr_operand_;
  BDD expr_operand_bdd_;
  UniquePtrVector<const CompiledProperty> other_operands_;
};

// A compiled logic negation property.
class CompiledNotProperty : public CompiledProperty {
 public:
  explicit CompiledNotProperty(
      std::unique_ptr<const CompiledProperty>&& operand);

  static std::unique_ptr<const CompiledNotProperty> New(
      std::unique_ptr<const CompiledProperty>&& operand);

  const CompiledProperty& operand() const { return *operand_; }

 private:
  virtual void DoAccept(CompiledPropertyVisitor* visitor) const;

  std::unique_ptr<const CompiledProperty> operand_;
};

// Supported operators for compiled probabability threshold operations.
enum class CompiledProbabilityThresholdOperator {
  GREATER_EQUAL, GREATER
};

// Output operator for compiled probability threshold operators.
std::ostream& operator<<(std::ostream& os,
                         CompiledProbabilityThresholdOperator op);

// A compiled probability threshold property
class CompiledProbabilityThresholdProperty : public CompiledProperty {
 public:
  explicit CompiledProbabilityThresholdProperty(
      CompiledProbabilityThresholdOperator op, double threshold,
      std::unique_ptr<const CompiledPathProperty>&& path_property);

  static std::unique_ptr<const CompiledProbabilityThresholdProperty> New(
      CompiledProbabilityThresholdOperator op,
      double threshold,
      std::unique_ptr<const CompiledPathProperty>&& path_property);

  CompiledProbabilityThresholdOperator op() const { return op_; }

  double threshold() const { return threshold_; }

  const CompiledPathProperty& path_property() const { return *path_property_; }

 private:
  virtual void DoAccept(CompiledPropertyVisitor* visitor) const;

  CompiledProbabilityThresholdOperator op_;
  double threshold_;
  std::unique_ptr<const CompiledPathProperty> path_property_;
};

// A compiled expression property.
class CompiledExpressionProperty : public CompiledProperty {
 public:
  explicit CompiledExpressionProperty(
      const CompiledExpression& expr, const BDD& bdd);

  static std::unique_ptr<const CompiledExpressionProperty> New(
      const CompiledExpression& expr, const BDD& bdd);

  const CompiledExpression& expr() const { return expr_; }

  const BDD& bdd() const { return bdd_; }

 private:
  virtual void DoAccept(CompiledPropertyVisitor* visitor) const;

  CompiledExpression expr_;
  BDD bdd_;
};

// A compiled until property.
class CompiledUntilProperty : public CompiledPathProperty {
 public:
  explicit CompiledUntilProperty(double min_time, double max_time,
                                 std::unique_ptr<const CompiledProperty>&& pre,
                                 std::unique_ptr<const CompiledProperty>&& post,
                                 int index, const std::string& string);

  static std::unique_ptr<const CompiledUntilProperty> New(
      double min_time, double max_time,
      std::unique_ptr<const CompiledProperty>&& pre_property,
      std::unique_ptr<const CompiledProperty>&& post_property,
      int index, const std::string& string);

  double min_time() const { return min_time_; }

  double max_time() const { return max_time_; }

  const CompiledProperty& pre_property() const { return *pre_property_; }

  const CompiledProperty& post_property() const { return *post_property_; }

 private:
  virtual void DoAccept(CompiledPathPropertyVisitor* visitor) const;

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
// using the given identifier name to info map to compile identifiers.  On
// error, the result contains a null compiled property and the errors vector
// will be populated with error messages.
CompilePropertyResult CompileProperty(
    const Expression& expr,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    const DecisionDiagramManager& dd_manager);

// The result of a path property compilation on success, path_property will hold
// the compiled path propetty.  On error, path_property will be null and errors
// will be populated with error message.
struct CompilePathPropertyResult {
  std::unique_ptr<const CompiledPathProperty> path_property;
  std::vector<std::string> errors;
};

// Compiles the given path property, using the given identifier name to info map
// to compile identifiers.  On error, the result contains a null compiled path
// property and the errors vector will be populated with error messages.
CompilePathPropertyResult CompilePathProperty(
    const PathProperty& path_property,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name);

// Optimizes the given property.
std::unique_ptr<const CompiledProperty> OptimizeProperty(
    const CompiledProperty& property);

// Optimizes the given path property.
std::unique_ptr<const CompiledPathProperty> OptimizePathProperty(
    const CompiledPathProperty& path_property);

#endif  // COMPILED_PROPERTY_H_

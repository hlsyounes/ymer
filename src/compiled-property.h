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
#include "pointer-vector.h"

class CompiledPropertyVisitor;

// Abstract base class for compiled properties that supports the visitor
// pattern.
class CompiledProperty {
 public:
  // Disallow copy and assign.
  CompiledProperty(const CompiledProperty&) = delete;
  CompiledProperty& operator=(const CompiledProperty&) = delete;

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
  // Disallow copy and assign.
  CompiledPathProperty(const CompiledPathProperty&) = delete;
  CompiledPathProperty& operator=(const CompiledPathProperty&) = delete;

  virtual ~CompiledPathProperty();

  int index() const { return index_; }

  bool is_probabilistic() const { return is_probabilistic_; }

  const std::string& string() const { return string_; }

  void Accept(CompiledPathPropertyVisitor* visitor) const;

 protected:
  CompiledPathProperty(int index, bool is_probabilistic,
                       const std::string& string);

 private:
  virtual void DoAccept(CompiledPathPropertyVisitor* visitor) const = 0;

  int index_;
  bool is_probabilistic_;
  std::string string_;
};

// Output operator for compiled path properties.
std::ostream& operator<<(std::ostream& os, const CompiledPathProperty& p);

// A compiled AND property.
class CompiledAndProperty : public CompiledProperty {
 public:
  virtual ~CompiledAndProperty();

  static std::unique_ptr<const CompiledProperty> Make(
      PointerVector<const CompiledProperty>&& operands);

  const PointerVector<const CompiledProperty>& operands() const {
    return operands_;
  }

 private:
  CompiledAndProperty(PointerVector<const CompiledProperty>&& operands);

  virtual void DoAccept(CompiledPropertyVisitor* visitor) const;

  PointerVector<const CompiledProperty> operands_;
};

// A compiled NOT property.
class CompiledNotProperty : public CompiledProperty {
 public:
  virtual ~CompiledNotProperty();

  static std::unique_ptr<const CompiledProperty> Make(
      std::unique_ptr<const CompiledProperty>&& operand);

  const CompiledProperty& operand() const { return *operand_; }

 private:
  CompiledNotProperty(std::unique_ptr<const CompiledProperty>&& operand);

  virtual void DoAccept(CompiledPropertyVisitor* visitor) const;

  std::unique_ptr<const CompiledProperty> operand_;
};

// A compiled probabilistic property.
class CompiledProbabilisticProperty : public CompiledProperty {
 public:
  enum class Operator {
    GREATER_EQUAL, GREATER
  };

  virtual ~CompiledProbabilisticProperty();

  static std::unique_ptr<const CompiledProperty> MakeGreaterEqual(
      double threshold,
      std::unique_ptr<const CompiledPathProperty>&& path_property);
  static std::unique_ptr<const CompiledProperty> MakeGreater(
      double threshold,
      std::unique_ptr<const CompiledPathProperty>&& path_property);

  Operator op() const { return op_; }

  double threshold() const { return threshold_; }

  const CompiledPathProperty& path_property() const { return *path_property_; }

 private:
  CompiledProbabilisticProperty(
      Operator op, double threshold,
      std::unique_ptr<const CompiledPathProperty>&& path_property);

  virtual void DoAccept(CompiledPropertyVisitor* visitor) const;

  Operator op_;
  double threshold_;
  std::unique_ptr<const CompiledPathProperty> path_property_;
};

// A compiled expression property.
class CompiledExpressionProperty : public CompiledProperty {
 public:
  virtual ~CompiledExpressionProperty();

  static std::unique_ptr<const CompiledProperty> Make(
      const CompiledExpression& expr);

  const CompiledExpression& expr() const { return expr_; }

 private:
  explicit CompiledExpressionProperty(const CompiledExpression& expr);

  virtual void DoAccept(CompiledPropertyVisitor* visitor) const;

  CompiledExpression expr_;
};

// TODO(hlsyounes): get rid of dependency on Until class.
class Until;

// A compiled until property.
class CompiledUntilProperty : public CompiledPathProperty {
 public:
  virtual ~CompiledUntilProperty();

  static std::unique_ptr<const CompiledPathProperty> Make(
      double min_time, double max_time,
      std::unique_ptr<const CompiledProperty>&& pre,
      std::unique_ptr<const CompiledProperty>&& post,
      int index, const std::string& string, const Until* formula);

  double min_time() const { return min_time_; }

  double max_time() const { return max_time_; }

  const CompiledProperty& pre() const { return *pre_; }

  const CompiledProperty& post() const { return *post_; }

  const Until& formula() const { return *formula_; }

 private:
  CompiledUntilProperty(double min_time, double max_time,
                        std::unique_ptr<const CompiledProperty>&& pre,
                        std::unique_ptr<const CompiledProperty>&& post,
                        int index, const std::string& string,
                        const Until* formula);

  virtual void DoAccept(CompiledPathPropertyVisitor* visitor) const;

  double min_time_;
  double max_time_;
  std::unique_ptr<const CompiledProperty> pre_;
  std::unique_ptr<const CompiledProperty> post_;
  const Until* formula_;
};

// Abstract base class for compiled property visitors.
class CompiledPropertyVisitor {
 public:
  void VisitCompiledAndProperty(const CompiledAndProperty& property);
  void VisitCompiledNotProperty(const CompiledNotProperty& property);
  void VisitCompiledProbabilisticProperty(
      const CompiledProbabilisticProperty& property);
  void VisitCompiledExpressionProperty(
      const CompiledExpressionProperty& property);

 protected:
  CompiledPropertyVisitor();
  CompiledPropertyVisitor(const CompiledPropertyVisitor&);
  CompiledPropertyVisitor& operator=(const CompiledPropertyVisitor&);
  ~CompiledPropertyVisitor();

 private:
  virtual void DoVisitCompiledAndProperty(
      const CompiledAndProperty& property) = 0;
  virtual void DoVisitCompiledNotProperty(
      const CompiledNotProperty& property) = 0;
  virtual void DoVisitCompiledProbabilisticProperty(
      const CompiledProbabilisticProperty& property) = 0;
  virtual void DoVisitCompiledExpressionProperty(
      const CompiledExpressionProperty& property) = 0;
};

// Abstract base class for compiled path property visitors.
class CompiledPathPropertyVisitor {
 public:
  void VisitCompiledUntilProperty(const CompiledUntilProperty& property);

 protected:
  CompiledPathPropertyVisitor();
  CompiledPathPropertyVisitor(const CompiledPathPropertyVisitor&);
  CompiledPathPropertyVisitor& operator=(const CompiledPathPropertyVisitor&);
  ~CompiledPathPropertyVisitor();

 private:
  virtual void DoVisitCompiledUntilProperty(
      const CompiledUntilProperty& property) = 0;
};

#endif  // COMPILED_PROPERTY_H_

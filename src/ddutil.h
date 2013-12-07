// Copyright (C) 2003--2005 Carnegie Mellon University
// Copyright (C) 2011--2012 Google Inc
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
// Convenient wrappers for CUDD library.

#ifndef DDUTIL_H_
#define DDUTIL_H_

#include <vector>

class DdManager;
class DdNode;

template <typename DD> class VariableArray;

// Abstract wrapper class for DdNode, with automatic referencing and
// dereferencing.
class DecisionDiagram {
 public:
  // Returns true if this decision diagram is a constant.
  bool IsConstant() const;

  // Returns the minterm count for this decision diagram assuming the given
  // number of variables.
  double MintermCount(int num_variables) const;

  // TODO(hlsyounes): remove once all code is using wrapper classes.
  DdNode* release();
  DdNode* get() const { return node(); }

 protected:
  DecisionDiagram(DdManager* manager, DdNode* node);
  DecisionDiagram(const DecisionDiagram& dd);
  DecisionDiagram& operator=(const DecisionDiagram& dd);

  ~DecisionDiagram();

  DdManager* manager() const { return manager_; }
  DdNode* node() const { return node_; }

 private:
  void Ref();
  void Deref();

  // The decision-diagram manager.
  DdManager* manager_;
  // The decision-diagram node.
  DdNode* node_;

  template <typename DD> friend class VariableArray;
};

class ADD;

// Wrapper class for BDDs, with automatic referencing and dereferencing.
class BDD : public DecisionDiagram {
 public:
  // Returns the value of this BDD.  Requires that this BDD is constant.
  bool Value() const;

  // Returns the value of this BDD for the given variable assignment.
  bool ValueInState(const std::vector<bool>& state) const;

  // Returns a permutation of this BDD.
  BDD Permutation(const std::vector<int>& permutation) const;

  // Returns the BDD that existentially abstracts all the variables in cube from
  // this BDD.
  BDD ExistAbstract(const BDD& cube) const;

  // Logical operators for BDDs.
  BDD operator!() const;
  BDD operator&&(const BDD& dd) const;
  BDD operator||(const BDD& dd) const;

  // Comparison operators for BDDs.
  BDD operator==(const BDD& dd) const;
  BDD operator!=(const BDD& dd) const;
  BDD operator<(const BDD& dd) const;
  BDD operator<=(const BDD& dd) const;
  BDD operator>=(const BDD& dd) const;
  BDD operator>(const BDD& dd) const;

 private:
  // A BDD operator to use with Apply().
  typedef DdNode* (*Op)(DdManager*, DdNode*, DdNode*);

  BDD(DdManager* manager, DdNode* node);

  // Returns the result of applying op to BDDs dd1 and dd2.
  static BDD Apply(Op op, const BDD& dd1, const BDD& dd2);

  friend class DecisionDiagramManager;
  friend class ADD;
  friend BDD Ite(const BDD&, const BDD&, const BDD&);
  friend ADD Ite(const BDD&, const ADD&, const ADD&);
};

// Returns the BDD for dd1 ? dd2 : dd3.
BDD Ite(const BDD& dd1, const BDD& dd2, const BDD& dd3);

// Wrapper class for ADDs, with automatic referencing and dereferencing.
class ADD : public DecisionDiagram {
 public:
  // Explicit conversion from BDD to ADD.
  explicit ADD(const BDD& dd);

  // Returns the value of this ADD.  Requires that this ADD is constant.
  double Value() const;

  // Returns the value of this BDD for the given variable assignment.
  double ValueInState(const std::vector<bool>& state) const;

  // Returns the BDD representing low <= *this <= high.
  BDD Interval(double low, double high) const;

  // Returns the BDD representing *this > threshold.
  BDD StrictThreshold(double threshold) const;

  // Arithmetic operators for ADDs.
  ADD operator+(const ADD& dd) const;
  ADD operator-(const ADD& dd) const;
  ADD operator*(const ADD& dd) const;
  ADD operator/(const ADD& dd) const;

  // Comparison operators for ADDs.
  BDD operator==(const ADD& dd) const;
  BDD operator!=(const ADD& dd) const;
  BDD operator<(const ADD& dd) const;
  BDD operator<=(const ADD& dd) const;
  BDD operator>=(const ADD& dd) const;
  BDD operator>(const ADD& dd) const;

 private:
  // An ADD operator to use with Apply().
  typedef DdNode* (*Op)(DdManager*, DdNode**, DdNode**);

  ADD(DdManager* manager, DdNode* node);

  // Returns the result of applying op to ADDs dd1 and dd2.
  static ADD Apply(Op op, const ADD& dd1, const ADD& dd2);

  friend class DecisionDiagramManager;
  friend ADD Ite(const BDD&, const ADD&, const ADD&);
};

// Returns the ADD for dd1 ? dd2 : dd3.
ADD Ite(const BDD& dd1, const ADD& dd2, const ADD& dd3);

// Wrapper class for variable arrays.
template <typename DD>
class VariableArray {
 public:
  VariableArray();

  std::vector<DdNode*>::size_type size() const { return variables_.size(); }

 private:
  VariableArray(std::vector<DD> variables);

  DdNode* const * data() const { return variables_.data(); }

  std::vector<DdNode*> variables_;

  friend class DecisionDiagramManager;
};

// Wrapper class for DdManager.
class DecisionDiagramManager {
 public:
  // Constructs a decision-diagram manager with the given number of variables.
  explicit DecisionDiagramManager(int num_variables);

  // Disallow copy and assign.
  DecisionDiagramManager(const DecisionDiagramManager&) = delete;
  DecisionDiagramManager& operator=(const DecisionDiagramManager&) = delete;

  ~DecisionDiagramManager();

  // Returns the number of variables.
  int GetNumVariables() const;

  // Sets the epsilon parameter of this manager.  The epsilon parameter
  // controls the comparison between floating point numbers.  One effect of
  // the epsilon parameter is that any value less than epsilon is represented
  // as 0.  By default, epsilon is set to std::numeric_limits<double>::min().
  // Increasing epsilon can siplify ADDs, and thereby reducing memory
  // requirements, but it can result in unintended numeric imprecision.
  void SetEpsilon(double epsilon) const;

  // Returns the epsilon parameter of this manager.
  double GetEpsilon() const;

  // Returns the BDD for the given constant value.
  BDD GetConstant(bool value) const;

  // Returns the ADD for the given constant value.
  ADD GetConstant(int value) const;
  ADD GetConstant(double value) const;

  // Returns the BDD for the ith variable.
  BDD GetBddVariable(int i) const;

  // Returns the ADD for the ith variable.
  ADD GetAddVariable(int i) const;

  // Returns an array of every incr BDD variables between start (inclusive) and
  // end (exclusive).
  VariableArray<BDD> GetBddVariableArray(int start, int incr, int end) const;

  // Returns the positive unate cube of the given variables.
  BDD GetCube(const VariableArray<BDD>& variables) const;

  // TODO(hlsyounes): remove once all code is using wrapper classes.
  DdManager* manager() const { return manager_; }

 private:
  DdManager* manager_;
};

// Returns the base-2 logarithm of the given integer.
int Log2(int n);

template <typename DD>
VariableArray<DD>::VariableArray() = default;

template <typename DD>
VariableArray<DD>::VariableArray(std::vector<DD> variables) {
  variables_.reserve(variables.size());
  for (const DD& dd : variables) {
    variables_.push_back(dd.node());
  }
}

#endif  // DDUTIL_H_

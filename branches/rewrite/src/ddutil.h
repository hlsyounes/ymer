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
// Convenient wrappers for CUDD library.

#ifndef DDUTIL_H_
#define DDUTIL_H_

#include <vector>

class DdManager;
class DdNode;

// Abstract wrapper class for DdNode, with automatic referencing and
// dereferencing.
class DecisionDiagram {
 public:
  // Returns true if this decision diagram is a constant.
  bool IsConstant() const;

  // Prints this decision diagram to standard output at the given verbosity:
  //
  //   0: prints nothing.
  //   1: prints counts of nodes and minterms.
  //   2: prints counts + disjoint sum of product.
  //   3: prints counts + list of nodes.
  //   > 3: prints counts + disjoint sum of product + list of nodes.
  //
  void PrintDebug(int verbosity) const;

 protected:
  DecisionDiagram(DdManager* dd_manager, DdNode* dd_node);

  DecisionDiagram(DecisionDiagram&& dd);
  DecisionDiagram(const DecisionDiagram& dd);
  DecisionDiagram& operator=(const DecisionDiagram& dd);

  ~DecisionDiagram();

  DdManager* dd_manager() const { return dd_manager_; }
  DdNode* dd_node() const { return dd_node_; }

 private:
  void Ref();
  void Deref();

  // The decision-diagram manager.
  DdManager* dd_manager_;
  // The decision-diagram node.
  DdNode* dd_node_;
};

class ADD;

// Wrapper class for BDDs, with automatic referencing and dereferencing.
class BDD : public DecisionDiagram {
 public:
  BDD(BDD&& dd);
  BDD(const BDD& dd);
  BDD& operator=(const BDD& dd);

  ~BDD() {}

  // Returns the value of BDD.  Requires that this BDD is constant.
  bool BoolValue() const;

  // Returns the BDD for !(*this).
  BDD operator!() const;

  // Returns the BDD for (*this) && dd.
  BDD operator&&(const BDD& dd) const;

  // Returns the BDD for (*this) || dd.
  BDD operator||(const BDD& dd) const;

  // Returns the BDD for (*this) == dd.
  BDD operator==(const BDD& dd) const;

  // Returns the BDD for (*this) != dd.
  BDD operator!=(const BDD& dd) const;

  // Returns the BDD for (*this) < dd.
  BDD operator<(const BDD& dd) const;

  // Returns the BDD for (*this) <= dd.
  BDD operator<=(const BDD& dd) const;

  // Returns the BDD for (*this) >= dd.
  BDD operator>=(const BDD& dd) const;

  // Returns the BDD for (*this) > dd.
  BDD operator>(const BDD& dd) const;

  // Returns the BDD that existentially abstracts all the variables in cube
  // from this decision diagram.
  BDD ExistAbstract(const BDD& cube) const;

 private:
  // A BDD operator to use with BddApply().
  typedef DdNode* (*Op)(DdManager*, DdNode*, DdNode*);

  BDD(DdManager* dd_manager, DdNode* dd_node);

  // Returns the result of applying op to BDDs dd1 and dd2.
  static BDD Apply(Op op, const BDD& dd1, const BDD& dd2);

  friend class DecisionDiagramManager;
  friend class ADD;
  friend BDD Ite(const BDD&, const BDD&, const BDD&);
  friend ADD Ite(const BDD&, const ADD&, const ADD&);
};

// Wrapper class for ADDs, with automatic referencing and dereferencing.
class ADD : public DecisionDiagram {
 public:
  ADD(ADD&& dd);
  ADD(const ADD& dd);
  ADD& operator=(const ADD& dd);

  ~ADD() {}

  // Returns the value of this ADD.  Requires that this ADD is constant.
  double DoubleValue() const;

  // Returns the ADD for -(*this).
  ADD operator-() const;

  // Returns the ADD for (*this) * dd.
  ADD operator*(const ADD& dd) const;

  // Returns the ADD for (*this) / dd.
  ADD operator/(const ADD& dd) const;

  // Returns the ADD for (*this) + dd.
  ADD operator+(const ADD& dd) const;

  // Returns the ADD for (*this) - dd.
  ADD operator-(const ADD& dd) const;

  // Returns the ADD for (*this) % dd.
  ADD operator%(const ADD& dd) const;

  // Returns the BDD for (*this) == dd.
  BDD operator==(const ADD& dd) const;

  // Returns the BDD for (*this) != dd.
  BDD operator!=(const ADD& dd) const;

  // Returns the BDD for (*this) < dd.
  BDD operator<(const ADD& dd) const;

  // Returns the BDD for (*this) <= dd.
  BDD operator<=(const ADD& dd) const;

  // Returns the BDD for (*this) >= dd.
  BDD operator>=(const ADD& dd) const;

  // Returns the BDD for (*this) > dd.
  BDD operator>(const ADD& dd) const;

  // Returns the ADD that existentially abstracts all the variables in cube
  // from this decision diagram.
  ADD ExistAbstract(const ADD& cube) const;

 private:
  // An ADD operator to use with Apply().
  typedef DdNode* (*Op)(DdManager*, DdNode**, DdNode**);
  // An ADD operator to use with MonadicApply().
  typedef DdNode* (*MonadicOp)(DdManager*, DdNode*);

  ADD(DdManager* dd_manager, DdNode* dd_node);

  // Returns the BDD for (*this) == value.
  BDD operator==(double value) const;

  // Returns the BDD for (*this) > value.
  BDD operator>(double value) const;

  // Returns the result of applying op to ADDs dd1 and dd2.
  static ADD Apply(Op op, const ADD& dd1, const ADD& dd2);

  // Returns the result of applying op to ADD dd.
  static ADD MonadicApply(MonadicOp op, const ADD& dd);

  friend class DecisionDiagramManager;
  friend ADD Ite(const BDD&, const ADD&, const ADD&);
  friend ADD min(const ADD& dd1, const ADD& dd2);
  friend ADD max(const ADD& dd1, const ADD& dd2);
  friend ADD floor(const ADD& dd);
  friend ADD ceil(const ADD& dd);
  friend ADD pow(const ADD& dd1, const ADD& dd2);
  friend ADD log(const ADD& dd);
};

// Returns the BDD for dd1 ? dd2 : dd3.
BDD Ite(const BDD& dd1, const BDD& dd2, const BDD& dd3);

// Returns the ADD for dd1 ? dd2 : dd3.
ADD Ite(const BDD& dd1, const ADD& dd2, const ADD& dd3);

// Returns the ADD for min(dd1, dd2).
ADD min(const ADD& dd1, const ADD& dd2);

// Returns the ADD for max(dd1, dd2).
ADD max(const ADD& dd1, const ADD& dd2);

// Returns the ADD for floor(dd).
ADD floor(const ADD& dd);

// Returns the ADD for ceil(dd).
ADD ceil(const ADD& dd);

// Returns the ADD for pow(dd1, dd2).
ADD pow(const ADD& dd1, const ADD& dd2);

// Returns the ADD for log(dd).
ADD log(const ADD& dd);

// Wrapper class for DdManager.
class DecisionDiagramManager {
 public:
  // Constructs a decision-diagram manager with the given number of variables.
  explicit DecisionDiagramManager(int num_variables);

  ~DecisionDiagramManager();

  // Disallow copy and assign.
  DecisionDiagramManager(const DecisionDiagramManager&) = delete;
  DecisionDiagramManager& operator=(const DecisionDiagramManager&) = delete;

  // Returns the number of variables.
  int num_variables() const;

  // Sets the epsilon parameter of this manager.  The epsilon parameter
  // controls the comparison between floating point numbers.  One effect of
  // the epsilon parameter is that any value less than epsilon is represented
  // as 0.  By default, epsilon is set to std::numeric_limits<double>::min().
  // Increasing epsilon can siplify ADDs, and thereby reducing memory
  // requirements, but it can result in unintended numeric imprecision.
  void SetEpsilon(double epsilon);

  // Returns the epsilon parameter of this manager.
  double GetEpsilon() const;

  // Returns a BDD for the given constant value.
  BDD GetConstant(bool value) const;

  // Returns an ADD for the given constant value.
  ADD GetConstant(int value) const;

  // Returns an ADD for the given constant value.
  ADD GetConstant(double value) const;

  // Returns a BDD for the ith variable.
  BDD GetBddVariable(int i) const;

  // Returns an ADD for the ith variable.
  ADD GetAddVariable(int i) const;

  // Returns the BDD cube for all variables.
  BDD GetBddCube() const;

  // Returns the ADD cube for all variables.
  ADD GetAddCube() const;

  // Returns the BDD representing x = x', assuming the bits representing x and
  // x' are interleaved.
  BDD VariableIdentity(int low_bit, int high_bit) const;

 private:
  DdManager* const dd_manager_;
};

#endif  // DDUTIL_H_

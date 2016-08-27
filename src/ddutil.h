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

#include <functional>
#include <iterator>
#include <string>
#include <vector>

#include "optional.h"

struct DdManager;
struct DdNode;

template <typename DD>
class VariableArray;
class ODD;

// Information for a state variable, used for evaluating a decision diagram in a
// state represented by integer-valued state variables.
class StateVariableInfo {
 public:
  StateVariableInfo(const std::string& name, int min_value, int bit_count);

  const std::string& name() const { return name_; }
  int min_value() const { return min_value_; }
  int bit_count() const { return bit_count_; }

 private:
  std::string name_;
  int min_value_;
  int bit_count_;
};

// Abstract wrapper class for DdNode, with automatic referencing and
// dereferencing.
class DecisionDiagram {
 public:
  // Returns true if this decision diagram is a constant.
  bool IsConstant() const;

  // Returns the node count for this decision diagram.
  int NodeCount() const;

  // Returns the leaf count for this decision diagram.
  int LeafCount() const;

  // Returns the minterm count for this decision diagram assuming the given
  // variable count.
  double MintermCount(int variable_count) const;

  // Returns a short debug string with node, leaf, and minterm counts (assuming
  // the given variable count) for this decision diagram.
  std::string ShortDebugString(int variable_count) const;

  // Returns true if the given decision diagram is the same as this one.
  bool is_same(const DecisionDiagram& dd) { return node_ == dd.node_; }

  // TODO(hlsyounes): remove once all code is using wrapper classes.
  DdNode* get() const { return node(); }

 protected:
  DecisionDiagram(DdManager* manager, DdNode* node);
  DecisionDiagram(const DecisionDiagram& dd);
  DecisionDiagram& operator=(const DecisionDiagram& dd);

  ~DecisionDiagram();

  DdManager* manager() const { return manager_; }
  DdNode* node() const { return node_; }

 private:
  static void Ref(DdNode* node);
  void Ref();
  static void Deref(DdManager* manager, DdNode* node);
  void Deref();

  // The decision-diagram manager.
  DdManager* manager_;
  // The decision-diagram node.
  DdNode* node_;

  template <typename DD>
  friend class VariableArray;
  friend class ODD;
};

class ADD;

// Wrapper class for BDDs, with automatic referencing and dereferencing.
class BDD : public DecisionDiagram {
 public:
  // Explicit conversion from ADD to BDD.
  explicit BDD(const ADD& dd);

  // Returns the value of this BDD.  Requires that this BDD is constant.
  bool Value() const;

  // Returns the value of this BDD for the given variable assignment.
  bool ValueInState(const std::vector<bool>& state) const;

  // Variation of ValueInState for integer-valued state variables.  Assumes an
  // interleaved bit-representation, where even bits represent the current state
  // and odd bits represent the next state (in a state transition matrix).  This
  // method assumes only even bits are set in the decision diagram.
  bool ValueInState(const std::vector<int>& values,
                    const std::vector<StateVariableInfo>& variables) const;

  // Returns a permutation of this BDD.
  BDD Permutation(const std::vector<int>& permutation) const;

  // Returns the BDD that existentially abstracts all the variables in cube from
  // this BDD.
  BDD ExistAbstract(const BDD& cube) const;

  // Returns the BDD that is the conjunction of this BDD and the given BDD and
  // simultaneously abstracts all the variables in the cube from the resulting
  // BDD.  This method corresponds to semiring matrix multiplication for the
  // Boolean semiring.
  BDD AndAbstract(const BDD& dd, const BDD& cube) const;

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

  // Returns the maximum value of this ADD.
  double MaxValue() const;

  // Returns the minimum value of this ADD.
  double MinValue() const;

  // Returns the value of this ADD for the given variable assignment.
  double ValueInState(const std::vector<bool>& state) const;

  // Variation of ValueInState for integer-valued state variables.  Assumes an
  // interleaved bit-representation, where even bits represent the current state
  // and odd bits represent the next state (in a state transition matrix).  This
  // method assumes only even bits are set in the decision diagram.
  double ValueInState(const std::vector<int>& values,
                      const std::vector<StateVariableInfo>& variables) const;

  // Returns the BDD representing low <= *this <= high.
  BDD Interval(double low, double high) const;

  // Returns the BDD representing *this > threshold.
  BDD StrictThreshold(double threshold) const;

  // Returns the ADD that abstracts through summation all the variables in cube
  // from this ADD.
  ADD ExistAbstract(const ADD& cube) const;

  // Arithmetic operators for ADDs.
  ADD operator-() const;
  ADD operator+(const ADD& dd) const;
  ADD operator-(const ADD& dd) const;
  ADD operator*(const ADD& dd) const;
  ADD operator/(const ADD& dd) const;
  ADD operator%(const ADD& dd) const;

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
  // An ADD operator to use with MonadicApply().
  typedef DdNode* (*MonadicOp)(DdManager*, DdNode*);

  ADD(DdManager* manager, DdNode* node);

  // Returns the result of applying op to ADDs dd1 and dd2.
  static ADD Apply(Op op, const ADD& dd1, const ADD& dd2);

  // Returns the result of applying op to ADD dd.
  static ADD MonadicApply(MonadicOp op, const ADD& dd);

  friend class DecisionDiagramManager;
  friend class BDD;
  friend ADD Ite(const BDD&, const ADD&, const ADD&);
  friend ADD min(const ADD& dd1, const ADD& dd2);
  friend ADD max(const ADD& dd1, const ADD& dd2);
  friend ADD floor(const ADD& dd);
  friend ADD ceil(const ADD& dd);
  friend ADD pow(const ADD& dd1, const ADD& dd2);
  friend ADD log(const ADD& dd);
};

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

// Wrapper class for variable arrays.
template <typename DD>
class VariableArray {
 public:
  VariableArray();

  ~VariableArray();

  std::vector<DdNode*>::size_type size() const { return variables_.size(); }

 private:
  VariableArray(std::vector<DD> variables);

  DdNode* const* data() const { return variables_.data(); }

  DdManager* manager_;
  std::vector<DdNode*> variables_;

  friend class DecisionDiagramManager;
};

// Wrapper class for DdManager.
class DecisionDiagramManager {
 public:
  // Constructs a decision-diagram manager with the given variable count.
  explicit DecisionDiagramManager(int variable_count);

  DecisionDiagramManager(DecisionDiagramManager&& dd_manager);

  // Disallow copy and assign.
  DecisionDiagramManager(const DecisionDiagramManager&) = delete;
  DecisionDiagramManager& operator=(const DecisionDiagramManager&) = delete;

  ~DecisionDiagramManager();

  // Returns the variable count.
  int GetVariableCount() const;

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

  // Returns an array of every incr ADD variables between start (inclusive) and
  // end (exclusive).
  VariableArray<ADD> GetAddVariableArray(int start, int incr, int end) const;

  // Returns the positive unate cube of the given variables.
  BDD GetCube(const VariableArray<BDD>& variables) const;

  // Returns the positive unate cube of the given variables.
  ADD GetCube(const VariableArray<ADD>& variables) const;

  // TODO(hlsyounes): remove once all code is using wrapper classes.
  DdManager* manager() const { return manager_; }

 private:
  DdManager* manager_;
};

struct OddNode {
  const OddNode* e;
  const OddNode* t;
  size_t eoff;
  size_t toff;
};

class ODD {
 public:
  ODD(ODD&& odd);

  // Disallow copy and assign.
  ODD(const ODD&) = delete;
  ODD& operator=(const ODD&) = delete;

  ~ODD();

  static ODD Make(const BDD& reachable_states);

  // TODO(hlsyounes): remove once all code is using wrapper classes.
  const OddNode* root() const { return root_; }

  int node_count() const { return node_count_; }

  size_t state_count() const { return root_->eoff + root_->toff; }

  Optional<int> StateIndex(const BDD& state) const;

  // Returns a vector representation of the given ADD, using this ODD to map
  // decision diagram nodes to vector indices.
  std::vector<double> AddToVector(const ADD& dd) const;

  // Returns a BDD representation of the given vector, using this ODD to map
  // vector indices to decision diagram nodes and the given function to turn
  // double values into bool.
  BDD VectorToBdd(const DecisionDiagramManager& manager,
                  const std::vector<double>& values,
                  const std::function<bool(double)>& value_to_bool) const;

 private:
  explicit ODD(const OddNode* root, int node_count);

  const OddNode* root_;
  int node_count_;
};

// A matrix represented using compressed row storage format.
class SparseMatrix {
 public:
  class Iterator;

  // A sparse matrix element.
  class Element {
   public:
    double value() const { return value_; }
    size_t row() const { return row_; }
    size_t column() const { return column_; }

   private:
    explicit Element(double value, size_t row, size_t column)
        : value_(value), row_(row), column_(column) {}

    double value_;
    size_t row_;
    size_t column_;

    friend class Iterator;
  };

  // Iterator over sparse matrix elements.
  class Iterator : public std::iterator<std::input_iterator_tag, Element> {
   public:
    bool operator==(const Iterator& rhs) const { return i_ == rhs.i_; }
    bool operator!=(const Iterator& rhs) const { return i_ != rhs.i_; }
    Element operator*() const {
      return Element(m_->values_[i_], r_, m_->columns_[i_]);
    }
    Iterator& operator++() {
      ++i_;
      update_r();
      return *this;
    }
    Iterator operator++(int) {
      Iterator tmp(*this);
      ++i_;
      update_r();
      return tmp;
    }

   private:
    explicit Iterator(const SparseMatrix* m, size_t i) : m_(m), i_(i), r_(0) {
      update_r();
    }

    void update_r() {
      if (i_ < m_->values_.size()) {
        while (i_ >= m_->row_starts_[r_ + 1]) {
          ++r_;
        }
      }
    }

    const SparseMatrix* m_;
    size_t i_;
    size_t r_;

    friend class SparseMatrix;
 };

 SparseMatrix(std::vector<double>&& values, std::vector<size_t>&& columns,
              std::vector<size_t>&& row_starts);

 Iterator begin() const { return Iterator(this, 0); }
 Iterator end() const { return Iterator(this, values_.size()); }

private:
 std::vector<double> values_;
 std::vector<size_t> columns_;
 std::vector<size_t> row_starts_;
};

// Returns the base-2 logarithm of the given integer.
int Log2(int n);

template <typename DD>
VariableArray<DD>::VariableArray()
    : manager_(nullptr) {}

template <typename DD>
VariableArray<DD>::VariableArray(std::vector<DD> variables)
    : manager_(nullptr) {
  variables_.reserve(variables.size());
  for (const DD& dd : variables) {
    if (manager_ == nullptr) {
      manager_ = dd.manager();
    }
    variables_.push_back(dd.node());
    DecisionDiagram::Ref(dd.node());
  }
}

template <typename DD>
VariableArray<DD>::~VariableArray() {
  for (DdNode* node : variables_) {
    DecisionDiagram::Deref(manager_, node);
  }
}

#endif  // DDUTIL_H_

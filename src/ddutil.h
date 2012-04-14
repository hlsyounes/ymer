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

class DdManager;
class DdNode;

// Abstract wrapper class for DdNode, with automatic referencing and
// dereferencing.
class DecisionDiagram {
 public:
  // Returns true if this decision diagram is a constant.
  bool IsConstant() const;

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
};

// Wrapper class for ADDs, with automatic referencing and dereferencing.
class ADD : public DecisionDiagram {
 public:
  ADD(const ADD& dd);
  ADD& operator=(const ADD& dd);

  // Returns the value of this ADD.  Requires that this ADD is constant.
  double Value() const;

  // Arithmetic operators for ADDs.
  ADD operator+(const ADD& dd) const;
  ADD operator-(const ADD& dd) const;
  ADD operator*(const ADD& dd) const;
  ADD operator/(const ADD& dd) const;

 private:
  ADD(DdManager* manager_, DdNode* node);

  friend class DecisionDiagramManager;
};

// Wrapper class for DdManager.
class DecisionDiagramManager {
 public:
  // Constructs a decision-diagram manager with the given number of variables.
  explicit DecisionDiagramManager(int num_variables);

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

  // Returns an ADD for the given constant value.
  ADD GetConstant(double value) const;

  // TODO(hlsyounes): remove once all code is using wrapper classes.
  DdManager* manager() const { return manager_; }

 private:
  // Disallow copy and assign.
  DecisionDiagramManager(const DecisionDiagramManager&);
  DecisionDiagramManager& operator=(const DecisionDiagramManager&);

  DdManager* manager_;
};

#endif  // DDUTIL_H_

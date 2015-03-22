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

#include "compiled-expression.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <ostream>
#include <queue>
#include <set>
#include <vector>
#include <utility>

#include "strutil.h"

#include "glog/logging.h"

std::ostream& operator<<(std::ostream& os, Opcode opcode) {
  switch (opcode) {
    case Opcode::ICONST:
      return os << "ICONST";
    case Opcode::DCONST:
      return os << "DCONST";
    case Opcode::ILOAD:
      return os << "ILOAD";
    case Opcode::I2D:
      return os << "I2D";
    case Opcode::INEG:
      return os << "INEG";
    case Opcode::DNEG:
      return os << "DNEG";
    case Opcode::NOT:
      return os << "NOT";
    case Opcode::IADD:
      return os << "IADD";
    case Opcode::DADD:
      return os << "DADD";
    case Opcode::ISUB:
      return os << "ISUB";
    case Opcode::DSUB:
      return os << "DSUB";
    case Opcode::IMUL:
      return os << "IMUL";
    case Opcode::DMUL:
      return os << "DMUL";
    case Opcode::DDIV:
      return os << "DDIV";
    case Opcode::IEQ:
      return os << "IEQ";
    case Opcode::DEQ:
      return os << "DEQ";
    case Opcode::INE:
      return os << "INE";
    case Opcode::DNE:
      return os << "DNE";
    case Opcode::ILT:
      return os << "ILT";
    case Opcode::DLT:
      return os << "DLT";
    case Opcode::ILE:
      return os << "ILE";
    case Opcode::DLE:
      return os << "DLE";
    case Opcode::IGE:
      return os << "IGE";
    case Opcode::DGE:
      return os << "DGE";
    case Opcode::IGT:
      return os << "IGT";
    case Opcode::DGT:
      return os << "DGT";
    case Opcode::IFFALSE:
      return os << "IFFALSE";
    case Opcode::IFTRUE:
      return os << "IFTRUE";
    case Opcode::GOTO:
      return os << "GOTO";
    case Opcode::NOP:
      return os << "NOP";
    case Opcode::IMIN:
      return os << "IMIN";
    case Opcode::DMIN:
      return os << "DMIN";
    case Opcode::IMAX:
      return os << "IMAX";
    case Opcode::DMAX:
      return os << "DMAX";
    case Opcode::FLOOR:
      return os << "FLOOR";
    case Opcode::CEIL:
      return os << "CEIL";
    case Opcode::POW:
      return os << "POW";
    case Opcode::LOG:
      return os << "LOG";
    case Opcode::MOD:
      return os << "MOD";
  }
  LOG(FATAL) << "bad opcode";
}

Operation Operation::MakeICONST(int value, int dst) {
  return Operation(Opcode::ICONST, value, dst);
}

Operation Operation::MakeICONST(bool value, int dst) {
  return MakeICONST(value ? 1 : 0, dst);
}

Operation Operation::MakeDCONST(double value, int dst) {
  return Operation(Opcode::DCONST, value, dst);
}

Operation Operation::MakeILOAD(int variable, int dst) {
  return Operation(Opcode::ILOAD, variable, dst);
}

Operation Operation::MakeI2D(int src_dst) {
  return Operation(Opcode::I2D, src_dst);
}

Operation Operation::MakeINEG(int src_dst) {
  return Operation(Opcode::INEG, src_dst);
}

Operation Operation::MakeDNEG(int src_dst) {
  return Operation(Opcode::DNEG, src_dst);
}

Operation Operation::MakeNOT(int src_dst) {
  return Operation(Opcode::NOT, src_dst);
}

Operation Operation::MakeIADD(int src1_dst, int src2) {
  return Operation(Opcode::IADD, src1_dst, src2);
}

Operation Operation::MakeDADD(int src1_dst, int src2) {
  return Operation(Opcode::DADD, src1_dst, src2);
}

Operation Operation::MakeISUB(int src1_dst, int src2) {
  return Operation(Opcode::ISUB, src1_dst, src2);
}

Operation Operation::MakeDSUB(int src1_dst, int src2) {
  return Operation(Opcode::DSUB, src1_dst, src2);
}

Operation Operation::MakeIMUL(int src1_dst, int src2) {
  return Operation(Opcode::IMUL, src1_dst, src2);
}

Operation Operation::MakeDMUL(int src1_dst, int src2) {
  return Operation(Opcode::DMUL, src1_dst, src2);
}

Operation Operation::MakeDDIV(int src1_dst, int src2) {
  return Operation(Opcode::DDIV, src1_dst, src2);
}

Operation Operation::MakeIEQ(int src1_dst, int src2) {
  return Operation(Opcode::IEQ, src1_dst, src2);
}

Operation Operation::MakeDEQ(int src1_dst, int src2) {
  return Operation(Opcode::DEQ, src1_dst, src2);
}

Operation Operation::MakeINE(int src1_dst, int src2) {
  return Operation(Opcode::INE, src1_dst, src2);
}

Operation Operation::MakeDNE(int src1_dst, int src2) {
  return Operation(Opcode::DNE, src1_dst, src2);
}

Operation Operation::MakeILT(int src1_dst, int src2) {
  return Operation(Opcode::ILT, src1_dst, src2);
}

Operation Operation::MakeDLT(int src1_dst, int src2) {
  return Operation(Opcode::DLT, src1_dst, src2);
}

Operation Operation::MakeILE(int src1_dst, int src2) {
  return Operation(Opcode::ILE, src1_dst, src2);
}

Operation Operation::MakeDLE(int src1_dst, int src2) {
  return Operation(Opcode::DLE, src1_dst, src2);
}

Operation Operation::MakeIGE(int src1_dst, int src2) {
  return Operation(Opcode::IGE, src1_dst, src2);
}

Operation Operation::MakeDGE(int src1_dst, int src2) {
  return Operation(Opcode::DGE, src1_dst, src2);
}

Operation Operation::MakeIGT(int src1_dst, int src2) {
  return Operation(Opcode::IGT, src1_dst, src2);
}

Operation Operation::MakeDGT(int src1_dst, int src2) {
  return Operation(Opcode::DGT, src1_dst, src2);
}

Operation Operation::MakeIFFALSE(int src, int pc) {
  return Operation(Opcode::IFFALSE, src, pc);
}

Operation Operation::MakeIFTRUE(int src, int pc) {
  return Operation(Opcode::IFTRUE, src, pc);
}

Operation Operation::MakeGOTO(int pc) { return Operation(Opcode::GOTO, pc); }

Operation Operation::MakeNOP() { return Operation(Opcode::NOP); }

Operation Operation::MakeIMIN(int src1_dst, int src2) {
  return Operation(Opcode::IMIN, src1_dst, src2);
}

Operation Operation::MakeDMIN(int src1_dst, int src2) {
  return Operation(Opcode::DMIN, src1_dst, src2);
}

Operation Operation::MakeIMAX(int src1_dst, int src2) {
  return Operation(Opcode::IMAX, src1_dst, src2);
}

Operation Operation::MakeDMAX(int src1_dst, int src2) {
  return Operation(Opcode::DMAX, src1_dst, src2);
}

Operation Operation::MakeFLOOR(int src_dst) {
  return Operation(Opcode::FLOOR, src_dst);
}

Operation Operation::MakeCEIL(int src_dst) {
  return Operation(Opcode::CEIL, src_dst);
}

Operation Operation::MakePOW(int src1_dst, int src2) {
  return Operation(Opcode::POW, src1_dst, src2);
}

Operation Operation::MakeLOG(int src1_dst, int src2) {
  return Operation(Opcode::LOG, src1_dst, src2);
}

Operation Operation::MakeMOD(int src1_dst, int src2) {
  return Operation(Opcode::MOD, src1_dst, src2);
}

Operation::Operation(Opcode opcode, int operand1, int operand2)
    : opcode_(opcode), operand2_(operand2) {
  operand1_.i = operand1;
}

Operation::Operation(Opcode opcode, double operand1, int operand2)
    : opcode_(opcode), operand2_(operand2) {
  operand1_.d = operand1;
}

Operation::Operation(Opcode opcode, int operand) : opcode_(opcode) {
  operand1_.i = operand;
}

Operation::Operation(Opcode opcode) : opcode_(opcode) {}

Operation Operation::Shift(int pc_shift, int reg_shift) const {
  switch (opcode()) {
    case Opcode::ICONST:
    case Opcode::ILOAD:
      return Operation(opcode(), ioperand1(), operand2() + reg_shift);
    case Opcode::DCONST:
      return Operation(opcode(), doperand1(), operand2() + reg_shift);
    case Opcode::I2D:
    case Opcode::FLOOR:
    case Opcode::CEIL:
    case Opcode::INEG:
    case Opcode::DNEG:
    case Opcode::NOT:
    case Opcode::IADD:
      return Operation(opcode(), ioperand1() + reg_shift);
    case Opcode::ISUB:
    case Opcode::IMUL:
    case Opcode::IEQ:
    case Opcode::INE:
    case Opcode::ILT:
    case Opcode::ILE:
    case Opcode::IGE:
    case Opcode::IGT:
    case Opcode::IMIN:
    case Opcode::IMAX:
    case Opcode::MOD:
    case Opcode::DADD:
    case Opcode::DSUB:
    case Opcode::DMUL:
    case Opcode::DDIV:
    case Opcode::DMIN:
    case Opcode::DMAX:
    case Opcode::POW:
    case Opcode::LOG:
    case Opcode::DEQ:
    case Opcode::DNE:
    case Opcode::DLT:
    case Opcode::DLE:
    case Opcode::DGE:
    case Opcode::DGT:
      return Operation(opcode(), ioperand1() + reg_shift,
                       operand2() + reg_shift);
    case Opcode::NOP:
      return *this;
    case Opcode::IFFALSE:
    case Opcode::IFTRUE:
      return Operation(opcode(), ioperand1() + reg_shift,
                       operand2() + pc_shift);
    case Opcode::GOTO:
      return Operation(opcode(), ioperand1() + pc_shift);
  }
  LOG(FATAL) << "bad opcode";
}

bool operator==(const Operation& left, const Operation& right) {
  if (left.opcode() != right.opcode()) {
    return false;
  }
  switch (left.opcode()) {
    case Opcode::ICONST:
    case Opcode::ILOAD:
    case Opcode::IADD:
    case Opcode::DADD:
    case Opcode::ISUB:
    case Opcode::DSUB:
    case Opcode::IMUL:
    case Opcode::DMUL:
    case Opcode::DDIV:
    case Opcode::IEQ:
    case Opcode::DEQ:
    case Opcode::INE:
    case Opcode::DNE:
    case Opcode::ILT:
    case Opcode::DLT:
    case Opcode::ILE:
    case Opcode::DLE:
    case Opcode::IGE:
    case Opcode::DGE:
    case Opcode::IGT:
    case Opcode::DGT:
    case Opcode::IFFALSE:
    case Opcode::IFTRUE:
    case Opcode::IMIN:
    case Opcode::DMIN:
    case Opcode::IMAX:
    case Opcode::DMAX:
    case Opcode::POW:
    case Opcode::LOG:
    case Opcode::MOD:
      return left.ioperand1() == right.ioperand1() &&
             left.operand2() == right.operand2();
    case Opcode::DCONST:
      return left.doperand1() == right.doperand1() &&
             left.operand2() == right.operand2();
    case Opcode::I2D:
    case Opcode::INEG:
    case Opcode::DNEG:
    case Opcode::NOT:
    case Opcode::GOTO:
    case Opcode::FLOOR:
    case Opcode::CEIL:
      return left.ioperand1() == right.ioperand1();
    case Opcode::NOP:
      return true;
  }
  LOG(FATAL) << "bad opcode";
}

std::ostream& operator<<(std::ostream& os, const Operation& operation) {
  os << operation.opcode();
  switch (operation.opcode()) {
    case Opcode::ICONST:
    case Opcode::ILOAD:
    case Opcode::IADD:
    case Opcode::DADD:
    case Opcode::ISUB:
    case Opcode::DSUB:
    case Opcode::IMUL:
    case Opcode::DMUL:
    case Opcode::DDIV:
    case Opcode::IEQ:
    case Opcode::DEQ:
    case Opcode::INE:
    case Opcode::DNE:
    case Opcode::ILT:
    case Opcode::DLT:
    case Opcode::ILE:
    case Opcode::DLE:
    case Opcode::IGE:
    case Opcode::DGE:
    case Opcode::IGT:
    case Opcode::DGT:
    case Opcode::IFFALSE:
    case Opcode::IFTRUE:
    case Opcode::IMIN:
    case Opcode::DMIN:
    case Opcode::IMAX:
    case Opcode::DMAX:
    case Opcode::POW:
    case Opcode::LOG:
    case Opcode::MOD:
      return os << ' ' << operation.ioperand1() << ' ' << operation.operand2();
    case Opcode::DCONST:
      return os << ' ' << operation.doperand1() << ' ' << operation.operand2();
    case Opcode::I2D:
    case Opcode::INEG:
    case Opcode::DNEG:
    case Opcode::NOT:
    case Opcode::GOTO:
    case Opcode::FLOOR:
    case Opcode::CEIL:
      return os << ' ' << operation.ioperand1();
    case Opcode::NOP:
      return os;
  }
  LOG(FATAL) << "bad opcode";
}

std::vector<Operation> MakeConjunction(
    const std::vector<Operation>& operations1,
    const std::vector<Operation>& operations2) {
  if (operations1.empty()) {
    return operations2;
  }
  if (operations2.empty()) {
    return operations1;
  }
  std::vector<Operation> operations(operations1);
  const size_t pc_shift = operations1.size() + 1;
  operations.push_back(
      Operation::MakeIFFALSE(0, pc_shift + operations2.size()));
  for (const Operation& o : operations2) {
    switch (o.opcode()) {
      case Opcode::ICONST:
      case Opcode::ILOAD:
      case Opcode::DCONST:
      case Opcode::I2D:
      case Opcode::FLOOR:
      case Opcode::CEIL:
      case Opcode::INEG:
      case Opcode::DNEG:
      case Opcode::NOT:
      case Opcode::IADD:
      case Opcode::ISUB:
      case Opcode::IMUL:
      case Opcode::IEQ:
      case Opcode::INE:
      case Opcode::ILT:
      case Opcode::ILE:
      case Opcode::IGE:
      case Opcode::IGT:
      case Opcode::IMIN:
      case Opcode::IMAX:
      case Opcode::MOD:
      case Opcode::DADD:
      case Opcode::DSUB:
      case Opcode::DMUL:
      case Opcode::DDIV:
      case Opcode::DMIN:
      case Opcode::DMAX:
      case Opcode::POW:
      case Opcode::LOG:
      case Opcode::DEQ:
      case Opcode::DNE:
      case Opcode::DLT:
      case Opcode::DLE:
      case Opcode::DGE:
      case Opcode::DGT:
      case Opcode::NOP:
        operations.push_back(o);
        continue;
      case Opcode::IFFALSE:
        operations.push_back(
            Operation::MakeIFFALSE(o.ioperand1(), o.operand2() + pc_shift));
        continue;
      case Opcode::IFTRUE:
        operations.push_back(
            Operation::MakeIFTRUE(o.ioperand1(), o.operand2() + pc_shift));
        continue;
      case Opcode::GOTO:
        operations.push_back(Operation::MakeGOTO(o.ioperand1() + pc_shift));
        continue;
    }
    LOG(FATAL) << "bad opcode";
  }
  return operations;
}

CompiledExpression::CompiledExpression() = default;

CompiledExpression::CompiledExpression(const std::vector<Operation>& operations,
                                       const Optional<ADD>& dd)
    : operations_(operations), dd_(dd) {}

CompiledExpression CompiledExpression::WithAssignment(
    const IdentifierInfo& variable, int value,
    const Optional<DecisionDiagramManager>& dd_manager) const {
  CHECK(variable.is_variable());
  std::vector<Operation> operations;
  operations.reserve(operations_.size());
  for (size_t pc = 0; pc < operations_.size(); ++pc) {
    const Operation& o = operations_[pc];
    if (o.opcode() == Opcode::ILOAD &&
        o.ioperand1() == variable.variable_index()) {
      operations.push_back(Operation::MakeICONST(value, o.operand2()));
    } else {
      operations.push_back(o);
    }
  }
  Optional<ADD> dd;
  if (dd_.has_value()) {
    dd = ADD(IdentifierToAdd(dd_manager.value(), variable)
                 .Interval(value, value)) *
         dd_.value();
  }
  return CompiledExpression(operations, dd);
}

std::ostream& operator<<(std::ostream& os, const CompiledExpression& expr) {
  for (size_t pc = 0; pc < expr.operations().size(); ++pc) {
    if (pc > 0) {
      os << std::endl;
    }
    os << pc << ": " << expr.operations()[pc];
  }
  return os;
}

std::pair<int, int> GetExpressionRegisterCounts(
    const CompiledExpression& expr) {
  int max_ireg = -1;
  int max_dreg = -1;
  for (const Operation& o : expr.operations()) {
    switch (o.opcode()) {
      case Opcode::ICONST:
      case Opcode::ILOAD:
        max_ireg = std::max(max_ireg, o.operand2());
        continue;
      case Opcode::DCONST:
        max_dreg = std::max(max_dreg, o.operand2());
        continue;
      case Opcode::I2D:
      case Opcode::FLOOR:
      case Opcode::CEIL:
        max_ireg = std::max(max_ireg, o.ioperand1());
        max_dreg = std::max(max_dreg, o.ioperand1());
        continue;
      case Opcode::INEG:
      case Opcode::NOT:
      case Opcode::IFFALSE:
      case Opcode::IFTRUE:
        max_ireg = std::max(max_ireg, o.ioperand1());
        continue;
      case Opcode::DNEG:
        max_dreg = std::max(max_dreg, o.ioperand1());
        continue;
      case Opcode::IADD:
      case Opcode::ISUB:
      case Opcode::IMUL:
      case Opcode::IEQ:
      case Opcode::INE:
      case Opcode::ILT:
      case Opcode::ILE:
      case Opcode::IGE:
      case Opcode::IGT:
      case Opcode::IMIN:
      case Opcode::IMAX:
      case Opcode::MOD:
        max_ireg = std::max({max_ireg, o.ioperand1(), o.operand2()});
        continue;
      case Opcode::DADD:
      case Opcode::DSUB:
      case Opcode::DMUL:
      case Opcode::DDIV:
      case Opcode::DMIN:
      case Opcode::DMAX:
      case Opcode::POW:
      case Opcode::LOG:
        max_dreg = std::max({max_dreg, o.ioperand1(), o.operand2()});
        continue;
      case Opcode::DEQ:
      case Opcode::DNE:
      case Opcode::DLT:
      case Opcode::DLE:
      case Opcode::DGE:
      case Opcode::DGT:
        max_ireg = std::max(max_ireg, o.ioperand1());
        max_dreg = std::max({max_dreg, o.ioperand1(), o.operand2()});
        continue;
      case Opcode::GOTO:
      case Opcode::NOP:
        continue;
    }
    LOG(FATAL) << "bad opcode";
  }
  return {max_ireg + 1, max_dreg + 1};
}

CompiledExpressionEvaluator::CompiledExpressionEvaluator(int ireg_count,
                                                         int dreg_count)
    : iregs_(ireg_count), dregs_(dreg_count) {}

int CompiledExpressionEvaluator::EvaluateIntExpression(
    const CompiledExpression& expr, const std::vector<int>& state) {
  ExecuteOperations(expr.operations(), state);
  return iregs_[0];
}

double CompiledExpressionEvaluator::EvaluateDoubleExpression(
    const CompiledExpression& expr, const std::vector<int>& state) {
  ExecuteOperations(expr.operations(), state);
  return dregs_[0];
}

void CompiledExpressionEvaluator::ExecuteOperations(
    const std::vector<Operation>& operations, const std::vector<int>& state) {
  for (size_t pc = 0; pc < operations.size(); ++pc) {
    const Operation& o = operations[pc];
    switch (o.opcode()) {
      case Opcode::ICONST:
        iregs_[o.operand2()] = o.ioperand1();
        continue;
      case Opcode::DCONST:
        dregs_[o.operand2()] = o.doperand1();
        continue;
      case Opcode::ILOAD:
        iregs_[o.operand2()] = state[o.ioperand1()];
        continue;
      case Opcode::I2D:
        dregs_[o.ioperand1()] = iregs_[o.ioperand1()];
        continue;
      case Opcode::INEG:
        iregs_[o.ioperand1()] *= -1;
        continue;
      case Opcode::DNEG:
        dregs_[o.ioperand1()] *= -1.0;
        continue;
      case Opcode::NOT:
        iregs_[o.ioperand1()] = !iregs_[o.ioperand1()];
        continue;
      case Opcode::IADD:
        iregs_[o.ioperand1()] += iregs_[o.operand2()];
        continue;
      case Opcode::DADD:
        dregs_[o.ioperand1()] += dregs_[o.operand2()];
        continue;
      case Opcode::ISUB:
        iregs_[o.ioperand1()] -= iregs_[o.operand2()];
        continue;
      case Opcode::DSUB:
        dregs_[o.ioperand1()] -= dregs_[o.operand2()];
        continue;
      case Opcode::IMUL:
        iregs_[o.ioperand1()] *= iregs_[o.operand2()];
        continue;
      case Opcode::DMUL:
        dregs_[o.ioperand1()] *= dregs_[o.operand2()];
        continue;
      case Opcode::DDIV:
        dregs_[o.ioperand1()] /= dregs_[o.operand2()];
        continue;
      case Opcode::IEQ:
        iregs_[o.ioperand1()] = iregs_[o.ioperand1()] == iregs_[o.operand2()];
        continue;
      case Opcode::DEQ:
        iregs_[o.ioperand1()] = dregs_[o.ioperand1()] == dregs_[o.operand2()];
        continue;
      case Opcode::INE:
        iregs_[o.ioperand1()] = iregs_[o.ioperand1()] != iregs_[o.operand2()];
        continue;
      case Opcode::DNE:
        iregs_[o.ioperand1()] = dregs_[o.ioperand1()] != dregs_[o.operand2()];
        continue;
      case Opcode::ILT:
        iregs_[o.ioperand1()] = iregs_[o.ioperand1()] < iregs_[o.operand2()];
        continue;
      case Opcode::DLT:
        iregs_[o.ioperand1()] = dregs_[o.ioperand1()] < dregs_[o.operand2()];
        continue;
      case Opcode::ILE:
        iregs_[o.ioperand1()] = iregs_[o.ioperand1()] <= iregs_[o.operand2()];
        continue;
      case Opcode::DLE:
        iregs_[o.ioperand1()] = dregs_[o.ioperand1()] <= dregs_[o.operand2()];
        continue;
      case Opcode::IGE:
        iregs_[o.ioperand1()] = iregs_[o.ioperand1()] >= iregs_[o.operand2()];
        continue;
      case Opcode::DGE:
        iregs_[o.ioperand1()] = dregs_[o.ioperand1()] >= dregs_[o.operand2()];
        continue;
      case Opcode::IGT:
        iregs_[o.ioperand1()] = iregs_[o.ioperand1()] > iregs_[o.operand2()];
        continue;
      case Opcode::DGT:
        iregs_[o.ioperand1()] = dregs_[o.ioperand1()] > dregs_[o.operand2()];
        continue;
      case Opcode::IFFALSE:
        if (!iregs_[o.ioperand1()]) pc = o.operand2() - 1;
        continue;
      case Opcode::IFTRUE:
        if (iregs_[o.ioperand1()]) pc = o.operand2() - 1;
        continue;
      case Opcode::GOTO:
        pc = o.ioperand1() - 1;
        continue;
      case Opcode::NOP:
        // Do nothing.
        continue;
      case Opcode::IMIN:
        iregs_[o.ioperand1()] =
            std::min(iregs_[o.ioperand1()], iregs_[o.operand2()]);
        continue;
      case Opcode::DMIN:
        dregs_[o.ioperand1()] =
            std::min(dregs_[o.ioperand1()], dregs_[o.operand2()]);
        continue;
      case Opcode::IMAX:
        iregs_[o.ioperand1()] =
            std::max(iregs_[o.ioperand1()], iregs_[o.operand2()]);
        continue;
      case Opcode::DMAX:
        dregs_[o.ioperand1()] =
            std::max(dregs_[o.ioperand1()], dregs_[o.operand2()]);
        continue;
      case Opcode::FLOOR:
        iregs_[o.ioperand1()] = floor(dregs_[o.ioperand1()]);
        continue;
      case Opcode::CEIL:
        iregs_[o.ioperand1()] = ceil(dregs_[o.ioperand1()]);
        continue;
      case Opcode::POW:
        dregs_[o.ioperand1()] =
            pow(dregs_[o.ioperand1()], dregs_[o.operand2()]);
        continue;
      case Opcode::LOG:
        dregs_[o.ioperand1()] =
            log(dregs_[o.ioperand1()]) / log(dregs_[o.operand2()]);
        continue;
      case Opcode::MOD:
        iregs_[o.ioperand1()] %= iregs_[o.operand2()];
        continue;
    }
    LOG(FATAL) << "bad opcode";
  }
}

namespace {

class ExpressionCompiler : public ExpressionVisitor {
 public:
  explicit ExpressionCompiler(
      const std::map<std::string, const Expression*>* formulas_by_name,
      const std::map<std::string, IdentifierInfo>* identifiers_by_name,
      std::vector<std::string>* errors);

  std::vector<Operation> release_operations() { return std::move(operations_); }

  Type type() const { return type_; }

 private:
  void DoVisitLiteral(const Literal& expr) override;
  void DoVisitIdentifier(const Identifier& expr) override;
  void DoVisitLabel(const Label& expr) override;
  void DoVisitFunctionCall(const FunctionCall& expr) override;
  void DoVisitUnaryOperation(const UnaryOperation& expr) override;
  void DoVisitBinaryOperation(const BinaryOperation& expr) override;
  void DoVisitConditional(const Conditional& expr) override;
  void DoVisitProbabilityThresholdOperation(
      const ProbabilityThresholdOperation& expr) override;
  void DoVisitProbabilityEstimationOperation(
      const ProbabilityEstimationOperation& expr) override;

  std::vector<Operation> operations_;
  int dst_;
  Type type_;
  const std::map<std::string, const Expression*>* formulas_by_name_;
  const std::map<std::string, IdentifierInfo>* identifiers_by_name_;
  std::vector<std::string>* errors_;
};

ExpressionCompiler::ExpressionCompiler(
    const std::map<std::string, const Expression*>* formulas_by_name,
    const std::map<std::string, IdentifierInfo>* identifiers_by_name,
    std::vector<std::string>* errors)
    : dst_(0),
      formulas_by_name_(formulas_by_name),
      identifiers_by_name_(identifiers_by_name),
      errors_(errors) {}

void ExpressionCompiler::DoVisitLiteral(const Literal& expr) {
  TypedValue value = expr.value();
  type_ = value.type();
  switch (value.type()) {
    case Type::INT:
      operations_.push_back(Operation::MakeICONST(value.value<int>(), dst_));
      return;
    case Type::DOUBLE:
      operations_.push_back(Operation::MakeDCONST(value.value<double>(), dst_));
      return;
    case Type::BOOL:
      operations_.push_back(Operation::MakeICONST(value.value<bool>(), dst_));
      return;
  }
  LOG(FATAL) << "bad type";
}

void ExpressionCompiler::DoVisitIdentifier(const Identifier& expr) {
  auto f = formulas_by_name_->find(expr.name());
  if (f != formulas_by_name_->end()) {
    f->second->Accept(this);
    return;
  }
  auto i = identifiers_by_name_->find(expr.name());
  if (i == identifiers_by_name_->end()) {
    errors_->push_back(
        StrCat("undefined identifier '", expr.name(), "' in expression"));
    return;
  }
  const IdentifierInfo& info = i->second;
  type_ = info.type();
  switch (info.type()) {
    case Type::INT:
      if (info.is_variable()) {
        operations_.push_back(
            Operation::MakeILOAD(info.variable_index(), dst_));
      } else {
        operations_.push_back(
            Operation::MakeICONST(info.constant_value().value<int>(), dst_));
      }
      return;
    case Type::DOUBLE:
      if (info.is_variable()) {
        errors_->push_back(StrCat(info.type(), " variables not supported"));
      } else {
        operations_.push_back(
            Operation::MakeDCONST(info.constant_value().value<double>(), dst_));
      }
      return;
    case Type::BOOL:
      if (info.is_variable()) {
        operations_.push_back(
            Operation::MakeILOAD(info.variable_index(), dst_));
      } else {
        operations_.push_back(
            Operation::MakeICONST(info.constant_value().value<bool>(), dst_));
      }
      return;
  }
  LOG(FATAL) << "bad type";
}

void ExpressionCompiler::DoVisitLabel(const Label& expr) {
  errors_->push_back("unexpected label in expression");
}

void ExpressionCompiler::DoVisitFunctionCall(const FunctionCall& expr) {
  const size_t n = expr.arguments().size();
  switch (expr.function()) {
    case Function::UNKNOWN:
      errors_->push_back("unknown function call");
      return;
    case Function::MIN:
    case Function::MAX:
      if (n == 0) {
        errors_->push_back(
            StrCat(expr.function(),
                   " applied to 0 arguments; expecting at least 1 argument"));
        return;
      }
      expr.arguments()[0].Accept(this);
      if (!errors_->empty()) {
        return;
      }
      for (size_t i = 1; i < n; ++i) {
        Type type = type_;
        ++dst_;
        expr.arguments()[i].Accept(this);
        --dst_;
        if (!errors_->empty()) {
          return;
        }
        if (type != type_) {
          if (type == Type::BOOL || type_ == Type::BOOL) {
            errors_->push_back(
                StrCat("type mismatch; incompatible argument types ", type,
                       " and ", type_));
            return;
          }
          if (type != Type::DOUBLE && type_ == Type::DOUBLE) {
            operations_.push_back(Operation::MakeI2D(dst_));
          }
          if (type == Type::DOUBLE && type_ != Type::DOUBLE) {
            operations_.push_back(Operation::MakeI2D(dst_ + 1));
            type_ = Type::DOUBLE;
          }
        }
        if (type_ == Type::DOUBLE) {
          if (expr.function() == Function::MIN) {
            operations_.push_back(Operation::MakeDMIN(dst_, dst_ + 1));
          } else {
            operations_.push_back(Operation::MakeDMAX(dst_, dst_ + 1));
          }
        } else {
          if (expr.function() == Function::MIN) {
            operations_.push_back(Operation::MakeIMIN(dst_, dst_ + 1));
          } else {
            operations_.push_back(Operation::MakeIMAX(dst_, dst_ + 1));
          }
        }
      }
      return;
    case Function::FLOOR:
    case Function::CEIL:
      if (n != 1) {
        errors_->push_back(StrCat(expr.function(), " applied to ", n,
                                  " arguments; expecting 1 argument"));
        return;
      }
      expr.arguments()[0].Accept(this);
      if (!errors_->empty()) {
        return;
      }
      if (type_ == Type::BOOL) {
        errors_->push_back(StrCat("type mismatch; expecting argument of type ",
                                  Type::DOUBLE, "; found ", type_));
        return;
      }
      if (type_ != Type::DOUBLE) {
        operations_.push_back(Operation::MakeI2D(dst_));
      }
      if (expr.function() == Function::FLOOR) {
        operations_.push_back(Operation::MakeFLOOR(dst_));
      } else {
        operations_.push_back(Operation::MakeCEIL(dst_));
      }
      type_ = Type::INT;
      return;
    case Function::POW:
    case Function::LOG:
      if (n != 2) {
        errors_->push_back(StrCat(expr.function(), " applied to ", n,
                                  " argument", (n == 1) ? "" : "s",
                                  "; expecting 2 arguments"));
        return;
      }
      for (size_t i = 0; i < 2; ++i) {
        dst_ = dst_ + i;
        expr.arguments()[i].Accept(this);
        dst_ = dst_ - i;
        if (!errors_->empty()) {
          return;
        }
        if (type_ == Type::BOOL) {
          errors_->push_back(
              StrCat("type mismatch; expecting argument of type ", Type::DOUBLE,
                     "; found ", type_));
          return;
        }
        if (type_ != Type::DOUBLE) {
          operations_.push_back(Operation::MakeI2D(dst_ + i));
        }
      }
      if (expr.function() == Function::POW) {
        operations_.push_back(Operation::MakePOW(dst_, dst_ + 1));
      } else {
        operations_.push_back(Operation::MakeLOG(dst_, dst_ + 1));
      }
      type_ = Type::DOUBLE;
      return;
    case Function::MOD:
      if (n != 2) {
        errors_->push_back(StrCat(expr.function(), " applied to ", n,
                                  " argument", (n == 1) ? "" : "s",
                                  "; expecting 2 arguments"));
        return;
      }
      for (size_t i = 0; i < 2; ++i) {
        dst_ = dst_ + i;
        expr.arguments()[i].Accept(this);
        dst_ = dst_ - i;
        if (!errors_->empty()) {
          return;
        }
        if (type_ != Type::INT) {
          errors_->push_back(
              StrCat("type mismatch; expecting argument of type ", Type::INT,
                     "; found ", type_));
          return;
        }
      }
      operations_.push_back(Operation::MakeMOD(dst_, dst_ + 1));
      return;
  }
  LOG(FATAL) << "bad function";
}

void ExpressionCompiler::DoVisitUnaryOperation(const UnaryOperation& expr) {
  expr.operand().Accept(this);
  if (!errors_->empty()) {
    return;
  }
  switch (expr.op()) {
    case UnaryOperator::NEGATE:
      if (type_ == Type::BOOL) {
        errors_->push_back(StrCat("type mismatch; unary operator ", expr.op(),
                                  " applied to ", type_));
        return;
      }
      if (type_ == Type::DOUBLE) {
        operations_.push_back(Operation::MakeDNEG(dst_));
      } else {
        operations_.push_back(Operation::MakeINEG(dst_));
      }
      return;
    case UnaryOperator::NOT:
      if (type_ != Type::BOOL) {
        errors_->push_back(StrCat("type mismatch; unary operator ", expr.op(),
                                  " applied to ", type_));
        return;
      }
      operations_.push_back(Operation::MakeNOT(dst_));
      return;
  }
  LOG(FATAL) << "bad unary operator";
}

void ExpressionCompiler::DoVisitBinaryOperation(const BinaryOperation& expr) {
  expr.operand1().Accept(this);
  if (!errors_->empty()) {
    return;
  }
  switch (expr.op()) {
    case BinaryOperator::PLUS:
    case BinaryOperator::MINUS:
    case BinaryOperator::MULTIPLY:
    case BinaryOperator::DIVIDE: {
      if (type_ == Type::BOOL) {
        errors_->push_back(StrCat("type mismatch; binary operator ", expr.op(),
                                  " applied to ", type_));
        return;
      }
      Type type = type_;
      ++dst_;
      expr.operand2().Accept(this);
      --dst_;
      if (!errors_->empty()) {
        return;
      }
      if (type_ == Type::BOOL) {
        errors_->push_back(StrCat("type mismatch; binary operator ", expr.op(),
                                  " applied to ", type_));
        return;
      }
      if (type != Type::DOUBLE &&
          (type_ == Type::DOUBLE || expr.op() == BinaryOperator::DIVIDE)) {
        operations_.push_back(Operation::MakeI2D(dst_));
        type = Type::DOUBLE;
      }
      if (type == Type::DOUBLE && type_ != Type::DOUBLE) {
        operations_.push_back(Operation::MakeI2D(dst_ + 1));
        type_ = Type::DOUBLE;
      }
      if (type_ == Type::DOUBLE) {
        if (expr.op() == BinaryOperator::PLUS) {
          operations_.push_back(Operation::MakeDADD(dst_, dst_ + 1));
        } else if (expr.op() == BinaryOperator::MINUS) {
          operations_.push_back(Operation::MakeDSUB(dst_, dst_ + 1));
        } else if (expr.op() == BinaryOperator::MULTIPLY) {
          operations_.push_back(Operation::MakeDMUL(dst_, dst_ + 1));
        } else {
          operations_.push_back(Operation::MakeDDIV(dst_, dst_ + 1));
        }
      } else {
        if (expr.op() == BinaryOperator::PLUS) {
          operations_.push_back(Operation::MakeIADD(dst_, dst_ + 1));
        } else if (expr.op() == BinaryOperator::MINUS) {
          operations_.push_back(Operation::MakeISUB(dst_, dst_ + 1));
        } else {
          operations_.push_back(Operation::MakeIMUL(dst_, dst_ + 1));
        }
      }
      return;
    }
    case BinaryOperator::AND:
    case BinaryOperator::OR:
    case BinaryOperator::IMPLY:
    case BinaryOperator::IFF: {
      if (type_ != Type::BOOL) {
        errors_->push_back(StrCat("type mismatch; binary operator ", expr.op(),
                                  " applied to ", type_));
        return;
      }
      size_t jump_pos;
      if (expr.op() == BinaryOperator::IFF) {
        ++dst_;
      } else {
        if (expr.op() == BinaryOperator::IMPLY) {
          operations_.push_back(Operation::MakeNOT(dst_));
        }
        jump_pos = operations_.size();
        operations_.push_back(Operation::MakeNOP());  // placeholder for jump
      }
      expr.operand2().Accept(this);
      if (expr.op() == BinaryOperator::IFF) {
        --dst_;
      }
      if (!errors_->empty()) {
        return;
      }
      if (type_ != Type::BOOL) {
        errors_->push_back(StrCat("type mismatch; binary operator ", expr.op(),
                                  " applied to ", type_));
        return;
      }
      if (expr.op() == BinaryOperator::IFF) {
        operations_.push_back(Operation::MakeIEQ(dst_, dst_ + 1));
      } else if (expr.op() == BinaryOperator::AND) {
        operations_[jump_pos] =
            Operation::MakeIFFALSE(dst_, operations_.size());
      } else {
        operations_[jump_pos] = Operation::MakeIFTRUE(dst_, operations_.size());
      }
      return;
    }
    case BinaryOperator::LESS:
    case BinaryOperator::LESS_EQUAL:
    case BinaryOperator::GREATER_EQUAL:
    case BinaryOperator::GREATER:
    case BinaryOperator::EQUAL:
    case BinaryOperator::NOT_EQUAL: {
      const Type type = type_;
      ++dst_;
      expr.operand2().Accept(this);
      --dst_;
      if (!errors_->empty()) {
        return;
      }
      if (type != type_) {
        if (type == Type::BOOL || type_ == Type::BOOL) {
          errors_->push_back(
              StrCat("type mismatch; incompatible argument types ", type,
                     " and ", type_));
          return;
        }
        if (type != Type::DOUBLE && type_ == Type::DOUBLE) {
          operations_.push_back(Operation::MakeI2D(dst_));
        }
        if (type == Type::DOUBLE && type_ != Type::DOUBLE) {
          operations_.push_back(Operation::MakeI2D(dst_ + 1));
          type_ = Type::DOUBLE;
        }
      }
      if (type_ == Type::DOUBLE) {
        if (expr.op() == BinaryOperator::LESS) {
          operations_.push_back(Operation::MakeDLT(dst_, dst_ + 1));
        } else if (expr.op() == BinaryOperator::LESS_EQUAL) {
          operations_.push_back(Operation::MakeDLE(dst_, dst_ + 1));
        } else if (expr.op() == BinaryOperator::GREATER_EQUAL) {
          operations_.push_back(Operation::MakeDGE(dst_, dst_ + 1));
        } else if (expr.op() == BinaryOperator::GREATER) {
          operations_.push_back(Operation::MakeDGT(dst_, dst_ + 1));
        } else if (expr.op() == BinaryOperator::EQUAL) {
          operations_.push_back(Operation::MakeDEQ(dst_, dst_ + 1));
        } else {
          operations_.push_back(Operation::MakeDNE(dst_, dst_ + 1));
        }
      } else {
        if (expr.op() == BinaryOperator::LESS) {
          operations_.push_back(Operation::MakeILT(dst_, dst_ + 1));
        } else if (expr.op() == BinaryOperator::LESS_EQUAL) {
          operations_.push_back(Operation::MakeILE(dst_, dst_ + 1));
        } else if (expr.op() == BinaryOperator::GREATER_EQUAL) {
          operations_.push_back(Operation::MakeIGE(dst_, dst_ + 1));
        } else if (expr.op() == BinaryOperator::GREATER) {
          operations_.push_back(Operation::MakeIGT(dst_, dst_ + 1));
        } else if (expr.op() == BinaryOperator::EQUAL) {
          operations_.push_back(Operation::MakeIEQ(dst_, dst_ + 1));
        } else {
          operations_.push_back(Operation::MakeINE(dst_, dst_ + 1));
        }
      }
      type_ = Type::BOOL;
      return;
    }
  }
  LOG(FATAL) << "bad binary operator";
}

void ExpressionCompiler::DoVisitConditional(const Conditional& expr) {
  expr.condition().Accept(this);
  if (!errors_->empty()) {
    return;
  }
  if (type_ != Type::BOOL) {
    errors_->push_back(StrCat(
        "type mismatch; expecting condition of type bool; found ", type_));
    return;
  }
  const size_t iffalse_pos = operations_.size();
  operations_.push_back(Operation::MakeNOP());  // placeholder for IFFALSE
  expr.if_branch().Accept(this);
  if (!errors_->empty()) {
    return;
  }
  const Type type = type_;
  const size_t i2d_pos = operations_.size();
  if (type == Type::INT) {
    operations_.push_back(Operation::MakeNOP());  // placeholder for I2D
  }
  const size_t goto_pos = operations_.size();
  operations_.push_back(Operation::MakeNOP());  // placeholder for GOTO
  operations_[iffalse_pos] = Operation::MakeIFFALSE(dst_, operations_.size());
  expr.else_branch().Accept(this);
  if (!errors_->empty()) {
    return;
  }
  if (type != type_) {
    if (type == Type::BOOL || type_ == Type::BOOL) {
      errors_->push_back(StrCat("type mismatch; incompatible branch types ",
                                type, " and ", type_));
      return;
    }
    if (type != Type::DOUBLE && type_ == Type::DOUBLE) {
      operations_[i2d_pos] = Operation::MakeI2D(dst_);
    }
    if (type == Type::DOUBLE && type_ != Type::DOUBLE) {
      operations_.push_back(Operation::MakeI2D(dst_));
      type_ = Type::DOUBLE;
    }
  }
  operations_[goto_pos] = Operation::MakeGOTO(operations_.size());
}

void ExpressionCompiler::DoVisitProbabilityThresholdOperation(
    const ProbabilityThresholdOperation& expr) {
  errors_->push_back(
      "unexpected probability threshold operation in expression");
}

void ExpressionCompiler::DoVisitProbabilityEstimationOperation(
    const ProbabilityEstimationOperation& expr) {
  errors_->push_back(
      "unexpected probability estimation operation in expression");
}

class ExpressionToAddConverter : public ExpressionVisitor {
 public:
  explicit ExpressionToAddConverter(
      const std::map<std::string, const Expression*>* formulas_by_name,
      const std::map<std::string, IdentifierInfo>* identifiers_by_name,
      const DecisionDiagramManager* dd_manager);

  ADD add() const { return add_; }

 private:
  void DoVisitLiteral(const Literal& expr) override;
  void DoVisitIdentifier(const Identifier& expr) override;
  void DoVisitLabel(const Label& expr) override;
  void DoVisitFunctionCall(const FunctionCall& expr) override;
  void DoVisitUnaryOperation(const UnaryOperation& expr) override;
  void DoVisitBinaryOperation(const BinaryOperation& expr) override;
  void DoVisitConditional(const Conditional& expr) override;
  void DoVisitProbabilityThresholdOperation(
      const ProbabilityThresholdOperation& expr) override;
  void DoVisitProbabilityEstimationOperation(
      const ProbabilityEstimationOperation& expr) override;

  ADD add_;
  const std::map<std::string, const Expression*>* formulas_by_name_;
  const std::map<std::string, IdentifierInfo>* identifiers_by_name_;
  const DecisionDiagramManager* dd_manager_;
};

ExpressionToAddConverter::ExpressionToAddConverter(
    const std::map<std::string, const Expression*>* formulas_by_name,
    const std::map<std::string, IdentifierInfo>* identifiers_by_name,
    const DecisionDiagramManager* dd_manager)
    : add_(dd_manager->GetConstant(0)),
      formulas_by_name_(formulas_by_name),
      identifiers_by_name_(identifiers_by_name),
      dd_manager_(dd_manager) {}

void ExpressionToAddConverter::DoVisitLiteral(const Literal& expr) {
  if (expr.value().type() == Type::BOOL) {
    add_ = dd_manager_->GetConstant(expr.value().value<bool>() ? 1 : 0);
  } else {
    add_ = dd_manager_->GetConstant(expr.value().value<double>());
  }
}

void ExpressionToAddConverter::DoVisitIdentifier(const Identifier& expr) {
  auto j = formulas_by_name_->find(expr.name());
  if (j != formulas_by_name_->end()) {
    j->second->Accept(this);
    return;
  }
  auto i = identifiers_by_name_->find(expr.name());
  CHECK(i != identifiers_by_name_->end());
  add_ = IdentifierToAdd(*dd_manager_, i->second);

}

void ExpressionToAddConverter::DoVisitLabel(const Label& expr) {
  LOG(FATAL) << "not an expression";
}

void ExpressionToAddConverter::DoVisitFunctionCall(const FunctionCall& expr) {
  CHECK(!expr.arguments().empty());
  std::vector<ADD> arguments;
  for (const auto& argument : expr.arguments()) {
      argument.Accept(this);
      arguments.push_back(add_);
  }
  switch (expr.function()) {
    case Function::UNKNOWN:
      LOG(FATAL) << "bad function call";
    case Function::MIN:
      CHECK(!arguments.empty());
      add_ = arguments[0];
      for (size_t i = 1; i < arguments.size(); ++i) {
        add_ = min(add_, arguments[i]);
      }
      break;
    case Function::MAX:
      CHECK(!arguments.empty());
      add_ = arguments[0];
      for (size_t i = 1; i < arguments.size(); ++i) {
        add_ = max(add_, arguments[i]);
      }
      break;
    case Function::FLOOR:
      CHECK_EQ(arguments.size(), 1);
      add_ = floor(arguments[0]);
      break;
    case Function::CEIL:
      CHECK_EQ(arguments.size(), 1);
      add_ = ceil(arguments[0]);
      break;
    case Function::POW:
      CHECK_EQ(arguments.size(), 2);
      add_ = pow(arguments[0], arguments[1]);
      break;
    case Function::LOG:
      CHECK_EQ(arguments.size(), 2);
      add_ = log(arguments[0]) / log(arguments[1]);
      break;
    case Function::MOD:
      CHECK_EQ(arguments.size(), 2);
      add_ = arguments[0] % arguments[1];
      break;
  }
}

void ExpressionToAddConverter::DoVisitUnaryOperation(
    const UnaryOperation& expr) {
  expr.operand().Accept(this);
  switch (expr.op()) {
    case UnaryOperator::NEGATE:
      add_ = -add_;
      break;
    case UnaryOperator::NOT:
      add_ = ADD(!BDD(add_));
      break;
  }
}

void ExpressionToAddConverter::DoVisitBinaryOperation(
    const BinaryOperation& expr) {
  expr.operand1().Accept(this);
  ADD operand1 = add_;
  expr.operand2().Accept(this);
  switch (expr.op()) {
    case BinaryOperator::PLUS:
      add_ = operand1 + add_;
      break;
    case BinaryOperator::MINUS:
      add_ = operand1 - add_;
      break;
    case BinaryOperator::MULTIPLY:
      add_ = operand1 * add_;
      break;
    case BinaryOperator::DIVIDE:
      add_ = operand1 / add_;
      break;
    case BinaryOperator::AND:
      add_ = ADD(BDD(operand1) && BDD(add_));
      break;
    case BinaryOperator::OR:
      add_ = ADD(BDD(operand1) || BDD(add_));
      break;
    case BinaryOperator::IMPLY:
      add_ = ADD(!BDD(operand1) || BDD(add_));
      break;
    case BinaryOperator::LESS:
      add_ = ADD(operand1 < add_);
      break;
    case BinaryOperator::LESS_EQUAL:
      add_ = ADD(operand1 <= add_);
      break;
    case BinaryOperator::GREATER_EQUAL:
      add_ = ADD(operand1 >= add_);
      break;
    case BinaryOperator::GREATER:
      add_ = ADD(operand1 > add_);
      break;
    case BinaryOperator::EQUAL:
    case BinaryOperator::IFF:
      add_ = ADD(operand1 == add_);
      break;
    case BinaryOperator::NOT_EQUAL:
      add_ = ADD(operand1 != add_);
      break;
  }
}

void ExpressionToAddConverter::DoVisitConditional(const Conditional& expr) {
  expr.condition().Accept(this);
  ADD condition = add_;
  expr.if_branch().Accept(this);
  ADD if_branch = add_;
  expr.else_branch().Accept(this);
  add_ = Ite(BDD(condition), if_branch, add_);
}

void ExpressionToAddConverter::DoVisitProbabilityThresholdOperation(
    const ProbabilityThresholdOperation& expr) {
  LOG(FATAL) << "not an expression";
}

void ExpressionToAddConverter::DoVisitProbabilityEstimationOperation(
    const ProbabilityEstimationOperation& expr) {
  LOG(FATAL) << "not an expression";
}

Optional<ADD> ExpressionToAdd(
    const Expression& expr,
    const std::map<std::string, const Expression*>& formulas_by_name,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    const Optional<DecisionDiagramManager>& dd_manager) {
  if (dd_manager.has_value()) {
    ExpressionToAddConverter converter(&formulas_by_name, &identifiers_by_name,
                                       &dd_manager.value());
    expr.Accept(&converter);
    return converter.add();
  }
  return Optional<ADD>();
}

}  // namespace

CompileExpressionResult::CompileExpressionResult() = default;

IdentifierInfo::IdentifierInfo(Type type, int variable_index, int low_bit,
                               int high_bit, const TypedValue& value)
    : type_(type),
      variable_index_(variable_index),
      low_bit_(low_bit),
      high_bit_(high_bit),
      value_(value) {}

IdentifierInfo IdentifierInfo::Variable(Type type, int index, int low_bit,
                                        int high_bit,
                                        const TypedValue& min_value) {
  return IdentifierInfo(type, index, low_bit, high_bit, min_value);
}

IdentifierInfo IdentifierInfo::Constant(const TypedValue& value) {
  return IdentifierInfo(value.type(), -1, -1, -1, value);
}

CompileExpressionResult CompileExpression(
    const Expression& expr, Type expected_type,
    const std::map<std::string, const Expression*>& formulas_by_name,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    const Optional<DecisionDiagramManager>& dd_manager) {
  CompileExpressionResult result;
  ExpressionCompiler compiler(&formulas_by_name, &identifiers_by_name,
                              &result.errors);
  expr.Accept(&compiler);
  if (!result.errors.empty()) {
    return result;
  }
  Type type = compiler.type();
  std::vector<Operation> operations = compiler.release_operations();
  if (type == Type::INT && expected_type == Type::DOUBLE) {
    operations.push_back(Operation::MakeI2D(0));
  } else if (type != expected_type) {
    result.errors.push_back(
        StrCat("type mismatch; expecting expression of type ", expected_type,
               "; found ", type, ": ", expr));
    return result;
  }
  result.expr = CompiledExpression(
      operations,
      ExpressionToAdd(expr, formulas_by_name, identifiers_by_name, dd_manager));
  return result;
}

namespace {

ADD IdentifierToAddImpl(const DecisionDiagramManager& dd_manager,
                        const IdentifierInfo& info, int offset) {
  if (info.type() == Type::BOOL) {
    if (info.is_variable()) {
      return dd_manager.GetAddVariable(2 * info.low_bit() + offset);
    } else {
      return dd_manager.GetConstant(info.constant_value().value<bool>() ? 1
                                                                        : 0);
    }
  } else {
    if (info.is_variable()) {
      ADD add = dd_manager.GetConstant(0);
      for (int i = info.high_bit(); i >= info.low_bit(); --i) {
        add = add + (dd_manager.GetAddVariable(2 * i + offset) *
                     dd_manager.GetConstant(1 << (info.high_bit() - i)));
      }
      return add + dd_manager.GetConstant(info.min_value().value<double>());
    } else {
      return dd_manager.GetConstant(info.constant_value().value<double>());
    }
  }
}

}  // namespace

ADD IdentifierToAdd(const DecisionDiagramManager& dd_manager,
                    const IdentifierInfo& info) {
  return IdentifierToAddImpl(dd_manager, info, 0);
}

ADD PrimedIdentifierToAdd(const DecisionDiagramManager& dd_manager,
                          const IdentifierInfo& info) {
  return IdentifierToAddImpl(dd_manager, info, 1);
}

namespace {

// An operation index in a control flow graph.
struct OperationIndex {
  // The index of the basic block to which the operation belongs.
  size_t block;
  // The index of the operation within its block.
  size_t operation;
};

bool operator<(const OperationIndex& left, const OperationIndex& right) {
  return left.block < right.block ||
         (left.block == right.block && left.operation < right.operation);
}

// The state of a register in a basic block of a control flow graph.
template <typename T>
class RegisterState {
 public:
  RegisterState();

  void SetValue(T value, const OperationIndex& dependency);
  void SetDependency(const OperationIndex& dependency);
  void AddDependencies(const std::set<OperationIndex>& dependencies);
  void MergeWith(const RegisterState<T>& other_state);

  const T* value() const { return has_value_ ? &value_ : nullptr; }
  const std::set<OperationIndex>& dependencies() const { return dependencies_; }

 private:
  bool has_value_;
  T value_;
  std::set<OperationIndex> dependencies_;
};

template <typename T>
RegisterState<T>::RegisterState()
    : has_value_(false) {}

template <typename T>
void RegisterState<T>::SetValue(T value, const OperationIndex& dependency) {
  has_value_ = true;
  value_ = value;
  dependencies_ = {dependency};
}

template <typename T>
void RegisterState<T>::SetDependency(const OperationIndex& dependency) {
  has_value_ = false;
  dependencies_ = {dependency};
}

template <typename T>
void RegisterState<T>::AddDependencies(
    const std::set<OperationIndex>& dependencies) {
  has_value_ = false;
  dependencies_.insert(dependencies.begin(), dependencies.end());
}

template <typename T>
void RegisterState<T>::MergeWith(const RegisterState<T>& other_state) {
  if (has_value_) {
    if ((other_state.has_value_ && value_ != other_state.value_) ||
        (!other_state.has_value_ && other_state.dependencies_.empty())) {
      has_value_ = false;
    }
  } else if (other_state.has_value_ && dependencies_.empty()) {
    has_value_ = true;
    value_ = other_state.value_;
  }
  AddDependencies(other_state.dependencies_);
}

// A basic block of a control flow graph.
class BasicBlock {
 public:
  BasicBlock(size_t index, bool is_dead);

  void SetDead(bool is_dead);
  void SetIntValue(size_t r, int value);
  void SetIntDependency(size_t r, const Operation& o);
  void SetDoubleValue(size_t r, double value);
  void AddOperation(const Operation& o);
  void AddUnaryIntOperation(const Operation& o);
  void AddUnaryIntFromDoubleOperation(const Operation& o);
  void AddUnaryDoubleOperation(const Operation& o);
  void AddUnaryDoubleFromIntOperation(const Operation& o);
  void AddBinaryIntOperation(const Operation& o);
  void AddBinaryIntFromDoubleOperation(const Operation& o);
  void AddBinaryDoubleOperation(const Operation& o);
  void AddSuccessor(BasicBlock* successor);

  size_t index() const { return index_; }
  bool is_dead() const { return is_dead_; }
  const std::vector<Operation>& operations() const { return operations_; }
  const std::set<OperationIndex>& dependencies() const { return dependencies_; }
  const std::vector<size_t>& successors() const { return successors_; }
  const std::map<size_t, RegisterState<int>>& int_states() const {
    return int_states_;
  }
  const std::map<size_t, RegisterState<double>>& double_states() const {
    return double_states_;
  }

  const int* GetIntValue(size_t r) const;
  const double* GetDoubleValue(size_t r) const;
  std::set<OperationIndex> GetIntDependencies(size_t r) const;
  std::set<OperationIndex> GetDoubleDependencies(size_t r) const;
  bool IsFallthrough() const;

 private:
  void SetDoubleDependency(size_t r, const Operation& o);
  void AddIntDependency(size_t r, const Operation& o);
  void AddDoubleDependency(size_t r, const Operation& o);
  void AddIntDependenciesFromIntDependencies(size_t r, size_t src_r);
  void AddIntDependenciesFromDoubleDependencies(size_t r, size_t src_r);
  void AddDoubleDependenciesFromIntDependencies(size_t r, size_t src_r);
  void AddDoubleDependenciesFromDoubleDependencies(size_t r, size_t src_r);

  size_t index_;
  bool is_dead_;
  std::vector<Operation> operations_;
  std::set<OperationIndex> dependencies_;
  std::vector<size_t> successors_;
  std::map<size_t, RegisterState<int>> int_states_;
  std::map<size_t, RegisterState<double>> double_states_;
};

BasicBlock::BasicBlock(size_t index, bool is_dead)
    : index_(index), is_dead_(is_dead) {}

void BasicBlock::SetDead(bool is_dead) { is_dead_ = is_dead; }

void BasicBlock::SetIntValue(size_t r, int value) {
  int_states_[r].SetValue(value, {index_, operations_.size()});
  operations_.push_back(Operation::MakeICONST(value, r));
}

void BasicBlock::SetIntDependency(size_t r, const Operation& o) {
  int_states_[r].SetDependency({index_, operations_.size()});
  operations_.push_back(o);
}

void BasicBlock::SetDoubleValue(size_t r, double value) {
  double_states_[r].SetValue(value, {index_, operations_.size()});
  operations_.push_back(Operation::MakeDCONST(value, r));
}

void BasicBlock::AddOperation(const Operation& o) { operations_.push_back(o); }

void BasicBlock::AddUnaryIntOperation(const Operation& o) {
  const int* value = GetIntValue(o.ioperand1());
  if (value == nullptr) {
    AddIntDependency(o.ioperand1(), o);
  } else {
    int v;
    switch (o.opcode()) {
      case Opcode::INEG:
        v = -*value;
        break;
      case Opcode::NOT:
        v = !*value;
        break;
      default:
        LOG(FATAL) << "internal error";
    }
    SetIntValue(o.ioperand1(), v);
  }
}

void BasicBlock::AddUnaryIntFromDoubleOperation(const Operation& o) {
  const double* value = GetDoubleValue(o.ioperand1());
  if (value == nullptr) {
    SetIntDependency(o.ioperand1(), o);
    AddIntDependenciesFromDoubleDependencies(o.ioperand1(), o.ioperand1());
  } else {
    int v;
    switch (o.opcode()) {
      case Opcode::FLOOR:
        v = floor(*value);
        break;
      case Opcode::CEIL:
        v = ceil(*value);
        break;
      default:
        LOG(FATAL) << "internal error";
    }
    SetIntValue(o.ioperand1(), v);
  }
}

void BasicBlock::AddUnaryDoubleOperation(const Operation& o) {
  const double* value = GetDoubleValue(o.ioperand1());
  if (value == nullptr) {
    AddDoubleDependency(o.ioperand1(), o);
  } else {
    double v;
    switch (o.opcode()) {
      case Opcode::DNEG:
        v = -*value;
        break;
      default:
        LOG(FATAL) << "internal error";
    }
    SetDoubleValue(o.ioperand1(), v);
  }
}

void BasicBlock::AddUnaryDoubleFromIntOperation(const Operation& o) {
  const int* value = GetIntValue(o.ioperand1());
  if (value == nullptr) {
    SetDoubleDependency(o.ioperand1(), o);
    AddDoubleDependenciesFromIntDependencies(o.ioperand1(), o.ioperand1());
  } else {
    double v;
    switch (o.opcode()) {
      case Opcode::I2D:
        v = *value;
        break;
      default:
        LOG(FATAL) << "internal error";
    }
    SetDoubleValue(o.ioperand1(), v);
  }
}

void BasicBlock::AddBinaryIntOperation(const Operation& o) {
  const int* value1 = GetIntValue(o.ioperand1());
  const int* value2 = GetIntValue(o.operand2());
  if (value1 == nullptr || value2 == nullptr) {
    AddIntDependency(o.ioperand1(), o);
    AddIntDependenciesFromIntDependencies(o.ioperand1(), o.operand2());
  } else {
    int v;
    switch (o.opcode()) {
      case Opcode::IADD:
        v = *value1 + *value2;
        break;
      case Opcode::ISUB:
        v = *value1 - *value2;
        break;
      case Opcode::IMUL:
        v = *value1 * *value2;
        break;
      case Opcode::IEQ:
        v = *value1 == *value2;
        break;
      case Opcode::INE:
        v = *value1 != *value2;
        break;
      case Opcode::ILT:
        v = *value1 < *value2;
        break;
      case Opcode::ILE:
        v = *value1 <= *value2;
        break;
      case Opcode::IGE:
        v = *value1 >= *value2;
        break;
      case Opcode::IGT:
        v = *value1 > *value2;
        break;
      case Opcode::IMIN:
        v = std::min(*value1, *value2);
        break;
      case Opcode::IMAX:
        v = std::max(*value1, *value2);
        break;
      case Opcode::MOD:
        v = *value1 % *value2;
        break;
      default:
        LOG(FATAL) << "internal error";
    }
    SetIntValue(o.ioperand1(), v);
  }
}

void BasicBlock::AddBinaryIntFromDoubleOperation(const Operation& o) {
  const double* value1 = GetDoubleValue(o.ioperand1());
  const double* value2 = GetDoubleValue(o.operand2());
  if (value1 == nullptr || value2 == nullptr) {
    SetIntDependency(o.ioperand1(), o);
    AddIntDependenciesFromDoubleDependencies(o.ioperand1(), o.ioperand1());
    AddIntDependenciesFromDoubleDependencies(o.ioperand1(), o.operand2());
  } else {
    int v;
    switch (o.opcode()) {
      case Opcode::DEQ:
        v = *value1 == *value2;
        break;
      case Opcode::DNE:
        v = *value1 != *value2;
        break;
      case Opcode::DLT:
        v = *value1 < *value2;
        break;
      case Opcode::DLE:
        v = *value1 <= *value2;
        break;
      case Opcode::DGE:
        v = *value1 >= *value2;
        break;
      case Opcode::DGT:
        v = *value1 > *value2;
        break;
      default:
        LOG(FATAL) << "internal error";
    }
    SetIntValue(o.ioperand1(), v);
  }
}

void BasicBlock::AddBinaryDoubleOperation(const Operation& o) {
  const double* value1 = GetDoubleValue(o.ioperand1());
  const double* value2 = GetDoubleValue(o.operand2());
  if (value1 == nullptr || value2 == nullptr) {
    AddDoubleDependency(o.ioperand1(), o);
    AddDoubleDependenciesFromDoubleDependencies(o.ioperand1(), o.operand2());
  } else {
    double v;
    switch (o.opcode()) {
      case Opcode::DADD:
        v = *value1 + *value2;
        break;
      case Opcode::DSUB:
        v = *value1 - *value2;
        break;
      case Opcode::DMUL:
        v = *value1 * *value2;
        break;
      case Opcode::DDIV:
        v = *value1 / *value2;
        break;
      case Opcode::DMIN:
        v = std::min(*value1, *value2);
        break;
      case Opcode::DMAX:
        v = std::max(*value1, *value2);
        break;
      case Opcode::POW:
        v = pow(*value1, *value2);
        break;
      case Opcode::LOG:
        v = log(*value1) / log(*value2);
        break;
      default:
        LOG(FATAL) << "internal error";
    }
    SetDoubleValue(o.ioperand1(), v);
  }
}

void BasicBlock::SetDoubleDependency(size_t r, const Operation& o) {
  double_states_[r].SetDependency({index_, operations_.size()});
  operations_.push_back(o);
}

void BasicBlock::AddIntDependency(size_t r, const Operation& o) {
  if ((o.opcode() == Opcode::INEG || o.opcode() == Opcode::NOT) &&
      !operations_.empty() && operations_.back() == o) {
    operations_.back() = Operation::MakeNOP();
  } else {
    int_states_[r].AddDependencies({{index_, operations_.size()}});
    operations_.push_back(o);
  }
}

void BasicBlock::AddDoubleDependency(size_t r, const Operation& o) {
  if (o.opcode() == Opcode::DNEG && !operations_.empty() &&
      operations_.back() == o) {
    operations_.back() = Operation::MakeNOP();
  } else {
    double_states_[r].AddDependencies({{index_, operations_.size()}});
    operations_.push_back(o);
  }
}

void BasicBlock::AddIntDependenciesFromIntDependencies(size_t r, size_t src_r) {
  int_states_[r].AddDependencies(int_states_[src_r].dependencies());
}

void BasicBlock::AddIntDependenciesFromDoubleDependencies(size_t r,
                                                          size_t src_r) {
  int_states_[r].AddDependencies(double_states_[src_r].dependencies());
}

void BasicBlock::AddDoubleDependenciesFromIntDependencies(size_t r,
                                                          size_t src_r) {
  double_states_[r].AddDependencies(int_states_[src_r].dependencies());
}

void BasicBlock::AddDoubleDependenciesFromDoubleDependencies(size_t r,
                                                             size_t src_r) {
  double_states_[r].AddDependencies(double_states_[src_r].dependencies());
}

void BasicBlock::AddSuccessor(BasicBlock* successor) {
  successors_.push_back(successor->index());
  successor->SetDead(false);
  for (const auto& entry : int_states()) {
    successor->int_states_[entry.first].MergeWith(entry.second);
  }
  for (const auto& entry : double_states()) {
    successor->double_states_[entry.first].MergeWith(entry.second);
  }
  successor->dependencies_.insert(dependencies_.begin(), dependencies_.end());
  if (!IsFallthrough()) {
    successor->dependencies_.insert({index_, operations_.size() - 1});
    const Operation& o = operations_.back();
    if (o.opcode() != Opcode::GOTO) {
      const std::set<OperationIndex>& branch_dependencies =
          int_states_[o.ioperand1()].dependencies();
      successor->dependencies_.insert(branch_dependencies.begin(),
                                      branch_dependencies.end());
    }
  }
}

const int* BasicBlock::GetIntValue(size_t r) const {
  const auto i = int_states_.find(r);
  return (i == int_states_.end()) ? nullptr : i->second.value();
}

const double* BasicBlock::GetDoubleValue(size_t r) const {
  const auto i = double_states_.find(r);
  return (i == double_states_.end()) ? nullptr : i->second.value();
}

std::set<OperationIndex> BasicBlock::GetIntDependencies(size_t r) const {
  std::set<OperationIndex> int_dependencies;
  const auto i = int_states_.find(r);
  if (i != int_states_.end()) {
    int_dependencies = i->second.dependencies();
    int_dependencies.insert(dependencies_.begin(), dependencies_.end());
  }
  return int_dependencies;
}

std::set<OperationIndex> BasicBlock::GetDoubleDependencies(size_t r) const {
  std::set<OperationIndex> double_dependencies;
  const auto i = double_states_.find(r);
  if (i != double_states_.end()) {
    double_dependencies = i->second.dependencies();
    double_dependencies.insert(dependencies_.begin(), dependencies_.end());
  }
  return double_dependencies;
}

bool IsJump(const Operation& o) {
  switch (o.opcode()) {
    case Opcode::IFFALSE:
    case Opcode::IFTRUE:
    case Opcode::GOTO:
      return true;
    default:
      return false;
  }
}

bool BasicBlock::IsFallthrough() const {
  return operations_.empty() || !IsJump(operations_.back());
}

void MaybeAddBlock(size_t predecessor, size_t start,
                   std::vector<BasicBlock>* blocks,
                   std::map<size_t, size_t>* block_starts) {
  const auto result = block_starts->insert({start, blocks->size()});
  if (result.second) {
    blocks->emplace_back(blocks->size(), false);
  }
  (*blocks)[predecessor].AddSuccessor(&(*blocks)[result.first->second]);
}

void MaybeAddDeadBlock(size_t predecessor, size_t start,
                       std::vector<BasicBlock>* blocks,
                       std::map<size_t, size_t>* block_starts) {
  const auto result = block_starts->insert({start, blocks->size()});
  if (result.second) {
    blocks->emplace_back(blocks->size(), true);
  }
}

void MaybeAddBranch(size_t block_index, const Operation& o, size_t pc,
                    std::vector<BasicBlock>* blocks,
                    std::map<size_t, size_t>* block_starts) {
  BasicBlock& block = (*blocks)[block_index];
  const int* value = block.GetIntValue(o.ioperand1());
  if (value == nullptr) {
    block.AddOperation(o);
    MaybeAddBlock(block_index, pc + 1, blocks, block_starts);
    MaybeAddBlock(block_index, o.operand2(), blocks, block_starts);
  } else if ((o.opcode() == Opcode::IFFALSE && !*value) ||
             (o.opcode() == Opcode::IFTRUE && *value)) {
    block.AddOperation(Operation::MakeGOTO(o.operand2()));
    MaybeAddDeadBlock(block_index, pc + 1, blocks, block_starts);
    MaybeAddBlock(block_index, o.operand2(), blocks, block_starts);
  } else {
    block.AddOperation(Operation::MakeNOP());
    MaybeAddDeadBlock(block_index, o.operand2(), blocks, block_starts);
  }
}

std::vector<BasicBlock> MakeControlFlowGraph(
    const std::vector<Operation>& operations) {
  std::vector<BasicBlock> blocks;
  std::map<size_t, size_t> block_starts;
  size_t block_index = 0;
  blocks.emplace_back(block_index, false);
  for (size_t pc = 0; pc < operations.size(); ++pc) {
    const auto i = block_starts.find(pc);
    if (i != block_starts.end()) {
      if (blocks[block_index].IsFallthrough()) {
        MaybeAddBlock(block_index, pc, &blocks, &block_starts);
      }
      block_index = i->second;
    }
    BasicBlock& block = blocks[block_index];
    if (block.is_dead()) {
      continue;
    }
    const Operation& o = operations[pc];
    switch (o.opcode()) {
      case Opcode::ICONST:
        block.SetIntValue(o.operand2(), o.ioperand1());
        continue;
      case Opcode::DCONST:
        block.SetDoubleValue(o.operand2(), o.doperand1());
        continue;
      case Opcode::ILOAD:
        block.SetIntDependency(o.operand2(), o);
        continue;
      case Opcode::INEG:
      case Opcode::NOT:
        block.AddUnaryIntOperation(o);
        continue;
      case Opcode::FLOOR:
      case Opcode::CEIL:
        block.AddUnaryIntFromDoubleOperation(o);
        continue;
      case Opcode::DNEG:
        block.AddUnaryDoubleOperation(o);
        continue;
      case Opcode::I2D:
        block.AddUnaryDoubleFromIntOperation(o);
        continue;
      case Opcode::IADD:
      case Opcode::ISUB:
      case Opcode::IMUL:
      case Opcode::IEQ:
      case Opcode::INE:
      case Opcode::ILT:
      case Opcode::ILE:
      case Opcode::IGE:
      case Opcode::IGT:
      case Opcode::IMIN:
      case Opcode::IMAX:
      case Opcode::MOD:
        block.AddBinaryIntOperation(o);
        continue;
      case Opcode::DADD:
      case Opcode::DSUB:
      case Opcode::DMUL:
      case Opcode::DDIV:
      case Opcode::DMIN:
      case Opcode::DMAX:
      case Opcode::POW:
      case Opcode::LOG:
        block.AddBinaryDoubleOperation(o);
        continue;
      case Opcode::DEQ:
      case Opcode::DNE:
      case Opcode::DLT:
      case Opcode::DLE:
      case Opcode::DGE:
      case Opcode::DGT:
        block.AddBinaryIntFromDoubleOperation(o);
        continue;
      case Opcode::IFFALSE:
      case Opcode::IFTRUE:
        MaybeAddBranch(block_index, o, pc, &blocks, &block_starts);
        continue;
      case Opcode::GOTO:
        block.AddOperation(o);
        MaybeAddBlock(block_index, o.ioperand1(), &blocks, &block_starts);
        continue;
      case Opcode::NOP:
        continue;
    }
    LOG(FATAL) << "bad opcode";
  }
  if (blocks[block_index].IsFallthrough()) {
    MaybeAddBlock(block_index, operations.size(), &blocks, &block_starts);
  }
  return blocks;
}

size_t GetEndBlockIndex(const std::vector<BasicBlock>& blocks) {
  std::queue<size_t> live_blocks;
  live_blocks.push(0);
  while (!live_blocks.empty()) {
    size_t i = live_blocks.front();
    live_blocks.pop();
    if (blocks[i].successors().empty()) {
      return i;
    }
    for (size_t j : blocks[i].successors()) {
      live_blocks.push(j);
    }
  }
  LOG(FATAL) << "internal error";
}

Operation SetJumpDestination(const Operation& jump_operation, int destination) {
  switch (jump_operation.opcode()) {
    case Opcode::IFFALSE:
      return Operation::MakeIFFALSE(jump_operation.ioperand1(), destination);
    case Opcode::IFTRUE:
      return Operation::MakeIFTRUE(jump_operation.ioperand1(), destination);
    case Opcode::GOTO:
      return Operation::MakeGOTO(destination);
    default:
      LOG(FATAL) << "internal error";
  }
}

CompiledExpression OptimizeExpressionImpl(
    const std::vector<BasicBlock>& blocks, size_t end_block_index,
    const std::set<OperationIndex>& live_operations, const Optional<ADD>& dd) {
  std::vector<Operation> optimized_operations;
  std::multimap<size_t, size_t> jump_targets;
  for (const OperationIndex& live_operation : live_operations) {
    const BasicBlock& block = blocks[live_operation.block];
    const Operation& operation = block.operations()[live_operation.operation];
    bool overwrite_jump_operation = false;
    std::set<size_t> jump_operations_to_adjust;
    while (true) {
      auto i = jump_targets.find(block.index());
      if (i == jump_targets.end()) {
        break;
      }
      Operation& jump_operation = optimized_operations[i->second];
      if (i->second == optimized_operations.size() - 1) {
        overwrite_jump_operation = true;
      } else if (operation.opcode() == jump_operation.opcode() &&
                 (operation.opcode() == Opcode::GOTO ||
                  operation.ioperand1() == jump_operation.ioperand1())) {
        jump_targets.insert({block.successors().back(), i->second});
      } else {
        jump_operations_to_adjust.insert(i->second);
      }
      jump_targets.erase(i);
    }
    if (overwrite_jump_operation) {
      optimized_operations.pop_back();
    }
    for (const size_t i : jump_operations_to_adjust) {
      optimized_operations[i] = SetJumpDestination(optimized_operations[i],
                                                   optimized_operations.size());
    }
    if (operation.opcode() != Opcode::NOP) {
      if (IsJump(operation)) {
        jump_targets.insert(
            {block.successors().back(), optimized_operations.size()});
      }
      optimized_operations.push_back(operation);
    }
  }
  if (IsJump(optimized_operations.back())) {
    optimized_operations.pop_back();
  }
  const auto range = jump_targets.equal_range(end_block_index);
  for (auto i = range.first; i != range.second; ++i) {
    if (i->second < optimized_operations.size()) {
      optimized_operations[i->second] = SetJumpDestination(
          optimized_operations[i->second], optimized_operations.size());
    }
  }
  return CompiledExpression(optimized_operations, dd);
}

}  // namespace

CompiledExpression OptimizeIntExpression(const CompiledExpression& expr) {
  const std::vector<BasicBlock> blocks =
      MakeControlFlowGraph(expr.operations());
  const size_t end_block_index = GetEndBlockIndex(blocks);
  const std::set<OperationIndex> live_operations =
      blocks[end_block_index].GetIntDependencies(0);
  return OptimizeExpressionImpl(blocks, end_block_index, live_operations,
                                expr.dd());
}

CompiledExpression OptimizeDoubleExpression(const CompiledExpression& expr) {
  const std::vector<BasicBlock> blocks =
      MakeControlFlowGraph(expr.operations());
  const size_t end_block_index = GetEndBlockIndex(blocks);
  const std::set<OperationIndex> live_operations =
      blocks[end_block_index].GetDoubleDependencies(0);
  return OptimizeExpressionImpl(blocks, end_block_index, live_operations,
                                expr.dd());
}

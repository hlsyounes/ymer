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
#include <ostream>
#include <vector>
#include <utility>

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

Operation Operation::MakeGOTO(int pc) {
  return Operation(Opcode::GOTO, pc);
}

Operation Operation::MakeNOP() {
  return Operation(Opcode::NOP);
}

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

Operation::Operation(Opcode opcode, int operand)
    : opcode_(opcode) {
  operand1_.i = operand;
}

Operation::Operation(Opcode opcode)
    : opcode_(opcode) {
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
      os << ' ' << operation.ioperand1() << ' ' << operation.operand2();
      break;
    case Opcode::DCONST:
      os << ' ' << operation.doperand1() << ' ' << operation.operand2();
      break;
    case Opcode::I2D:
    case Opcode::INEG:
    case Opcode::DNEG:
    case Opcode::NOT:
    case Opcode::GOTO:
    case Opcode::FLOOR:
    case Opcode::CEIL:
      os << ' ' << operation.ioperand1();
      break;
    case Opcode::NOP:
      break;
  }
  return os;
}

CompiledExpression::CompiledExpression(
    const std::vector<Operation>& operations)
    : operations_(operations) {
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

std::pair<int, int> GetNumRegisters(const CompiledExpression& expr) {
  int max_ireg = -1;
  int max_dreg = -1;
  for (const Operation& o : expr.operations()) {
    switch (o.opcode()) {
      case Opcode::ICONST:
      case Opcode::ILOAD:
        max_ireg = std::max(max_ireg, o.operand2());
        break;
      case Opcode::DCONST:
        max_dreg = std::max(max_dreg, o.operand2());
        break;
      case Opcode::I2D:
      case Opcode::FLOOR:
      case Opcode::CEIL:
        max_ireg = std::max(max_ireg, o.ioperand1());
        max_dreg = std::max(max_dreg, o.ioperand1());
        break;
      case Opcode::INEG:
      case Opcode::NOT:
      case Opcode::IFFALSE:
      case Opcode::IFTRUE:
        max_ireg = std::max(max_ireg, o.ioperand1());
        break;
      case Opcode::DNEG:
        max_dreg = std::max(max_dreg, o.ioperand1());
        break;
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
        max_ireg = std::max({ max_ireg, o.ioperand1(), o.operand2() });
        break;
      case Opcode::DADD:
      case Opcode::DSUB:
      case Opcode::DMUL:
      case Opcode::DDIV:
      case Opcode::DMIN:
      case Opcode::DMAX:
      case Opcode::POW:
      case Opcode::LOG:
        max_dreg = std::max({ max_dreg, o.ioperand1(), o.operand2() });
        break;
      case Opcode::DEQ:
      case Opcode::DNE:
      case Opcode::DLT:
      case Opcode::DLE:
      case Opcode::DGE:
      case Opcode::DGT:
        max_ireg = std::max(max_ireg, o.ioperand1());
        max_dreg = std::max({ max_dreg, o.ioperand1(), o.operand2() });
        break;
      case Opcode::GOTO:
      case Opcode::NOP:
        break;
    }
  }
  return { max_ireg + 1, max_dreg + 1 };
}

CompiledExpressionEvaluator::CompiledExpressionEvaluator(
    int num_iregs, int num_dregs)
    : iregs_(num_iregs), dregs_(num_dregs) {
}

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
        break;
      case Opcode::DCONST:
        dregs_[o.operand2()] = o.doperand1();
        break;
      case Opcode::ILOAD:
        iregs_[o.operand2()] = state[o.ioperand1()];
        break;
      case Opcode::I2D:
        dregs_[o.ioperand1()] = iregs_[o.ioperand1()];
        break;
      case Opcode::INEG:
        iregs_[o.ioperand1()] *= -1;
        break;
      case Opcode::DNEG:
        dregs_[o.ioperand1()] *= -1.0;
        break;
      case Opcode::NOT:
        iregs_[o.ioperand1()] = !iregs_[o.ioperand1()];
        break;
      case Opcode::IADD:
        iregs_[o.ioperand1()] += iregs_[o.operand2()];
        break;
      case Opcode::DADD:
        dregs_[o.ioperand1()] += dregs_[o.operand2()];
        break;
      case Opcode::ISUB:
        iregs_[o.ioperand1()] -= iregs_[o.operand2()];
        break;
      case Opcode::DSUB:
        dregs_[o.ioperand1()] -= dregs_[o.operand2()];
        break;
      case Opcode::IMUL:
        iregs_[o.ioperand1()] *= iregs_[o.operand2()];
        break;
      case Opcode::DMUL:
        dregs_[o.ioperand1()] *= dregs_[o.operand2()];
        break;
      case Opcode::DDIV:
        dregs_[o.ioperand1()] /= dregs_[o.operand2()];
        break;
      case Opcode::IEQ:
        iregs_[o.ioperand1()] = iregs_[o.ioperand1()] == iregs_[o.operand2()];
        break;
      case Opcode::DEQ:
        iregs_[o.ioperand1()] = dregs_[o.ioperand1()] == dregs_[o.operand2()];
        break;
      case Opcode::INE:
        iregs_[o.ioperand1()] = iregs_[o.ioperand1()] != iregs_[o.operand2()];
        break;
      case Opcode::DNE:
        iregs_[o.ioperand1()] = dregs_[o.ioperand1()] != dregs_[o.operand2()];
        break;
      case Opcode::ILT:
        iregs_[o.ioperand1()] = iregs_[o.ioperand1()] < iregs_[o.operand2()];
        break;
      case Opcode::DLT:
        iregs_[o.ioperand1()] = dregs_[o.ioperand1()] < dregs_[o.operand2()];
        break;
      case Opcode::ILE:
        iregs_[o.ioperand1()] = iregs_[o.ioperand1()] <= iregs_[o.operand2()];
        break;
      case Opcode::DLE:
        iregs_[o.ioperand1()] = dregs_[o.ioperand1()] <= dregs_[o.operand2()];
        break;
      case Opcode::IGE:
        iregs_[o.ioperand1()] = iregs_[o.ioperand1()] >= iregs_[o.operand2()];
        break;
      case Opcode::DGE:
        iregs_[o.ioperand1()] = dregs_[o.ioperand1()] >= dregs_[o.operand2()];
        break;
      case Opcode::IGT:
        iregs_[o.ioperand1()] = iregs_[o.ioperand1()] > iregs_[o.operand2()];
        break;
      case Opcode::DGT:
        iregs_[o.ioperand1()] = dregs_[o.ioperand1()] > dregs_[o.operand2()];
        break;
      case Opcode::IFFALSE:
        if (!iregs_[o.ioperand1()]) pc = o.operand2() - 1;
        break;
      case Opcode::IFTRUE:
        if (iregs_[o.ioperand1()]) pc = o.operand2() - 1;
        break;
      case Opcode::GOTO:
        pc = o.ioperand1() - 1;
        break;
      case Opcode::NOP:
        // Do nothing.
        break;
      case Opcode::IMIN:
        iregs_[o.ioperand1()] =
            std::min(iregs_[o.ioperand1()], iregs_[o.operand2()]);
        break;
      case Opcode::DMIN:
        dregs_[o.ioperand1()] =
            std::min(dregs_[o.ioperand1()], dregs_[o.operand2()]);
        break;
      case Opcode::IMAX:
        iregs_[o.ioperand1()] =
            std::max(iregs_[o.ioperand1()], iregs_[o.operand2()]);
        break;
      case Opcode::DMAX:
        dregs_[o.ioperand1()] =
            std::max(dregs_[o.ioperand1()], dregs_[o.operand2()]);
        break;
      case Opcode::FLOOR:
        iregs_[o.ioperand1()] = floor(dregs_[o.ioperand1()]);
        break;
      case Opcode::CEIL:
        iregs_[o.ioperand1()] = ceil(dregs_[o.ioperand1()]);
        break;
      case Opcode::POW:
        dregs_[o.ioperand1()] =
            pow(dregs_[o.ioperand1()], dregs_[o.operand2()]);
        break;
      case Opcode::LOG:
        dregs_[o.ioperand1()] =
            log(dregs_[o.ioperand1()]) / log(dregs_[o.operand2()]);
        break;
      case Opcode::MOD:
        iregs_[o.ioperand1()] %= iregs_[o.operand2()];
        break;
    }
  }
}

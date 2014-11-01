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
// Classes for representing and evaluating compiled expressions.  A compiled
// expression is represented as a sequence of operations for a simple virtual
// machine with a fixed number of integer and double registers.

#ifndef COMPILED_EXPRESSION_H_
#define COMPILED_EXPRESSION_H_

#include <map>
#include <ostream>
#include <utility>
#include <vector>

#include "expression.h"
#include "typed-value.h"

// Opcodes supported by the virtual machine used for evaluating compiled
// expressions.
enum class Opcode {
  ICONST,
  DCONST,
  ILOAD,
  I2D,
  INEG,
  DNEG,
  NOT,
  IADD,
  DADD,
  ISUB,
  DSUB,
  IMUL,
  DMUL,
  DDIV,
  IEQ,
  DEQ,
  INE,
  DNE,
  ILT,
  DLT,
  ILE,
  DLE,
  IGE,
  DGE,
  IGT,
  DGT,
  IFFALSE,
  IFTRUE,
  GOTO,
  NOP,
  IMIN,
  DMIN,
  IMAX,
  DMAX,
  FLOOR,
  CEIL,
  POW,
  LOG,
  MOD
};

// Output operator for opcodes.
std::ostream& operator<<(std::ostream& os, Opcode opcode);

// An operation on the virtual machine used for evaluating compiled expressions.
class Operation {
 public:
  // Put value into integer register dst.
  static Operation MakeICONST(int value, int dst);
  // Put value into integer register dst.
  static Operation MakeICONST(bool value, int dst);
  // Put value into double register dst.
  static Operation MakeDCONST(double value, int dst);
  // Put variable into integer register dst.
  static Operation MakeILOAD(int variable, int dst);
  // Copy integer register src_dst to double register src_dst.
  static Operation MakeI2D(int src_dst);
  // Negate integer register src_dst.
  static Operation MakeINEG(int src_dst);
  // Negate double register src_dst.
  static Operation MakeDNEG(int src_dst);
  // Flip the logical value of integer register src_dst.
  static Operation MakeNOT(int src_dst);
  // Add integer register src2 to integer register src1_dst.
  static Operation MakeIADD(int src1_dst, int src2);
  // Add double register src2 to double register src1_dst.
  static Operation MakeDADD(int src1_dst, int src2);
  // Subtract integer register src2 from integer register src1_dst.
  static Operation MakeISUB(int src1_dst, int src2);
  // Subtract double register src2 from double register src1_dst.
  static Operation MakeDSUB(int src1_dst, int src2);
  // Multiply integer register src1_dst by integer register src2.
  static Operation MakeIMUL(int src1_dst, int src2);
  // Multiply double register src1_dst by double register src2.
  static Operation MakeDMUL(int src1_dst, int src2);
  // Divide double register src1_dst by double register src2.
  static Operation MakeDDIV(int src1_dst, int src2);
  // Compare integer registers src1_dst and src2 using ==, and put result in
  // integer register src1_dst.
  static Operation MakeIEQ(int src1_dst, int src2);
  // Compare double registers src1_dst and src2 using ==, and put result in
  // integer register src1_dst.
  static Operation MakeDEQ(int src1_dst, int src2);
  // Compare integer registers src1_dst and src2 using !=, and put result in
  // integer register src1_dst.
  static Operation MakeINE(int src1_dst, int src2);
  // Compare double registers src1_dst and src2 using !=, and put result in
  // integer register src1_dst.
  static Operation MakeDNE(int src1_dst, int src2);
  // Compare integer registers src1_dst and src2 using <, and put result in
  // integer register src1_dst.
  static Operation MakeILT(int src1_dst, int src2);
  // Compare double registers src1_dst and src2 using <, and put result in
  // integer register src1_dst.
  static Operation MakeDLT(int src1_dst, int src2);
  // Compare integer registers src1_dst and src2 using <=, and put result in
  // integer register src1_dst.
  static Operation MakeILE(int src1_dst, int src2);
  // Compare double registers src1_dst and src2 using <=, and put result in
  // integer register src1_dst.
  static Operation MakeDLE(int src1_dst, int src2);
  // Compare integer registers src1_dst and src2 using >=, and put result in
  // integer register src1_dst.
  static Operation MakeIGE(int src1_dst, int src2);
  // Compare double registers src1_dst and src2 using >=, and put result in
  // integer register src1_dst.
  static Operation MakeDGE(int src1_dst, int src2);
  // Compare integer registers src1_dst and src2 using >, and put result in
  // integer register src1_dst.
  static Operation MakeIGT(int src1_dst, int src2);
  // Compare double registers src1_dst and src2 using >, and put result in
  // integer register src1_dst.
  static Operation MakeDGT(int src1_dst, int src2);
  // Set program counter to pc if logical value integer register src is false.
  static Operation MakeIFFALSE(int src, int pc);
  // Set program counter to pc if logical value integer register src is true.
  static Operation MakeIFTRUE(int src, int pc);
  // Unconditionally set program counter to pc.
  static Operation MakeGOTO(int pc);
  // Does nothing.
  static Operation MakeNOP();
  // Put min of integer registers src1_dst and src2 in integer register
  // src1_dst.
  static Operation MakeIMIN(int src1_dst, int src2);
  // Put min of double registers src1_dst and src2 in double register src2_dst.
  static Operation MakeDMIN(int src1_dst, int src2);
  // Put max of integer registers src1_dst and src2 in integer register
  // src1_dst.
  static Operation MakeIMAX(int src1_dst, int src2);
  // Put max of double registers src1_dst and src2 in double register src2_dst.
  static Operation MakeDMAX(int src1_dst, int src2);
  // Put floor of double register src_dst in integer register src_dst.
  static Operation MakeFLOOR(int src_dst);
  // Put ceiling of double register src_dst in integer register src_dst.
  static Operation MakeCEIL(int src_dst);
  // Raise double register src1_dst to the power of double register src2.
  static Operation MakePOW(int src1_dst, int src2);
  // Take the logarithm of double register src1_dst with base in double register
  // src2.
  static Operation MakeLOG(int src1_dst, int src2);
  // Make integer register src1_dst modulo value in integer register src2.
  static Operation MakeMOD(int src1_dst, int src2);

  // Returns the opcode for this operation.
  Opcode opcode() const { return opcode_; }

  // Returns the first operand as an integer.
  int ioperand1() const { return operand1_.i; }

  // Returns the first operand as a double.
  double doperand1() const { return operand1_.d; }

  // Returns the second operand.
  int operand2() const { return operand2_; }

  // Returns a copy of this operation with program counters and registers
  // shifted the given number of positions.
  Operation Shift(int pc_shift, int reg_shift) const;

 private:
  // Constructs an operation with no operands.
  explicit Operation(Opcode opcode);
  // Constructs an operation with a single integer operand.
  Operation(Opcode opcode, int operand1);
  // Constructs an operation with two integer operands.
  Operation(Opcode opcode, int operand1, int operand2);
  // Constructs an operation with a double and an integer operand.
  Operation(Opcode opcode, double operand1, int operand2);

  Opcode opcode_;
  union {
    int i;
    double d;
  } operand1_;
  int operand2_;
};

// Equality operator for operations.
bool operator==(const Operation& left, const Operation& right);

// Output operator for operations.
std::ostream& operator<<(std::ostream& os, const Operation& operation);

// A compiled expression.
class CompiledExpression {
 public:
  // Constructs a compiled expression with the given operations.
  explicit CompiledExpression(const std::vector<Operation>& operations);

  // Returns the operations for this compiled expression.
  const std::vector<Operation>& operations() const { return operations_; }

 private:
  std::vector<Operation> operations_;
};

// Comparison operators for compiled expressions.
bool operator==(const CompiledExpression& e1, const CompiledExpression& e2);
bool operator!=(const CompiledExpression& e1, const CompiledExpression& e2);

// Output operator for compiled expressions.
std::ostream& operator<<(std::ostream& os, const CompiledExpression& expr);

// Returns the number of integer and double registers referenced by the given
// compiled expression.
std::pair<int, int> GetNumRegisters(const CompiledExpression& expr);

// A virtual machine for evaluating for compiled expressions.
class CompiledExpressionEvaluator {
 public:
  // Constructs an evaluator for compiled expressions with num_iregs integer
  // registers and num_dregs double registers.
  CompiledExpressionEvaluator(int num_iregs, int num_dregs);

  // Evaluates expr as an integer expression in the given state.  Assumes that
  // the result of the evaluation ends up in integer register 0.
  int EvaluateIntExpression(const CompiledExpression& expr,
                            const std::vector<int>& state);

  // Evaluates expr as a double expression in the given state.  Assumes that the
  // result of the evaluation ends up in double register 0.
  double EvaluateDoubleExpression(const CompiledExpression& expr,
                                  const std::vector<int>& state);

 private:
  // Executes a sequence of operations in a given state.
  void ExecuteOperations(const std::vector<Operation>& operations,
                         const std::vector<int>& state);

  std::vector<int> iregs_;
  std::vector<double> dregs_;
};

// The result of an expression compilation.  On success, expr will hold the
// compiled expression.  On error, expr will be an empty compiled expression and
// errors will be populated with error messages.
struct CompileExpressionResult {
  CompileExpressionResult();

  CompiledExpression expr;
  std::vector<std::string> errors;
};

// Information for an identifier, used for expression compilation.  Can
// represent either a variable or a constant.
class IdentifierInfo {
 public:
  static IdentifierInfo Variable(Type type, int index, int low_bit,
                                 int high_bit, const TypedValue& min_value);
  static IdentifierInfo Constant(const TypedValue& value);

  Type type() const { return type_; }
  bool is_variable() const { return variable_index_ >= 0; }
  int variable_index() const { return variable_index_; }
  int low_bit() const { return low_bit_; }
  int high_bit() const { return high_bit_; }
  TypedValue min_value() const { return value_; }
  TypedValue constant_value() const { return value_; }

 private:
  explicit IdentifierInfo(Type type, int variable_index, int low_bit,
                          int high_bit, const TypedValue& value);

  Type type_;
  int variable_index_;
  int low_bit_;
  int high_bit_;
  TypedValue value_;
};

// Compiles the given expression, expecting it to be of the given type, and
// using the given identifier name to info map to compile identifiers.  On
// error, the result contains an empty compiled expression and the errors vector
// will be populated with error messages.
CompileExpressionResult CompileExpression(
    const Expression& expr, Type expected_type,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name);

// Optimizes the given expression, assuming it evaluates to an integer in
// register 0.
CompiledExpression OptimizeIntExpression(const CompiledExpression& expr);

// Optimizes the given expression, assuming it evaluates to a double in
// register 0.
CompiledExpression OptimizeDoubleExpression(const CompiledExpression& expr);

#endif  // COMPILED_EXPRESSION_H_

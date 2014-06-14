/*
 * Main program.
 *
 * Copyright (C) 2003--2005 Carnegie Mellon University
 * Copyright (C) 2011--2012 Google Inc
 *
 * This file is part of Ymer.
 *
 * Ymer is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Ymer is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
 * License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ymer; if not, write to the Free Software Foundation,
 * Inc., #59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
#include <config.h>
#include "comm.h"
#include "distributions.h"
#include "states.h"
#include "models.h"
#include "formulas.h"
#include "src/compiled-property.h"
#include "src/ddutil.h"
#include "src/rng.h"
#include "src/strutil.h"
#include "src/typed-value.h"
#include "glog/logging.h"
#include <cudd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#if HAVE_GETOPT_LONG
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <getopt.h>
#endif
#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

/* The parse function. */
extern int yyparse();
/* File to parse. */
extern FILE* yyin;
/* Current model. */
extern const Model* global_model;
/* Parsed properties. */
extern std::vector<const StateFormula*> properties;
/* Clears all previously parsed declarations. */
extern void clear_declarations();

/* Name of current file. */
std::string current_file;
/* Constant overrides. */
std::map<std::string, TypedValue> const_overrides;
/* Sockets for communication. */
int server_socket = -1;
/* Current property. */
int current_property;

/* Program options. */
static option long_options[] = {
  { "alpha", required_argument, 0, 'A' },
  { "beta", required_argument, 0, 'B' },
  { "delta", required_argument, 0, 'D' },
  { "epsilon", required_argument, 0, 'E' },
  { "const", required_argument, 0, 'c' },
  { "engine", required_argument, 0, 'e' },
  { "host", required_argument, 0, 'H' },
  { "max-path-length", required_argument, 0, 'L' },
  { "memoization", no_argument, 0, 'M' },
  { "matching-moments", required_argument, 0, 'm' },
  { "fixed-sample-size", required_argument, 0, 'N' },
  { "nested-error", required_argument, 0, 'n' },
  { "estimate-probabilities", no_argument, 0, 'p' },
  { "port", required_argument, 0, 'P' },
  { "sampling-algorithm", required_argument, 0, 's' },
  { "seed", required_argument, 0, 'S' },
  { "trials", required_argument, 0, 'T' },
  { "version", no_argument, 0, 'V' },
  { "help", no_argument, 0, 'h' },
  { 0, 0, 0, 0 }
};
static const char OPTION_STRING[] = "A:B:c:D:E:e:H:hL:Mm:N:n:pP:s:S:T:V";

namespace {

/* Displays help. */
void display_help() {
  std::cout << "usage: " << PACKAGE << " [options] [file ...]" << std::endl
	    << "options:" << std::endl
	    << "  -A a,  --alpha=a\t"
	    << "use bound a on false negatives with sampling engine"
	    << std::endl
	    << "\t\t\t  (default is 1e-2)" << std::endl
	    << "  -B b,  --beta=b\t"
	    << "use bound b on false positives with sampling engine"
	    << std::endl
	    << "\t\t\t  (default is 1e-2)" << std::endl
            << "  -c c,  --const=c\t"
            << "overrides for model constants" << std::endl
            << "\t\t\t  (for example, --const=N=2,M=3)" << std::endl
	    << "  -D d,  --delta=d\t"
	    << "use indifference region of width 2*d with sampling"
	    << std::endl
	    << "\t\t\t  engine (default is 1e-2)" << std::endl
	    << "  -E e,  --epsilon=e\t"
	    << "use precision e with hybrid engine (default is 1e-6)"
	    << std::endl
	    << "  -e e,  --engine=e\t"
	    << "use engine e; can be `sampling' (default), `hybrid',"
	    << std::endl
	    << "\t\t\t  or `mixed'" << std::endl
	    << "  -H h,  --host=h\t"
	    << "connect to server on host h" << std::endl
            << "  -L l,  --max_path-length=l" << std::endl
            << "\t\t\tlimit sample path to l states" << std::endl
	    << "  -M,    --memoization\t"
	    << "use memoization for sampling engine" << std::endl
	    << "  -m m,  --matching-moments=m" << std::endl
	    << "\t\t\tmatch the first m moments of general distributions"
	    << std::endl
            << "  -N n,  --fixed-sample-size=n" << std::endl
            << "\t\t\tuse a fixed sample size" << std::endl
	    << "  -p,    --estimate-probabilities" << std::endl
	    << "\t\t\testimates probabilities of path formulae holding"
	    << std::endl
	    << "  -P p,  --port=p\t"
	    << "communicate using port p" << std::endl
	    << "  -s s,  --sampling-algorithm=s" << std::endl
	    << "\t\t\tuse sampling algorithm s"
	    << std::endl
	    << "  -S s,  --seed=s\t"
	    << "use seed s with random number generator" << std::endl
	    << "\t\t\t  (sampling engine only)" << std::endl
	    << "  -T t,  --trials=t\t"
	    << "number of trials for sampling engine (default is 1)"
	    << std::endl
	    << "\t\t\t  default level is 1 if optional argument is left out"
	    << std::endl
	    << "  -V,    --version\t"
	    << "display version information and exit" << std::endl
	    << "  -h,    --help\t\t"
	    << "display this help and exit" << std::endl
	    << "  file ...\t\t"
	    << "files containing models and properties;" << std::endl
	    << "\t\t\t  if none, descriptions are read from standard input"
	    << std::endl
	    << std::endl
	    << "Report bugs to <" PACKAGE_BUGREPORT ">." << std::endl;
}


/* Displays version information. */
void display_version() {
  std::cout << PACKAGE_STRING << std::endl
	    << "Copyright (C) 2003--2005 Carnegie Mellon University"
	    << std::endl
            << "Copyright (C) 2011--2012 Google Inc" << std::endl
	    << PACKAGE_NAME
	    << " comes with NO WARRANTY, to the extent permitted by law."
	    << std::endl
	    << "For information about the terms of redistribution,"
	    << std::endl
	    << "see the file named COPYING in the " PACKAGE_NAME
	    << " distribution." << std::endl
	    << std::endl
	    << "Written by Haakan Younes." << std::endl;
}


/* Parses spec for const overrides.  Returns true on success. */
bool parse_const_overrides(
    const std::string& spec,
    std::map<std::string, TypedValue>* const_overrides) {
  if (spec.empty()) {
    return true;
  }
  std::string::const_iterator comma = spec.begin() - 1;
  while (comma != spec.end()) {
    std::string::const_iterator next_comma = find(comma + 1, spec.end(), ',');
    std::string::const_iterator assignment = find(comma + 1, next_comma, '=');
    if (assignment == next_comma) {
      return false;
    }
    const std::string name(comma + 1, assignment);
    const std::string value(assignment + 1, next_comma);
    char* endptr;
    TypedValue v = static_cast<int>(strtol(value.c_str(), &endptr, 10));
    if (*endptr != '\0') {
      v = strtod(value.c_str(), &endptr);
    }
    if (endptr == value.c_str()) {
      return false;
    }
    if (!const_overrides->insert(std::make_pair(name, v)).second) {
      return false;
    }
    comma = next_comma;
  }
  return true;
}

/* Parses the given file, and returns true on success. */
bool read_file(const char* name) {
  yyin = fopen(name, "r");
  if (yyin == 0) {
    std::cerr << PACKAGE << ':' << name << ": " << strerror(errno)
	      << std::endl;
    return false;
  } else {
    current_file = name;
    bool success = (yyparse() == 0);
    fclose(yyin);
    return success;
  }
}


/* Extracts a path formula from the given state formula. */
bool extract_path_formula(const PathFormula*& pf, double& theta,
			  const StateFormula& f) {
  const Conjunction* cf = dynamic_cast<const Conjunction*>(&f);
  if (cf != 0) {
    for (const StateFormula* conjunct : cf->conjuncts()) {
      if (!extract_path_formula(pf, theta, *conjunct)) {
	return false;
      }
    }
    return true;
  }
  const Disjunction* df = dynamic_cast<const Disjunction*>(&f);
  if (df != 0) {
    for (const StateFormula* disjunct : df->disjuncts()) {
      if (!extract_path_formula(pf, theta, *disjunct)) {
	return false;
      }
    }
    return true;
  }
  const Negation* nf = dynamic_cast<const Negation*>(&f);
  if (nf != 0) {
    return extract_path_formula(pf, theta, nf->negand());
  }
  const Implication* wf = dynamic_cast<const Implication*>(&f);
  if (wf != 0) {
    return (extract_path_formula(pf, theta, wf->antecedent())
	    && extract_path_formula(pf, theta, wf->consequent()));
  }
  const Probabilistic* qf = dynamic_cast<const Probabilistic*>(&f);
  if (qf != 0) {
    if (pf == 0) {
      pf = &qf->formula();
      theta = qf->threshold().value<double>();
      return true;
    } else {
      pf = 0;
      return false;
    }
  }
  return true;
}

class ExpressionCompiler
    : public ExpressionVisitor, public StateFormulaVisitor {
 public:
  ExpressionCompiler(const std::map<std::string, int>* variables_by_name,
                     std::vector<std::string>* errors);

  std::vector<Operation> release_operations() { return std::move(operations_); }

  Type type() const { return type_; }

 private:
  virtual void DoVisitLiteral(const Literal& expr);
  virtual void DoVisitIdentifier(const Identifier& expr);
  virtual void DoVisitFunctionCall(const FunctionCall& expr);
  virtual void DoVisitUnaryOperation(const UnaryOperation& expr);
  virtual void DoVisitBinaryOperation(const BinaryOperation& expr);
  virtual void DoVisitConditional(const Conditional& expr);
  virtual void DoVisitConjunction(const Conjunction& formula);
  virtual void DoVisitDisjunction(const Disjunction& formula);
  virtual void DoVisitNegation(const Negation& formula);
  virtual void DoVisitImplication(const Implication& formula);
  virtual void DoVisitProbabilistic(const Probabilistic& formula);
  virtual void DoVisitComparison(const Comparison& formula);

  std::vector<Operation> operations_;
  int dst_;
  Type type_;
  const std::map<std::string, int>* variables_by_name_;
  std::vector<std::string>* errors_;
};

ExpressionCompiler::ExpressionCompiler(
    const std::map<std::string, int>* variables_by_name,
    std::vector<std::string>* errors)
    : dst_(0), variables_by_name_(variables_by_name), errors_(errors) {
  CHECK(variables_by_name);
  CHECK(errors);
}

void ExpressionCompiler::DoVisitLiteral(const Literal& expr) {
  const TypedValue& value = expr.value();
  if (value.type() == Type::DOUBLE) {
    operations_.push_back(Operation::MakeDCONST(value.value<double>(), dst_));
  } else {
    operations_.push_back(Operation::MakeICONST(value.value<int>(), dst_));
  }
  type_ = value.type();
}

void ExpressionCompiler::DoVisitIdentifier(const Identifier& expr) {
  auto i = variables_by_name_->find(expr.name());
  int variable;
  if (i == variables_by_name_->end()) {
    errors_->push_back(StrCat(
        "undefined variable '", expr.name(), "' in expression"));
    variable = -1;
  } else {
    variable = i->second;
  }
  operations_.push_back(Operation::MakeILOAD(variable, dst_));
  type_ = Type::INT;
}

void ExpressionCompiler::DoVisitFunctionCall(const FunctionCall& expr) {
  std::vector<Type> argument_types;
  for (const Expression& argument : expr.arguments()) {
    argument.Accept(this);
    ++dst_;
    argument_types.push_back(type_);
  }
  dst_ -= argument_types.size();
  switch (expr.function()) {
    case Function::UNKNOWN:
      errors_->push_back("unknown function call");
      break;
    case Function::MIN:
      if (argument_types.empty()) {
        errors_->push_back(StrCat(expr.function(), " applied to 0 arguments"));
      } else {
        type_ = argument_types[0];
        for (size_t i = 1; i < argument_types.size(); ++i) {
          if (type_ != argument_types[i]) {
            if (type_ == Type::BOOL || argument_types[i] == Type::BOOL) {
              errors_->push_back(StrCat("type mismatch; ", expr.function(),
                                        " applied to ", type_, " and ",
                                        argument_types[i]));
            }
            if (type_ != Type::DOUBLE && argument_types[i] == Type::DOUBLE) {
              operations_.push_back(Operation::MakeI2D(dst_));
              type_ = Type::DOUBLE;
            }
            if (argument_types[i] != Type::DOUBLE && type_ == Type::DOUBLE) {
              operations_.push_back(Operation::MakeI2D(dst_ + i));
            }
          }
          if (type_ == Type::DOUBLE) {
            operations_.push_back(Operation::MakeDMIN(dst_, dst_ + i));
          } else {
            operations_.push_back(Operation::MakeIMIN(dst_, dst_ + i));
          }
        }
      }
      break;
    case Function::MAX:
      if (argument_types.empty()) {
        errors_->push_back(StrCat(expr.function(), " applied to 0 arguments"));
      } else {
        type_ = argument_types[0];
        for (size_t i = 1; i < argument_types.size(); ++i) {
          if (type_ != argument_types[i]) {
            if (type_ == Type::BOOL || argument_types[i] == Type::BOOL) {
              errors_->push_back(StrCat("type mismatch; ", expr.function(),
                                        " applied to ", type_, " and ",
                                        argument_types[i]));
            }
            if (type_ != Type::DOUBLE && argument_types[i] == Type::DOUBLE) {
              operations_.push_back(Operation::MakeI2D(dst_));
              type_ = Type::DOUBLE;
            }
            if (argument_types[i] != Type::DOUBLE && type_ == Type::DOUBLE) {
              operations_.push_back(Operation::MakeI2D(dst_ + i));
            }
          }
          if (type_ == Type::DOUBLE) {
            operations_.push_back(Operation::MakeDMAX(dst_, dst_ + i));
          } else {
            operations_.push_back(Operation::MakeIMAX(dst_, dst_ + i));
          }
        }
      }
      break;
    case Function::FLOOR:
      if (argument_types.size() != 1) {
        errors_->push_back(StrCat(expr.function(), " applied to ",
                                  argument_types.size(), " arguments"));
      } else {
        if (argument_types[0] == Type::BOOL) {
          errors_->push_back(StrCat("type mismatch; ", expr.function(),
                                    " applied to ", Type::BOOL));
        }
        if (argument_types[0] != Type::DOUBLE) {
          operations_.push_back(Operation::MakeI2D(dst_));
        }
        operations_.push_back(Operation::MakeFLOOR(dst_));
        type_ = Type::INT;
      }
      break;
    case Function::CEIL:
      if (argument_types.size() != 1) {
        errors_->push_back(StrCat(expr.function(), " applied to ",
                                  argument_types.size(), " arguments"));
      } else {
        if (argument_types[0] == Type::BOOL) {
          errors_->push_back(StrCat("type mismatch; ", expr.function(),
                                    " applied to ", Type::BOOL));
        }
        if (argument_types[0] != Type::DOUBLE) {
          operations_.push_back(Operation::MakeI2D(dst_));
        }
        operations_.push_back(Operation::MakeCEIL(dst_));
        type_ = Type::INT;
      }
      break;
    case Function::POW:
      if (argument_types.size() != 2) {
        errors_->push_back(StrCat(expr.function(), " applied to ",
                                  argument_types.size(), " arguments"));
      } else {
        if (argument_types[0] == Type::BOOL
            || argument_types[1] == Type::BOOL) {
          errors_->push_back(StrCat("type mismatch; ", expr.function(),
                                    " applied to ", Type::BOOL));
        }
        if (argument_types[0] != Type::DOUBLE) {
          operations_.push_back(Operation::MakeI2D(dst_));
        }
        if (argument_types[1] != Type::DOUBLE) {
          operations_.push_back(Operation::MakeI2D(dst_ + 1));
        }
        operations_.push_back(Operation::MakePOW(dst_, dst_ + 1));
        type_ = Type::DOUBLE;
      }
      break;
    case Function::LOG:
      if (argument_types.size() != 2) {
        errors_->push_back(StrCat(expr.function(), " applied to ",
                                  argument_types.size(), " arguments"));
      } else {
        if (argument_types[0] == Type::BOOL
            || argument_types[1] == Type::BOOL) {
          errors_->push_back(StrCat("type mismatch; ", expr.function(),
                                    " applied to ", Type::BOOL));
        }
        if (argument_types[0] != Type::DOUBLE) {
          operations_.push_back(Operation::MakeI2D(dst_));
        }
        if (argument_types[1] != Type::DOUBLE) {
          operations_.push_back(Operation::MakeI2D(dst_ + 1));
        }
        operations_.push_back(Operation::MakeLOG(dst_, dst_ + 1));
        type_ = Type::DOUBLE;
      }
      break;
    case Function::MOD:
      if (argument_types.size() != 2) {
        errors_->push_back(StrCat(expr.function(), " applied to ",
                                  argument_types.size(), " arguments"));
      } else {
        if (argument_types[0] != Type::INT) {
          errors_->push_back(StrCat("type mismatch; ", expr.function(),
                                    " applied to ", argument_types[0]));
        }
        if (argument_types[1] != Type::INT) {
          errors_->push_back(StrCat("type mismatch; ", expr.function(),
                                    " applied to ", argument_types[1]));
        }
        operations_.push_back(Operation::MakeMOD(dst_, dst_ + 1));
        type_ = Type::INT;
      }
      break;
  }
}

void ExpressionCompiler::DoVisitUnaryOperation(const UnaryOperation& expr) {
  expr.operand().Accept(this);
  bool type_mismatch = false;
  switch (expr.op()) {
    case UnaryOperator::NEGATE:
      if (type_ == Type::DOUBLE) {
        operations_.push_back(Operation::MakeDNEG(dst_));
      } else if (type_ == Type::INT) {
        operations_.push_back(Operation::MakeINEG(dst_));
      } else {
        type_mismatch = true;
      }
      break;
    case UnaryOperator::NOT:
      if (type_ == Type::BOOL) {
        operations_.push_back(Operation::MakeNOT(dst_));
      } else {
        type_mismatch = true;
      }
      break;
  }
  if (type_mismatch) {
    errors_->push_back(StrCat("type mismatch; unary operator ", expr.op(),
                              " applied to ", type_));
  }
}

void ExpressionCompiler::DoVisitBinaryOperation(const BinaryOperation& expr) {
  expr.operand1().Accept(this);
  Type type1 = type_;
  ++dst_;
  expr.operand2().Accept(this);
  Type type2 = type_;
  --dst_;
  if (type1 == Type::BOOL || type2 == Type::BOOL) {
    errors_->push_back(StrCat("type mismatch; binary operator ", expr.op(),
                              " applied to ", Type::BOOL));
  }
  if (type1 != Type::DOUBLE &&
      (type2 == Type::DOUBLE || expr.op() == BinaryOperator::DIVIDE)) {
    operations_.push_back(Operation::MakeI2D(dst_));
    type1 = Type::DOUBLE;
  }
  if (type2 != Type::DOUBLE && type1 == Type::DOUBLE) {
    operations_.push_back(Operation::MakeI2D(dst_ + 1));
    type2 = Type::DOUBLE;
  }
  switch (expr.op()) {
    case BinaryOperator::PLUS:
      if (type1 == Type::DOUBLE) {
        operations_.push_back(Operation::MakeDADD(dst_, dst_ + 1));
      } else {
        operations_.push_back(Operation::MakeIADD(dst_, dst_ + 1));
      }
      break;
    case BinaryOperator::MINUS:
      if (type1 == Type::DOUBLE) {
        operations_.push_back(Operation::MakeDSUB(dst_, dst_ + 1));
      } else {
        operations_.push_back(Operation::MakeISUB(dst_, dst_ + 1));
      }
      break;
    case BinaryOperator::MULTIPLY:
      if (type1 == Type::DOUBLE) {
        operations_.push_back(Operation::MakeDMUL(dst_, dst_ + 1));
      } else {
        operations_.push_back(Operation::MakeIMUL(dst_, dst_ + 1));
      }
      break;
    case BinaryOperator::DIVIDE:
      operations_.push_back(Operation::MakeDDIV(dst_, dst_ + 1));
      break;
    default:
      // TODO(hlsyounes): implement.
      LOG(FATAL) << "not implemented";
      break;
  }
  type_ = type1;
}

void ExpressionCompiler::DoVisitConditional(const Conditional& expr) {
  expr.condition().Accept(this);
  int iffalse_pos = operations_.size();
  operations_.push_back(Operation::MakeNOP());  // placeholder for IFFALSE
  if (type_ != Type::BOOL) {
    errors_->push_back(StrCat("type mismatch; expecting condition of type ",
                              Type::BOOL, "; found ", type_));
  }
  ++dst_;
  expr.if_branch().Accept(this);
  Type if_type = type_;
  size_t if_end = operations_.size();
  ++dst_;
  expr.else_branch().Accept(this);
  Type else_type = type_;
  dst_ -= 2;
  if (!(if_type == else_type ||
        (if_type == Type::INT && else_type == Type::DOUBLE) ||
        (else_type == Type::INT && if_type == Type::DOUBLE))) {
    errors_->push_back(StrCat("incompatible types for condition branches; ",
                              if_type, " and ", else_type));
  }
  if (if_type != Type::DOUBLE && else_type == Type::DOUBLE) {
    operations_.insert(operations_.begin() + if_end,
                       Operation::MakeI2D(dst_ + 1));
    ++if_end;
    if_type = Type::DOUBLE;
  }
  if (else_type != Type::DOUBLE && if_type == Type::DOUBLE) {
    operations_.push_back(Operation::MakeI2D(dst_ + 2));
  }
  operations_.insert(operations_.begin() + if_end,
                     Operation::MakeGOTO(operations_.size()));
  operations_[iffalse_pos] = Operation::MakeIFFALSE(dst_, if_end + 1);
  type_ = if_type;
}

void ExpressionCompiler::DoVisitConjunction(const Conjunction& formula) {
  size_t n = formula.conjuncts().size();
  operations_.push_back(Operation::MakeICONST(1, dst_));
  for (size_t i = 0; i < n; ++i) {
    const StateFormula& conjunct = *formula.conjuncts()[i];
    size_t iffalse_pos = operations_.size();
    operations_.push_back(Operation::MakeNOP());  // placeholder for IFFALSE
    conjunct.Accept(this);
    if (type_ != Type::BOOL) {
      errors_->push_back(StrCat("type mismatch; binary operator & applied to ",
                                type_));
    }
    operations_[iffalse_pos] = Operation::MakeIFFALSE(dst_, operations_.size());
  }
  type_ = Type::BOOL;
}

void ExpressionCompiler::DoVisitDisjunction(const Disjunction& formula) {
  size_t n = formula.disjuncts().size();
  operations_.push_back(Operation::MakeICONST(0, dst_));
  for (size_t i = 0; i < n; ++i) {
    const StateFormula& disjunct = *formula.disjuncts()[i];
    size_t iftrue_pos = operations_.size();
    operations_.push_back(Operation::MakeNOP());  // placeholder for IFTRUE
    disjunct.Accept(this);
    if (type_ != Type::BOOL) {
      errors_->push_back(StrCat("type mismatch; binary operator | applied to ",
                                type_));
    }
    operations_[iftrue_pos] = Operation::MakeIFTRUE(dst_, operations_.size());
  }
  type_ = Type::BOOL;
}

void ExpressionCompiler::DoVisitNegation(const Negation& formula) {
  formula.negand().Accept(this);
  if (type_ != Type::BOOL) {
    errors_->push_back(StrCat("type mismatch; unary operator ! applied to ",
                              type_));
  }
  operations_.push_back(Operation::MakeNOT(dst_));
  type_ = Type::BOOL;
}

void ExpressionCompiler::DoVisitImplication(const Implication& formula) {
  formula.antecedent().Accept(this);
  if (type_ != Type::BOOL) {
    errors_->push_back(StrCat("type mismatch; binary operator => applied to ",
                              type_));
  }
  operations_.push_back(Operation::MakeNOT(dst_));
  size_t iftrue_pos = operations_.size();
  operations_.push_back(Operation::MakeNOP());  // placeholder for IFTRUE
  formula.consequent().Accept(this);
  if (type_ != Type::BOOL) {
    errors_->push_back(StrCat("type mismatch; binary operator => applied to ",
                              type_));
  }
  operations_[iftrue_pos] = Operation::MakeIFTRUE(dst_, operations_.size());
  type_ = Type::BOOL;
}

void ExpressionCompiler::DoVisitProbabilistic(const Probabilistic& formula) {
  LOG(FATAL) << "not an expression";
}

void ExpressionCompiler::DoVisitComparison(const Comparison& formula) {
  formula.expr1().Accept(this);
  Type type1 = type_;
  ++dst_;
  formula.expr2().Accept(this);
  Type type2 = type_;
  --dst_;
  if (!(type1 == type2 ||
        (type1 == Type::INT && type2 == Type::DOUBLE) ||
        (type2 == Type::INT && type1 == Type::DOUBLE))) {
    errors_->push_back(StrCat("incompatible types for binary operator ",
                              formula.op(), "; ", type1, " and ", type2));
  }
  if (type1 != Type::DOUBLE && type2 == Type::DOUBLE) {
    operations_.push_back(Operation::MakeI2D(dst_));
    type1 = Type::DOUBLE;
  }
  if (type2 != Type::DOUBLE && type1 == Type::DOUBLE) {
    operations_.push_back(Operation::MakeI2D(dst_ + 1));
    type2 = Type::DOUBLE;
  }
  switch (formula.op()) {
    case Comparison::LESS:
      if (type1 == Type::DOUBLE) {
        operations_.push_back(Operation::MakeDLT(dst_, dst_ + 1));
      } else {
        operations_.push_back(Operation::MakeILT(dst_, dst_ + 1));
      }
      break;
    case Comparison::LESS_EQUAL:
      if (type1 == Type::DOUBLE) {
        operations_.push_back(Operation::MakeDLE(dst_, dst_ + 1));
      } else {
        operations_.push_back(Operation::MakeILE(dst_, dst_ + 1));
      }
      break;
    case Comparison::GREATER_EQUAL:
      if (type1 == Type::DOUBLE) {
        operations_.push_back(Operation::MakeDGE(dst_, dst_ + 1));
      } else {
        operations_.push_back(Operation::MakeIGE(dst_, dst_ + 1));
      }
      break;
    case Comparison::GREATER:
      if (type1 == Type::DOUBLE) {
        operations_.push_back(Operation::MakeDGT(dst_, dst_ + 1));
      } else {
        operations_.push_back(Operation::MakeIGT(dst_, dst_ + 1));
      }
      break;
    case Comparison::EQUAL:
      if (type1 == Type::DOUBLE) {
        operations_.push_back(Operation::MakeDEQ(dst_, dst_ + 1));
      } else {
        operations_.push_back(Operation::MakeIEQ(dst_, dst_ + 1));
      }
      break;
    case Comparison::NOT_EQUAL:
      if (type1 == Type::DOUBLE) {
        operations_.push_back(Operation::MakeDNE(dst_, dst_ + 1));
      } else {
        operations_.push_back(Operation::MakeINE(dst_, dst_ + 1));
      }
      break;
  }
  type_ = Type::BOOL;
}

CompiledExpression CompileAndOptimizeExpression(
    const Expression& expr, Type expected_type,
    const std::map<std::string, int>& variables_by_name,
    std::vector<std::string>* errors) {
  ExpressionCompiler compiler(&variables_by_name, errors);
  expr.Accept(&compiler);
  std::vector<Operation> operations = compiler.release_operations();
  Type type = compiler.type();
  if (type == Type::INT && expected_type == Type::DOUBLE) {
    operations.push_back(Operation::MakeI2D(0));
    type = Type::DOUBLE;
  }
  if (type != expected_type) {
    errors->push_back(StrCat("type mismatch; expecting expression of type ",
                             expected_type, "; found ", type));
    return CompiledExpression({});
  }
  if (expected_type == Type::DOUBLE) {
    return OptimizeDoubleExpression(CompiledExpression(operations));
  } else {
    return OptimizeIntExpression(CompiledExpression(operations));
  }
}

CompiledExpression CompileExpression(
    const StateFormula& expr,
    const std::map<std::string, int>& variables_by_name,
    std::vector<std::string>* errors) {
  ExpressionCompiler compiler(&variables_by_name, errors);
  expr.Accept(&compiler);
  if (compiler.type() != Type::BOOL) {
    errors->push_back(StrCat("type mismatch; expecting expression of type ",
                             Type::BOOL, "; found ", compiler.type()));
    return CompiledExpression({});
  }
  return CompiledExpression(compiler.release_operations());
}

CompiledExpression CompileAndOptimizeExpression(
    const StateFormula& expr,
    const std::map<std::string, int>& variables_by_name,
    std::vector<std::string>* errors) {
  return OptimizeIntExpression(
      CompileExpression(expr, variables_by_name, errors));
}

class DistributionCompiler : public DistributionVisitor {
 public:
  DistributionCompiler(const std::map<std::string, int>* variables_by_name,
                       std::vector<std::string>* errors);

  CompiledDistribution release_dist() { return std::move(dist_); }

 private:
  virtual void DoVisitExponential(const Exponential& dist);
  virtual void DoVisitWeibull(const Weibull& dist);
  virtual void DoVisitLognormal(const Lognormal& dist);
  virtual void DoVisitUniform(const Uniform& dist);

  CompiledDistribution dist_;
  const std::map<std::string, int>* variables_by_name_;
  std::vector<std::string>* errors_;
};

DistributionCompiler::DistributionCompiler(
    const std::map<std::string, int>* variables_by_name,
    std::vector<std::string>* errors)
    : dist_(CompiledDistribution::MakeMemoryless(CompiledExpression({}))),
      variables_by_name_(variables_by_name), errors_(errors) {
  CHECK(variables_by_name);
  CHECK(errors);
}

void DistributionCompiler::DoVisitExponential(const Exponential& dist) {
  dist_ = CompiledDistribution::MakeMemoryless(
      CompileAndOptimizeExpression(
          dist.rate(), Type::DOUBLE, *variables_by_name_, errors_));
}

void DistributionCompiler::DoVisitWeibull(const Weibull& dist) {
  dist_ = CompiledDistribution::MakeWeibull(
      CompileAndOptimizeExpression(
          dist.scale(), Type::DOUBLE, *variables_by_name_, errors_),
      CompileAndOptimizeExpression(
          dist.shape(), Type::DOUBLE, *variables_by_name_, errors_));
}

void DistributionCompiler::DoVisitLognormal(const Lognormal& dist) {
  dist_ = CompiledDistribution::MakeLognormal(
      CompileAndOptimizeExpression(
          dist.scale(), Type::DOUBLE, *variables_by_name_, errors_),
      CompileAndOptimizeExpression(
          dist.shape(), Type::DOUBLE, *variables_by_name_, errors_));
}

void DistributionCompiler::DoVisitUniform(const Uniform& dist) {
  dist_ = CompiledDistribution::MakeUniform(
      CompileAndOptimizeExpression(
          dist.low(), Type::DOUBLE, *variables_by_name_, errors_),
      CompileAndOptimizeExpression(
          dist.high(), Type::DOUBLE, *variables_by_name_, errors_));
}

CompiledDistribution CompileDistribution(
    const Distribution& dist,
    const std::map<std::string, int>& variables_by_name,
    std::vector<std::string>* errors) {
  DistributionCompiler compiler(&variables_by_name, errors);
  dist.Accept(&compiler);
  return compiler.release_dist();
}

CompiledUpdate CompileUpdate(
    const Update& update,
    const std::map<std::string, int>& variables_by_name,
    std::vector<std::string>* errors) {
  auto i = variables_by_name.find(update.variable());
  int variable;
  if (i == variables_by_name.end()) {
    errors->push_back(StrCat(
        "undefined variable '", update.variable(), "' in update"));
    variable = -1;
  } else {
    variable = i->second;
  }
  return CompiledUpdate(
      variable,
      CompileAndOptimizeExpression(
          update.expr(), Type::INT, variables_by_name, errors));
}

std::vector<CompiledUpdate> CompileUpdates(
    const std::vector<const Update*>& updates,
    const std::map<std::string, int>& variables_by_name,
    std::vector<std::string>* errors) {
  std::vector<CompiledUpdate> compiled_updates;
  for (const Update* update : updates) {
    compiled_updates.push_back(
        CompileUpdate(*update, variables_by_name, errors));
  }
  return compiled_updates;
}

CompiledOutcome CompileOutcome(
    const Distribution& delay,
    const std::vector<const Update*>& updates,
    const std::map<std::string, int>& variables_by_name,
    std::vector<std::string>* errors) {
  return CompiledOutcome(CompileDistribution(delay, variables_by_name, errors),
                         CompileUpdates(updates, variables_by_name, errors));
}

CompiledCommand CompileCommand(
    const Command& command,
    const std::map<std::string, int>& variables_by_name,
    std::vector<std::string>* errors) {
  return CompiledCommand(
      CompileAndOptimizeExpression(command.guard(), variables_by_name, errors),
      { CompileOutcome(command.delay(), command.updates(),
                       variables_by_name, errors) });
}

CompiledModel CompileModel(const Model& model,
                           std::vector<std::string>* errors) {
  CompiledModel compiled_model;

  std::map<std::string, int> variables_by_name;
  for (const ParsedVariable& v : model.variables()) {
    compiled_model.AddVariable(
        v.name(), v.min_value(), v.max_value(), v.init_value());
    variables_by_name.insert({ v.name(), variables_by_name.size() });
  }

  for (const Command* c : model.commands()) {
    compiled_model.AddCommand(CompileCommand(*c, variables_by_name, errors));
  }

  return compiled_model;
}

class PropertyCompiler : public StateFormulaVisitor {
 public:
  PropertyCompiler(const std::map<std::string, int>* variables_by_name,
                   int next_path_property_index,
                   std::vector<std::string>* errors);

  std::unique_ptr<const CompiledProperty> release_property() {
    return std::move(property_);
  }

  int next_path_property_index() const { return next_path_property_index_; }

 private:
  virtual void DoVisitConjunction(const Conjunction& formula);
  virtual void DoVisitDisjunction(const Disjunction& formula);
  virtual void DoVisitNegation(const Negation& formula);
  virtual void DoVisitImplication(const Implication& formula);
  virtual void DoVisitProbabilistic(const Probabilistic& formula);
  virtual void DoVisitComparison(const Comparison& formula);

  std::unique_ptr<const CompiledProperty> property_;
  const std::map<std::string, int>* variables_by_name_;
  int next_path_property_index_;
  std::vector<std::string>* errors_;
};

std::unique_ptr<const CompiledProperty> CompileProperty(
    const StateFormula& property,
    const std::map<std::string, int>& variables_by_name,
    int* next_path_property_index,
    std::vector<std::string>* errors) {
  PropertyCompiler compiler(&variables_by_name, *next_path_property_index,
                            errors);
  property.Accept(&compiler);
  *next_path_property_index = compiler.next_path_property_index();
  return compiler.release_property();
}

class PathPropertyCompiler : public PathFormulaVisitor {
 public:
  PathPropertyCompiler(const std::map<std::string, int>* variables_by_name,
                       int next_index, std::vector<std::string>* errors);

  std::unique_ptr<const CompiledPathProperty> release_path_property() {
    return std::move(path_property_);
  }

  int next_index() const { return next_index_; }

 private:
  virtual void DoVisitUntil(const Until& formula);

  std::unique_ptr<const CompiledPathProperty> path_property_;
  const std::map<std::string, int>* variables_by_name_;
  int next_index_;
  std::vector<std::string>* errors_;
};

std::unique_ptr<const CompiledPathProperty> CompilePathProperty(
    const PathFormula& path_property,
    const std::map<std::string, int> variables_by_name, int* next_index,
    std::vector<std::string>* errors) {
  PathPropertyCompiler compiler(&variables_by_name, *next_index, errors);
  path_property.Accept(&compiler);
  *next_index = compiler.next_index();
  return compiler.release_path_property();
}

PropertyCompiler::PropertyCompiler(
    const std::map<std::string, int>* variables_by_name,
    int next_path_property_index,
    std::vector<std::string>* errors)
    : variables_by_name_(variables_by_name),
      next_path_property_index_(next_path_property_index), errors_(errors) {
  CHECK(variables_by_name);
  CHECK(errors);
}

void PropertyCompiler::DoVisitConjunction(const Conjunction& formula) {
  size_t n = formula.conjuncts().size();
  UniquePtrVector<const CompiledProperty> operands;
  for (size_t i = 0; i < n; ++i) {
    const StateFormula& operand = *formula.conjuncts()[i];
    operand.Accept(this);
    operands.push_back(std::move(property_));
  }
  if (operands.empty()) {
    property_ = CompiledExpressionProperty::New(CompiledExpression(
        CompiledExpression({Operation::MakeICONST(1, 0)})));
  } else {
    property_ = CompiledAndProperty::New(CompiledExpression({}),
                                         std::move(operands));
  }
}

void PropertyCompiler::DoVisitDisjunction(const Disjunction& formula) {
  size_t n = formula.disjuncts().size();
  UniquePtrVector<const CompiledProperty> operands;
  for (size_t i = 0; i < n; ++i) {
    const StateFormula& operand = *formula.disjuncts()[i];
    operand.Accept(this);
    operands.push_back(CompiledNotProperty::New(std::move(property_)));
  }
  property_ = CompiledNotProperty::New(CompiledAndProperty::New(
      CompiledExpression({}), std::move(operands)));
}

void PropertyCompiler::DoVisitNegation(const Negation& formula) {
  formula.negand().Accept(this);
  property_ = CompiledNotProperty::New(std::move(property_));
}

void PropertyCompiler::DoVisitImplication(const Implication& formula) {
  UniquePtrVector<const CompiledProperty> operands;
  formula.antecedent().Accept(this);
  operands.push_back(std::move(property_));
  formula.consequent().Accept(this);
  operands.push_back(CompiledNotProperty::New(std::move(property_)));
  property_ = CompiledNotProperty::New(CompiledAndProperty::New(
      CompiledExpression({}), std::move(operands)));
}

void PropertyCompiler::DoVisitProbabilistic(const Probabilistic& formula) {
  std::unique_ptr<const CompiledPathProperty> path_property =
      CompilePathProperty(formula.formula(), *variables_by_name_,
                          &next_path_property_index_, errors_);
  if (!formula.strict()) {
    property_ = CompiledProbabilisticProperty::New(
        CompiledProbabilisticOperator::GREATER_EQUAL,
        formula.threshold().value<double>(), std::move(path_property));
  } else {
    property_ = CompiledProbabilisticProperty::New(
        CompiledProbabilisticOperator::GREATER,
        formula.threshold().value<double>(), std::move(path_property));
  }
}

void PropertyCompiler::DoVisitComparison(const Comparison& formula) {
  CompiledExpression compiled_expr =
      CompileExpression(formula, *variables_by_name_, errors_);
  property_ = CompiledExpressionProperty::New(compiled_expr);
}

PathPropertyCompiler::PathPropertyCompiler(
    const std::map<std::string, int>* variables_by_name, int next_index,
    std::vector<std::string>* errors)
    : variables_by_name_(variables_by_name), next_index_(next_index),
      errors_(errors) {
  CHECK(variables_by_name);
  CHECK(errors);
}

void PathPropertyCompiler::DoVisitUntil(const Until& formula) {
  int index = next_index_;
  ++next_index_;
  std::unique_ptr<const CompiledProperty> pre = CompileProperty(
      formula.pre(), *variables_by_name_, &next_index_, errors_);
  std::unique_ptr<const CompiledProperty> post = CompileProperty(
      formula.post(), *variables_by_name_, &next_index_, errors_);
  path_property_ = CompiledUntilProperty::New(
      formula.min_time().value<double>(), formula.max_time().value<double>(),
      std::move(pre), std::move(post), index, StrCat(formula), &formula);
}

std::unique_ptr<const CompiledProperty> CompileProperty(
    const StateFormula& property,
    const CompiledModel& model,
    std::vector<std::string>* errors) {
  std::map<std::string, int> variables_by_name;
  for (const CompiledVariable& v : model.variables()) {
    variables_by_name.insert({ v.name(), variables_by_name.size() });
  }
  int next_path_property_index = 0;
  return CompileProperty(property, variables_by_name, &next_path_property_index,
                         errors);
}

std::unique_ptr<const CompiledProperty> CompileAndOptimizeProperty(
    const StateFormula& property,
    const CompiledModel& model,
    std::vector<std::string>* errors) {
  return OptimizeProperty(*CompileProperty(property, model, errors));
}

std::unique_ptr<const CompiledPathProperty> CompilePathProperty(
    const PathFormula& property,
    const CompiledModel& model,
    std::vector<std::string>* errors) {
  std::map<std::string, int> variables_by_name;
  for (const CompiledVariable& v : model.variables()) {
    variables_by_name.insert({ v.name(), variables_by_name.size() });
  }
  int next_index = 0;
  return CompilePathProperty(property, variables_by_name,
                             &next_index, errors);
}

std::unique_ptr<const CompiledPathProperty> OptimizeAndCompilePathProperty(
    const PathFormula& property,
    const CompiledModel& model,
    std::vector<std::string>* errors) {
  return OptimizePathProperty(*CompilePathProperty(property, model, errors));
}

}  // namespace

/* The main program. */
int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);

  ModelCheckingParams params;
  /* Verification without estimation by default. */
  bool estimate = false;
  /* Set default engine. */
  enum { SAMPLING_ENGINE, HYBRID_ENGINE, MIXED_ENGINE } engine =
							  SAMPLING_ENGINE;
  /* Number of moments to match. */
  size_t moments = 3;
  /* Set default seed. */
  size_t seed = time(0);
  /* Set default number of trials. */
  size_t trials = 1;
  /* Sever hostname */
  std::string hostname;
  /* Server port. */
  int port = -1;

  try {
    /*
     * Get command line options.
     */
    while (1) {
      int option_index = 0;
#if HAVE_GETOPT_LONG
      int c = getopt_long(argc, argv, OPTION_STRING,
			  long_options, &option_index);
#else
      int c = getopt(argc, argv, OPTION_STRING);
#endif
      if (c == -1) {
	break;
      }
      switch (c) {
      case 'A':
	params.alpha = atof(optarg);
	if (params.alpha < 1e-10) {
	  throw std::invalid_argument("alpha < 1e-10");
	} else if (params.alpha >= 0.5) {
	  throw std::invalid_argument("alpha >= 0.5");
	}
	break;
      case 'B':
	params.beta = atof(optarg);
	if (params.beta < 1e-10) {
	  throw std::invalid_argument("beta < 1e-10");
	} else if (params.beta >= 0.5) {
	  throw std::invalid_argument("beta >= 0.5");
	}
	break;
      case 'c':
        if (!parse_const_overrides(optarg, &const_overrides)) {
          throw std::invalid_argument("bad --const specification");
        }
        break;
      case 'D':
	params.delta = atof(optarg);
	if (params.delta < 1e-10) {
	  throw std::invalid_argument("delta < 1e-10");
	} else if (params.delta > 0.5) {
	  throw std::invalid_argument("delta > 0.5");
	}
	break;
      case 'E':
	params.epsilon = atof(optarg);
	if (params.epsilon < 1e-10) {
	  throw std::invalid_argument("epsilon < 1e-10");
	} else if (params.epsilon > 1.0) {
	  throw std::invalid_argument("epsilon > 1.0");
	}
	break;
      case 'e':
	if (strcasecmp(optarg, "sampling") == 0) {
	  engine = SAMPLING_ENGINE;
	} else if (strcasecmp(optarg, "hybrid") == 0) {
	  engine = HYBRID_ENGINE;
	} else if (strcasecmp(optarg, "mixed") == 0) {
	  engine = MIXED_ENGINE;
	} else {
	  throw std::invalid_argument("unsupported engine `"
				      + std::string(optarg) + "'");
	}
	break;
      case 'H':
	hostname = optarg;
	break;
      case 'L':
        params.max_path_length = atoi(optarg);
        break;
      case 'M':
	params.memoization = true;
	break;
      case 'm':
	moments = atoi(optarg);
	if (moments < 1) {
	  throw std::invalid_argument("must match at least one moment");
	} else if (moments > 3) {
	  throw std::invalid_argument("cannot match more than three moments");
	}
	break;
      case 'N':
	params.algorithm = FIXED;
        params.fixed_sample_size = atoi(optarg);
	break;
      case 'n':
	params.nested_error = atof(optarg);
	break;
      case 'p':
	estimate = true;
	params.algorithm = ESTIMATE;
	break;
      case 'P':
	port = atoi(optarg);
	if (port < 0 || port > 0xffff) {
	  throw std::invalid_argument("invalid port number");
	}
	break;
      case 's':
	if (strcasecmp(optarg, "ssp") == 0) {
	  params.algorithm = SSP;
	} else if (strcasecmp(optarg, "sprt") == 0) {
	  params.algorithm = SPRT;
	} else {
	  throw std::invalid_argument("unsupported sampling algorithm `"
				      + std::string(optarg) + "'");
	}
	break;
      case 'S':
	seed = atoi(optarg);
	break;
      case 'T':
	trials = atoi(optarg);
	break;
      case 'V':
	display_version();
	return 0;
      case 'h':
        display_help();
        return 0;
      case ':':
      default:
	std::cerr << "Try `" PACKAGE " --help' for more information."
		  << std::endl;
	return 1;
      }
    }
    if (params.nested_error > 0) {
      CHECK_LT(params.nested_error, MaxNestedError(params.delta));
    }

    /*
     * Read files.
     */
    if (optind < argc) {
      /*
       * Use remaining command line arguments as file names.
       */
      while (optind < argc) {
	if (!read_file(argv[optind++])) {
	  return 1;
	}
      }
    } else {
      /*
       * No remaining command line argument, so read from standard input.
       */
      yyin = stdin;
      if (yyparse() != 0) {
	return 1;
      }
    }
    clear_declarations();
    if (global_model == 0) {
      std::cout << "no model" << std::endl;
      return 0;
    }
    VLOG(2) << *global_model;
    std::vector<std::string> errors;
    const CompiledModel compiled_model = CompileModel(*global_model, &errors);
    if (!errors.empty()) {
      for (const std::string& error : errors) {
        std::cerr << PACKAGE << ": " << error << std::endl;
      }
      return 0;
    }

    std::cout.setf(std::ios::unitbuf);
    if (port >= 0) {
      sockaddr_in srvaddr;
      srvaddr.sin_family = AF_INET;
      srvaddr.sin_port = htons(port);
      if (!hostname.empty()) {
	/* Client mode. */
	hostent* host = gethostbyname(hostname.c_str());
	if (host == 0) {
	  herror(PACKAGE);
	  return 1;
	}
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
	  perror(PACKAGE);
	  return 1;
	}
	srvaddr.sin_addr = *(in_addr*) host->h_addr;
	if (-1 == connect(sockfd, (sockaddr*) &srvaddr, sizeof srvaddr)) {
	  perror(PACKAGE);
	  return 1;
	}
	ServerMsg smsg;
	int nbytes = recv(sockfd, &smsg, sizeof smsg, 0);
	if (nbytes == -1) {
	  perror(PACKAGE);
	  return 1;
	} else if (nbytes == 0) {
	  VLOG(1) << "Shutting down (server unavailable)";
	  return 0;
	}
	if (smsg.id != ServerMsg::REGISTER) {
	  throw std::logic_error("expecting register message");
	}
	int client_id = smsg.value;
        std::cout << "Client " << client_id << std::endl;
        std::cout << "Initializing random number generator...";
        DCEngine dc_engine(client_id, seed);
        dc_engine.seed(seed);
        std::cout << "done" << std::endl;
	fd_set master_fds;
	FD_ZERO(&master_fds);
	FD_SET(sockfd, &master_fds);
        std::pair<int, int> num_regs = compiled_model.GetNumRegisters();
        CompiledExpressionEvaluator evaluator(num_regs.first, num_regs.second);
        CompiledDistributionSampler<DCEngine> sampler(&evaluator, &dc_engine);
	const State init_state(&compiled_model, &evaluator, &sampler);
	const PathFormula* pf = 0;
        std::unique_ptr<const CompiledPathProperty> property;
        ModelCheckingParams nested_params = params;
	timeval timeout;
	timeval* to = 0;
	while (true) {
	  if (to != 0) {
	    to->tv_sec = 0;
	    to->tv_usec = 0;
	  }
	  fd_set read_fds = master_fds;
	  int result = select(sockfd + 1, &read_fds, 0, 0, to);
	  if (result == -1) {
	    perror(PACKAGE);
	    exit(-1);
	  } else if (result == 0) {
	    if (pf != 0) {
	      ClientMsg msg = { ClientMsg::SAMPLE };
              ModelCheckingStats stats;
	      msg.value = GetObservation(*property, *global_model, nullptr,
                                         nested_params, &evaluator, init_state,
                                         &stats);
	      VLOG(2) << "Sending sample " << msg.value;
	      nbytes = send(sockfd, &msg, sizeof msg, 0);
	      if (nbytes == -1) {
		perror(PACKAGE);
		return 1;
	      } else if (nbytes == 0) {
		VLOG(1) << "Shutting down (server unavailable)";
		return 0;
	      }
	    }
	  } else {
	    nbytes = recv(sockfd, &smsg, sizeof smsg, 0);
	    if (nbytes == -1) {
	      perror(PACKAGE);
	      return 1;
	    } else if (nbytes == 0) {
	      VLOG(1) << "Shutting down (server unavailable)";
	      return 0;
	    }
	    if (smsg.id == ServerMsg::START) {
	      double theta;
	      if (!extract_path_formula(pf, theta, *properties[smsg.value])) {
		std::cerr << PACKAGE << ": multiple path formulae"
			  << std::endl;
		return 1;
	      }
	      if (pf != 0) {
                // Since we currently do not support distributed sampling for
                // properties with nested probabilistic operators, we can just
                // set the nested error bounds to 0.
                nested_params.alpha = 0;
                nested_params.beta = 0;
                property = OptimizeAndCompilePathProperty(
                    *pf, compiled_model, &errors);
	      }
	      to = &timeout;
	      VLOG(1) << "Sampling started for property " << smsg.value;
	    } else if (smsg.id == ServerMsg::STOP) {
	      to = 0;
	      VLOG(1) << "Sampling stopped.";
	    } else {
	      std::cerr << "Message with bad id (" << smsg.id << ") ignored."
			<< std::endl;
	    }
	  }
	}
      } else {
	/* Server mode. */
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1) {
	  perror(PACKAGE);
	  return 1;
	}
	int yes = 1;
	if (-1 == setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes,
			     sizeof yes)) {
	  perror(PACKAGE);
	  return 1;
	}
	srvaddr.sin_addr.s_addr = INADDR_ANY;
	if (-1 == bind(server_socket, (sockaddr*) &srvaddr, sizeof srvaddr)) {
	  perror(PACKAGE);
	  return 1;
	}
	if (-1 == listen(server_socket, 100)) {
	  perror(PACKAGE);
	  return 1;
	}
	VLOG(1) << "Server at port " << port;
      }
    }

    if (engine == SAMPLING_ENGINE) {
      DCEngine dc_engine;
      dc_engine.seed(seed);
      std::cout << "Sampling engine: alpha=" << params.alpha
                << ", beta=" << params.beta << ", delta=" << params.delta
                << ", seed=" << seed << std::endl;
      itimerval timer = { { 0L, 0L }, { 40000000L, 0L } };
      itimerval stimer;
#ifdef PROFILING
      setitimer(ITIMER_VIRTUAL, &timer, 0);
      getitimer(ITIMER_VIRTUAL, &stimer);
#else
      setitimer(ITIMER_PROF, &timer, 0);
      getitimer(ITIMER_PROF, &stimer);
#endif
      std::pair<int, int> num_regs = compiled_model.GetNumRegisters();
      CompiledExpressionEvaluator evaluator(num_regs.first, num_regs.second);
      CompiledDistributionSampler<DCEngine> sampler(&evaluator, &dc_engine);
      const State init_state(&compiled_model, &evaluator, &sampler);
#ifdef PROFILING
      getitimer(ITIMER_VIRTUAL, &timer);
#else
      getitimer(ITIMER_PROF, &timer);
#endif
      long sec = stimer.it_value.tv_sec - timer.it_value.tv_sec;
      long usec = stimer.it_value.tv_usec - timer.it_value.tv_usec;
      double t = std::max(0.0, sec + usec*1e-6);
      std::cout << "Model built in " << t << " seconds." << std::endl;
      std::cout << "Variables: " << init_state.values().size() << std::endl;
      std::cout << "Events:    " << global_model->commands().size()
                << std::endl;
      for (auto fi = properties.begin(); fi != properties.end(); fi++) {
	std::cout << std::endl << "Model checking " << **fi << " ..."
		  << std::endl;
        std::unique_ptr<const CompiledProperty> property =
            CompileAndOptimizeProperty(**fi, compiled_model, &errors);
	current_property = fi - properties.begin();
	size_t accepts = 0;
        ModelCheckingStats stats;
	for (size_t i = 0; i < trials; i++) {
	  timeval start_time;
	  itimerval timer = { { 0, 0 }, { 40000000L, 0 } };
	  itimerval stimer;
	  if (server_socket != -1) {
	    gettimeofday(&start_time, 0);
	  } else {
#ifdef PROFILING
	    setitimer(ITIMER_VIRTUAL, &timer, 0);
	    getitimer(ITIMER_VIRTUAL, &stimer);
#else
	    setitimer(ITIMER_PROF, &timer, 0);
	    getitimer(ITIMER_PROF, &stimer);
#endif
	  }
	  bool sol = Verify(*property, *global_model, nullptr, params,
                            &evaluator, init_state, &stats);
	  double t;
	  if (server_socket != -1) {
	    timeval end_time;
	    gettimeofday(&end_time, 0);
	    long sec = end_time.tv_sec - start_time.tv_sec;
	    long usec = end_time.tv_usec - start_time.tv_usec;
	    if (usec < 0) {
	      sec--;
	      usec = 1000000 + usec;
	    }
	    t = std::max(0.0, sec + usec*1e-6);
	  } else {
#ifdef PROFILING
	    getitimer(ITIMER_VIRTUAL, &timer);
#else
	    getitimer(ITIMER_PROF, &timer);
#endif
	    long sec = stimer.it_value.tv_sec - timer.it_value.tv_sec;
	    long usec = stimer.it_value.tv_usec - timer.it_value.tv_usec;
	    t = std::max(0.0, sec + usec*1e-6);
	  }
	  if (trials == 1) {
	    std::cout << "Model checking completed in " << t << " seconds."
		      << std::endl;
	    if (sol) {
	      std::cout << "Property is true in the initial state."
			<< std::endl;
	    } else {
	      std::cout << "Property is false in the initial state."
			<< std::endl;
	    }
	  } else {
	    if (sol) {
	      accepts++;
	    }
            stats.time.AddObservation(t);
	  }
	}
	if (trials > 1) {
	  std::cout << "Model checking time mean: " << stats.time.mean()
		    << " seconds" << std::endl
                    << "Model checking time min: " << stats.time.min()
                    << " seconds" << std::endl
                    << "Model checking time max: " << stats.time.max()
                    << " seconds" << std::endl
		    << "Model checking time std.dev.: "
                    << stats.time.sample_stddev() << std::endl
		    << "Sample size mean: " << stats.sample_size.mean()
                    << std::endl
                    << "Sample size min: " << stats.sample_size.min()
                    << std::endl
                    << "Sample size max: " << stats.sample_size.max()
                    << std::endl
                    << "Sample size std.dev.: "
                    << stats.sample_size.sample_stddev() << std::endl
		    << "Path length mean: " << stats.path_length.mean()
                    << std::endl
		    << "Path length min: " << stats.path_length.min()
                    << std::endl
		    << "Path length max: " << stats.path_length.max()
                    << std::endl
                    << "Path length std.dev.: "
                    << stats.path_length.sample_stddev() << std::endl
		    << accepts << " accepted, " << (trials - accepts)
		    << " rejected" << std::endl
		    << "Average cached: " << stats.sample_cache_size.mean()
                    << std::endl;
	}
      }
    } else if (engine == HYBRID_ENGINE) {
      std::cout << "Hybrid engine: epsilon=" << params.epsilon << std::endl;
      DecisionDiagramManager dd_man(2*compiled_model.NumBits());
      itimerval timer = { { 0L, 0L }, { 40000000L, 0L } };
      itimerval stimer;
#ifdef PROFILING
      setitimer(ITIMER_VIRTUAL, &timer, 0);
      getitimer(ITIMER_VIRTUAL, &stimer);
#else
      setitimer(ITIMER_PROF, &timer, 0);
      getitimer(ITIMER_VIRTUAL, &stimer);
#endif
      DecisionDiagramModel dd_model = DecisionDiagramModel::Create(
          &dd_man, moments, *global_model);
#ifdef PROFILING
      getitimer(ITIMER_VIRTUAL, &timer);
#else
      getitimer(ITIMER_PROF, &timer);
#endif
      long sec = stimer.it_value.tv_sec - timer.it_value.tv_sec;
      long usec = stimer.it_value.tv_usec - timer.it_value.tv_usec;
      double t = std::max(0.0, sec + usec*1e-6);
      std::cout << "Model built in " << t << " seconds." << std::endl;
      std::cout << "States:      "
                << dd_model.reachable_states().MintermCount(
                    dd_man.GetNumVariables() / 2)
                << std::endl
                << "Transitions: "
                << dd_model.rate_matrix().MintermCount(dd_man.GetNumVariables())
                << std::endl;
      std::cout << "Rate matrix";
      Cudd_PrintDebug(dd_man.manager(), dd_model.rate_matrix().get(),
                      dd_man.GetNumVariables(), 1);
      std::cout << "ODD:         " << get_num_odd_nodes() << " nodes"
                << std::endl;
      for (auto fi = properties.begin(); fi != properties.end(); fi++) {
	std::cout << std::endl << "Model checking " << **fi << " ..."
		  << std::endl;
	double total_time = 0.0;
	bool accepted = false;
	for (size_t i = 0; i < trials; i++) {
	  itimerval timer = { { 0L, 0L }, { 40000000L, 0L } };
	  itimerval stimer;
#ifdef PROFILING
	  setitimer(ITIMER_VIRTUAL, &timer, 0);
	  getitimer(ITIMER_VIRTUAL, &stimer);
#else
	  setitimer(ITIMER_PROF, &timer, 0);
	  getitimer(ITIMER_PROF, &stimer);
#endif
	  BDD ddf = Verify(**fi, dd_model, estimate, true, params.epsilon);
	  BDD sol = ddf && dd_model.initial_state();
#ifdef PROFILING
	  getitimer(ITIMER_VIRTUAL, &timer);
#else
	  getitimer(ITIMER_PROF, &timer);
#endif
	  long sec = stimer.it_value.tv_sec - timer.it_value.tv_sec;
	  long usec = stimer.it_value.tv_usec - timer.it_value.tv_usec;
	  double t = std::max(0.0, sec + usec*1e-6);
	  total_time += t;
	  accepted = (sol.get() != dd_man.GetConstant(false).get());
	  if (t > 1.0) {
	    total_time *= trials;
	    break;
	  }
	}
	std::cout << "Model checking completed in " << total_time/trials
		  << " seconds." << std::endl;
	if (accepted) {
	  std::cout << "Property is true in the initial state." << std::endl;
	} else {
	  std::cout << "Property is false in the initial state." << std::endl;
	}
      }
    } else if (engine == MIXED_ENGINE) {
      DCEngine dc_engine;
      dc_engine.seed(seed);
      std::cout << "Mixed engine: alpha=" << params.alpha
                << ", beta=" << params.beta << ", delta=" << params.delta
                << ", epsilon=" << params.epsilon << ", seed=" << seed
                << std::endl;
      DecisionDiagramManager dd_man(2*compiled_model.NumBits());
      itimerval timer = { { 0L, 0L }, { 40000000L, 0L } };
      itimerval stimer;
#ifdef PROFILING
      setitimer(ITIMER_VIRTUAL, &timer, 0);
      getitimer(ITIMER_VIRTUAL, &stimer);
#else
      setitimer(ITIMER_PROF, &timer, 0);
      getitimer(ITIMER_PROF, &stimer);
#endif
      DecisionDiagramModel dd_model = DecisionDiagramModel::Create(
          &dd_man, moments, *global_model);
      std::pair<int, int> num_regs = compiled_model.GetNumRegisters();
      CompiledExpressionEvaluator evaluator(num_regs.first, num_regs.second);
      CompiledDistributionSampler<DCEngine> sampler(&evaluator, &dc_engine);
      const State init_state(&compiled_model, &evaluator, &sampler);
#ifdef PROFILING
      getitimer(ITIMER_VIRTUAL, &timer);
#else
      getitimer(ITIMER_PROF, &timer);
#endif
      long sec = stimer.it_value.tv_sec - timer.it_value.tv_sec;
      long usec = stimer.it_value.tv_usec - timer.it_value.tv_usec;
      double t = std::max(0.0, sec + usec*1e-6);
      std::cout << "Model built in " << t << " seconds." << std::endl;
      std::cout << "Variables: " << init_state.values().size() << std::endl;
      std::cout << "Events:    " << global_model->commands().size()
                << std::endl;
      std::cout << "States:      "
                << dd_model.reachable_states().MintermCount(
                    dd_man.GetNumVariables() / 2)
                << std::endl
                << "Transitions: "
                << dd_model.rate_matrix().MintermCount(dd_man.GetNumVariables())
                << std::endl;
      std::cout << "Rate matrix";
      Cudd_PrintDebug(dd_man.manager(), dd_model.rate_matrix().get(),
                      dd_man.GetNumVariables(), 1);
      std::cout << "ODD:         " << get_num_odd_nodes() << " nodes"
                << std::endl;
      for (auto fi = properties.begin(); fi != properties.end(); fi++) {
	std::cout << std::endl << "Model checking " << **fi << " ..."
		  << std::endl;
        std::unique_ptr<const CompiledProperty> property =
            CompileAndOptimizeProperty(**fi, compiled_model, &errors);
	size_t accepts = 0;
        ModelCheckingStats stats;
	for (size_t i = 0; i < trials; i++) {
	  itimerval timer = { { 0L, 0L }, { 40000000L, 0L } };
	  itimerval stimer;
#ifdef PROFILING
	  setitimer(ITIMER_VIRTUAL, &timer, 0);
	  getitimer(ITIMER_VIRTUAL, &stimer);
#else
	  setitimer(ITIMER_PROF, &timer, 0);
	  getitimer(ITIMER_PROF, &stimer);
#endif
	  bool sol = Verify(*property, *global_model, &dd_model, params,
                            &evaluator, init_state, &stats);
#ifdef PROFILING
	  getitimer(ITIMER_VIRTUAL, &timer);
#else
	  getitimer(ITIMER_PROF, &timer);
#endif
	  long sec = stimer.it_value.tv_sec - timer.it_value.tv_sec;
	  long usec = stimer.it_value.tv_usec - timer.it_value.tv_usec;
	  double t = std::max(0.0, sec + usec*1e-6);
	  if (trials == 1) {
	    std::cout << "Model checking completed in " << t << " seconds."
		      << std::endl;
	    if (sol) {
	      std::cout << "Property is true in the initial state."
			<< std::endl;
	    } else {
	      std::cout << "Property is false in the initial state."
			<< std::endl;
	    }
	  } else {
	    if (sol) {
	      accepts++;
	    }
            stats.time.AddObservation(t);
	  }
	}
	if (trials > 1) {
	  std::cout << "Model checking time mean: " << stats.time.mean()
		    << " seconds" << std::endl
                    << "Model checking time min: " << stats.time.min()
                    << " seconds" << std::endl
                    << "Model checking time max: " << stats.time.max()
                    << " seconds" << std::endl
		    << "Model checking time std.dev.: "
                    << stats.time.sample_stddev() << std::endl
		    << "Sample size mean: " << stats.sample_size.mean()
                    << std::endl
                    << "Sample size min: " << stats.sample_size.min()
                    << std::endl
                    << "Sample size max: " << stats.sample_size.max()
                    << std::endl
                    << "Sample size std.dev.: "
                    << stats.sample_size.sample_stddev() << std::endl
		    << "Path length mean: " << stats.path_length.mean()
                    << std::endl
		    << "Path length min: " << stats.path_length.min()
                    << std::endl
		    << "Path length max: " << stats.path_length.max()
                    << std::endl
                    << "Path length std.dev.: "
                    << stats.path_length.sample_stddev() << std::endl
		    << accepts << " accepted, " << (trials - accepts)
		    << " rejected" << std::endl;
	}
      }
    }
    delete global_model;
    for (const StateFormula* f : properties) {
      delete f;
    }
    properties.clear();
  } catch (const std::exception& e) {
    std::cerr << std::endl << PACKAGE ": " << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << std::endl << PACKAGE ": fatal error" << std::endl;
    return 1;
  }

  return 0;
}

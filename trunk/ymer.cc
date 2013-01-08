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
/* Whether memoization is enabled. */
bool memoization = false;
/* Fixed nested error. */
double nested_error = -1.0;
/* Fixed sample size. */
int fixed_sample_size = 0;
/* Maxumum path length. */
size_t max_path_length = std::numeric_limits<int>::max();
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
  virtual void DoVisitVariable(const Variable& expr);
  virtual void DoVisitComputation(const Computation& expr);
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

void ExpressionCompiler::DoVisitVariable(const Variable& expr) {
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

void ExpressionCompiler::DoVisitComputation(const Computation& expr) {
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
      (type2 == Type::DOUBLE || expr.op() == Computation::DIVIDE)) {
    operations_.push_back(Operation::MakeI2D(dst_));
    type1 = Type::DOUBLE;
  }
  if (type2 != Type::DOUBLE && type1 == Type::DOUBLE) {
    operations_.push_back(Operation::MakeI2D(dst_ + 1));
    type2 = Type::DOUBLE;
  }
  switch (expr.op()) {
    case Computation::PLUS:
      if (type1 == Type::DOUBLE) {
        operations_.push_back(Operation::MakeDADD(dst_, dst_ + 1));
      } else {
        operations_.push_back(Operation::MakeIADD(dst_, dst_ + 1));
      }
      break;
    case Computation::MINUS:
      if (type1 == Type::DOUBLE) {
        operations_.push_back(Operation::MakeDSUB(dst_, dst_ + 1));
      } else {
        operations_.push_back(Operation::MakeISUB(dst_, dst_ + 1));
      }
      break;
    case Computation::MULTIPLY:
      if (type1 == Type::DOUBLE) {
        operations_.push_back(Operation::MakeDMUL(dst_, dst_ + 1));
      } else {
        operations_.push_back(Operation::MakeIMUL(dst_, dst_ + 1));
      }
      break;
    case Computation::DIVIDE:
      operations_.push_back(Operation::MakeDDIV(dst_, dst_ + 1));
      break;
  }
  type_ = type1;
}

void ExpressionCompiler::DoVisitConjunction(const Conjunction& formula) {
  size_t n = formula.conjuncts().size();
  for (size_t i = 0; i < n; ++i) {
    const StateFormula& conjunct = *formula.conjuncts()[i];
    size_t iffalse_pos = operations_.size();
    if (i > 0) {
      operations_.push_back(Operation::MakeNOP());  // placeholder for IFFALSE
    }
    conjunct.Accept(this);
    if (type_ != Type::BOOL) {
      errors_->push_back(StrCat("type mismatch; binary operator & applied to ",
                                type_));
    }
    if (i > 0) {
      operations_[iffalse_pos] =
          Operation::MakeIFFALSE(dst_, operations_.size());
    }
  }
  type_ = Type::BOOL;
}

void ExpressionCompiler::DoVisitDisjunction(const Disjunction& formula) {
  size_t n = formula.disjuncts().size();
  for (size_t i = 0; i < n; ++i) {
    const StateFormula& disjunct = *formula.disjuncts()[i];
    size_t iftrue_pos = operations_.size();
    if (i > 0) {
      operations_.push_back(Operation::MakeNOP());  // placeholder for IFTRUE
    }
    disjunct.Accept(this);
    if (type_ != Type::BOOL) {
      errors_->push_back(StrCat("type mismatch; binary operator | applied to ",
                                type_));
    }
    if (i > 0) {
      operations_[iftrue_pos] = Operation::MakeIFTRUE(dst_, operations_.size());
    }
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
        (type2 == Type::INT && type2 == Type::DOUBLE))) {
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

CompiledExpression CompileExpression(
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
  }
  return CompiledExpression(operations);
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
  }
  return CompiledExpression(compiler.release_operations());
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
      CompileExpression(
          dist.rate(), Type::DOUBLE, *variables_by_name_, errors_));
}

void DistributionCompiler::DoVisitWeibull(const Weibull& dist) {
  dist_ = CompiledDistribution::MakeWeibull(
      CompileExpression(
          dist.scale(), Type::DOUBLE, *variables_by_name_, errors_),
      CompileExpression(
          dist.shape(), Type::DOUBLE, *variables_by_name_, errors_));
}

void DistributionCompiler::DoVisitLognormal(const Lognormal& dist) {
  dist_ = CompiledDistribution::MakeLognormal(
      CompileExpression(
          dist.scale(), Type::DOUBLE, *variables_by_name_, errors_),
      CompileExpression(
          dist.shape(), Type::DOUBLE, *variables_by_name_, errors_));
}

void DistributionCompiler::DoVisitUniform(const Uniform& dist) {
  dist_ = CompiledDistribution::MakeUniform(
      CompileExpression(dist.low(), Type::DOUBLE, *variables_by_name_, errors_),
      CompileExpression(
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
  auto i = variables_by_name.find(update.variable().name());
  int variable;
  if (i == variables_by_name.end()) {
    errors->push_back(StrCat(
        "undefined variable '", update.variable().name(), "' in update"));
    variable = -1;
  } else {
    variable = i->second;
  }
  return CompiledUpdate(
      variable,
      CompileExpression(update.expr(), Type::INT, variables_by_name, errors));
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
      CompileExpression(command.guard(), variables_by_name, errors),
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

}  // namespace

/* The main program. */
int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);

  /* Set default alpha. */
  double alpha = 1e-2;
  /* Set default beta. */
  double beta = 1e-2;
  /* Set default delta. */
  double delta = 1e-2;
  /* Set default epsilon. */
  double epsilon = 1e-6;
  /* Verification without estimation by default. */
  bool estimate = false;
  /* Set default engine. */
  enum { SAMPLING_ENGINE, HYBRID_ENGINE, MIXED_ENGINE } engine =
							  SAMPLING_ENGINE;
  /* Sampling algorithm. */
  SamplingAlgorithm algorithm = SPRT;
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
	alpha = atof(optarg);
	if (alpha < 1e-10) {
	  throw std::invalid_argument("alpha < 1e-10");
	} else if (alpha >= 0.5) {
	  throw std::invalid_argument("alpha >= 0.5");
	}
	break;
      case 'B':
	beta = atof(optarg);
	if (beta < 1e-10) {
	  throw std::invalid_argument("beta < 1e-10");
	} else if (beta >= 0.5) {
	  throw std::invalid_argument("beta >= 0.5");
	}
	break;
      case 'c':
        if (!parse_const_overrides(optarg, &const_overrides)) {
          throw std::invalid_argument("bad --const specification");
        }
        break;
      case 'D':
	delta = atof(optarg);
	if (delta < 1e-10) {
	  throw std::invalid_argument("delta < 1e-10");
	} else if (delta > 0.5) {
	  throw std::invalid_argument("delta > 0.5");
	}
	break;
      case 'E':
	epsilon = atof(optarg);
	if (epsilon < 1e-10) {
	  throw std::invalid_argument("epsilon < 1e-10");
	} else if (epsilon > 1.0) {
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
        max_path_length = atoi(optarg);
        break;
      case 'M':
	memoization = true;
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
	algorithm = FIXED;
        fixed_sample_size = atoi(optarg);
	break;
      case 'n':
	nested_error = atof(optarg);
	break;
      case 'p':
	estimate = true;
	algorithm = ESTIMATE;
	break;
      case 'P':
	port = atoi(optarg);
	if (port < 0 || port > 0xffff) {
	  throw std::invalid_argument("invalid port number");
	}
	break;
      case 's':
	if (strcasecmp(optarg, "sequential") == 0) {
	  algorithm = SEQUENTIAL;
	} else if (strcasecmp(optarg, "sprt") == 0) {
	  algorithm = SPRT;
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
	double alphap = alpha, betap = beta;
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
	      msg.value = pf->sample(*global_model, init_state,
				     delta, alphap, betap, algorithm, &stats);
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
		alphap = alpha;
		betap = beta;
		if (pf->probabilistic()) {
		  double p0, p1;
		  while (true) {
		    p0 = std::min(1.0, (theta + delta)*(1.0 - betap));
		    p1 = std::max(0.0, 1.0 - ((1.0 - (theta - delta))
					      *(1.0 - alphap)));
		    if (p1 < p0) {
		      break;
		    } else {
		      alphap *= 0.5;
		      betap *= 0.5;
		    }
		  }
		}
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
      std::cout << "Sampling engine: alpha=" << alpha << ", beta=" << beta
		<< ", delta=" << delta << ", seed=" << seed << std::endl;
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
	current_property = fi - properties.begin();
	size_t accepts = 0;
        ModelCheckingStats stats;
	double total_cached = 0.0;
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
	  bool sol = (*fi)->verify(*global_model, init_state,
				   delta, alpha, beta, algorithm, &stats);
	  total_cached += (*fi)->clear_cache();
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
	  double cached_avg = total_cached/trials;
	  std::cout << "Model checking time mean: " << stats.time.mean()
		    << " seconds" << std::endl
                    << "Model checking time min: " << stats.time.min()
                    << " seconds" << std::endl
                    << "Model checking time max: " << stats.time.max()
                    << " seconds" << std::endl
		    << "Model checking time std.dev.: " << stats.time.stddev()
                    << std::endl
		    << "Sample size mean: " << stats.sample_size.mean()
                    << std::endl
                    << "Sample size min: " << stats.sample_size.min()
                    << std::endl
                    << "Sample size max: " << stats.sample_size.max()
                    << std::endl
                    << "Sample size std.dev.: " << stats.sample_size.stddev()
                    << std::endl
		    << "Path length mean: " << stats.path_length.mean()
                    << std::endl
		    << "Path length min: " << stats.path_length.min()
                    << std::endl
		    << "Path length max: " << stats.path_length.max()
                    << std::endl
                    << "Path length std.dev.: " << stats.path_length.stddev()
                    << std::endl
		    << accepts << " accepted, " << (trials - accepts)
		    << " rejected" << std::endl
		    << "Average cached: " << cached_avg << std::endl;
	}
      }
    } else if (engine == HYBRID_ENGINE) {
      std::cout << "Hybrid engine: epsilon=" << epsilon << std::endl;
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
      global_model->cache_dds(dd_man, moments);
#ifdef PROFILING
      getitimer(ITIMER_VIRTUAL, &timer);
#else
      getitimer(ITIMER_PROF, &timer);
#endif
      long sec = stimer.it_value.tv_sec - timer.it_value.tv_sec;
      long usec = stimer.it_value.tv_usec - timer.it_value.tv_usec;
      double t = std::max(0.0, sec + usec*1e-6);
      std::cout << "Model built in " << t << " seconds." << std::endl;
      std::cout << "States:      " << global_model->num_states(dd_man)
                << std::endl
                << "Transitions: " << global_model->num_transitions(dd_man)
                << std::endl;
      DdNode* ddR = global_model->rate_mtbdd(dd_man);
      std::cout << "Rate matrix";
      Cudd_PrintDebug(dd_man.manager(), ddR, dd_man.GetNumVariables(), 1);
      Cudd_RecursiveDeref(dd_man.manager(), ddR);
      std::cout << "ODD:         " << get_num_odd_nodes() << " nodes"
                << std::endl;
      DdNode* init = global_model->init_bdd(dd_man);
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
	  DdNode* ddf = (*fi)->verify(dd_man, *global_model, epsilon,
				      estimate);
	  DdNode* sol = Cudd_bddAnd(dd_man.manager(), ddf, init);
	  Cudd_Ref(sol);
	  Cudd_RecursiveDeref(dd_man.manager(), ddf);
#ifdef PROFILING
	  getitimer(ITIMER_VIRTUAL, &timer);
#else
	  getitimer(ITIMER_PROF, &timer);
#endif
	  long sec = stimer.it_value.tv_sec - timer.it_value.tv_sec;
	  long usec = stimer.it_value.tv_usec - timer.it_value.tv_usec;
	  double t = std::max(0.0, sec + usec*1e-6);
	  total_time += t;
	  accepted = (sol != Cudd_ReadLogicZero(dd_man.manager()));
	  Cudd_RecursiveDeref(dd_man.manager(), sol);
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
      Cudd_RecursiveDeref(dd_man.manager(), init);
      global_model->uncache_dds(dd_man);
    } else if (engine == MIXED_ENGINE) {
      DCEngine dc_engine;
      dc_engine.seed(seed);
      std::cout << "Mixed engine: alpha=" << alpha << ", beta=" << beta
		<< ", delta=" << delta << ", epsilon=" << epsilon
		<< ", seed=" << seed << std::endl;
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
      global_model->cache_dds(dd_man, moments);
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
      std::cout << "States:      " << global_model->num_states(dd_man)
                << std::endl
                << "Transitions: " << global_model->num_transitions(dd_man)
                << std::endl;
      DdNode* ddR = global_model->rate_mtbdd(dd_man);
      std::cout << "Rate matrix";
      Cudd_PrintDebug(dd_man.manager(), ddR, dd_man.GetNumVariables(), 1);
      Cudd_RecursiveDeref(dd_man.manager(), ddR);
      std::cout << "ODD:         " << get_num_odd_nodes() << " nodes"
                << std::endl;
      for (auto fi = properties.begin(); fi != properties.end(); fi++) {
	std::cout << std::endl << "Model checking " << **fi << " ..."
		  << std::endl;
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
	  bool sol = (*fi)->verify(dd_man, *global_model, init_state,
				   delta, alpha, beta, algorithm, epsilon,
                                   &stats);
	  (*fi)->clear_cache();
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
		    << "Model checking time std.dev.: " << stats.time.stddev()
                    << std::endl
		    << "Sample size mean: " << stats.sample_size.mean()
                    << std::endl
                    << "Sample size min: " << stats.sample_size.min()
                    << std::endl
                    << "Sample size max: " << stats.sample_size.max()
                    << std::endl
                    << "Sample size std.dev.: " << stats.sample_size.stddev()
                    << std::endl
		    << "Path length mean: " << stats.path_length.mean()
                    << std::endl
		    << "Path length min: " << stats.path_length.min()
                    << std::endl
		    << "Path length max: " << stats.path_length.max()
                    << std::endl
                    << "Path length std.dev.: " << stats.path_length.stddev()
                    << std::endl
		    << accepts << " accepted, " << (trials - accepts)
		    << " rejected" << std::endl;
	}
      }
      global_model->uncache_dds(dd_man);
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

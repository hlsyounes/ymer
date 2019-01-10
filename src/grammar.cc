/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 24 "grammar.yy" /* yacc.c:339  */

#include <limits>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "distribution.h"
#include "expression.h"
#include "model.h"
#include "parser-state.h"
#include "process-algebra.h"
#include "strutil.h"
#include "typed-value.h"

#include "glog/logging.h"

#include "grammar.hh"

// Lexical analyzer function.
extern int yylex(YYSTYPE* lvalp, YYLTYPE* llocp, void* scanner);

namespace {

// Error reporting function.
void yyerror(const YYLTYPE& location, const std::string& msg,
             ParserState* state) {
  state->add_error(StrCat(location.first_line, ":", msg));
}

// Wrapper for error reporting function called by generated code.
void yyerror(YYLTYPE* llocp, void* scanner, ParserState* state,
             const std::string& msg) {
  yyerror(*llocp, msg, state);
}

// Wraps the given raw pointer in a std::unique_ptr<T>.
template <typename T>
std::unique_ptr<T> WrapUnique(T* p) {
  return std::unique_ptr<T>(p);
}

Function MakeFunction(const YYLTYPE& location, const std::string* name,
                      ParserState* state) {
  auto name_ptr = WrapUnique(name);
  if (*name == "min") {
    return Function::MIN;
  } else if (*name == "max") {
    return Function::MAX;
  } else if (*name == "floor") {
    return Function::FLOOR;
  } else if (*name == "ceil") {
    return Function::CEIL;
  } else if (*name == "pow") {
    return Function::POW;
  } else if (*name == "log") {
    return Function::LOG;
  } else if (*name == "mod") {
    return Function::MOD;
  } else {
    yyerror(location, "unknown function", state);
    return Function::UNKNOWN;
  }
}

UniquePtrVector<const Expression>* AddArgument(
    const Expression* argument, UniquePtrVector<const Expression>* arguments) {
  if (arguments == nullptr) {
    arguments = new UniquePtrVector<const Expression>();
  }
  arguments->push_back(WrapUnique(argument));
  return arguments;
}

const Literal* NewLiteral(const TypedValue* value) {
  return new Literal(*WrapUnique(value));
}

const Identifier* NewIdentifier(const std::string* name) {
  return new Identifier(*WrapUnique(name));
}

const Label* NewLabel(const std::string* name) {
  return new Label(*WrapUnique(name));
}

const FunctionCall* NewFunctionCall(
    Function function, UniquePtrVector<const Expression>* arguments) {
  return new FunctionCall(function, std::move(*WrapUnique(arguments)));
}

const UnaryOperation* NewNegate(const Expression* operand) {
  return new UnaryOperation(UnaryOperator::NEGATE, WrapUnique(operand));
}

const UnaryOperation* NewNot(const Expression* operand) {
  return new UnaryOperation(UnaryOperator::NOT, WrapUnique(operand));
}

const BinaryOperation* NewPlus(const Expression* operand1,
                               const Expression* operand2) {
  return new BinaryOperation(BinaryOperator::PLUS, WrapUnique(operand1),
                             WrapUnique(operand2));
}

const BinaryOperation* NewMinus(const Expression* operand1,
                                const Expression* operand2) {
  return new BinaryOperation(BinaryOperator::MINUS, WrapUnique(operand1),
                             WrapUnique(operand2));
}

const BinaryOperation* NewMultiply(const Expression* operand1,
                                   const Expression* operand2) {
  return new BinaryOperation(BinaryOperator::MULTIPLY, WrapUnique(operand1),
                             WrapUnique(operand2));
}

const BinaryOperation* NewDivide(const Expression* operand1,
                                 const Expression* operand2) {
  return new BinaryOperation(BinaryOperator::DIVIDE, WrapUnique(operand1),
                             WrapUnique(operand2));
}

const BinaryOperation* NewAnd(const Expression* operand1,
                              const Expression* operand2) {
  return new BinaryOperation(BinaryOperator::AND, WrapUnique(operand1),
                             WrapUnique(operand2));
}

const BinaryOperation* NewOr(const Expression* operand1,
                             const Expression* operand2) {
  return new BinaryOperation(BinaryOperator::OR, WrapUnique(operand1),
                             WrapUnique(operand2));
}

const BinaryOperation* NewImply(const Expression* operand1,
                                const Expression* operand2) {
  return new BinaryOperation(BinaryOperator::IMPLY, WrapUnique(operand1),
                             WrapUnique(operand2));
}

const BinaryOperation* NewIff(const Expression* operand1,
                              const Expression* operand2) {
  return new BinaryOperation(BinaryOperator::IFF, WrapUnique(operand1),
                             WrapUnique(operand2));
}

const BinaryOperation* NewLess(const Expression* operand1,
                               const Expression* operand2) {
  return new BinaryOperation(BinaryOperator::LESS, WrapUnique(operand1),
                             WrapUnique(operand2));
}

const BinaryOperation* NewLessEqual(const Expression* operand1,
                                    const Expression* operand2) {
  return new BinaryOperation(BinaryOperator::LESS_EQUAL, WrapUnique(operand1),
                             WrapUnique(operand2));
}

const BinaryOperation* NewGreaterEqual(const Expression* operand1,
                                       const Expression* operand2) {
  return new BinaryOperation(BinaryOperator::GREATER_EQUAL,
                             WrapUnique(operand1), WrapUnique(operand2));
}

const BinaryOperation* NewGreater(const Expression* operand1,
                                  const Expression* operand2) {
  return new BinaryOperation(BinaryOperator::GREATER, WrapUnique(operand1),
                             WrapUnique(operand2));
}

const BinaryOperation* NewEqual(const Expression* operand1,
                                const Expression* operand2) {
  return new BinaryOperation(BinaryOperator::EQUAL, WrapUnique(operand1),
                             WrapUnique(operand2));
}

const BinaryOperation* NewNotEqual(const Expression* operand1,
                                   const Expression* operand2) {
  return new BinaryOperation(BinaryOperator::NOT_EQUAL, WrapUnique(operand1),
                             WrapUnique(operand2));
}

const Conditional* NewConditional(const Expression* condition,
                                  const Expression* if_branch,
                                  const Expression* else_branch) {
  return new Conditional(WrapUnique(condition), WrapUnique(if_branch),
                         WrapUnique(else_branch));
}

double Double(const TypedValue* typed_value) {
  CHECK(typed_value->type() != Type::BOOL);
  return WrapUnique(typed_value)->value<double>();
}

double Infinity() { return std::numeric_limits<double>::infinity(); }

const ProbabilityThresholdOperation* NewProbabilityLess(
    double threshold, const PathProperty* path_property) {
  return new ProbabilityThresholdOperation(
      ProbabilityThresholdOperator::LESS, threshold, WrapUnique(path_property));
}

const ProbabilityThresholdOperation* NewProbabilityLessEqual(
    double threshold, const PathProperty* path_property) {
  return new ProbabilityThresholdOperation(
      ProbabilityThresholdOperator::LESS_EQUAL, threshold,
      WrapUnique(path_property));
}

const ProbabilityThresholdOperation* NewProbabilityGreaterEqual(
    double threshold, const PathProperty* path_property) {
  return new ProbabilityThresholdOperation(
      ProbabilityThresholdOperator::GREATER_EQUAL, threshold,
      WrapUnique(path_property));
}

const ProbabilityThresholdOperation* NewProbabilityGreater(
    double threshold, const PathProperty* path_property) {
  return new ProbabilityThresholdOperation(
      ProbabilityThresholdOperator::GREATER, threshold,
      WrapUnique(path_property));
}

const ProbabilityEstimationOperation* NewProbabilityEqual(
    const PathProperty* path_property) {
  return new ProbabilityEstimationOperation(WrapUnique(path_property));
}

const UntilProperty* NewUntil(const TimeRange* time_range,
                              const Expression* pre_expr,
                              const Expression* post_expr) {
  return new UntilProperty(*WrapUnique(time_range), WrapUnique(pre_expr),
                           WrapUnique(post_expr));
}

const EventuallyProperty* NewEventually(const TimeRange* time_range,
                                        const Expression* expr) {
  return new EventuallyProperty(*WrapUnique(time_range), WrapUnique(expr));
}

const Memoryless* NewMemoryless(const Expression* rate) {
  return new Memoryless(WrapUnique(rate));
}

const Distribution* NewWeibull(const Expression* scale,
                               const Expression* shape) {
  return new Weibull(WrapUnique(scale), WrapUnique(shape));
}

const Lognormal* NewLognormal(const Expression* scale,
                              const Expression* shape) {
  return new Lognormal(WrapUnique(scale), WrapUnique(shape));
}

const Uniform* NewUniform(const Expression* low, const Expression* high) {
  return new Uniform(WrapUnique(low), WrapUnique(high));
}

void SetModelType(const YYLTYPE& location, ModelType type, ParserState* state) {
  if (!state->mutable_model()->SetType(type)) {
    yyerror(location, "multiple model types", state);
  }
}

void AddConstant(const YYLTYPE& location, const std::string* name, Type type,
                 const Expression* init, ParserState* state) {
  std::vector<std::string> errors;
  if (!state->mutable_model()->AddConstant(*WrapUnique(name), type,
                                           WrapUnique(init), &errors)) {
    for (const auto& error : errors) {
      yyerror(location, error, state);
    }
  }
}

void AddIntVariable(const YYLTYPE& location, const std::string* name,
                    const Expression* low, const Expression* high,
                    const Expression* init, ParserState* state) {
  std::vector<std::string> errors;
  if (!state->mutable_model()->AddIntVariable(*WrapUnique(name),
                                              WrapUnique(low), WrapUnique(high),
                                              WrapUnique(init), &errors)) {
    for (const auto& error : errors) {
      yyerror(location, error, state);
    }
  }
}

void AddBoolVariable(const YYLTYPE& location, const std::string* name,
                     const Expression* init, ParserState* state) {
  std::vector<std::string> errors;
  if (!state->mutable_model()->AddBoolVariable(*WrapUnique(name),
                                               WrapUnique(init), &errors)) {
    for (const auto& error : errors) {
      yyerror(location, error, state);
    }
  }
}

void AddFormula(const YYLTYPE& location, const std::string* name,
                const Expression* expr, ParserState* state) {
  std::vector<std::string> errors;
  if (!state->mutable_model()->AddFormula(*WrapUnique(name), WrapUnique(expr),
                                          &errors)) {
    for (const auto& error : errors) {
      yyerror(location, error, state);
    }
  }
}

void AddLabel(const YYLTYPE& location, const std::string* name,
              const Expression* expr, ParserState* state) {
  auto name_ptr = WrapUnique(name);
  if (!state->mutable_model()->AddLabel(*name_ptr, WrapUnique(expr))) {
    yyerror(location, StrCat("duplicate label ", *name_ptr), state);
  }
}

void StartModule(const YYLTYPE& location, const std::string* name,
                 ParserState* state) {
  auto name_ptr = WrapUnique(name);
  if (!state->mutable_model()->StartModule(*name_ptr)) {
    yyerror(location, StrCat("duplicate module ", *name_ptr), state);
  }
}

void AddFromModule(const YYLTYPE& location, const std::string* from_name,
                   const std::map<std::string, std::string>* substitutions,
                   ParserState* state) {
  std::vector<std::string> errors;
  if (!state->mutable_model()->AddFromModule(
          *WrapUnique(from_name), *WrapUnique(substitutions), &errors)) {
    for (const auto& error : errors) {
      yyerror(location, error, state);
    }
  }
}

void EndModule(ParserState* state) { state->mutable_model()->EndModule(); }

std::map<std::string, std::string>* AddSubstitution(
    const YYLTYPE& location, const std::string* from_name,
    const std::string* to_name,
    std::map<std::string, std::string>* substitutions, ParserState* state) {
  if (substitutions == nullptr) {
    substitutions = new std::map<std::string, std::string>();
  }
  auto from_name_ptr = WrapUnique(from_name);
  if (!substitutions->emplace(*from_name_ptr, *WrapUnique(to_name)).second) {
    yyerror(location, StrCat("duplicate substitution for ", *from_name_ptr),
            state);
  }
  return substitutions;
}

Update* NewUpdate(const std::string* variable, const Expression* expr) {
  return new Update(*WrapUnique(variable), WrapUnique(expr));
}

std::vector<Update>* AddUpdate(Update* update, std::vector<Update>* updates) {
  if (updates == nullptr) {
    updates = new std::vector<Update>();
  }
  updates->push_back(std::move(*WrapUnique(update)));
  return updates;
}

Outcome* NewOutcome(const Distribution* delay, std::vector<Update>* updates) {
  return new Outcome((delay == nullptr) ? std::make_unique<Memoryless>(
                                              std::make_unique<Literal>(1))
                                        : WrapUnique(delay),
                     std::move(*WrapUnique(updates)));
}

std::vector<Outcome>* AddOutcome(Outcome* outcome,
                                 std::vector<Outcome>* outcomes) {
  if (outcomes == nullptr) {
    outcomes = new std::vector<Outcome>();
  }
  outcomes->push_back(std::move(*WrapUnique(outcome)));
  return outcomes;
}

void AddCommand(const YYLTYPE& location, const std::string* action,
                const Expression* guard, std::vector<Outcome>* outcomes,
                ParserState* state) {
  std::vector<std::string> errors;
  if (!state->mutable_model()->AddCommand(
          Command(*WrapUnique(action), WrapUnique(guard),
                  std::move(*WrapUnique(outcomes))),
          &errors)) {
    for (const auto& error : errors) {
      yyerror(location, error, state);
    }
  }
}

void SetInit(const YYLTYPE& location, const Expression* expr,
             ParserState* state) {
  if (!state->mutable_model()->SetInit(WrapUnique(expr))) {
    yyerror(location, "multiple init blocks", state);
  }
}

void StartRewards(const YYLTYPE& location, const std::string* label,
                  ParserState* state) {
  auto label_ptr = WrapUnique(label);
  if (!state->mutable_model()->StartRewardsStructure(*label_ptr)) {
    yyerror(location, StrCat("duplicate rewards ", *label_ptr), state);
  }
}

void AddStateReward(const Expression* guard, const Expression* reward,
                    ParserState* state) {
  state->mutable_model()->AddStateReward(
      ParsedStateReward(WrapUnique(guard), WrapUnique(reward)));
}

void AddTransitionReward(const YYLTYPE& location, const std::string* action,
                         const Expression* guard, const Expression* reward,
                         ParserState* state) {
  std::vector<std::string> errors;
  if (!state->mutable_model()->AddTransitionReward(
          ParsedTransitionReward(*WrapUnique(action), WrapUnique(guard),
                                 WrapUnique(reward)),
          &errors)) {
    for (const auto& error : errors) {
      yyerror(location, error, state);
    }
  }
}

void EndRewards(ParserState* state) {
  state->mutable_model()->EndRewardsStructure();
}

void SetSystem(const YYLTYPE& location, const ProcessAlgebra* system,
               ParserState* state) {
  if (!state->mutable_model()->SetSystem(WrapUnique(system))) {
    yyerror(location, "multiple system blocks", state);
  }
}

std::set<std::string>* AddAction(const std::string* action,
                                std::set<std::string>* actions) {
  if (actions == nullptr) {
    actions = new std::set<std::string>();
  }
  actions->insert(*WrapUnique(action));
  return actions;
}

const ProcessAlgebra* NewModuleIdentifier(const std::string* name) {
  return new ModuleIdentifier(*WrapUnique(name));
}

const ProcessAlgebra* NewActionHiding(const std::set<std::string>* actions,
                                      const ProcessAlgebra* operand) {
  return new ActionHiding(*WrapUnique(actions), WrapUnique(operand));
}

const ProcessAlgebra* NewActionRenaming(
    const std::map<std::string, std::string>* substitutions,
    const ProcessAlgebra* operand) {
  return new ActionRenaming(*WrapUnique(substitutions), WrapUnique(operand));
}

const ProcessAlgebra* NewRestrictedParallelComposition(
    const std::set<std::string>* actions, const ProcessAlgebra* operand1,
    const ProcessAlgebra* operand2) {
  return new RestrictedParallelComposition(
      *WrapUnique(actions), WrapUnique(operand1), WrapUnique(operand2));
}

const ProcessAlgebra* NewAlphabetizedParallelComposition(
    const ProcessAlgebra* operand1, const ProcessAlgebra* operand2) {
  return new ParallelComposition(ParallelCompositionOperator::ALPHABETIZED,
                                 WrapUnique(operand1), WrapUnique(operand2));
}

const ProcessAlgebra* NewAsynchronousParallelComposition(
    const ProcessAlgebra* operand1, const ProcessAlgebra* operand2) {
  return new ParallelComposition(ParallelCompositionOperator::ASYNCHRONOUS,
                                 WrapUnique(operand1), WrapUnique(operand2));
}

void AddProperty(const Expression* property, ParserState* state) {
  state->add_property(WrapUnique(property));
}

}  // namespace

#line 561 "src/grammar.cc" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    DTMC_TOKEN = 258,
    CTMC_TOKEN = 259,
    MDP_TOKEN = 260,
    GSMP_TOKEN = 261,
    PROBABILISTIC = 262,
    STOCHASTIC = 263,
    NONDETERMINISTIC = 264,
    CONST = 265,
    INT_TOKEN = 266,
    DOUBLE_TOKEN = 267,
    BOOL_TOKEN = 268,
    RATE = 269,
    PROB = 270,
    GLOBAL = 271,
    DOTDOT = 272,
    FORMULA = 273,
    LABEL = 274,
    INIT = 275,
    ENDINIT = 276,
    REWARDS = 277,
    ENDREWARDS = 278,
    MODULE = 279,
    ENDMODULE = 280,
    ARROW = 281,
    PRIME = 282,
    TRUE = 283,
    FALSE = 284,
    MAX_TOKEN = 285,
    MIN_TOKEN = 286,
    FUNC = 287,
    IDENTIFIER = 288,
    NUMBER = 289,
    LABEL_NAME = 290,
    SYSTEM = 291,
    ENDSYSTEM = 292,
    DOUBLE_BAR = 293,
    TRIPLE_BAR = 294,
    BACK_ARROW = 295,
    CLOCK = 296,
    ENDINVARIANT = 297,
    FILTER = 298,
    INVARIANT = 299,
    PTA = 300,
    A = 301,
    C = 302,
    E = 303,
    F = 304,
    G = 305,
    I = 306,
    PMAX = 307,
    PMIN = 308,
    P = 309,
    RMAX = 310,
    RMIN = 311,
    R = 312,
    S = 313,
    U = 314,
    W = 315,
    X = 316,
    LOGNORMAL = 317,
    UNIFORM = 318,
    WEIBULL = 319,
    IFF_TOKEN = 320,
    IMPLY_TOKEN = 321,
    NEQ = 322,
    LEQ = 323,
    GEQ = 324,
    UMINUS = 325
  };
#endif
/* Tokens.  */
#define DTMC_TOKEN 258
#define CTMC_TOKEN 259
#define MDP_TOKEN 260
#define GSMP_TOKEN 261
#define PROBABILISTIC 262
#define STOCHASTIC 263
#define NONDETERMINISTIC 264
#define CONST 265
#define INT_TOKEN 266
#define DOUBLE_TOKEN 267
#define BOOL_TOKEN 268
#define RATE 269
#define PROB 270
#define GLOBAL 271
#define DOTDOT 272
#define FORMULA 273
#define LABEL 274
#define INIT 275
#define ENDINIT 276
#define REWARDS 277
#define ENDREWARDS 278
#define MODULE 279
#define ENDMODULE 280
#define ARROW 281
#define PRIME 282
#define TRUE 283
#define FALSE 284
#define MAX_TOKEN 285
#define MIN_TOKEN 286
#define FUNC 287
#define IDENTIFIER 288
#define NUMBER 289
#define LABEL_NAME 290
#define SYSTEM 291
#define ENDSYSTEM 292
#define DOUBLE_BAR 293
#define TRIPLE_BAR 294
#define BACK_ARROW 295
#define CLOCK 296
#define ENDINVARIANT 297
#define FILTER 298
#define INVARIANT 299
#define PTA 300
#define A 301
#define C 302
#define E 303
#define F 304
#define G 305
#define I 306
#define PMAX 307
#define PMIN 308
#define P 309
#define RMAX 310
#define RMIN 311
#define R 312
#define S 313
#define U 314
#define W 315
#define X 316
#define LOGNORMAL 317
#define UNIFORM 318
#define WEIBULL 319
#define IFF_TOKEN 320
#define IMPLY_TOKEN 321
#define NEQ 322
#define LEQ 323
#define GEQ 324
#define UMINUS 325

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 562 "grammar.yy" /* yacc.c:355  */

  Type type;
  const std::string* str;
  std::set<std::string>* str_set;
  std::map<std::string, std::string>* substitutions;
  std::vector<Outcome>* outcomes;
  Outcome* outcome;
  const Distribution* dist;
  std::vector<Update>* updates;
  Update* update;
  const Expression* expr;
  const ProcessAlgebra* process_algebra;
  const TypedValue* number;
  Function function;
  UniquePtrVector<const Expression>* arguments;
  const PathProperty* path;
  const TimeRange* time_range;

#line 760 "src/grammar.cc" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



int yyparse (void* scanner, ParserState* state);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 790 "src/grammar.cc" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  82
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   959

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  91
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  48
/* YYNRULES -- Number of rules.  */
#define YYNRULES  156
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  347

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   325

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    83,     2,     2,     2,     2,    70,     2,
      88,    89,    79,    77,    87,    78,     2,    80,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    66,    84,
      73,    71,    76,    65,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    85,     2,    86,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    81,    69,    90,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      67,    68,    72,    74,    75,    82
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   614,   614,   618,   619,   622,   625,   626,   629,   630,
     631,   632,   633,   634,   635,   636,   637,   640,   642,   644,
     646,   648,   650,   652,   656,   660,   662,   664,   666,   668,
     670,   675,   676,   680,   683,   685,   690,   691,   695,   699,
     703,   704,   708,   710,   714,   715,   718,   719,   722,   727,
     728,   731,   733,   736,   738,   742,   746,   748,   750,   752,
     756,   758,   761,   763,   767,   771,   775,   779,   783,   788,
     789,   793,   794,   795,   798,   802,   806,   810,   812,   814,
     816,   818,   820,   822,   826,   828,   832,   835,   839,   841,
     843,   845,   847,   849,   851,   853,   855,   857,   859,   861,
     863,   865,   867,   869,   871,   873,   875,   877,   879,   881,
     883,   885,   889,   891,   893,   897,   899,   903,   904,   907,
     909,   913,   915,   917,   919,   921,   923,   925,   927,   929,
     931,   933,   935,   937,   939,   941,   943,   945,   947,   949,
     951,   953,   955,   957,   959,   961,   963,   965,   967,   969,
     971,   975,   977,   982,   983,   985,   987
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "DTMC_TOKEN", "CTMC_TOKEN", "MDP_TOKEN",
  "GSMP_TOKEN", "PROBABILISTIC", "STOCHASTIC", "NONDETERMINISTIC", "CONST",
  "INT_TOKEN", "DOUBLE_TOKEN", "BOOL_TOKEN", "RATE", "PROB", "GLOBAL",
  "DOTDOT", "FORMULA", "LABEL", "INIT", "ENDINIT", "REWARDS", "ENDREWARDS",
  "MODULE", "ENDMODULE", "ARROW", "PRIME", "TRUE", "FALSE", "MAX_TOKEN",
  "MIN_TOKEN", "FUNC", "IDENTIFIER", "NUMBER", "LABEL_NAME", "SYSTEM",
  "ENDSYSTEM", "DOUBLE_BAR", "TRIPLE_BAR", "BACK_ARROW", "CLOCK",
  "ENDINVARIANT", "FILTER", "INVARIANT", "PTA", "A", "C", "E", "F", "G",
  "I", "PMAX", "PMIN", "P", "RMAX", "RMIN", "R", "S", "U", "W", "X",
  "LOGNORMAL", "UNIFORM", "WEIBULL", "'?'", "':'", "IFF_TOKEN",
  "IMPLY_TOKEN", "'|'", "'&'", "'='", "NEQ", "'<'", "LEQ", "GEQ", "'>'",
  "'+'", "'-'", "'*'", "'/'", "'{'", "UMINUS", "'!'", "';'", "'['", "']'",
  "','", "'('", "')'", "'}'", "$accept", "file", "model_or_properties",
  "model", "model_components", "model_component", "model_type", "constant",
  "constant_type", "constant_init", "global", "variable", "variable_init",
  "module", "module_name", "module_def", "substitutions", "variables",
  "commands", "command", "action", "outcomes", "update_distribution",
  "distribution_and_updates", "distribution", "true_or_updates", "updates",
  "update", "formula", "label", "init", "rewards", "rewards_label",
  "reward_rules", "state_reward", "transition_reward", "system",
  "process_algebra", "action_set", "action_substitutions", "expr",
  "function", "arguments", "properties", "property_list", "property",
  "path_property", "time_range", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,    63,    58,   320,   321,   124,
      38,    61,   322,    60,   323,   324,    62,    43,    45,    42,
      47,   123,   325,    33,    59,    91,    93,    44,    40,    41,
     125
};
# endif

#define YYPACT_NINF -109

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-109)))

#define YYTABLE_NINF -113

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
       4,  -109,  -109,  -109,  -109,  -109,  -109,  -109,   218,  -109,
    -109,   -30,   -12,    10,   322,    15,    22,  -109,  -109,  -109,
    -109,   -28,    -7,  -109,  -109,   -27,    -6,   236,   236,   236,
      91,  -109,  -109,   448,  -109,  -109,  -109,    60,  -109,  -109,
    -109,  -109,  -109,  -109,  -109,    65,  -109,    64,   795,  -109,
    -109,  -109,   105,  -109,   131,   137,  -109,  -109,   124,    -7,
    -109,   322,   322,   322,   354,   145,  -109,  -109,  -109,   163,
      26,  -109,   -27,    14,   183,   215,   221,   226,   227,  -109,
    -109,   418,  -109,  -109,   201,   322,   236,   236,   236,   236,
     236,   236,   236,   236,   236,   236,   236,   236,   236,   236,
     236,   236,    -9,   322,   322,    26,  -109,  -109,   441,  -109,
     322,   322,   322,   322,   322,   322,   322,   322,   322,   322,
     322,   322,   322,   322,   322,   322,   126,   241,   250,   -30,
    -109,   189,    81,  -109,   -27,   -27,   200,   206,   255,   204,
     207,   209,   210,   212,  -109,   322,   214,   811,   -62,   795,
     731,   824,   307,   860,    50,   115,   115,     0,     0,     0,
       0,   -64,   -64,  -109,  -109,   280,   322,   657,   675,   219,
    -109,   747,   837,   849,   871,   402,   879,   879,    89,    89,
      89,    89,    -8,    -8,  -109,  -109,   -58,  -109,   268,  -109,
    -109,   763,   217,  -109,  -109,   222,   322,  -109,   -39,   -39,
     277,   277,   275,   109,   208,   208,   208,   208,   208,   811,
    -109,   322,  -109,   236,   322,   232,   148,  -109,  -109,   322,
     322,  -109,  -109,   231,   322,   285,   268,  -109,   116,  -109,
      -2,   111,   292,   293,  -109,   -31,   621,   242,   243,   244,
     247,   248,   811,   795,   811,  -109,   322,   169,   811,   322,
     693,   264,    31,   259,  -109,   267,   313,  -109,  -109,   308,
     330,   331,   332,   236,   -31,  -109,  -109,  -109,  -109,  -109,
     637,  -109,   779,  -109,   314,  -109,   334,   322,   -27,  -109,
     335,  -109,  -109,   282,   795,   236,   280,   322,  -109,   299,
     370,    83,  -109,   355,   795,   306,   711,   358,   249,   309,
    -109,  -109,  -109,   315,   304,   316,   318,   329,   317,   320,
    -109,   328,  -109,   333,  -109,   811,  -109,   322,   322,   322,
     -22,  -109,   310,   -26,   321,   556,   577,   598,   337,  -109,
    -109,   369,  -109,  -109,   322,   322,   322,   322,   384,   464,
     487,   510,   533,  -109,  -109,  -109,  -109
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    20,    22,    18,    23,    19,    21,    17,    26,    28,
      29,     0,     0,     0,     0,    69,     0,   122,   123,   114,
     113,     0,   124,   121,   125,     0,     0,     0,     0,     0,
       0,     2,     3,     5,     6,     8,     9,     0,    10,    11,
      12,    13,    14,    15,    16,     0,     4,   117,   119,    25,
      27,    30,     0,    33,     0,     0,    89,    90,     0,    91,
      88,     0,     0,     0,     0,     0,    70,    71,    39,    44,
       0,    77,     0,     0,     0,     0,     0,     0,     0,   133,
     134,     0,     1,     7,    31,     0,   118,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    94,    95,     0,    67,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    46,
     112,     0,     0,    76,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   150,     0,     0,   115,     0,   120,
       0,   142,   141,   140,   139,   147,   148,   143,   144,   145,
     146,   135,   136,   137,   138,    36,     0,     0,     0,     0,
     111,     0,   103,   102,   101,   100,   108,   109,   104,   105,
     106,   107,    96,    97,    98,    99,     0,    68,    49,    72,
      73,     0,     0,    38,    45,    40,     0,    78,    82,    83,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    32,
      24,     0,   126,     0,     0,     0,     0,    65,    66,     0,
       0,    92,    50,     0,     0,     0,    49,    47,     0,    84,
       0,     0,     0,     0,    80,   153,     0,     0,     0,     0,
       0,     0,   116,   149,    37,    35,     0,     0,   110,     0,
       0,     0,     0,     0,   127,     0,     0,    79,    86,     0,
       0,     0,     0,     0,   153,   132,   128,   129,   130,   131,
       0,    93,     0,    74,     0,    41,     0,     0,     0,    85,
       0,   154,   156,     0,   152,     0,    36,     0,    42,     0,
       0,    81,    87,     0,   151,     0,     0,     0,     0,     0,
      34,    75,    43,    89,     0,     0,     0,     0,     0,    52,
      53,     0,    51,    61,    62,    56,   155,     0,     0,     0,
      91,    48,     0,     0,     0,     0,     0,     0,     0,    54,
      60,     0,    55,    63,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    58,    59,    57,    64
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -109,  -109,  -109,  -109,  -109,   380,  -109,  -109,  -109,  -109,
    -109,   286,   128,  -109,  -109,  -109,  -109,  -109,  -109,  -109,
     190,  -109,  -109,    96,  -109,    97,  -109,   112,  -109,  -109,
    -109,  -109,  -109,  -109,  -109,  -109,  -109,   -71,   258,  -109,
     -14,    46,  -108,  -109,  -109,    86,    39,   196
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    30,    31,    32,    33,    34,    35,    36,    37,   146,
      38,    53,   215,    39,    69,   128,   252,   129,   195,   227,
     223,   308,   309,   310,   311,   312,   313,   314,    40,    41,
      42,    43,    67,   126,   189,   190,    44,    73,   230,   203,
     147,    65,   148,    46,    47,   236,   237,   263
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      64,   132,   330,    52,   165,   328,    71,     1,     2,     3,
       4,     5,     6,     7,     8,   100,   101,   186,     9,    10,
      11,    54,    12,    13,    14,   211,    15,   212,    16,   211,
     136,   221,    17,    18,    19,    20,    21,    22,    23,    24,
      25,   137,   138,   260,   261,    55,    45,   106,   107,   108,
      66,   133,   134,   135,   262,    68,    19,    20,    26,   130,
      70,    72,   331,   198,   199,    74,  -112,    75,    76,    77,
      78,   123,   124,    45,    45,    45,   166,    98,    99,   100,
     101,  -112,    27,   136,   255,   256,    48,    28,   228,   167,
     168,    82,    29,    84,   137,   138,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   247,   191,    79,    80,    81,   131,   275,   276,   134,
     135,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   209,    45,    45,    45,    45,    45,    45,    45,    45,
      45,    45,    45,    45,    45,    45,    45,    45,    86,   187,
     136,   169,   216,    85,    56,    57,    19,    20,    58,    59,
      60,   137,   138,   137,   138,   246,   121,   122,   123,   124,
     197,   102,   149,   150,   151,   152,   153,   154,   155,   156,
     157,   158,   159,   160,   161,   162,   163,   164,    94,    95,
      96,    97,    98,    99,   100,   101,   233,   242,   256,   234,
     244,   257,   103,   211,    61,   254,   248,   291,   104,    62,
     250,   188,   105,   110,    63,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,    49,
      50,    51,   270,   125,   127,   272,    17,    18,    19,    20,
      21,    22,    23,    24,   238,   239,   240,   241,   139,   140,
      45,    45,    45,    45,    45,   141,   211,   235,   271,    45,
     142,   143,    26,   290,    17,    18,    19,    20,    21,    22,
      23,    24,   145,   296,   192,   193,   196,   303,    57,    19,
      20,    58,    59,    60,   315,   200,    27,   201,   202,   204,
      26,    28,   205,   108,   206,   207,    29,   208,   210,   243,
     214,   222,   225,   325,   326,   327,   219,   226,   315,    45,
     229,   304,   305,   306,    27,   232,   245,   249,   251,    28,
     339,   340,   341,   342,    29,   258,   259,    61,   265,   266,
     267,    45,    62,   268,   269,   274,   278,   307,    56,    57,
      19,    20,    58,    59,    60,   277,   279,   288,   280,   284,
      56,    57,    19,    20,    58,    59,    60,    56,    57,    19,
      20,    58,   320,    60,   281,   282,   283,   289,   292,   293,
     297,   294,   304,   305,   306,   109,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,    61,   299,
     300,   302,   317,    62,   323,   316,   298,   322,    63,   -60,
      61,   321,   338,   324,   318,    62,   319,    61,   337,   331,
      63,   328,    62,    83,   295,   194,   253,    63,   329,   110,
     332,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   110,   333,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,     1,     2,     3,     4,     5,     6,     7,     8,   231,
     285,     0,     9,    10,    11,     0,    12,    13,    14,     0,
      15,     0,    16,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,    87,    25,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,     0,
       0,     0,     0,     0,     0,     0,   110,   144,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,     0,     0,     0,     0,     0,     0,     0,   110,
     170,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,     0,     0,     0,     0,     0,
       0,     0,   110,   343,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,     0,     0,
       0,     0,     0,     0,     0,   110,   344,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,     0,     0,     0,     0,     0,     0,     0,   110,   345,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,     0,     0,     0,     0,     0,     0,
       0,   110,   346,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,     0,     0,     0,
       0,     0,   110,   334,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,     0,     0,
       0,     0,     0,   110,   335,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,     0,
     264,     0,     0,     0,     0,   336,    87,     0,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   110,     0,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,     0,     0,
       0,     0,   110,   286,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,     0,     0,
     110,   217,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,     0,     0,   110,   218,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,     0,     0,   110,   273,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,     0,     0,     0,   301,    87,   213,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   110,   220,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   110,   224,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   110,   287,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
      87,     0,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   110,     0,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   117,   118,   119,   120,   121,   122,   123,   124
};

static const yytype_int16 yycheck[] =
{
      14,    72,    28,    33,    13,    27,    33,     3,     4,     5,
       6,     7,     8,     9,    10,    79,    80,   125,    14,    15,
      16,    33,    18,    19,    20,    87,    22,    89,    24,    87,
      69,    89,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    80,    81,    74,    75,    35,     0,    61,    62,    63,
      35,    37,    38,    39,    85,    33,    30,    31,    54,    33,
      88,    88,    88,   134,   135,    71,    88,    73,    74,    75,
      76,    79,    80,    27,    28,    29,    85,    77,    78,    79,
      80,    88,    78,    69,    86,    87,     0,    83,   196,   103,
     104,     0,    88,    33,    80,    81,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   219,   126,    27,    28,    29,    70,    86,    87,    38,
      39,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,   145,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,    84,    23,
      69,   105,   166,    88,    28,    29,    30,    31,    32,    33,
      34,    80,    81,    80,    81,    17,    77,    78,    79,    80,
      89,    66,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,    73,    74,
      75,    76,    77,    78,    79,    80,    87,   211,    87,    90,
     214,    90,    71,    87,    78,    89,   220,   278,    71,    83,
     224,    85,    88,    65,    88,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    11,
      12,    13,   246,    88,    71,   249,    28,    29,    30,    31,
      32,    33,    34,    35,   205,   206,   207,   208,    65,    34,
     204,   205,   206,   207,   208,    34,    87,    49,    89,   213,
      34,    34,    54,   277,    28,    29,    30,    31,    32,    33,
      34,    35,    71,   287,    33,    25,    87,    28,    29,    30,
      31,    32,    33,    34,   298,    85,    78,    81,    33,    85,
      54,    83,    85,   307,    85,    85,    88,    85,    84,   213,
      20,    33,    85,   317,   318,   319,    87,    85,   322,   263,
      33,    62,    63,    64,    78,    40,    84,    86,    33,    83,
     334,   335,   336,   337,    88,    33,    33,    78,    86,    86,
      86,   285,    83,    86,    86,    71,    69,    88,    28,    29,
      30,    31,    32,    33,    34,    86,    33,    33,    40,   263,
      28,    29,    30,    31,    32,    33,    34,    28,    29,    30,
      31,    32,    33,    34,    34,    34,    34,    33,    33,    87,
      71,   285,    62,    63,    64,    21,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    78,    34,
      84,    33,    88,    83,    66,    86,    26,    77,    88,    84,
      78,    84,    33,    70,    88,    83,    88,    78,    71,    88,
      88,    27,    83,    33,   286,   129,   226,    88,   322,    65,
     323,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    65,   324,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,     3,     4,     5,     6,     7,     8,     9,    10,   201,
     264,    -1,    14,    15,    16,    -1,    18,    19,    20,    -1,
      22,    -1,    24,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    65,    36,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    65,    89,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    65,
      89,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    65,    89,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    65,    89,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    65,    89,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    65,    89,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    -1,    -1,    -1,
      -1,    -1,    65,    87,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    -1,    -1,
      -1,    -1,    -1,    65,    87,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    -1,
      59,    -1,    -1,    -1,    -1,    87,    65,    -1,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    65,    -1,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    -1,    -1,
      -1,    -1,    65,    86,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    -1,    -1,
      65,    84,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    -1,    -1,    65,    84,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    -1,    -1,    65,    84,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    -1,    -1,    -1,    84,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      65,    -1,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    65,    -1,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    73,    74,    75,    76,    77,    78,    79,    80
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,     9,    10,    14,
      15,    16,    18,    19,    20,    22,    24,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    54,    78,    83,    88,
      92,    93,    94,    95,    96,    97,    98,    99,   101,   104,
     119,   120,   121,   122,   127,   132,   134,   135,   136,    11,
      12,    13,    33,   102,    33,    35,    28,    29,    32,    33,
      34,    78,    83,    88,   131,   132,    35,   123,    33,   105,
      88,    33,    88,   128,    71,    73,    74,    75,    76,   136,
     136,   136,     0,    96,    33,    88,    84,    65,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    66,    71,    71,    88,   131,   131,   131,    21,
      65,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    88,   124,    71,   106,   108,
      33,   132,   128,    37,    38,    39,    69,    80,    81,    65,
      34,    34,    34,    34,    89,    71,   100,   131,   133,   136,
     136,   136,   136,   136,   136,   136,   136,   136,   136,   136,
     136,   136,   136,   136,   136,    13,    85,   131,   131,   132,
      89,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   131,   131,   131,   131,   131,   133,    23,    85,   125,
     126,   131,    33,    25,   102,   109,    87,    89,   128,   128,
      85,    81,    33,   130,    85,    85,    85,    85,    85,   131,
      84,    87,    89,    66,    20,   103,   131,    84,    84,    87,
      66,    89,    33,   111,    66,    85,    85,   110,   133,    33,
     129,   129,    40,    87,    90,    49,   136,   137,   137,   137,
     137,   137,   131,   136,   131,    84,    17,   133,   131,    86,
     131,    33,   107,   111,    89,    86,    87,    90,    33,    33,
      74,    75,    85,   138,    59,    86,    86,    86,    86,    86,
     131,    89,   131,    84,    71,    86,    87,    86,    69,    33,
      40,    34,    34,    34,   136,   138,    86,    66,    33,    33,
     131,   128,    33,    87,   136,   103,   131,    71,    26,    34,
      84,    84,    33,    28,    62,    63,    64,    88,   112,   113,
     114,   115,   116,   117,   118,   131,    86,    88,    88,    88,
      33,    84,    77,    66,    70,   131,   131,   131,    27,   114,
      28,    88,   116,   118,    87,    87,    87,    71,    33,   131,
     131,   131,   131,    89,    89,    89,    89
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    91,    92,    93,    93,    94,    95,    95,    96,    96,
      96,    96,    96,    96,    96,    96,    96,    97,    97,    97,
      97,    97,    97,    97,    98,    99,    99,    99,    99,    99,
      99,   100,   100,   101,   102,   102,   103,   103,   104,   105,
     106,   106,   107,   107,   108,   108,   109,   109,   110,   111,
     111,   112,   112,   113,   113,   114,   115,   115,   115,   115,
     116,   116,   117,   117,   118,   119,   120,   121,   122,   123,
     123,   124,   124,   124,   125,   126,   127,   128,   128,   128,
     128,   128,   128,   128,   129,   129,   130,   130,   131,   131,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   131,   132,   132,   132,   133,   133,   134,   134,   135,
     135,   136,   136,   136,   136,   136,   136,   136,   136,   136,
     136,   136,   136,   136,   136,   136,   136,   136,   136,   136,
     136,   136,   136,   136,   136,   136,   136,   136,   136,   136,
     136,   137,   137,   138,   138,   138,   138
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     4,     2,     1,     2,     1,     1,
       2,     0,     2,     2,     9,     5,     0,     2,     4,     1,
       2,     5,     3,     5,     0,     2,     0,     2,     7,     0,
       1,     1,     1,     1,     3,     3,     1,     6,     6,     6,
       1,     1,     1,     3,     6,     5,     5,     3,     4,     0,
       1,     0,     2,     2,     4,     7,     3,     1,     3,     5,
       4,     7,     3,     3,     1,     3,     3,     5,     1,     1,
       1,     1,     4,     6,     2,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       5,     3,     1,     1,     1,     1,     3,     1,     2,     1,
       3,     1,     1,     1,     1,     1,     4,     6,     6,     6,
       6,     6,     6,     2,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     5,
       3,     4,     3,     0,     2,     5,     2
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (&yylloc, scanner, state, YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  unsigned res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location, scanner, state); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void* scanner, ParserState* state)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (yylocationp);
  YYUSE (scanner);
  YYUSE (state);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void* scanner, ParserState* state)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, scanner, state);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, void* scanner, ParserState* state)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       , scanner, state);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, scanner, state); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, void* scanner, ParserState* state)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (scanner);
  YYUSE (state);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yytype)
    {
          case 33: /* IDENTIFIER  */
#line 598 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).str); }
#line 2059 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 34: /* NUMBER  */
#line 607 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).number); }
#line 2065 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 35: /* LABEL_NAME  */
#line 598 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).str); }
#line 2071 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 100: /* constant_init  */
#line 605 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).expr); }
#line 2077 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 103: /* variable_init  */
#line 605 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).expr); }
#line 2083 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 107: /* substitutions  */
#line 600 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).substitutions); }
#line 2089 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 111: /* action  */
#line 598 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).str); }
#line 2095 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 112: /* outcomes  */
#line 601 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).outcomes); }
#line 2101 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 113: /* update_distribution  */
#line 601 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).outcomes); }
#line 2107 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 114: /* distribution_and_updates  */
#line 602 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).outcome); }
#line 2113 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 115: /* distribution  */
#line 603 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).dist); }
#line 2119 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 116: /* true_or_updates  */
#line 604 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).updates); }
#line 2125 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 117: /* updates  */
#line 604 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).updates); }
#line 2131 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 128: /* process_algebra  */
#line 606 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).process_algebra); }
#line 2137 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 129: /* action_set  */
#line 599 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).str_set); }
#line 2143 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 130: /* action_substitutions  */
#line 600 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).substitutions); }
#line 2149 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 131: /* expr  */
#line 605 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).expr); }
#line 2155 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 133: /* arguments  */
#line 608 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).arguments); }
#line 2161 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 136: /* property  */
#line 605 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).expr); }
#line 2167 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 137: /* path_property  */
#line 609 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).path); }
#line 2173 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 138: /* time_range  */
#line 610 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).time_range); }
#line 2179 "src/grammar.cc" /* yacc.c:1257  */
        break;


      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void* scanner, ParserState* state)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = yylloc;
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yyls1, yysize * sizeof (*yylsp),
                    &yystacksize);

        yyls = yyls1;
        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, &yylloc, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 615 "grammar.yy" /* yacc.c:1646  */
    { if (!state->success()) YYERROR; }
#line 2473 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 17:
#line 641 "grammar.yy" /* yacc.c:1646  */
    { SetModelType(yylloc, ModelType::MDP, state); }
#line 2479 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 18:
#line 643 "grammar.yy" /* yacc.c:1646  */
    { SetModelType(yylloc, ModelType::MDP, state); }
#line 2485 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 19:
#line 645 "grammar.yy" /* yacc.c:1646  */
    { SetModelType(yylloc, ModelType::DTMC, state); }
#line 2491 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 20:
#line 647 "grammar.yy" /* yacc.c:1646  */
    { SetModelType(yylloc, ModelType::DTMC, state); }
#line 2497 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 21:
#line 649 "grammar.yy" /* yacc.c:1646  */
    { SetModelType(yylloc, ModelType::CTMC, state); }
#line 2503 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 22:
#line 651 "grammar.yy" /* yacc.c:1646  */
    { SetModelType(yylloc, ModelType::CTMC, state); }
#line 2509 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 23:
#line 653 "grammar.yy" /* yacc.c:1646  */
    { SetModelType(yylloc, ModelType::GSMP, state); }
#line 2515 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 24:
#line 657 "grammar.yy" /* yacc.c:1646  */
    { AddConstant(yylloc, (yyvsp[-2].str), (yyvsp[-3].type), (yyvsp[-1].expr), state); }
#line 2521 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 25:
#line 661 "grammar.yy" /* yacc.c:1646  */
    { (yyval.type) = Type::INT; }
#line 2527 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 26:
#line 663 "grammar.yy" /* yacc.c:1646  */
    { (yyval.type) = Type::INT; }
#line 2533 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 27:
#line 665 "grammar.yy" /* yacc.c:1646  */
    { (yyval.type) = Type::DOUBLE; }
#line 2539 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 28:
#line 667 "grammar.yy" /* yacc.c:1646  */
    { (yyval.type) = Type::DOUBLE; }
#line 2545 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 29:
#line 669 "grammar.yy" /* yacc.c:1646  */
    { (yyval.type) = Type::DOUBLE; }
#line 2551 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 30:
#line 671 "grammar.yy" /* yacc.c:1646  */
    { (yyval.type) = Type::BOOL; }
#line 2557 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 31:
#line 675 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = nullptr; }
#line 2563 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 32:
#line 677 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 2569 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 34:
#line 684 "grammar.yy" /* yacc.c:1646  */
    { AddIntVariable(yylloc, (yyvsp[-8].str), (yyvsp[-5].expr), (yyvsp[-3].expr), (yyvsp[-1].expr), state); }
#line 2575 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 35:
#line 686 "grammar.yy" /* yacc.c:1646  */
    { AddBoolVariable(yylloc, (yyvsp[-4].str), (yyvsp[-1].expr), state); }
#line 2581 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 36:
#line 690 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = nullptr; }
#line 2587 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 37:
#line 692 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 2593 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 38:
#line 696 "grammar.yy" /* yacc.c:1646  */
    { EndModule(state); }
#line 2599 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 39:
#line 700 "grammar.yy" /* yacc.c:1646  */
    { StartModule(yylloc, (yyvsp[0].str), state); }
#line 2605 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 41:
#line 705 "grammar.yy" /* yacc.c:1646  */
    { AddFromModule(yylloc, (yyvsp[-3].str), (yyvsp[-1].substitutions), state); }
#line 2611 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 42:
#line 709 "grammar.yy" /* yacc.c:1646  */
    { (yyval.substitutions) = AddSubstitution(yylloc, (yyvsp[-2].str), (yyvsp[0].str), nullptr, state); }
#line 2617 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 43:
#line 711 "grammar.yy" /* yacc.c:1646  */
    { (yyval.substitutions) = AddSubstitution(yylloc, (yyvsp[-2].str), (yyvsp[0].str), (yyvsp[-4].substitutions), state); }
#line 2623 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 48:
#line 723 "grammar.yy" /* yacc.c:1646  */
    { AddCommand(yylloc, (yyvsp[-5].str), (yyvsp[-3].expr), (yyvsp[-1].outcomes), state); }
#line 2629 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 49:
#line 727 "grammar.yy" /* yacc.c:1646  */
    { (yyval.str) = new std::string(); }
#line 2635 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 51:
#line 732 "grammar.yy" /* yacc.c:1646  */
    { (yyval.outcomes) = AddOutcome(NewOutcome(nullptr, (yyvsp[0].updates)), nullptr); }
#line 2641 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 53:
#line 737 "grammar.yy" /* yacc.c:1646  */
    { (yyval.outcomes) = AddOutcome((yyvsp[0].outcome), nullptr); }
#line 2647 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 54:
#line 739 "grammar.yy" /* yacc.c:1646  */
    { (yyval.outcomes) = AddOutcome((yyvsp[0].outcome), (yyvsp[-2].outcomes)); }
#line 2653 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 55:
#line 743 "grammar.yy" /* yacc.c:1646  */
    { (yyval.outcome) = NewOutcome((yyvsp[-2].dist), (yyvsp[0].updates)); }
#line 2659 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 56:
#line 747 "grammar.yy" /* yacc.c:1646  */
    { (yyval.dist) = NewMemoryless((yyvsp[0].expr)); }
#line 2665 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 57:
#line 749 "grammar.yy" /* yacc.c:1646  */
    { (yyval.dist) = NewWeibull((yyvsp[-3].expr), (yyvsp[-1].expr)); }
#line 2671 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 58:
#line 751 "grammar.yy" /* yacc.c:1646  */
    { (yyval.dist) = NewLognormal((yyvsp[-3].expr), (yyvsp[-1].expr)); }
#line 2677 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 59:
#line 753 "grammar.yy" /* yacc.c:1646  */
    { (yyval.dist) = NewUniform((yyvsp[-3].expr), (yyvsp[-1].expr)); }
#line 2683 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 60:
#line 757 "grammar.yy" /* yacc.c:1646  */
    { (yyval.updates) = new std::vector<Update>(); }
#line 2689 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 62:
#line 762 "grammar.yy" /* yacc.c:1646  */
    { (yyval.updates) = AddUpdate((yyvsp[0].update), nullptr); }
#line 2695 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 63:
#line 764 "grammar.yy" /* yacc.c:1646  */
    { (yyval.updates) = AddUpdate((yyvsp[0].update), (yyvsp[-2].updates)); }
#line 2701 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 64:
#line 768 "grammar.yy" /* yacc.c:1646  */
    { (yyval.update) = NewUpdate((yyvsp[-4].str), (yyvsp[-1].expr)); }
#line 2707 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 65:
#line 772 "grammar.yy" /* yacc.c:1646  */
    { AddFormula(yylloc, (yyvsp[-3].str), (yyvsp[-1].expr), state); }
#line 2713 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 66:
#line 776 "grammar.yy" /* yacc.c:1646  */
    { AddLabel(yylloc, (yyvsp[-3].str), (yyvsp[-1].expr), state); }
#line 2719 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 67:
#line 780 "grammar.yy" /* yacc.c:1646  */
    { SetInit(yylloc, (yyvsp[-1].expr), state); }
#line 2725 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 68:
#line 784 "grammar.yy" /* yacc.c:1646  */
    { EndRewards(state); }
#line 2731 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 69:
#line 788 "grammar.yy" /* yacc.c:1646  */
    { StartRewards(yylloc, new std::string(), state); }
#line 2737 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 70:
#line 790 "grammar.yy" /* yacc.c:1646  */
    { StartRewards(yylloc, (yyvsp[0].str), state); }
#line 2743 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 74:
#line 799 "grammar.yy" /* yacc.c:1646  */
    { AddStateReward((yyvsp[-3].expr), (yyvsp[-1].expr), state); }
#line 2749 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 75:
#line 803 "grammar.yy" /* yacc.c:1646  */
    { AddTransitionReward(yylloc, (yyvsp[-5].str), (yyvsp[-3].expr), (yyvsp[-1].expr), state); }
#line 2755 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 76:
#line 807 "grammar.yy" /* yacc.c:1646  */
    { SetSystem(yylloc, (yyvsp[-1].process_algebra), state); }
#line 2761 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 77:
#line 811 "grammar.yy" /* yacc.c:1646  */
    { (yyval.process_algebra) = NewModuleIdentifier((yyvsp[0].str)); }
#line 2767 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 78:
#line 813 "grammar.yy" /* yacc.c:1646  */
    { (yyval.process_algebra) = (yyvsp[-1].process_algebra); }
#line 2773 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 79:
#line 815 "grammar.yy" /* yacc.c:1646  */
    { (yyval.process_algebra) = NewActionHiding((yyvsp[-1].str_set), (yyvsp[-4].process_algebra)); }
#line 2779 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 80:
#line 817 "grammar.yy" /* yacc.c:1646  */
    { (yyval.process_algebra) = NewActionRenaming((yyvsp[-1].substitutions), (yyvsp[-3].process_algebra)); }
#line 2785 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 81:
#line 819 "grammar.yy" /* yacc.c:1646  */
    { (yyval.process_algebra) = NewRestrictedParallelComposition((yyvsp[-3].str_set), (yyvsp[-6].process_algebra), (yyvsp[0].process_algebra)); }
#line 2791 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 82:
#line 821 "grammar.yy" /* yacc.c:1646  */
    { (yyval.process_algebra) = NewAlphabetizedParallelComposition((yyvsp[-2].process_algebra), (yyvsp[0].process_algebra)); }
#line 2797 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 83:
#line 823 "grammar.yy" /* yacc.c:1646  */
    { (yyval.process_algebra) = NewAsynchronousParallelComposition((yyvsp[-2].process_algebra), (yyvsp[0].process_algebra)); }
#line 2803 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 84:
#line 827 "grammar.yy" /* yacc.c:1646  */
    { (yyval.str_set) = AddAction((yyvsp[0].str), nullptr); }
#line 2809 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 85:
#line 829 "grammar.yy" /* yacc.c:1646  */
    { (yyval.str_set) = AddAction((yyvsp[0].str), (yyvsp[-2].str_set)); }
#line 2815 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 86:
#line 833 "grammar.yy" /* yacc.c:1646  */
    { (yyval.substitutions) = AddSubstitution(yylloc, (yyvsp[-2].str), (yyvsp[0].str), nullptr,
                                                state); }
#line 2822 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 87:
#line 836 "grammar.yy" /* yacc.c:1646  */
    { (yyval.substitutions) = AddSubstitution(yylloc, (yyvsp[-2].str), (yyvsp[0].str), (yyvsp[-4].substitutions), state); }
#line 2828 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 88:
#line 840 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLiteral((yyvsp[0].number)); }
#line 2834 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 89:
#line 842 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = new Literal(true); }
#line 2840 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 90:
#line 844 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = new Literal(false); }
#line 2846 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 91:
#line 846 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewIdentifier((yyvsp[0].str)); }
#line 2852 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 92:
#line 848 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewFunctionCall((yyvsp[-3].function), (yyvsp[-1].arguments)); }
#line 2858 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 93:
#line 850 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewFunctionCall((yyvsp[-3].function), (yyvsp[-1].arguments)); }
#line 2864 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 94:
#line 852 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNegate((yyvsp[0].expr)); }
#line 2870 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 95:
#line 854 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNot((yyvsp[0].expr)); }
#line 2876 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 96:
#line 856 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewPlus((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2882 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 97:
#line 858 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewMinus((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2888 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 98:
#line 860 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewMultiply((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2894 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 99:
#line 862 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewDivide((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2900 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 100:
#line 864 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewAnd((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2906 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 101:
#line 866 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewOr((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2912 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 102:
#line 868 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewImply((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2918 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 103:
#line 870 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewIff((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2924 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 104:
#line 872 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLess((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2930 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 105:
#line 874 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLessEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2936 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 106:
#line 876 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewGreaterEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2942 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 107:
#line 878 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewGreater((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2948 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 108:
#line 880 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2954 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 109:
#line 882 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNotEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2960 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 110:
#line 884 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewConditional((yyvsp[-4].expr), (yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2966 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 111:
#line 886 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[-1].expr); }
#line 2972 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 112:
#line 890 "grammar.yy" /* yacc.c:1646  */
    { (yyval.function) = MakeFunction(yylloc, (yyvsp[0].str), state); }
#line 2978 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 113:
#line 892 "grammar.yy" /* yacc.c:1646  */
    { (yyval.function) = Function::MIN; }
#line 2984 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 114:
#line 894 "grammar.yy" /* yacc.c:1646  */
    { (yyval.function) = Function::MAX; }
#line 2990 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 115:
#line 898 "grammar.yy" /* yacc.c:1646  */
    { (yyval.arguments) = AddArgument((yyvsp[0].expr), nullptr);  }
#line 2996 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 116:
#line 900 "grammar.yy" /* yacc.c:1646  */
    { (yyval.arguments) = AddArgument((yyvsp[0].expr), (yyvsp[-2].arguments)); }
#line 3002 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 119:
#line 908 "grammar.yy" /* yacc.c:1646  */
    { AddProperty((yyvsp[0].expr), state); }
#line 3008 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 120:
#line 910 "grammar.yy" /* yacc.c:1646  */
    { AddProperty((yyvsp[0].expr), state); }
#line 3014 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 121:
#line 914 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLiteral((yyvsp[0].number)); }
#line 3020 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 122:
#line 916 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = new Literal(true); }
#line 3026 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 123:
#line 918 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = new Literal(false); }
#line 3032 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 124:
#line 920 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewIdentifier((yyvsp[0].str)); }
#line 3038 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 125:
#line 922 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLabel((yyvsp[0].str)); }
#line 3044 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 126:
#line 924 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewFunctionCall((yyvsp[-3].function), (yyvsp[-1].arguments)); }
#line 3050 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 127:
#line 926 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewFunctionCall((yyvsp[-3].function), (yyvsp[-1].arguments)); }
#line 3056 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 128:
#line 928 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewProbabilityLess(Double((yyvsp[-3].number)), (yyvsp[-1].path)); }
#line 3062 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 129:
#line 930 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewProbabilityLessEqual(Double((yyvsp[-3].number)), (yyvsp[-1].path)); }
#line 3068 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 130:
#line 932 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewProbabilityGreaterEqual(Double((yyvsp[-3].number)), (yyvsp[-1].path)); }
#line 3074 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 131:
#line 934 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewProbabilityGreater(Double((yyvsp[-3].number)), (yyvsp[-1].path)); }
#line 3080 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 132:
#line 936 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewProbabilityEqual((yyvsp[-1].path)); }
#line 3086 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 133:
#line 938 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNegate((yyvsp[0].expr)); }
#line 3092 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 134:
#line 940 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNot((yyvsp[0].expr)); }
#line 3098 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 135:
#line 942 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewPlus((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3104 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 136:
#line 944 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewMinus((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3110 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 137:
#line 946 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewMultiply((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3116 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 138:
#line 948 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewDivide((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3122 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 139:
#line 950 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewAnd((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3128 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 140:
#line 952 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewOr((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3134 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 141:
#line 954 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewImply((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3140 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 142:
#line 956 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewIff((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3146 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 143:
#line 958 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLess((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3152 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 144:
#line 960 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLessEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3158 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 145:
#line 962 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewGreaterEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3164 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 146:
#line 964 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewGreater((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3170 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 147:
#line 966 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3176 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 148:
#line 968 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNotEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3182 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 149:
#line 970 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewConditional((yyvsp[-4].expr), (yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3188 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 150:
#line 972 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[-1].expr); }
#line 3194 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 151:
#line 976 "grammar.yy" /* yacc.c:1646  */
    { (yyval.path) = NewUntil((yyvsp[-1].time_range), (yyvsp[-3].expr), (yyvsp[0].expr)); }
#line 3200 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 152:
#line 978 "grammar.yy" /* yacc.c:1646  */
    { (yyval.path) = NewEventually((yyvsp[-1].time_range), (yyvsp[0].expr)); }
#line 3206 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 153:
#line 982 "grammar.yy" /* yacc.c:1646  */
    { (yyval.time_range) = new TimeRange(0, Infinity()); }
#line 3212 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 154:
#line 984 "grammar.yy" /* yacc.c:1646  */
    { (yyval.time_range) = new TimeRange(0, Double((yyvsp[0].number))); }
#line 3218 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 155:
#line 986 "grammar.yy" /* yacc.c:1646  */
    { (yyval.time_range) = new TimeRange(Double((yyvsp[-3].number)), Double((yyvsp[-1].number))); }
#line 3224 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 156:
#line 988 "grammar.yy" /* yacc.c:1646  */
    { (yyval.time_range) = new TimeRange(Double((yyvsp[0].number)), Infinity()); }
#line 3230 "src/grammar.cc" /* yacc.c:1646  */
    break;


#line 3234 "src/grammar.cc" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (&yylloc, scanner, state, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (&yylloc, scanner, state, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc, scanner, state);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[1] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, yylsp, scanner, state);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, scanner, state, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, scanner, state);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, yylsp, scanner, state);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 991 "grammar.yy" /* yacc.c:1906  */


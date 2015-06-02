/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

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
#define YYBISON_VERSION "3.0.2"

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
#include "ptrutil.h"
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
  return new Outcome((delay == nullptr)
                         ? MakeUnique<Memoryless>(MakeUnique<Literal>(1))
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

#line 556 "src/grammar.cc" /* yacc.c:339  */

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
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 557 "grammar.yy" /* yacc.c:355  */

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

#line 755 "src/grammar.cc" /* yacc.c:355  */
};
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

#line 783 "src/grammar.cc" /* yacc.c:358  */

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
       0,   609,   609,   613,   614,   617,   620,   621,   624,   625,
     626,   627,   628,   629,   630,   631,   632,   635,   637,   639,
     641,   643,   645,   647,   651,   655,   657,   659,   661,   663,
     665,   670,   671,   675,   678,   680,   685,   686,   690,   694,
     698,   699,   703,   705,   709,   710,   713,   714,   717,   722,
     723,   726,   728,   731,   733,   737,   741,   743,   745,   747,
     751,   753,   756,   758,   762,   766,   770,   774,   778,   783,
     784,   788,   789,   790,   793,   797,   801,   805,   807,   809,
     811,   813,   815,   817,   821,   823,   827,   830,   834,   836,
     838,   840,   842,   844,   846,   848,   850,   852,   854,   856,
     858,   860,   862,   864,   866,   868,   870,   872,   874,   876,
     878,   880,   884,   886,   888,   892,   894,   898,   899,   902,
     904,   908,   910,   912,   914,   916,   918,   920,   922,   924,
     926,   928,   930,   932,   934,   936,   938,   940,   942,   944,
     946,   948,   950,   952,   954,   956,   958,   960,   962,   964,
     966,   970,   972,   977,   978,   980,   982
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
#line 593 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).str); }
#line 2052 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 34: /* NUMBER  */
#line 602 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).number); }
#line 2058 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 35: /* LABEL_NAME  */
#line 593 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).str); }
#line 2064 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 100: /* constant_init  */
#line 600 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).expr); }
#line 2070 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 103: /* variable_init  */
#line 600 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).expr); }
#line 2076 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 107: /* substitutions  */
#line 595 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).substitutions); }
#line 2082 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 111: /* action  */
#line 593 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).str); }
#line 2088 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 112: /* outcomes  */
#line 596 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).outcomes); }
#line 2094 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 113: /* update_distribution  */
#line 596 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).outcomes); }
#line 2100 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 114: /* distribution_and_updates  */
#line 597 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).outcome); }
#line 2106 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 115: /* distribution  */
#line 598 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).dist); }
#line 2112 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 116: /* true_or_updates  */
#line 599 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).updates); }
#line 2118 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 117: /* updates  */
#line 599 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).updates); }
#line 2124 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 128: /* process_algebra  */
#line 601 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).process_algebra); }
#line 2130 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 129: /* action_set  */
#line 594 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).str_set); }
#line 2136 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 130: /* action_substitutions  */
#line 595 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).substitutions); }
#line 2142 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 131: /* expr  */
#line 600 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).expr); }
#line 2148 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 133: /* arguments  */
#line 603 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).arguments); }
#line 2154 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 136: /* property  */
#line 600 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).expr); }
#line 2160 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 137: /* path_property  */
#line 604 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).path); }
#line 2166 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 138: /* time_range  */
#line 605 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).time_range); }
#line 2172 "src/grammar.cc" /* yacc.c:1257  */
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
#line 610 "grammar.yy" /* yacc.c:1646  */
    { if (!state->success()) YYERROR; }
#line 2466 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 17:
#line 636 "grammar.yy" /* yacc.c:1646  */
    { SetModelType(yylloc, ModelType::MDP, state); }
#line 2472 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 18:
#line 638 "grammar.yy" /* yacc.c:1646  */
    { SetModelType(yylloc, ModelType::MDP, state); }
#line 2478 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 19:
#line 640 "grammar.yy" /* yacc.c:1646  */
    { SetModelType(yylloc, ModelType::DTMC, state); }
#line 2484 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 20:
#line 642 "grammar.yy" /* yacc.c:1646  */
    { SetModelType(yylloc, ModelType::DTMC, state); }
#line 2490 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 21:
#line 644 "grammar.yy" /* yacc.c:1646  */
    { SetModelType(yylloc, ModelType::CTMC, state); }
#line 2496 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 22:
#line 646 "grammar.yy" /* yacc.c:1646  */
    { SetModelType(yylloc, ModelType::CTMC, state); }
#line 2502 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 23:
#line 648 "grammar.yy" /* yacc.c:1646  */
    { SetModelType(yylloc, ModelType::GSMP, state); }
#line 2508 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 24:
#line 652 "grammar.yy" /* yacc.c:1646  */
    { AddConstant(yylloc, (yyvsp[-2].str), (yyvsp[-3].type), (yyvsp[-1].expr), state); }
#line 2514 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 25:
#line 656 "grammar.yy" /* yacc.c:1646  */
    { (yyval.type) = Type::INT; }
#line 2520 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 26:
#line 658 "grammar.yy" /* yacc.c:1646  */
    { (yyval.type) = Type::INT; }
#line 2526 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 27:
#line 660 "grammar.yy" /* yacc.c:1646  */
    { (yyval.type) = Type::DOUBLE; }
#line 2532 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 28:
#line 662 "grammar.yy" /* yacc.c:1646  */
    { (yyval.type) = Type::DOUBLE; }
#line 2538 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 29:
#line 664 "grammar.yy" /* yacc.c:1646  */
    { (yyval.type) = Type::DOUBLE; }
#line 2544 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 30:
#line 666 "grammar.yy" /* yacc.c:1646  */
    { (yyval.type) = Type::BOOL; }
#line 2550 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 31:
#line 670 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = nullptr; }
#line 2556 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 32:
#line 672 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 2562 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 34:
#line 679 "grammar.yy" /* yacc.c:1646  */
    { AddIntVariable(yylloc, (yyvsp[-8].str), (yyvsp[-5].expr), (yyvsp[-3].expr), (yyvsp[-1].expr), state); }
#line 2568 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 35:
#line 681 "grammar.yy" /* yacc.c:1646  */
    { AddBoolVariable(yylloc, (yyvsp[-4].str), (yyvsp[-1].expr), state); }
#line 2574 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 36:
#line 685 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = nullptr; }
#line 2580 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 37:
#line 687 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 2586 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 38:
#line 691 "grammar.yy" /* yacc.c:1646  */
    { EndModule(state); }
#line 2592 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 39:
#line 695 "grammar.yy" /* yacc.c:1646  */
    { StartModule(yylloc, (yyvsp[0].str), state); }
#line 2598 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 41:
#line 700 "grammar.yy" /* yacc.c:1646  */
    { AddFromModule(yylloc, (yyvsp[-3].str), (yyvsp[-1].substitutions), state); }
#line 2604 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 42:
#line 704 "grammar.yy" /* yacc.c:1646  */
    { (yyval.substitutions) = AddSubstitution(yylloc, (yyvsp[-2].str), (yyvsp[0].str), nullptr, state); }
#line 2610 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 43:
#line 706 "grammar.yy" /* yacc.c:1646  */
    { (yyval.substitutions) = AddSubstitution(yylloc, (yyvsp[-2].str), (yyvsp[0].str), (yyvsp[-4].substitutions), state); }
#line 2616 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 48:
#line 718 "grammar.yy" /* yacc.c:1646  */
    { AddCommand(yylloc, (yyvsp[-5].str), (yyvsp[-3].expr), (yyvsp[-1].outcomes), state); }
#line 2622 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 49:
#line 722 "grammar.yy" /* yacc.c:1646  */
    { (yyval.str) = new std::string(); }
#line 2628 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 51:
#line 727 "grammar.yy" /* yacc.c:1646  */
    { (yyval.outcomes) = AddOutcome(NewOutcome(nullptr, (yyvsp[0].updates)), nullptr); }
#line 2634 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 53:
#line 732 "grammar.yy" /* yacc.c:1646  */
    { (yyval.outcomes) = AddOutcome((yyvsp[0].outcome), nullptr); }
#line 2640 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 54:
#line 734 "grammar.yy" /* yacc.c:1646  */
    { (yyval.outcomes) = AddOutcome((yyvsp[0].outcome), (yyvsp[-2].outcomes)); }
#line 2646 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 55:
#line 738 "grammar.yy" /* yacc.c:1646  */
    { (yyval.outcome) = NewOutcome((yyvsp[-2].dist), (yyvsp[0].updates)); }
#line 2652 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 56:
#line 742 "grammar.yy" /* yacc.c:1646  */
    { (yyval.dist) = NewMemoryless((yyvsp[0].expr)); }
#line 2658 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 57:
#line 744 "grammar.yy" /* yacc.c:1646  */
    { (yyval.dist) = NewWeibull((yyvsp[-3].expr), (yyvsp[-1].expr)); }
#line 2664 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 58:
#line 746 "grammar.yy" /* yacc.c:1646  */
    { (yyval.dist) = NewLognormal((yyvsp[-3].expr), (yyvsp[-1].expr)); }
#line 2670 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 59:
#line 748 "grammar.yy" /* yacc.c:1646  */
    { (yyval.dist) = NewUniform((yyvsp[-3].expr), (yyvsp[-1].expr)); }
#line 2676 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 60:
#line 752 "grammar.yy" /* yacc.c:1646  */
    { (yyval.updates) = new std::vector<Update>(); }
#line 2682 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 62:
#line 757 "grammar.yy" /* yacc.c:1646  */
    { (yyval.updates) = AddUpdate((yyvsp[0].update), nullptr); }
#line 2688 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 63:
#line 759 "grammar.yy" /* yacc.c:1646  */
    { (yyval.updates) = AddUpdate((yyvsp[0].update), (yyvsp[-2].updates)); }
#line 2694 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 64:
#line 763 "grammar.yy" /* yacc.c:1646  */
    { (yyval.update) = NewUpdate((yyvsp[-4].str), (yyvsp[-1].expr)); }
#line 2700 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 65:
#line 767 "grammar.yy" /* yacc.c:1646  */
    { AddFormula(yylloc, (yyvsp[-3].str), (yyvsp[-1].expr), state); }
#line 2706 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 66:
#line 771 "grammar.yy" /* yacc.c:1646  */
    { AddLabel(yylloc, (yyvsp[-3].str), (yyvsp[-1].expr), state); }
#line 2712 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 67:
#line 775 "grammar.yy" /* yacc.c:1646  */
    { SetInit(yylloc, (yyvsp[-1].expr), state); }
#line 2718 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 68:
#line 779 "grammar.yy" /* yacc.c:1646  */
    { EndRewards(state); }
#line 2724 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 69:
#line 783 "grammar.yy" /* yacc.c:1646  */
    { StartRewards(yylloc, new std::string(), state); }
#line 2730 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 70:
#line 785 "grammar.yy" /* yacc.c:1646  */
    { StartRewards(yylloc, (yyvsp[0].str), state); }
#line 2736 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 74:
#line 794 "grammar.yy" /* yacc.c:1646  */
    { AddStateReward((yyvsp[-3].expr), (yyvsp[-1].expr), state); }
#line 2742 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 75:
#line 798 "grammar.yy" /* yacc.c:1646  */
    { AddTransitionReward(yylloc, (yyvsp[-5].str), (yyvsp[-3].expr), (yyvsp[-1].expr), state); }
#line 2748 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 76:
#line 802 "grammar.yy" /* yacc.c:1646  */
    { SetSystem(yylloc, (yyvsp[-1].process_algebra), state); }
#line 2754 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 77:
#line 806 "grammar.yy" /* yacc.c:1646  */
    { (yyval.process_algebra) = NewModuleIdentifier((yyvsp[0].str)); }
#line 2760 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 78:
#line 808 "grammar.yy" /* yacc.c:1646  */
    { (yyval.process_algebra) = (yyvsp[-1].process_algebra); }
#line 2766 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 79:
#line 810 "grammar.yy" /* yacc.c:1646  */
    { (yyval.process_algebra) = NewActionHiding((yyvsp[-1].str_set), (yyvsp[-4].process_algebra)); }
#line 2772 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 80:
#line 812 "grammar.yy" /* yacc.c:1646  */
    { (yyval.process_algebra) = NewActionRenaming((yyvsp[-1].substitutions), (yyvsp[-3].process_algebra)); }
#line 2778 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 81:
#line 814 "grammar.yy" /* yacc.c:1646  */
    { (yyval.process_algebra) = NewRestrictedParallelComposition((yyvsp[-3].str_set), (yyvsp[-6].process_algebra), (yyvsp[0].process_algebra)); }
#line 2784 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 82:
#line 816 "grammar.yy" /* yacc.c:1646  */
    { (yyval.process_algebra) = NewAlphabetizedParallelComposition((yyvsp[-2].process_algebra), (yyvsp[0].process_algebra)); }
#line 2790 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 83:
#line 818 "grammar.yy" /* yacc.c:1646  */
    { (yyval.process_algebra) = NewAsynchronousParallelComposition((yyvsp[-2].process_algebra), (yyvsp[0].process_algebra)); }
#line 2796 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 84:
#line 822 "grammar.yy" /* yacc.c:1646  */
    { (yyval.str_set) = AddAction((yyvsp[0].str), nullptr); }
#line 2802 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 85:
#line 824 "grammar.yy" /* yacc.c:1646  */
    { (yyval.str_set) = AddAction((yyvsp[0].str), (yyvsp[-2].str_set)); }
#line 2808 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 86:
#line 828 "grammar.yy" /* yacc.c:1646  */
    { (yyval.substitutions) = AddSubstitution(yylloc, (yyvsp[-2].str), (yyvsp[0].str), nullptr,
                                                state); }
#line 2815 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 87:
#line 831 "grammar.yy" /* yacc.c:1646  */
    { (yyval.substitutions) = AddSubstitution(yylloc, (yyvsp[-2].str), (yyvsp[0].str), (yyvsp[-4].substitutions), state); }
#line 2821 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 88:
#line 835 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLiteral((yyvsp[0].number)); }
#line 2827 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 89:
#line 837 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = new Literal(true); }
#line 2833 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 90:
#line 839 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = new Literal(false); }
#line 2839 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 91:
#line 841 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewIdentifier((yyvsp[0].str)); }
#line 2845 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 92:
#line 843 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewFunctionCall((yyvsp[-3].function), (yyvsp[-1].arguments)); }
#line 2851 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 93:
#line 845 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewFunctionCall((yyvsp[-3].function), (yyvsp[-1].arguments)); }
#line 2857 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 94:
#line 847 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNegate((yyvsp[0].expr)); }
#line 2863 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 95:
#line 849 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNot((yyvsp[0].expr)); }
#line 2869 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 96:
#line 851 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewPlus((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2875 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 97:
#line 853 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewMinus((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2881 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 98:
#line 855 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewMultiply((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2887 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 99:
#line 857 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewDivide((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2893 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 100:
#line 859 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewAnd((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2899 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 101:
#line 861 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewOr((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2905 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 102:
#line 863 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewImply((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2911 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 103:
#line 865 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewIff((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2917 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 104:
#line 867 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLess((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2923 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 105:
#line 869 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLessEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2929 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 106:
#line 871 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewGreaterEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2935 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 107:
#line 873 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewGreater((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2941 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 108:
#line 875 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2947 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 109:
#line 877 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNotEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2953 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 110:
#line 879 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewConditional((yyvsp[-4].expr), (yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2959 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 111:
#line 881 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[-1].expr); }
#line 2965 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 112:
#line 885 "grammar.yy" /* yacc.c:1646  */
    { (yyval.function) = MakeFunction(yylloc, (yyvsp[0].str), state); }
#line 2971 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 113:
#line 887 "grammar.yy" /* yacc.c:1646  */
    { (yyval.function) = Function::MIN; }
#line 2977 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 114:
#line 889 "grammar.yy" /* yacc.c:1646  */
    { (yyval.function) = Function::MAX; }
#line 2983 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 115:
#line 893 "grammar.yy" /* yacc.c:1646  */
    { (yyval.arguments) = AddArgument((yyvsp[0].expr), nullptr);  }
#line 2989 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 116:
#line 895 "grammar.yy" /* yacc.c:1646  */
    { (yyval.arguments) = AddArgument((yyvsp[0].expr), (yyvsp[-2].arguments)); }
#line 2995 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 119:
#line 903 "grammar.yy" /* yacc.c:1646  */
    { AddProperty((yyvsp[0].expr), state); }
#line 3001 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 120:
#line 905 "grammar.yy" /* yacc.c:1646  */
    { AddProperty((yyvsp[0].expr), state); }
#line 3007 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 121:
#line 909 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLiteral((yyvsp[0].number)); }
#line 3013 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 122:
#line 911 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = new Literal(true); }
#line 3019 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 123:
#line 913 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = new Literal(false); }
#line 3025 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 124:
#line 915 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewIdentifier((yyvsp[0].str)); }
#line 3031 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 125:
#line 917 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLabel((yyvsp[0].str)); }
#line 3037 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 126:
#line 919 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewFunctionCall((yyvsp[-3].function), (yyvsp[-1].arguments)); }
#line 3043 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 127:
#line 921 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewFunctionCall((yyvsp[-3].function), (yyvsp[-1].arguments)); }
#line 3049 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 128:
#line 923 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewProbabilityLess(Double((yyvsp[-3].number)), (yyvsp[-1].path)); }
#line 3055 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 129:
#line 925 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewProbabilityLessEqual(Double((yyvsp[-3].number)), (yyvsp[-1].path)); }
#line 3061 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 130:
#line 927 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewProbabilityGreaterEqual(Double((yyvsp[-3].number)), (yyvsp[-1].path)); }
#line 3067 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 131:
#line 929 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewProbabilityGreater(Double((yyvsp[-3].number)), (yyvsp[-1].path)); }
#line 3073 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 132:
#line 931 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewProbabilityEqual((yyvsp[-1].path)); }
#line 3079 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 133:
#line 933 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNegate((yyvsp[0].expr)); }
#line 3085 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 134:
#line 935 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNot((yyvsp[0].expr)); }
#line 3091 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 135:
#line 937 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewPlus((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3097 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 136:
#line 939 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewMinus((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3103 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 137:
#line 941 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewMultiply((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3109 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 138:
#line 943 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewDivide((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3115 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 139:
#line 945 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewAnd((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3121 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 140:
#line 947 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewOr((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3127 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 141:
#line 949 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewImply((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3133 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 142:
#line 951 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewIff((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3139 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 143:
#line 953 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLess((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3145 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 144:
#line 955 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLessEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3151 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 145:
#line 957 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewGreaterEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3157 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 146:
#line 959 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewGreater((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3163 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 147:
#line 961 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3169 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 148:
#line 963 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNotEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3175 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 149:
#line 965 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewConditional((yyvsp[-4].expr), (yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3181 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 150:
#line 967 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[-1].expr); }
#line 3187 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 151:
#line 971 "grammar.yy" /* yacc.c:1646  */
    { (yyval.path) = NewUntil((yyvsp[-1].time_range), (yyvsp[-3].expr), (yyvsp[0].expr)); }
#line 3193 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 152:
#line 973 "grammar.yy" /* yacc.c:1646  */
    { (yyval.path) = NewEventually((yyvsp[-1].time_range), (yyvsp[0].expr)); }
#line 3199 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 153:
#line 977 "grammar.yy" /* yacc.c:1646  */
    { (yyval.time_range) = new TimeRange(0, Infinity()); }
#line 3205 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 154:
#line 979 "grammar.yy" /* yacc.c:1646  */
    { (yyval.time_range) = new TimeRange(0, Double((yyvsp[0].number))); }
#line 3211 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 155:
#line 981 "grammar.yy" /* yacc.c:1646  */
    { (yyval.time_range) = new TimeRange(Double((yyvsp[-3].number)), Double((yyvsp[-1].number))); }
#line 3217 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 156:
#line 983 "grammar.yy" /* yacc.c:1646  */
    { (yyval.time_range) = new TimeRange(Double((yyvsp[0].number)), Infinity()); }
#line 3223 "src/grammar.cc" /* yacc.c:1646  */
    break;


#line 3227 "src/grammar.cc" /* yacc.c:1646  */
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
#line 986 "grammar.yy" /* yacc.c:1906  */


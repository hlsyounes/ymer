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
#include <string>
#include <vector>

#include "distribution.h"
#include "expression.h"
#include "model.h"
#include "parser-state.h"
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

template <typename T>
std::unique_ptr<T> WrapUnique(T* ptr) {
  return std::unique_ptr<T>(ptr);
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
  return new Outcome(
      (delay == nullptr) ? Memoryless::New(Literal::New(1)) : WrapUnique(delay),
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

void AddProperty(const Expression* property, ParserState* state) {
  state->add_property(WrapUnique(property));
}

}  // namespace

#line 507 "src/grammar.cc" /* yacc.c:339  */

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
    A = 296,
    C = 297,
    E = 298,
    F = 299,
    G = 300,
    I = 301,
    PMAX = 302,
    PMIN = 303,
    P = 304,
    RMAX = 305,
    RMIN = 306,
    R = 307,
    S = 308,
    U = 309,
    W = 310,
    X = 311,
    L = 312,
    IFF_TOKEN = 313,
    IMPLY_TOKEN = 314,
    NEQ = 315,
    LEQ = 316,
    GEQ = 317,
    UMINUS = 318
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
#define A 296
#define C 297
#define E 298
#define F 299
#define G 300
#define I 301
#define PMAX 302
#define PMIN 303
#define P 304
#define RMAX 305
#define RMIN 306
#define R 307
#define S 308
#define U 309
#define W 310
#define X 311
#define L 312
#define IFF_TOKEN 313
#define IMPLY_TOKEN 314
#define NEQ 315
#define LEQ 316
#define GEQ 317
#define UMINUS 318

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 506 "grammar.yy" /* yacc.c:355  */

  Type type;
  const std::string* str;
  std::map<std::string, std::string>* substitutions;
  std::vector<Outcome>* outcomes;
  Outcome* outcome;
  const Distribution* dist;
  std::vector<Update>* updates;
  Update* update;
  const Expression* expr;
  const TypedValue* number;
  Function function;
  UniquePtrVector<const Expression>* arguments;
  const PathProperty* path;
  const TimeRange* time_range;

#line 690 "src/grammar.cc" /* yacc.c:355  */
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

#line 718 "src/grammar.cc" /* yacc.c:358  */

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
#define YYLAST   990

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  84
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  48
/* YYNRULES -- Number of rules.  */
#define YYNRULES  156
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  347

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   318

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    76,     2,     2,     2,     2,    63,     2,
      81,    82,    72,    70,    80,    71,     2,    73,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    59,    77,
      66,    64,    69,    58,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    78,     2,    79,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    74,    62,    83,     2,     2,     2,     2,
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
      55,    56,    57,    60,    61,    65,    67,    68,    75
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   552,   552,   556,   557,   560,   563,   564,   567,   568,
     569,   570,   571,   572,   573,   574,   575,   578,   580,   582,
     584,   586,   588,   590,   594,   598,   600,   602,   604,   606,
     608,   613,   614,   618,   621,   623,   628,   629,   633,   637,
     641,   642,   646,   648,   652,   653,   656,   657,   660,   665,
     666,   669,   671,   674,   676,   680,   684,   686,   688,   690,
     694,   696,   699,   701,   705,   709,   713,   717,   721,   726,
     727,   731,   732,   733,   736,   740,   744,   747,   749,   750,
     751,   753,   754,   755,   758,   760,   764,   767,   771,   773,
     775,   777,   779,   781,   783,   785,   787,   789,   791,   793,
     795,   797,   799,   801,   803,   805,   807,   809,   811,   813,
     815,   817,   821,   823,   825,   829,   831,   835,   836,   839,
     841,   845,   847,   849,   851,   853,   855,   857,   859,   861,
     863,   865,   867,   869,   871,   873,   875,   877,   879,   881,
     883,   885,   887,   889,   891,   893,   895,   897,   899,   901,
     903,   907,   909,   914,   915,   917,   919
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
  "ENDSYSTEM", "DOUBLE_BAR", "TRIPLE_BAR", "BACK_ARROW", "A", "C", "E",
  "F", "G", "I", "PMAX", "PMIN", "P", "RMAX", "RMIN", "R", "S", "U", "W",
  "X", "L", "'?'", "':'", "IFF_TOKEN", "IMPLY_TOKEN", "'|'", "'&'", "'='",
  "NEQ", "'<'", "LEQ", "GEQ", "'>'", "'+'", "'-'", "'*'", "'/'", "'{'",
  "UMINUS", "'!'", "';'", "'['", "']'", "','", "'('", "')'", "'}'",
  "$accept", "file", "model_or_properties", "model", "model_components",
  "model_component", "model_type", "constant", "constant_type",
  "constant_init", "global", "variable", "variable_init", "module",
  "module_name", "module_def", "substitutions", "variables", "commands",
  "command", "action", "outcomes", "update_distribution",
  "distribution_and_updates", "distribution", "true_or_updates", "updates",
  "update", "formula", "label", "init", "rewards", "rewards_label",
  "reward_rules", "state_reward", "transition_reward", "system",
  "process_algebra", "action_list", "action_substitutions", "expr",
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
     305,   306,   307,   308,   309,   310,   311,   312,    63,    58,
     313,   314,   124,    38,    61,   315,    60,   316,   317,    62,
      43,    45,    42,    47,   123,   318,    33,    59,    91,    93,
      44,    40,    41,   125
};
# endif

#define YYPACT_NINF -147

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-147)))

#define YYTABLE_NINF -113

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
       6,  -147,  -147,  -147,  -147,  -147,  -147,  -147,    72,  -147,
    -147,   -10,    -2,    30,   367,    37,    43,  -147,  -147,  -147,
    -147,    10,    14,  -147,  -147,   -29,     2,   345,   345,   345,
     130,  -147,  -147,   484,  -147,  -147,  -147,   133,  -147,  -147,
    -147,  -147,  -147,  -147,  -147,   123,  -147,   150,   840,  -147,
    -147,  -147,   169,  -147,   165,   166,  -147,  -147,   155,    14,
    -147,   367,   367,   367,   396,   156,  -147,  -147,  -147,   167,
      20,  -147,   -29,    55,   180,   213,   214,   215,   222,  -147,
    -147,   461,  -147,  -147,   194,   367,   345,   345,   345,   345,
     345,   345,   345,   345,   345,   345,   345,   345,   345,   345,
     345,   345,   -11,   367,   367,    20,  -147,  -147,   486,  -147,
     367,   367,   367,   367,   367,   367,   367,   367,   367,   367,
     367,   367,   367,   367,   367,   367,   127,   228,   237,   -10,
    -147,   184,   195,  -147,   -29,   -29,   187,   192,   234,   193,
     196,   197,   198,   200,  -147,   367,   202,   856,   -74,   840,
     776,   869,   894,   364,    54,   122,   122,     8,     8,     8,
       8,   -28,   -28,  -147,  -147,   250,   367,   684,   702,   201,
    -147,   792,   882,   906,   917,   445,   469,   469,    92,    92,
      92,    92,    76,    76,  -147,  -147,   -53,  -147,   239,  -147,
    -147,   808,   205,  -147,  -147,   207,   367,  -147,   -55,   -55,
     247,   247,   246,    87,   211,   211,   211,   211,   211,   856,
    -147,   367,  -147,   345,   367,   212,   151,  -147,  -147,   367,
     367,  -147,  -147,   209,   367,   257,   239,  -147,   -26,  -147,
      74,   116,   258,   261,  -147,   -35,   722,   216,   217,   218,
     219,   221,   856,   840,   856,  -147,   367,    89,   856,   367,
     738,   238,   146,   227,  -147,   245,   268,  -147,  -147,   277,
     284,   285,   290,   345,   -35,  -147,  -147,  -147,  -147,  -147,
     664,  -147,   824,  -147,   292,  -147,   293,   367,   -29,  -147,
     294,  -147,  -147,   248,   840,   345,   250,   367,  -147,   265,
     413,   128,  -147,   296,   840,   255,   756,   300,   282,   256,
    -147,  -147,  -147,   270,   253,   267,   269,   374,   274,   286,
    -147,   279,  -147,   289,  -147,   856,  -147,   367,   367,   367,
     -24,  -147,   312,   -23,   273,   601,   622,   643,   291,  -147,
    -147,   324,  -147,  -147,   367,   367,   367,   367,   332,   509,
     532,   555,   578,  -147,  -147,  -147,  -147
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
       0,     0,     0,    59,    57,    58,    64
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -147,  -147,  -147,  -147,  -147,   327,  -147,  -147,  -147,  -147,
    -147,   232,    78,  -147,  -147,  -147,  -147,  -147,  -147,  -147,
     136,  -147,  -147,    48,  -147,    42,  -147,    44,  -147,  -147,
    -147,  -147,  -147,  -147,  -147,  -147,  -147,   -71,   171,  -147,
     -14,    46,  -108,  -147,  -147,    86,  -146,   117
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
      64,   132,   165,   328,    71,   330,   211,   136,   212,     1,
       2,     3,     4,     5,     6,     7,     8,   186,   137,   138,
       9,    10,    11,    52,    12,    13,    14,   211,    15,   221,
      16,    54,   260,   261,    17,    18,    19,    20,    21,    22,
      23,    24,    25,   262,   100,   101,    45,   106,   107,   108,
      19,    20,    72,   130,   211,    26,   254,  -112,   331,   238,
     239,   240,   241,   198,   199,    55,    74,   166,    75,    76,
      77,    78,    66,    45,    45,    45,    68,    27,    98,    99,
     100,   101,    28,    49,    50,    51,    48,    29,   228,   167,
     168,    70,   133,   134,   135,  -112,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   247,   191,    79,    80,    81,   131,   136,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   137,   138,
      82,   209,    45,    45,    45,    45,    45,    45,    45,    45,
      45,    45,    45,    45,    45,    45,    45,    45,   123,   124,
     187,   169,   216,   255,   256,    56,    57,    19,    20,    58,
      59,    60,   121,   122,   123,   124,    84,   233,   246,   211,
     234,   271,   149,   150,   151,   152,   153,   154,   155,   156,
     157,   158,   159,   160,   161,   162,   163,   164,    94,    95,
      96,    97,    98,    99,   100,   101,   256,   242,    61,   257,
     244,   137,   138,    62,    85,   188,   248,   291,    63,   110,
     250,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   275,   276,    86,   102,   103,
     104,   127,   270,   134,   135,   272,   105,   125,   139,    17,
      18,    19,    20,    21,    22,    23,    24,   140,   141,   142,
      45,    45,    45,    45,    45,   235,   143,   136,   145,    45,
      26,   192,   193,   290,   196,   200,   201,   202,   137,   138,
     214,   204,   222,   296,   205,   206,   207,   197,   208,   210,
     229,   219,    27,   225,   315,   226,   232,    28,   249,   245,
     251,   258,    29,   108,   259,   265,   266,   267,   268,   243,
     269,   279,   274,   325,   326,   327,   277,   278,   315,    45,
     303,    57,    19,    20,    58,    59,    60,   280,   281,   282,
     339,   340,   341,   342,   283,   288,   289,   292,   293,   297,
     299,    45,   300,   302,   317,   316,   304,   305,   323,   306,
      56,    57,    19,    20,    58,    59,    60,   -60,   318,   284,
     319,   321,   324,    61,   331,   337,   322,   338,    62,   328,
      83,   194,   253,   307,   295,   332,   304,   305,   333,   306,
     329,   294,   231,    17,    18,    19,    20,    21,    22,    23,
      24,   285,     0,    61,     0,     0,     0,     0,    62,     0,
       0,     0,     0,    63,    26,    56,    57,    19,    20,    58,
      59,    60,    56,    57,    19,    20,    58,   320,    60,     0,
       0,     0,     0,     0,     0,     0,    27,   109,     0,     0,
       0,    28,     0,     0,     0,     0,    29,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,    61,   298,
       0,     0,     0,    62,     0,    61,     0,     0,    63,     0,
      62,     0,     0,     0,   110,    63,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
       0,   110,     0,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,     1,     2,     3,
       4,     5,     6,     7,     8,     0,     0,     0,     9,    10,
      11,     0,    12,    13,    14,     0,    15,     0,    16,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,    87,
      25,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   117,   118,   119,   120,   121,
     122,   123,   124,   144,   110,     0,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
       0,     0,     0,     0,     0,     0,     0,   110,   170,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,     0,     0,     0,     0,     0,     0,     0,
     110,   343,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,     0,     0,     0,     0,
       0,     0,     0,   110,   344,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,     0,
       0,     0,     0,     0,     0,     0,   110,   345,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,     0,     0,     0,     0,     0,     0,     0,   110,
     346,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,     0,     0,     0,     0,     0,
     110,   334,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,     0,     0,     0,     0,
       0,   110,   335,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,     0,     0,     0,
       0,     0,   110,   336,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,     0,     0,
       0,     0,   110,   286,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,     0,     0,
     110,   217,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   264,     0,     0,   218,
      87,     0,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   110,     0,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,     0,     0,   110,   273,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
       0,     0,     0,   301,    87,   213,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     110,   220,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   110,   224,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   110,   287,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,    87,     0,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   110,     0,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124
};

static const yytype_int16 yycheck[] =
{
      14,    72,    13,    27,    33,    28,    80,    62,    82,     3,
       4,     5,     6,     7,     8,     9,    10,   125,    73,    74,
      14,    15,    16,    33,    18,    19,    20,    80,    22,    82,
      24,    33,    67,    68,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    78,    72,    73,     0,    61,    62,    63,
      30,    31,    81,    33,    80,    49,    82,    81,    81,   205,
     206,   207,   208,   134,   135,    35,    64,    78,    66,    67,
      68,    69,    35,    27,    28,    29,    33,    71,    70,    71,
      72,    73,    76,    11,    12,    13,     0,    81,   196,   103,
     104,    81,    37,    38,    39,    81,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   219,   126,    27,    28,    29,    70,    62,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    73,    74,
       0,   145,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,    72,    73,
      23,   105,   166,    79,    80,    28,    29,    30,    31,    32,
      33,    34,    70,    71,    72,    73,    33,    80,    17,    80,
      83,    82,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,    66,    67,
      68,    69,    70,    71,    72,    73,    80,   211,    71,    83,
     214,    73,    74,    76,    81,    78,   220,   278,    81,    58,
     224,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    79,    80,    77,    59,    64,
      64,    64,   246,    38,    39,   249,    81,    81,    58,    28,
      29,    30,    31,    32,    33,    34,    35,    34,    34,    34,
     204,   205,   206,   207,   208,    44,    34,    62,    64,   213,
      49,    33,    25,   277,    80,    78,    74,    33,    73,    74,
      20,    78,    33,   287,    78,    78,    78,    82,    78,    77,
      33,    80,    71,    78,   298,    78,    40,    76,    79,    77,
      33,    33,    81,   307,    33,    79,    79,    79,    79,   213,
      79,    33,    64,   317,   318,   319,    79,    62,   322,   263,
      28,    29,    30,    31,    32,    33,    34,    40,    34,    34,
     334,   335,   336,   337,    34,    33,    33,    33,    80,    64,
      34,   285,    77,    33,    81,    79,    54,    55,    59,    57,
      28,    29,    30,    31,    32,    33,    34,    77,    81,   263,
      81,    77,    63,    71,    81,    64,    70,    33,    76,    27,
      33,   129,   226,    81,   286,   323,    54,    55,   324,    57,
     322,   285,   201,    28,    29,    30,    31,    32,    33,    34,
      35,   264,    -1,    71,    -1,    -1,    -1,    -1,    76,    -1,
      -1,    -1,    -1,    81,    49,    28,    29,    30,    31,    32,
      33,    34,    28,    29,    30,    31,    32,    33,    34,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    71,    21,    -1,    -1,
      -1,    76,    -1,    -1,    -1,    -1,    81,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    71,    26,
      -1,    -1,    -1,    76,    -1,    71,    -1,    -1,    81,    -1,
      76,    -1,    -1,    -1,    58,    81,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      -1,    58,    -1,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,     3,     4,     5,
       6,     7,     8,     9,    10,    -1,    -1,    -1,    14,    15,
      16,    -1,    18,    19,    20,    -1,    22,    -1,    24,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    58,
      36,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    66,    67,    68,    69,    70,
      71,    72,    73,    82,    58,    -1,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,    82,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      58,    82,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    58,    82,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    58,    82,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,
      82,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    -1,    -1,    -1,    -1,    -1,
      58,    80,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    -1,    -1,    -1,    -1,
      -1,    58,    80,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    -1,    -1,    -1,
      -1,    -1,    58,    80,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    -1,    -1,
      -1,    -1,    58,    79,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    -1,    -1,
      58,    77,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    54,    -1,    -1,    77,
      58,    -1,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    58,    -1,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    -1,    -1,    58,    77,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      -1,    -1,    -1,    77,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    58,    -1,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    58,    -1,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,     9,    10,    14,
      15,    16,    18,    19,    20,    22,    24,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    49,    71,    76,    81,
      85,    86,    87,    88,    89,    90,    91,    92,    94,    97,
     112,   113,   114,   115,   120,   125,   127,   128,   129,    11,
      12,    13,    33,    95,    33,    35,    28,    29,    32,    33,
      34,    71,    76,    81,   124,   125,    35,   116,    33,    98,
      81,    33,    81,   121,    64,    66,    67,    68,    69,   129,
     129,   129,     0,    89,    33,    81,    77,    58,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    59,    64,    64,    81,   124,   124,   124,    21,
      58,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    81,   117,    64,    99,   101,
      33,   125,   121,    37,    38,    39,    62,    73,    74,    58,
      34,    34,    34,    34,    82,    64,    93,   124,   126,   129,
     129,   129,   129,   129,   129,   129,   129,   129,   129,   129,
     129,   129,   129,   129,   129,    13,    78,   124,   124,   125,
      82,   124,   124,   124,   124,   124,   124,   124,   124,   124,
     124,   124,   124,   124,   124,   124,   126,    23,    78,   118,
     119,   124,    33,    25,    95,   102,    80,    82,   121,   121,
      78,    74,    33,   123,    78,    78,    78,    78,    78,   124,
      77,    80,    82,    59,    20,    96,   124,    77,    77,    80,
      59,    82,    33,   104,    59,    78,    78,   103,   126,    33,
     122,   122,    40,    80,    83,    44,   129,   130,   130,   130,
     130,   130,   124,   129,   124,    77,    17,   126,   124,    79,
     124,    33,   100,   104,    82,    79,    80,    83,    33,    33,
      67,    68,    78,   131,    54,    79,    79,    79,    79,    79,
     124,    82,   124,    77,    64,    79,    80,    79,    62,    33,
      40,    34,    34,    34,   129,   131,    79,    59,    33,    33,
     124,   121,    33,    80,   129,    96,   124,    64,    26,    34,
      77,    77,    33,    28,    54,    55,    57,    81,   105,   106,
     107,   108,   109,   110,   111,   124,    79,    81,    81,    81,
      33,    77,    70,    59,    63,   124,   124,   124,    27,   107,
      28,    81,   109,   111,    80,    80,    80,    64,    33,   124,
     124,   124,   124,    82,    82,    82,    82
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    84,    85,    86,    86,    87,    88,    88,    89,    89,
      89,    89,    89,    89,    89,    89,    89,    90,    90,    90,
      90,    90,    90,    90,    91,    92,    92,    92,    92,    92,
      92,    93,    93,    94,    95,    95,    96,    96,    97,    98,
      99,    99,   100,   100,   101,   101,   102,   102,   103,   104,
     104,   105,   105,   106,   106,   107,   108,   108,   108,   108,
     109,   109,   110,   110,   111,   112,   113,   114,   115,   116,
     116,   117,   117,   117,   118,   119,   120,   121,   121,   121,
     121,   121,   121,   121,   122,   122,   123,   123,   124,   124,
     124,   124,   124,   124,   124,   124,   124,   124,   124,   124,
     124,   124,   124,   124,   124,   124,   124,   124,   124,   124,
     124,   124,   125,   125,   125,   126,   126,   127,   127,   128,
     128,   129,   129,   129,   129,   129,   129,   129,   129,   129,
     129,   129,   129,   129,   129,   129,   129,   129,   129,   129,
     129,   129,   129,   129,   129,   129,   129,   129,   129,   129,
     129,   130,   130,   131,   131,   131,   131
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
#line 538 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).str); }
#line 1992 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 34: /* NUMBER  */
#line 545 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).number); }
#line 1998 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 35: /* LABEL_NAME  */
#line 538 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).str); }
#line 2004 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 93: /* constant_init  */
#line 544 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).expr); }
#line 2010 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 96: /* variable_init  */
#line 544 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).expr); }
#line 2016 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 100: /* substitutions  */
#line 539 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).substitutions); }
#line 2022 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 104: /* action  */
#line 538 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).str); }
#line 2028 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 105: /* outcomes  */
#line 540 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).outcomes); }
#line 2034 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 106: /* update_distribution  */
#line 540 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).outcomes); }
#line 2040 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 107: /* distribution_and_updates  */
#line 541 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).outcome); }
#line 2046 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 108: /* distribution  */
#line 542 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).dist); }
#line 2052 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 109: /* true_or_updates  */
#line 543 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).updates); }
#line 2058 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 110: /* updates  */
#line 543 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).updates); }
#line 2064 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 123: /* action_substitutions  */
#line 539 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).substitutions); }
#line 2070 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 124: /* expr  */
#line 544 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).expr); }
#line 2076 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 126: /* arguments  */
#line 546 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).arguments); }
#line 2082 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 129: /* property  */
#line 544 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).expr); }
#line 2088 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 130: /* path_property  */
#line 547 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).path); }
#line 2094 "src/grammar.cc" /* yacc.c:1257  */
        break;

    case 131: /* time_range  */
#line 548 "grammar.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).time_range); }
#line 2100 "src/grammar.cc" /* yacc.c:1257  */
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
#line 553 "grammar.yy" /* yacc.c:1646  */
    { if (!state->success()) YYERROR; }
#line 2394 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 17:
#line 579 "grammar.yy" /* yacc.c:1646  */
    { SetModelType(yylloc, ModelType::MDP, state); }
#line 2400 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 18:
#line 581 "grammar.yy" /* yacc.c:1646  */
    { SetModelType(yylloc, ModelType::MDP, state); }
#line 2406 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 19:
#line 583 "grammar.yy" /* yacc.c:1646  */
    { SetModelType(yylloc, ModelType::DTMC, state); }
#line 2412 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 20:
#line 585 "grammar.yy" /* yacc.c:1646  */
    { SetModelType(yylloc, ModelType::DTMC, state); }
#line 2418 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 21:
#line 587 "grammar.yy" /* yacc.c:1646  */
    { SetModelType(yylloc, ModelType::CTMC, state); }
#line 2424 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 22:
#line 589 "grammar.yy" /* yacc.c:1646  */
    { SetModelType(yylloc, ModelType::CTMC, state); }
#line 2430 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 23:
#line 591 "grammar.yy" /* yacc.c:1646  */
    { SetModelType(yylloc, ModelType::GSMP, state); }
#line 2436 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 24:
#line 595 "grammar.yy" /* yacc.c:1646  */
    { AddConstant(yylloc, (yyvsp[-2].str), (yyvsp[-3].type), (yyvsp[-1].expr), state); }
#line 2442 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 25:
#line 599 "grammar.yy" /* yacc.c:1646  */
    { (yyval.type) = Type::INT; }
#line 2448 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 26:
#line 601 "grammar.yy" /* yacc.c:1646  */
    { (yyval.type) = Type::INT; }
#line 2454 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 27:
#line 603 "grammar.yy" /* yacc.c:1646  */
    { (yyval.type) = Type::DOUBLE; }
#line 2460 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 28:
#line 605 "grammar.yy" /* yacc.c:1646  */
    { (yyval.type) = Type::DOUBLE; }
#line 2466 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 29:
#line 607 "grammar.yy" /* yacc.c:1646  */
    { (yyval.type) = Type::DOUBLE; }
#line 2472 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 30:
#line 609 "grammar.yy" /* yacc.c:1646  */
    { (yyval.type) = Type::BOOL; }
#line 2478 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 31:
#line 613 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = nullptr; }
#line 2484 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 32:
#line 615 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 2490 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 34:
#line 622 "grammar.yy" /* yacc.c:1646  */
    { AddIntVariable(yylloc, (yyvsp[-8].str), (yyvsp[-5].expr), (yyvsp[-3].expr), (yyvsp[-1].expr), state); }
#line 2496 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 35:
#line 624 "grammar.yy" /* yacc.c:1646  */
    { AddBoolVariable(yylloc, (yyvsp[-4].str), (yyvsp[-1].expr), state); }
#line 2502 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 36:
#line 628 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = nullptr; }
#line 2508 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 37:
#line 630 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 2514 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 38:
#line 634 "grammar.yy" /* yacc.c:1646  */
    { EndModule(state); }
#line 2520 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 39:
#line 638 "grammar.yy" /* yacc.c:1646  */
    { StartModule(yylloc, (yyvsp[0].str), state); }
#line 2526 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 41:
#line 643 "grammar.yy" /* yacc.c:1646  */
    { AddFromModule(yylloc, (yyvsp[-3].str), (yyvsp[-1].substitutions), state); }
#line 2532 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 42:
#line 647 "grammar.yy" /* yacc.c:1646  */
    { (yyval.substitutions) = AddSubstitution(yylloc, (yyvsp[-2].str), (yyvsp[0].str), nullptr, state); }
#line 2538 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 43:
#line 649 "grammar.yy" /* yacc.c:1646  */
    { (yyval.substitutions) = AddSubstitution(yylloc, (yyvsp[-2].str), (yyvsp[0].str), (yyvsp[-4].substitutions), state); }
#line 2544 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 48:
#line 661 "grammar.yy" /* yacc.c:1646  */
    { AddCommand(yylloc, (yyvsp[-5].str), (yyvsp[-3].expr), (yyvsp[-1].outcomes), state); }
#line 2550 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 49:
#line 665 "grammar.yy" /* yacc.c:1646  */
    { (yyval.str) = new std::string(); }
#line 2556 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 51:
#line 670 "grammar.yy" /* yacc.c:1646  */
    { (yyval.outcomes) = AddOutcome(NewOutcome(nullptr, (yyvsp[0].updates)), nullptr); }
#line 2562 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 53:
#line 675 "grammar.yy" /* yacc.c:1646  */
    { (yyval.outcomes) = AddOutcome((yyvsp[0].outcome), nullptr); }
#line 2568 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 54:
#line 677 "grammar.yy" /* yacc.c:1646  */
    { (yyval.outcomes) = AddOutcome((yyvsp[0].outcome), (yyvsp[-2].outcomes)); }
#line 2574 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 55:
#line 681 "grammar.yy" /* yacc.c:1646  */
    { (yyval.outcome) = NewOutcome((yyvsp[-2].dist), (yyvsp[0].updates)); }
#line 2580 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 56:
#line 685 "grammar.yy" /* yacc.c:1646  */
    { (yyval.dist) = NewMemoryless((yyvsp[0].expr)); }
#line 2586 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 57:
#line 687 "grammar.yy" /* yacc.c:1646  */
    { (yyval.dist) = NewWeibull((yyvsp[-3].expr), (yyvsp[-1].expr)); }
#line 2592 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 58:
#line 689 "grammar.yy" /* yacc.c:1646  */
    { (yyval.dist) = NewLognormal((yyvsp[-3].expr), (yyvsp[-1].expr)); }
#line 2598 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 59:
#line 691 "grammar.yy" /* yacc.c:1646  */
    { (yyval.dist) = NewUniform((yyvsp[-3].expr), (yyvsp[-1].expr)); }
#line 2604 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 60:
#line 695 "grammar.yy" /* yacc.c:1646  */
    { (yyval.updates) = new std::vector<Update>(); }
#line 2610 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 62:
#line 700 "grammar.yy" /* yacc.c:1646  */
    { (yyval.updates) = AddUpdate((yyvsp[0].update), nullptr); }
#line 2616 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 63:
#line 702 "grammar.yy" /* yacc.c:1646  */
    { (yyval.updates) = AddUpdate((yyvsp[0].update), (yyvsp[-2].updates)); }
#line 2622 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 64:
#line 706 "grammar.yy" /* yacc.c:1646  */
    { (yyval.update) = NewUpdate((yyvsp[-4].str), (yyvsp[-1].expr)); }
#line 2628 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 65:
#line 710 "grammar.yy" /* yacc.c:1646  */
    { AddFormula(yylloc, (yyvsp[-3].str), (yyvsp[-1].expr), state); }
#line 2634 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 66:
#line 714 "grammar.yy" /* yacc.c:1646  */
    { AddLabel(yylloc, (yyvsp[-3].str), (yyvsp[-1].expr), state); }
#line 2640 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 67:
#line 718 "grammar.yy" /* yacc.c:1646  */
    { SetInit(yylloc, (yyvsp[-1].expr), state); }
#line 2646 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 68:
#line 722 "grammar.yy" /* yacc.c:1646  */
    { EndRewards(state); }
#line 2652 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 69:
#line 726 "grammar.yy" /* yacc.c:1646  */
    { StartRewards(yylloc, new std::string(), state); }
#line 2658 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 70:
#line 728 "grammar.yy" /* yacc.c:1646  */
    { StartRewards(yylloc, (yyvsp[0].str), state); }
#line 2664 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 74:
#line 737 "grammar.yy" /* yacc.c:1646  */
    { AddStateReward((yyvsp[-3].expr), (yyvsp[-1].expr), state); }
#line 2670 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 75:
#line 741 "grammar.yy" /* yacc.c:1646  */
    { AddTransitionReward(yylloc, (yyvsp[-5].str), (yyvsp[-3].expr), (yyvsp[-1].expr), state); }
#line 2676 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 77:
#line 748 "grammar.yy" /* yacc.c:1646  */
    { delete (yyvsp[0].str); }
#line 2682 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 80:
#line 752 "grammar.yy" /* yacc.c:1646  */
    { delete (yyvsp[-1].substitutions); }
#line 2688 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 84:
#line 759 "grammar.yy" /* yacc.c:1646  */
    { delete (yyvsp[0].str); }
#line 2694 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 85:
#line 761 "grammar.yy" /* yacc.c:1646  */
    { delete (yyvsp[0].str); }
#line 2700 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 86:
#line 765 "grammar.yy" /* yacc.c:1646  */
    { (yyval.substitutions) = AddSubstitution(yylloc, (yyvsp[-2].str), (yyvsp[0].str), nullptr,
                                                state); }
#line 2707 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 87:
#line 768 "grammar.yy" /* yacc.c:1646  */
    { (yyval.substitutions) = AddSubstitution(yylloc, (yyvsp[-2].str), (yyvsp[0].str), (yyvsp[-4].substitutions), state); }
#line 2713 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 88:
#line 772 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLiteral((yyvsp[0].number)); }
#line 2719 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 89:
#line 774 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = new Literal(true); }
#line 2725 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 90:
#line 776 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = new Literal(false); }
#line 2731 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 91:
#line 778 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewIdentifier((yyvsp[0].str)); }
#line 2737 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 92:
#line 780 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewFunctionCall((yyvsp[-3].function), (yyvsp[-1].arguments)); }
#line 2743 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 93:
#line 782 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewFunctionCall((yyvsp[-3].function), (yyvsp[-1].arguments)); }
#line 2749 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 94:
#line 784 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNegate((yyvsp[0].expr)); }
#line 2755 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 95:
#line 786 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNot((yyvsp[0].expr)); }
#line 2761 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 96:
#line 788 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewPlus((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2767 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 97:
#line 790 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewMinus((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2773 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 98:
#line 792 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewMultiply((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2779 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 99:
#line 794 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewDivide((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2785 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 100:
#line 796 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewAnd((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2791 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 101:
#line 798 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewOr((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2797 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 102:
#line 800 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewImply((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2803 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 103:
#line 802 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewIff((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2809 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 104:
#line 804 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLess((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2815 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 105:
#line 806 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLessEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2821 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 106:
#line 808 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewGreaterEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2827 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 107:
#line 810 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewGreater((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2833 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 108:
#line 812 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2839 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 109:
#line 814 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNotEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2845 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 110:
#line 816 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewConditional((yyvsp[-4].expr), (yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2851 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 111:
#line 818 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[-1].expr); }
#line 2857 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 112:
#line 822 "grammar.yy" /* yacc.c:1646  */
    { (yyval.function) = MakeFunction(yylloc, (yyvsp[0].str), state); }
#line 2863 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 113:
#line 824 "grammar.yy" /* yacc.c:1646  */
    { (yyval.function) = Function::MIN; }
#line 2869 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 114:
#line 826 "grammar.yy" /* yacc.c:1646  */
    { (yyval.function) = Function::MAX; }
#line 2875 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 115:
#line 830 "grammar.yy" /* yacc.c:1646  */
    { (yyval.arguments) = AddArgument((yyvsp[0].expr), nullptr);  }
#line 2881 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 116:
#line 832 "grammar.yy" /* yacc.c:1646  */
    { (yyval.arguments) = AddArgument((yyvsp[0].expr), (yyvsp[-2].arguments)); }
#line 2887 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 119:
#line 840 "grammar.yy" /* yacc.c:1646  */
    { AddProperty((yyvsp[0].expr), state); }
#line 2893 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 120:
#line 842 "grammar.yy" /* yacc.c:1646  */
    { AddProperty((yyvsp[0].expr), state); }
#line 2899 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 121:
#line 846 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLiteral((yyvsp[0].number)); }
#line 2905 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 122:
#line 848 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = new Literal(true); }
#line 2911 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 123:
#line 850 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = new Literal(false); }
#line 2917 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 124:
#line 852 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewIdentifier((yyvsp[0].str)); }
#line 2923 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 125:
#line 854 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLabel((yyvsp[0].str)); }
#line 2929 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 126:
#line 856 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewFunctionCall((yyvsp[-3].function), (yyvsp[-1].arguments)); }
#line 2935 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 127:
#line 858 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewFunctionCall((yyvsp[-3].function), (yyvsp[-1].arguments)); }
#line 2941 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 128:
#line 860 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewProbabilityLess(Double((yyvsp[-3].number)), (yyvsp[-1].path)); }
#line 2947 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 129:
#line 862 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewProbabilityLessEqual(Double((yyvsp[-3].number)), (yyvsp[-1].path)); }
#line 2953 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 130:
#line 864 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewProbabilityGreaterEqual(Double((yyvsp[-3].number)), (yyvsp[-1].path)); }
#line 2959 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 131:
#line 866 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewProbabilityGreater(Double((yyvsp[-3].number)), (yyvsp[-1].path)); }
#line 2965 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 132:
#line 868 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewProbabilityEqual((yyvsp[-1].path)); }
#line 2971 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 133:
#line 870 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNegate((yyvsp[0].expr)); }
#line 2977 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 134:
#line 872 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNot((yyvsp[0].expr)); }
#line 2983 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 135:
#line 874 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewPlus((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2989 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 136:
#line 876 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewMinus((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2995 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 137:
#line 878 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewMultiply((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3001 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 138:
#line 880 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewDivide((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3007 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 139:
#line 882 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewAnd((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3013 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 140:
#line 884 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewOr((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3019 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 141:
#line 886 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewImply((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3025 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 142:
#line 888 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewIff((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3031 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 143:
#line 890 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLess((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3037 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 144:
#line 892 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLessEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3043 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 145:
#line 894 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewGreaterEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3049 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 146:
#line 896 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewGreater((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3055 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 147:
#line 898 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3061 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 148:
#line 900 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNotEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3067 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 149:
#line 902 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewConditional((yyvsp[-4].expr), (yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3073 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 150:
#line 904 "grammar.yy" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[-1].expr); }
#line 3079 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 151:
#line 908 "grammar.yy" /* yacc.c:1646  */
    { (yyval.path) = NewUntil((yyvsp[-1].time_range), (yyvsp[-3].expr), (yyvsp[0].expr)); }
#line 3085 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 152:
#line 910 "grammar.yy" /* yacc.c:1646  */
    { (yyval.path) = NewEventually((yyvsp[-1].time_range), (yyvsp[0].expr)); }
#line 3091 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 153:
#line 914 "grammar.yy" /* yacc.c:1646  */
    { (yyval.time_range) = new TimeRange(0, Infinity()); }
#line 3097 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 154:
#line 916 "grammar.yy" /* yacc.c:1646  */
    { (yyval.time_range) = new TimeRange(0, Double((yyvsp[0].number))); }
#line 3103 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 155:
#line 918 "grammar.yy" /* yacc.c:1646  */
    { (yyval.time_range) = new TimeRange(Double((yyvsp[-3].number)), Double((yyvsp[-1].number))); }
#line 3109 "src/grammar.cc" /* yacc.c:1646  */
    break;

  case 156:
#line 920 "grammar.yy" /* yacc.c:1646  */
    { (yyval.time_range) = new TimeRange(Double((yyvsp[0].number)), Infinity()); }
#line 3115 "src/grammar.cc" /* yacc.c:1646  */
    break;


#line 3119 "src/grammar.cc" /* yacc.c:1646  */
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
#line 923 "grammar.yy" /* yacc.c:1906  */


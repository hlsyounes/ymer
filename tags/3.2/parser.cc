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
#line 25 "parser.yy" /* yacc.c:339  */

#include <limits>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "parser-state.h"
#include "src/distribution.h"
#include "src/model.h"
#include "src/strutil.h"

#include "glog/logging.h"

#include "parser.hh"

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
  return new BinaryOperation(BinaryOperator::NOT_EQUAL,
                             WrapUnique(operand1), WrapUnique(operand2));
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

double Infinity() {
  return std::numeric_limits<double>::infinity();
}

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

const UntilProperty* NewUntil(double min_time, double max_time,
                              const Expression* pre_expr,
                              const Expression* post_expr) {
  return new UntilProperty(min_time, max_time, WrapUnique(pre_expr),
                           WrapUnique(post_expr));
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

const Uniform* NewUniform(const Expression* low,
                          const Expression* high) {
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

void EndModule(ParserState* state) {
  state->mutable_model()->EndModule();
}

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

void StartRewards(const std::string* label) { delete label; }

void AddStateReward(const Expression* guard, const Expression* reward) {
  delete guard;
  delete reward;
}

void AddTransitionReward(const std::string* action, const Expression* guard,
                         const Expression* reward) {
  delete action;
  delete guard;
  delete reward;
}

void EndRewards() {}

void AddProperty(const Expression* property, ParserState* state) {
  state->add_property(WrapUnique(property));
}

}  // namespace

#line 454 "parser.cc" /* yacc.c:339  */

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
#line 454 "parser.yy" /* yacc.c:355  */

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

#line 636 "parser.cc" /* yacc.c:355  */
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

#line 664 "parser.cc" /* yacc.c:358  */

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
#define YYFINAL  57
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   901

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  83
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  40
/* YYNRULES -- Number of rules.  */
#define YYNRULES  133
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  289

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
       2,     2,     2,    74,    62,     2,     2,     2,     2,     2,
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
       0,   497,   497,   501,   502,   505,   508,   509,   512,   513,
     514,   515,   516,   519,   521,   523,   525,   527,   529,   531,
     535,   539,   541,   543,   545,   547,   549,   554,   555,   559,
     562,   564,   569,   570,   574,   578,   582,   583,   587,   589,
     593,   594,   597,   598,   601,   606,   607,   610,   612,   615,
     617,   621,   625,   627,   629,   631,   635,   637,   640,   642,
     646,   650,   655,   656,   660,   661,   662,   665,   669,   673,
     675,   677,   679,   681,   683,   685,   687,   689,   691,   693,
     695,   697,   699,   701,   703,   705,   707,   709,   711,   713,
     715,   717,   719,   723,   725,   727,   731,   733,   737,   738,
     741,   743,   747,   749,   751,   753,   755,   757,   759,   761,
     763,   765,   767,   769,   771,   773,   775,   777,   779,   781,
     783,   785,   787,   789,   791,   793,   795,   797,   799,   801,
     805,   807,   809,   811
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
  "UMINUS", "'!'", "';'", "'['", "']'", "','", "'('", "')'", "$accept",
  "file", "model_or_properties", "model", "model_components",
  "model_component", "model_type", "constant", "constant_type",
  "constant_init", "global", "variable", "variable_init", "module",
  "module_name", "module_def", "substitutions", "variables", "commands",
  "command", "action", "outcomes", "update_distribution",
  "distribution_and_updates", "distribution", "true_or_updates", "updates",
  "update", "rewards", "rewards_label", "reward_rules", "state_reward",
  "transition_reward", "expr", "function", "arguments", "properties",
  "property_list", "property", "path_property", YY_NULLPTR
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
      44,    40,    41
};
# endif

#define YYPACT_NINF -115

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-115)))

#define YYTABLE_NINF -94

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     323,  -115,  -115,  -115,  -115,  -115,  -115,  -115,    14,  -115,
    -115,   -32,   -14,   -19,  -115,  -115,  -115,  -115,   -44,   -22,
    -115,   -49,   287,   287,   287,    52,  -115,  -115,   381,  -115,
    -115,  -115,    63,  -115,  -115,  -115,    60,  -115,    70,   722,
    -115,  -115,  -115,    89,  -115,  -115,  -115,  -115,    86,    95,
     115,   117,   120,   121,  -115,  -115,   383,  -115,  -115,    93,
     147,   287,   287,   287,   287,   287,   287,   287,   287,   287,
     287,   287,   287,   287,   287,   287,   287,    15,   -21,   126,
     135,   -32,  -115,    82,    90,    91,    92,    94,  -115,   147,
      87,  -115,  -115,   101,   -22,  -115,   147,   147,   147,   738,
     102,   -29,   722,   658,   751,   776,   799,   342,   -37,   -37,
     -28,   -28,   -28,   -28,   -67,   -67,  -115,  -115,   151,   147,
    -115,   134,  -115,  -115,   674,   106,  -115,  -115,   107,   147,
     287,   287,   287,   287,   738,  -115,    95,  -115,  -115,   406,
     147,   147,   147,   147,   147,   147,   147,   147,   147,   147,
     147,   147,   147,   147,   147,   147,   147,  -115,   287,   147,
      96,    37,  -115,   122,   147,   171,   134,  -115,    47,   604,
     127,   128,   129,   130,   140,  -115,   690,   764,   788,   810,
     820,   828,   828,    50,    50,    50,    50,    39,    39,  -115,
    -115,    57,   738,   722,   738,  -115,   147,   147,   620,   157,
      66,   143,  -115,    85,  -115,  -115,  -115,  -115,   147,   147,
    -115,   584,   706,  -115,   191,  -115,   192,   147,   193,   197,
     198,   722,    58,   738,   151,   147,  -115,   162,   367,   287,
     287,   153,  -115,   158,   638,   201,   183,   722,   722,   202,
    -115,  -115,  -115,   164,   163,   175,   176,   345,   181,   169,
    -115,   203,  -115,   204,  -115,   738,   182,   147,   147,   147,
     -23,  -115,   219,   -25,   179,   287,   521,   542,   563,   205,
    -115,  -115,   233,  -115,  -115,   722,   147,   147,   147,   147,
     241,   429,   452,   475,   498,  -115,  -115,  -115,  -115
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    16,    18,    14,    19,    15,    17,    13,    22,    24,
      25,     0,    62,     0,   103,   104,    95,    94,     0,   105,
     102,     0,     0,     0,     0,     0,     2,     3,     5,     6,
       8,     9,     0,    10,    11,    12,     0,     4,    98,   100,
      21,    23,    26,     0,    29,    63,    64,    35,    40,     0,
       0,     0,     0,     0,   112,   113,     0,     1,     7,    27,
       0,    99,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    42,    93,     0,     0,     0,     0,     0,   129,     0,
       0,    70,    71,     0,    72,    69,     0,     0,     0,    96,
       0,     0,   101,     0,   121,   120,   119,   118,   126,   127,
     122,   123,   124,   125,   114,   115,   116,   117,    32,     0,
      61,    45,    65,    66,     0,     0,    34,    41,    36,     0,
       0,     0,     0,     0,    28,    20,     0,    75,    76,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   106,     0,     0,
       0,     0,    46,     0,     0,     0,    45,    43,     0,     0,
       0,     0,     0,     0,     0,    92,     0,    84,    83,    82,
      81,    89,    90,    85,    86,    87,    88,    77,    78,    79,
      80,     0,    97,   128,    33,    31,     0,     0,     0,     0,
       0,     0,   107,     0,   108,   109,   110,   111,     0,     0,
      73,     0,     0,    67,     0,    37,     0,     0,     0,     0,
       0,   133,     0,    91,    32,     0,    38,     0,     0,     0,
       0,     0,    74,     0,     0,     0,     0,   130,   132,     0,
      30,    68,    39,    70,     0,     0,     0,     0,     0,    48,
      49,     0,    47,    57,    58,    52,     0,     0,     0,     0,
      72,    44,     0,     0,     0,     0,     0,     0,     0,     0,
      50,    56,     0,    51,    59,   131,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    55,    53,    54,    60
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -115,  -115,  -115,  -115,  -115,   242,  -115,  -115,  -115,  -115,
    -115,   194,    48,  -115,  -115,  -115,  -115,  -115,  -115,  -115,
     111,  -115,  -115,    17,  -115,    20,  -115,    21,  -115,  -115,
    -115,  -115,  -115,    46,     0,  -114,  -115,  -115,    16,   -85
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    25,    26,    27,    28,    29,    30,    31,    32,    90,
      33,    44,   160,    34,    48,    80,   200,    81,   128,   167,
     163,   248,   249,   250,   251,   252,   253,   254,    35,    46,
      78,   122,   123,    99,   100,   101,    37,    38,   169,   170
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      36,    43,   120,   271,   269,    75,    76,    91,    92,    16,
      17,    93,    94,    95,    47,   168,    39,    50,    51,    52,
      53,    45,    36,    36,    36,    40,    41,    42,   118,    69,
      70,    71,    72,    73,    74,    75,    76,    49,    54,    55,
      56,   191,    73,    74,    75,    76,   171,   172,   173,    83,
      96,   156,    57,   157,   196,    97,   272,   121,   -93,   -93,
      98,    36,    36,    36,    36,    36,    36,    36,    36,    36,
      36,    36,    36,    36,    36,    36,    36,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   119,   222,   140,    59,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     154,   153,   154,    14,    15,    16,    17,    18,    19,    20,
     151,   152,   153,   154,   124,    16,    17,   156,    82,   202,
      36,    36,    36,    36,    21,   134,   174,   156,   156,   210,
     232,    60,   137,   138,   139,   215,   216,    61,    77,    84,
      79,    85,   218,   219,    86,    87,    22,    89,    36,   125,
     126,    23,   129,   220,   135,   161,    24,   162,   130,   131,
     132,   159,   133,   195,   193,    91,    92,    16,    17,    93,
      94,    95,   136,   155,   165,   166,   176,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,   188,   189,
     190,   197,   192,    36,   199,   194,   204,   205,   206,   207,
     198,   243,    92,    16,    17,    93,    94,    95,    96,   221,
     208,   214,   217,    97,   226,   227,   235,   229,    98,    36,
      36,   230,   231,   239,   242,   240,   256,   244,   245,   262,
     246,   -56,   211,   212,   257,   237,   238,    91,    92,    16,
      17,    93,    94,    95,    96,   223,   258,   259,   261,    97,
     272,   265,   263,   228,   247,    36,   280,   264,   269,   279,
      58,   234,   233,   244,   245,   127,   246,   201,     0,   270,
       0,   275,   255,   273,     0,   274,     0,     0,     0,     0,
      96,     0,     0,   139,     0,    97,     0,     0,     0,     0,
      98,     0,     0,   266,   267,   268,     0,     0,   255,     0,
       0,     0,     0,     0,     0,    14,    15,    16,    17,    18,
      19,    20,   281,   282,   283,   284,     1,     2,     3,     4,
       5,     6,     7,     8,     0,     0,    21,     9,    10,    11,
       0,     0,     0,     0,     0,    12,     0,    13,     0,     0,
       0,    14,    15,    16,    17,    18,    19,    20,    22,     0,
       0,     0,     0,    23,     0,     0,     0,     0,    24,     0,
       0,     0,    21,    91,    92,    16,    17,    93,   260,    95,
       0,     0,     0,     0,     1,     2,     3,     4,     5,     6,
       7,     8,     0,   236,    22,     9,    10,    11,     0,    23,
       0,     0,     0,    12,    24,    13,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    96,     0,     0,     0,
       0,    97,     0,     0,     0,   140,    98,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     154,    62,     0,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,     0,     0,     0,
       0,     0,     0,     0,   140,    88,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
       0,     0,     0,     0,     0,     0,     0,   140,   175,   141,
     142,   143,   144,   145,   146,   147,   148,   149,   150,   151,
     152,   153,   154,     0,     0,     0,     0,     0,     0,     0,
     140,   285,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   152,   153,   154,     0,     0,     0,     0,
       0,     0,     0,   140,   286,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   154,     0,
       0,     0,     0,     0,     0,     0,   140,   287,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   152,
     153,   154,     0,     0,     0,     0,     0,     0,     0,   140,
     288,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     150,   151,   152,   153,   154,     0,     0,     0,     0,     0,
     140,   276,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   152,   153,   154,     0,     0,     0,     0,
       0,   140,   277,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,   153,   154,     0,     0,     0,
       0,     0,   140,   278,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   152,   153,   154,   203,     0,
       0,     0,    62,   224,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,   140,     0,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   154,     0,     0,   140,   213,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   152,
     153,   154,     0,     0,     0,   241,    62,   158,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,   140,   164,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   152,   153,   154,   140,   209,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   140,   225,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
      62,     0,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,   140,     0,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   152,
     153,   154,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   152,   153,   154,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   152,
     153,   154,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   147,   148,   149,   150,   151,   152,
     153,   154
};

static const yytype_int16 yycheck[] =
{
       0,    33,    23,    28,    27,    72,    73,    28,    29,    30,
      31,    32,    33,    34,    33,   129,     0,    66,    67,    68,
      69,    35,    22,    23,    24,    11,    12,    13,    13,    66,
      67,    68,    69,    70,    71,    72,    73,    81,    22,    23,
      24,   155,    70,    71,    72,    73,   131,   132,   133,    49,
      71,    80,     0,    82,    17,    76,    81,    78,    81,    81,
      81,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    78,   208,    58,    33,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    72,    73,    28,    29,    30,    31,    32,    33,    34,
      70,    71,    72,    73,    78,    30,    31,    80,    33,    82,
     130,   131,   132,   133,    49,    89,   136,    80,    80,    82,
      82,    81,    96,    97,    98,    79,    80,    77,    59,    34,
      64,    34,    67,    68,    34,    34,    71,    64,   158,    33,
      25,    76,    80,    78,    77,   119,    81,    33,    78,    78,
      78,    20,    78,    77,   158,    28,    29,    30,    31,    32,
      33,    34,    81,    81,    78,    78,   140,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     154,    79,   156,   203,    33,   159,    79,    79,    79,    79,
     164,    28,    29,    30,    31,    32,    33,    34,    71,   203,
      80,    64,    79,    76,    33,    33,    64,    34,    81,   229,
     230,    34,    34,    80,    33,    77,    34,    54,    55,    70,
      57,    77,   196,   197,    81,   229,   230,    28,    29,    30,
      31,    32,    33,    34,    71,   209,    81,    81,    77,    76,
      81,    79,    59,   217,    81,   265,    33,    63,    27,    64,
      28,   225,   224,    54,    55,    81,    57,   166,    -1,   262,
      -1,   265,   236,   263,    -1,   264,    -1,    -1,    -1,    -1,
      71,    -1,    -1,   247,    -1,    76,    -1,    -1,    -1,    -1,
      81,    -1,    -1,   257,   258,   259,    -1,    -1,   262,    -1,
      -1,    -1,    -1,    -1,    -1,    28,    29,    30,    31,    32,
      33,    34,   276,   277,   278,   279,     3,     4,     5,     6,
       7,     8,     9,    10,    -1,    -1,    49,    14,    15,    16,
      -1,    -1,    -1,    -1,    -1,    22,    -1,    24,    -1,    -1,
      -1,    28,    29,    30,    31,    32,    33,    34,    71,    -1,
      -1,    -1,    -1,    76,    -1,    -1,    -1,    -1,    81,    -1,
      -1,    -1,    49,    28,    29,    30,    31,    32,    33,    34,
      -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,     8,
       9,    10,    -1,    26,    71,    14,    15,    16,    -1,    76,
      -1,    -1,    -1,    22,    81,    24,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    71,    -1,    -1,    -1,
      -1,    76,    -1,    -1,    -1,    58,    81,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    58,    -1,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    58,    82,    60,    61,    62,    63,
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
      66,    67,    68,    69,    70,    71,    72,    73,    54,    -1,
      -1,    -1,    58,    79,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    58,    -1,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    -1,    -1,    58,    77,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    -1,    -1,    -1,    77,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      58,    -1,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    58,    -1,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    66,    67,    68,    69,    70,    71,
      72,    73
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,     9,    10,    14,
      15,    16,    22,    24,    28,    29,    30,    31,    32,    33,
      34,    49,    71,    76,    81,    84,    85,    86,    87,    88,
      89,    90,    91,    93,    96,   111,   117,   119,   120,   121,
      11,    12,    13,    33,    94,    35,   112,    33,    97,    81,
      66,    67,    68,    69,   121,   121,   121,     0,    88,    33,
      81,    77,    58,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    59,   113,    64,
      98,   100,    33,   117,    34,    34,    34,    34,    82,    64,
      92,    28,    29,    32,    33,    34,    71,    76,    81,   116,
     117,   118,   121,   121,   121,   121,   121,   121,   121,   121,
     121,   121,   121,   121,   121,   121,   121,   121,    13,    78,
      23,    78,   114,   115,   116,    33,    25,    94,   101,    80,
      78,    78,    78,    78,   116,    77,    81,   116,   116,   116,
      58,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    81,    80,    82,    59,    20,
      95,   116,    33,   103,    59,    78,    78,   102,   118,   121,
     122,   122,   122,   122,   117,    82,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   118,   116,   121,   116,    77,    17,    79,   116,    33,
      99,   103,    82,    54,    79,    79,    79,    79,    80,    59,
      82,   116,   116,    77,    64,    79,    80,    79,    67,    68,
      78,   121,   118,   116,    79,    59,    33,    33,   116,    34,
      34,    34,    82,    95,   116,    64,    26,   121,   121,    80,
      77,    77,    33,    28,    54,    55,    57,    81,   104,   105,
     106,   107,   108,   109,   110,   116,    34,    81,    81,    81,
      33,    77,    70,    59,    63,    79,   116,   116,   116,    27,
     106,    28,    81,   108,   110,   121,    80,    80,    80,    64,
      33,   116,   116,   116,   116,    82,    82,    82,    82
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    83,    84,    85,    85,    86,    87,    87,    88,    88,
      88,    88,    88,    89,    89,    89,    89,    89,    89,    89,
      90,    91,    91,    91,    91,    91,    91,    92,    92,    93,
      94,    94,    95,    95,    96,    97,    98,    98,    99,    99,
     100,   100,   101,   101,   102,   103,   103,   104,   104,   105,
     105,   106,   107,   107,   107,   107,   108,   108,   109,   109,
     110,   111,   112,   112,   113,   113,   113,   114,   115,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   117,   117,   117,   118,   118,   119,   119,
     120,   120,   121,   121,   121,   121,   121,   121,   121,   121,
     121,   121,   121,   121,   121,   121,   121,   121,   121,   121,
     121,   121,   121,   121,   121,   121,   121,   121,   121,   121,
     122,   122,   122,   122
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       4,     2,     1,     2,     1,     1,     2,     0,     2,     2,
       9,     5,     0,     2,     4,     1,     2,     5,     3,     5,
       0,     2,     0,     2,     7,     0,     1,     1,     1,     1,
       3,     3,     1,     6,     6,     6,     1,     1,     1,     3,
       6,     4,     0,     1,     0,     2,     2,     4,     7,     1,
       1,     1,     1,     4,     6,     2,     2,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     5,     3,     1,     1,     1,     1,     3,     1,     2,
       1,     3,     1,     1,     1,     1,     4,     6,     6,     6,
       6,     6,     2,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     5,     3,
       5,     8,     5,     3
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
#line 484 "parser.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).str); }
#line 1892 "parser.cc" /* yacc.c:1257  */
        break;

    case 34: /* NUMBER  */
#line 491 "parser.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).number); }
#line 1898 "parser.cc" /* yacc.c:1257  */
        break;

    case 35: /* LABEL_NAME  */
#line 484 "parser.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).str); }
#line 1904 "parser.cc" /* yacc.c:1257  */
        break;

    case 92: /* constant_init  */
#line 490 "parser.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).expr); }
#line 1910 "parser.cc" /* yacc.c:1257  */
        break;

    case 95: /* variable_init  */
#line 490 "parser.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).expr); }
#line 1916 "parser.cc" /* yacc.c:1257  */
        break;

    case 99: /* substitutions  */
#line 485 "parser.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).substitutions); }
#line 1922 "parser.cc" /* yacc.c:1257  */
        break;

    case 103: /* action  */
#line 484 "parser.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).str); }
#line 1928 "parser.cc" /* yacc.c:1257  */
        break;

    case 104: /* outcomes  */
#line 486 "parser.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).outcomes); }
#line 1934 "parser.cc" /* yacc.c:1257  */
        break;

    case 105: /* update_distribution  */
#line 486 "parser.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).outcomes); }
#line 1940 "parser.cc" /* yacc.c:1257  */
        break;

    case 106: /* distribution_and_updates  */
#line 487 "parser.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).outcome); }
#line 1946 "parser.cc" /* yacc.c:1257  */
        break;

    case 107: /* distribution  */
#line 488 "parser.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).dist); }
#line 1952 "parser.cc" /* yacc.c:1257  */
        break;

    case 108: /* true_or_updates  */
#line 489 "parser.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).updates); }
#line 1958 "parser.cc" /* yacc.c:1257  */
        break;

    case 109: /* updates  */
#line 489 "parser.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).updates); }
#line 1964 "parser.cc" /* yacc.c:1257  */
        break;

    case 116: /* expr  */
#line 490 "parser.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).expr); }
#line 1970 "parser.cc" /* yacc.c:1257  */
        break;

    case 118: /* arguments  */
#line 492 "parser.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).arguments); }
#line 1976 "parser.cc" /* yacc.c:1257  */
        break;

    case 121: /* property  */
#line 490 "parser.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).expr); }
#line 1982 "parser.cc" /* yacc.c:1257  */
        break;

    case 122: /* path_property  */
#line 493 "parser.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).path); }
#line 1988 "parser.cc" /* yacc.c:1257  */
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
#line 498 "parser.yy" /* yacc.c:1646  */
    { if (!state->success()) YYERROR; }
#line 2282 "parser.cc" /* yacc.c:1646  */
    break;

  case 13:
#line 520 "parser.yy" /* yacc.c:1646  */
    { SetModelType(yylloc, ModelType::MDP, state); }
#line 2288 "parser.cc" /* yacc.c:1646  */
    break;

  case 14:
#line 522 "parser.yy" /* yacc.c:1646  */
    { SetModelType(yylloc, ModelType::MDP, state); }
#line 2294 "parser.cc" /* yacc.c:1646  */
    break;

  case 15:
#line 524 "parser.yy" /* yacc.c:1646  */
    { SetModelType(yylloc, ModelType::DTMC, state); }
#line 2300 "parser.cc" /* yacc.c:1646  */
    break;

  case 16:
#line 526 "parser.yy" /* yacc.c:1646  */
    { SetModelType(yylloc, ModelType::DTMC, state); }
#line 2306 "parser.cc" /* yacc.c:1646  */
    break;

  case 17:
#line 528 "parser.yy" /* yacc.c:1646  */
    { SetModelType(yylloc, ModelType::CTMC, state); }
#line 2312 "parser.cc" /* yacc.c:1646  */
    break;

  case 18:
#line 530 "parser.yy" /* yacc.c:1646  */
    { SetModelType(yylloc, ModelType::CTMC, state); }
#line 2318 "parser.cc" /* yacc.c:1646  */
    break;

  case 19:
#line 532 "parser.yy" /* yacc.c:1646  */
    { SetModelType(yylloc, ModelType::GSMP, state); }
#line 2324 "parser.cc" /* yacc.c:1646  */
    break;

  case 20:
#line 536 "parser.yy" /* yacc.c:1646  */
    { AddConstant(yylloc, (yyvsp[-2].str), (yyvsp[-3].type), (yyvsp[-1].expr), state); }
#line 2330 "parser.cc" /* yacc.c:1646  */
    break;

  case 21:
#line 540 "parser.yy" /* yacc.c:1646  */
    { (yyval.type) = Type::INT; }
#line 2336 "parser.cc" /* yacc.c:1646  */
    break;

  case 22:
#line 542 "parser.yy" /* yacc.c:1646  */
    { (yyval.type) = Type::INT; }
#line 2342 "parser.cc" /* yacc.c:1646  */
    break;

  case 23:
#line 544 "parser.yy" /* yacc.c:1646  */
    { (yyval.type) = Type::DOUBLE; }
#line 2348 "parser.cc" /* yacc.c:1646  */
    break;

  case 24:
#line 546 "parser.yy" /* yacc.c:1646  */
    { (yyval.type) = Type::DOUBLE; }
#line 2354 "parser.cc" /* yacc.c:1646  */
    break;

  case 25:
#line 548 "parser.yy" /* yacc.c:1646  */
    { (yyval.type) = Type::DOUBLE; }
#line 2360 "parser.cc" /* yacc.c:1646  */
    break;

  case 26:
#line 550 "parser.yy" /* yacc.c:1646  */
    { (yyval.type) = Type::BOOL; }
#line 2366 "parser.cc" /* yacc.c:1646  */
    break;

  case 27:
#line 554 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = nullptr; }
#line 2372 "parser.cc" /* yacc.c:1646  */
    break;

  case 28:
#line 556 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 2378 "parser.cc" /* yacc.c:1646  */
    break;

  case 30:
#line 563 "parser.yy" /* yacc.c:1646  */
    { AddIntVariable(yylloc, (yyvsp[-8].str), (yyvsp[-5].expr), (yyvsp[-3].expr), (yyvsp[-1].expr), state); }
#line 2384 "parser.cc" /* yacc.c:1646  */
    break;

  case 31:
#line 565 "parser.yy" /* yacc.c:1646  */
    { AddBoolVariable(yylloc, (yyvsp[-4].str), (yyvsp[-1].expr), state); }
#line 2390 "parser.cc" /* yacc.c:1646  */
    break;

  case 32:
#line 569 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = nullptr; }
#line 2396 "parser.cc" /* yacc.c:1646  */
    break;

  case 33:
#line 571 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 2402 "parser.cc" /* yacc.c:1646  */
    break;

  case 34:
#line 575 "parser.yy" /* yacc.c:1646  */
    { EndModule(state); }
#line 2408 "parser.cc" /* yacc.c:1646  */
    break;

  case 35:
#line 579 "parser.yy" /* yacc.c:1646  */
    { StartModule(yylloc, (yyvsp[0].str), state); }
#line 2414 "parser.cc" /* yacc.c:1646  */
    break;

  case 37:
#line 584 "parser.yy" /* yacc.c:1646  */
    { AddFromModule(yylloc, (yyvsp[-3].str), (yyvsp[-1].substitutions), state); }
#line 2420 "parser.cc" /* yacc.c:1646  */
    break;

  case 38:
#line 588 "parser.yy" /* yacc.c:1646  */
    { (yyval.substitutions) = AddSubstitution(yylloc, (yyvsp[-2].str), (yyvsp[0].str), nullptr, state); }
#line 2426 "parser.cc" /* yacc.c:1646  */
    break;

  case 39:
#line 590 "parser.yy" /* yacc.c:1646  */
    { (yyval.substitutions) = AddSubstitution(yylloc, (yyvsp[-2].str), (yyvsp[0].str), (yyvsp[-4].substitutions), state); }
#line 2432 "parser.cc" /* yacc.c:1646  */
    break;

  case 44:
#line 602 "parser.yy" /* yacc.c:1646  */
    { AddCommand(yylloc, (yyvsp[-5].str), (yyvsp[-3].expr), (yyvsp[-1].outcomes), state); }
#line 2438 "parser.cc" /* yacc.c:1646  */
    break;

  case 45:
#line 606 "parser.yy" /* yacc.c:1646  */
    { (yyval.str) = new std::string(); }
#line 2444 "parser.cc" /* yacc.c:1646  */
    break;

  case 47:
#line 611 "parser.yy" /* yacc.c:1646  */
    { (yyval.outcomes) = AddOutcome(NewOutcome(nullptr, (yyvsp[0].updates)), nullptr); }
#line 2450 "parser.cc" /* yacc.c:1646  */
    break;

  case 49:
#line 616 "parser.yy" /* yacc.c:1646  */
    { (yyval.outcomes) = AddOutcome((yyvsp[0].outcome), nullptr); }
#line 2456 "parser.cc" /* yacc.c:1646  */
    break;

  case 50:
#line 618 "parser.yy" /* yacc.c:1646  */
    { (yyval.outcomes) = AddOutcome((yyvsp[0].outcome), (yyvsp[-2].outcomes)); }
#line 2462 "parser.cc" /* yacc.c:1646  */
    break;

  case 51:
#line 622 "parser.yy" /* yacc.c:1646  */
    { (yyval.outcome) = NewOutcome((yyvsp[-2].dist), (yyvsp[0].updates)); }
#line 2468 "parser.cc" /* yacc.c:1646  */
    break;

  case 52:
#line 626 "parser.yy" /* yacc.c:1646  */
    { (yyval.dist) = NewMemoryless((yyvsp[0].expr)); }
#line 2474 "parser.cc" /* yacc.c:1646  */
    break;

  case 53:
#line 628 "parser.yy" /* yacc.c:1646  */
    { (yyval.dist) = NewWeibull((yyvsp[-3].expr), (yyvsp[-1].expr)); }
#line 2480 "parser.cc" /* yacc.c:1646  */
    break;

  case 54:
#line 630 "parser.yy" /* yacc.c:1646  */
    { (yyval.dist) = NewLognormal((yyvsp[-3].expr), (yyvsp[-1].expr)); }
#line 2486 "parser.cc" /* yacc.c:1646  */
    break;

  case 55:
#line 632 "parser.yy" /* yacc.c:1646  */
    { (yyval.dist) = NewUniform((yyvsp[-3].expr), (yyvsp[-1].expr)); }
#line 2492 "parser.cc" /* yacc.c:1646  */
    break;

  case 56:
#line 636 "parser.yy" /* yacc.c:1646  */
    { (yyval.updates) = new std::vector<Update>(); }
#line 2498 "parser.cc" /* yacc.c:1646  */
    break;

  case 58:
#line 641 "parser.yy" /* yacc.c:1646  */
    { (yyval.updates) = AddUpdate((yyvsp[0].update), nullptr); }
#line 2504 "parser.cc" /* yacc.c:1646  */
    break;

  case 59:
#line 643 "parser.yy" /* yacc.c:1646  */
    { (yyval.updates) = AddUpdate((yyvsp[0].update), (yyvsp[-2].updates)); }
#line 2510 "parser.cc" /* yacc.c:1646  */
    break;

  case 60:
#line 647 "parser.yy" /* yacc.c:1646  */
    { (yyval.update) = NewUpdate((yyvsp[-4].str), (yyvsp[-1].expr)); }
#line 2516 "parser.cc" /* yacc.c:1646  */
    break;

  case 61:
#line 651 "parser.yy" /* yacc.c:1646  */
    { EndRewards(); }
#line 2522 "parser.cc" /* yacc.c:1646  */
    break;

  case 62:
#line 655 "parser.yy" /* yacc.c:1646  */
    { StartRewards(nullptr); }
#line 2528 "parser.cc" /* yacc.c:1646  */
    break;

  case 63:
#line 657 "parser.yy" /* yacc.c:1646  */
    { StartRewards((yyvsp[0].str)); }
#line 2534 "parser.cc" /* yacc.c:1646  */
    break;

  case 67:
#line 666 "parser.yy" /* yacc.c:1646  */
    { AddStateReward((yyvsp[-3].expr), (yyvsp[-1].expr)); }
#line 2540 "parser.cc" /* yacc.c:1646  */
    break;

  case 68:
#line 670 "parser.yy" /* yacc.c:1646  */
    { AddTransitionReward((yyvsp[-5].str), (yyvsp[-3].expr), (yyvsp[-1].expr)); }
#line 2546 "parser.cc" /* yacc.c:1646  */
    break;

  case 69:
#line 674 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLiteral((yyvsp[0].number)); }
#line 2552 "parser.cc" /* yacc.c:1646  */
    break;

  case 70:
#line 676 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = new Literal(true); }
#line 2558 "parser.cc" /* yacc.c:1646  */
    break;

  case 71:
#line 678 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = new Literal(false); }
#line 2564 "parser.cc" /* yacc.c:1646  */
    break;

  case 72:
#line 680 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewIdentifier((yyvsp[0].str)); }
#line 2570 "parser.cc" /* yacc.c:1646  */
    break;

  case 73:
#line 682 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewFunctionCall((yyvsp[-3].function), (yyvsp[-1].arguments)); }
#line 2576 "parser.cc" /* yacc.c:1646  */
    break;

  case 74:
#line 684 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewFunctionCall((yyvsp[-3].function), (yyvsp[-1].arguments)); }
#line 2582 "parser.cc" /* yacc.c:1646  */
    break;

  case 75:
#line 686 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNegate((yyvsp[0].expr)); }
#line 2588 "parser.cc" /* yacc.c:1646  */
    break;

  case 76:
#line 688 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNot((yyvsp[0].expr)); }
#line 2594 "parser.cc" /* yacc.c:1646  */
    break;

  case 77:
#line 690 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewPlus((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2600 "parser.cc" /* yacc.c:1646  */
    break;

  case 78:
#line 692 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewMinus((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2606 "parser.cc" /* yacc.c:1646  */
    break;

  case 79:
#line 694 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewMultiply((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2612 "parser.cc" /* yacc.c:1646  */
    break;

  case 80:
#line 696 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewDivide((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2618 "parser.cc" /* yacc.c:1646  */
    break;

  case 81:
#line 698 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewAnd((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2624 "parser.cc" /* yacc.c:1646  */
    break;

  case 82:
#line 700 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewOr((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2630 "parser.cc" /* yacc.c:1646  */
    break;

  case 83:
#line 702 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewImply((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2636 "parser.cc" /* yacc.c:1646  */
    break;

  case 84:
#line 704 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewIff((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2642 "parser.cc" /* yacc.c:1646  */
    break;

  case 85:
#line 706 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLess((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2648 "parser.cc" /* yacc.c:1646  */
    break;

  case 86:
#line 708 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLessEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2654 "parser.cc" /* yacc.c:1646  */
    break;

  case 87:
#line 710 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewGreaterEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2660 "parser.cc" /* yacc.c:1646  */
    break;

  case 88:
#line 712 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewGreater((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2666 "parser.cc" /* yacc.c:1646  */
    break;

  case 89:
#line 714 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2672 "parser.cc" /* yacc.c:1646  */
    break;

  case 90:
#line 716 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNotEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2678 "parser.cc" /* yacc.c:1646  */
    break;

  case 91:
#line 718 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewConditional((yyvsp[-4].expr), (yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2684 "parser.cc" /* yacc.c:1646  */
    break;

  case 92:
#line 720 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[-1].expr); }
#line 2690 "parser.cc" /* yacc.c:1646  */
    break;

  case 93:
#line 724 "parser.yy" /* yacc.c:1646  */
    { (yyval.function) = MakeFunction(yylloc, (yyvsp[0].str), state); }
#line 2696 "parser.cc" /* yacc.c:1646  */
    break;

  case 94:
#line 726 "parser.yy" /* yacc.c:1646  */
    { (yyval.function) = Function::MIN; }
#line 2702 "parser.cc" /* yacc.c:1646  */
    break;

  case 95:
#line 728 "parser.yy" /* yacc.c:1646  */
    { (yyval.function) = Function::MAX; }
#line 2708 "parser.cc" /* yacc.c:1646  */
    break;

  case 96:
#line 732 "parser.yy" /* yacc.c:1646  */
    { (yyval.arguments) = AddArgument((yyvsp[0].expr), nullptr);  }
#line 2714 "parser.cc" /* yacc.c:1646  */
    break;

  case 97:
#line 734 "parser.yy" /* yacc.c:1646  */
    { (yyval.arguments) = AddArgument((yyvsp[0].expr), (yyvsp[-2].arguments)); }
#line 2720 "parser.cc" /* yacc.c:1646  */
    break;

  case 100:
#line 742 "parser.yy" /* yacc.c:1646  */
    { AddProperty((yyvsp[0].expr), state); }
#line 2726 "parser.cc" /* yacc.c:1646  */
    break;

  case 101:
#line 744 "parser.yy" /* yacc.c:1646  */
    { AddProperty((yyvsp[0].expr), state); }
#line 2732 "parser.cc" /* yacc.c:1646  */
    break;

  case 102:
#line 748 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLiteral((yyvsp[0].number)); }
#line 2738 "parser.cc" /* yacc.c:1646  */
    break;

  case 103:
#line 750 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = new Literal(true); }
#line 2744 "parser.cc" /* yacc.c:1646  */
    break;

  case 104:
#line 752 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = new Literal(false); }
#line 2750 "parser.cc" /* yacc.c:1646  */
    break;

  case 105:
#line 754 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewIdentifier((yyvsp[0].str)); }
#line 2756 "parser.cc" /* yacc.c:1646  */
    break;

  case 106:
#line 756 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewFunctionCall((yyvsp[-3].function), (yyvsp[-1].arguments)); }
#line 2762 "parser.cc" /* yacc.c:1646  */
    break;

  case 107:
#line 758 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewFunctionCall((yyvsp[-3].function), (yyvsp[-1].arguments)); }
#line 2768 "parser.cc" /* yacc.c:1646  */
    break;

  case 108:
#line 760 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewProbabilityLess(Double((yyvsp[-3].number)), (yyvsp[-1].path)); }
#line 2774 "parser.cc" /* yacc.c:1646  */
    break;

  case 109:
#line 762 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewProbabilityLessEqual(Double((yyvsp[-3].number)), (yyvsp[-1].path)); }
#line 2780 "parser.cc" /* yacc.c:1646  */
    break;

  case 110:
#line 764 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewProbabilityGreaterEqual(Double((yyvsp[-3].number)), (yyvsp[-1].path)); }
#line 2786 "parser.cc" /* yacc.c:1646  */
    break;

  case 111:
#line 766 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewProbabilityGreater(Double((yyvsp[-3].number)), (yyvsp[-1].path)); }
#line 2792 "parser.cc" /* yacc.c:1646  */
    break;

  case 112:
#line 768 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNegate((yyvsp[0].expr)); }
#line 2798 "parser.cc" /* yacc.c:1646  */
    break;

  case 113:
#line 770 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNot((yyvsp[0].expr)); }
#line 2804 "parser.cc" /* yacc.c:1646  */
    break;

  case 114:
#line 772 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewPlus((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2810 "parser.cc" /* yacc.c:1646  */
    break;

  case 115:
#line 774 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewMinus((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2816 "parser.cc" /* yacc.c:1646  */
    break;

  case 116:
#line 776 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewMultiply((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2822 "parser.cc" /* yacc.c:1646  */
    break;

  case 117:
#line 778 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewDivide((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2828 "parser.cc" /* yacc.c:1646  */
    break;

  case 118:
#line 780 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewAnd((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2834 "parser.cc" /* yacc.c:1646  */
    break;

  case 119:
#line 782 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewOr((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2840 "parser.cc" /* yacc.c:1646  */
    break;

  case 120:
#line 784 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewImply((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2846 "parser.cc" /* yacc.c:1646  */
    break;

  case 121:
#line 786 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewIff((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2852 "parser.cc" /* yacc.c:1646  */
    break;

  case 122:
#line 788 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLess((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2858 "parser.cc" /* yacc.c:1646  */
    break;

  case 123:
#line 790 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLessEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2864 "parser.cc" /* yacc.c:1646  */
    break;

  case 124:
#line 792 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewGreaterEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2870 "parser.cc" /* yacc.c:1646  */
    break;

  case 125:
#line 794 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewGreater((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2876 "parser.cc" /* yacc.c:1646  */
    break;

  case 126:
#line 796 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2882 "parser.cc" /* yacc.c:1646  */
    break;

  case 127:
#line 798 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNotEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2888 "parser.cc" /* yacc.c:1646  */
    break;

  case 128:
#line 800 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewConditional((yyvsp[-4].expr), (yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2894 "parser.cc" /* yacc.c:1646  */
    break;

  case 129:
#line 802 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[-1].expr); }
#line 2900 "parser.cc" /* yacc.c:1646  */
    break;

  case 130:
#line 806 "parser.yy" /* yacc.c:1646  */
    { (yyval.path) = NewUntil(0, Double((yyvsp[-1].number)), (yyvsp[-4].expr), (yyvsp[0].expr)); }
#line 2906 "parser.cc" /* yacc.c:1646  */
    break;

  case 131:
#line 808 "parser.yy" /* yacc.c:1646  */
    { (yyval.path) = NewUntil(Double((yyvsp[-4].number)), Double((yyvsp[-2].number)), (yyvsp[-7].expr), (yyvsp[0].expr)); }
#line 2912 "parser.cc" /* yacc.c:1646  */
    break;

  case 132:
#line 810 "parser.yy" /* yacc.c:1646  */
    { (yyval.path) = NewUntil(Double((yyvsp[-1].number)), Infinity(), (yyvsp[-4].expr), (yyvsp[0].expr)); }
#line 2918 "parser.cc" /* yacc.c:1646  */
    break;

  case 133:
#line 812 "parser.yy" /* yacc.c:1646  */
    { (yyval.path) = NewUntil(0, Infinity(), (yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2924 "parser.cc" /* yacc.c:1646  */
    break;


#line 2928 "parser.cc" /* yacc.c:1646  */
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
#line 815 "parser.yy" /* yacc.c:1906  */


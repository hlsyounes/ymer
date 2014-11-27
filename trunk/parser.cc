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
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 24 "parser.yy" /* yacc.c:339  */

#include <config.h>
#include "models.h"
#include "distributions.h"
#include "formulas.h"
#include "glog/logging.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <string>

#include "parser.hh"

// Lexical analyzer function.
extern int yylex(YYSTYPE* lvalp, YYLTYPE* llocp, void* scanner);
/* Name of current file. */
extern std::string current_file;
/* Constant overrides. */
extern std::map<std::string, TypedValue> const_overrides;

/* Last model parsed. */
const Model* global_model = nullptr;
/* Parsed properties. */
UniquePtrVector<const Expression> properties;

/* Current model. */
static Model* model;
/* Current module. */
static Module* module;
/* Current identifier substitutions. */
static std::map<std::string, std::string> subst;
/* Current synchronization substitutions. */
static std::map<size_t, size_t> synch_subst;
/* Current command. */
static Command* command;
/* Declared integer constants. */
static std::set<std::string> constants;
/* Constant values. */
static std::map<std::string, TypedValue> constant_values;
/* Declared rate constants. */
static std::set<std::string> rates;
/* Rate values. */
static std::map<std::string, TypedValue> rate_values;
/* All state variables. */
static std::set<std::string> variables;
/* Variables lows. */
static std::map<std::string, std::unique_ptr<const Expression>> variable_lows;
/* Variables highs. */
static std::map<std::string, std::unique_ptr<const Expression>> variable_highs;
/* Variables starts. */
static std::map<std::string, std::unique_ptr<const Expression>> variable_starts;
/* Declared modules. */
static std::map<std::string, Module*> modules;
/* Declared synchronizations. */
static std::map<std::string, size_t> synchronizations;
/* Yet undeclared state variables. */
static std::set<std::string> undeclared;
/* Whether the last parsing attempt succeeded. */
static bool success = true;

/* Clears all previously parsed declarations. */
void clear_declarations();

/* Checks if undeclared variables were used. */
static void check_undeclared();
/* Returns the integer value of the given typed value, signaling an error if
   the type is not INT. */
static int integer_value(const TypedValue* q);
/* Returns an identifier representing an integer constant. */
static const Identifier* find_constant(const std::string* ident);
/* Returns an identifier representing a rate constant. */
static const Identifier* find_rate(const std::string* ident);
/* Returns an identifier representing a rate constant or an integer variable. */
static const Identifier* find_rate_or_variable(const std::string* ident);
/* Returns a literal expression. */
static const Literal* make_literal(int n);
/* Returns a literal expression. */
static const Literal* make_literal(const TypedValue* q);
/* Returns a constant value or a variable for the given identifier. */
static const Expression* value_or_variable(const std::string* ident);
/* Returns a variable for the given identifier. */
static const Identifier* find_variable(const std::string* ident);
/* Adds an update to the current command. */
static void add_update(const std::string* ident, const Expression* expr);
/* Returns the value of the given synchronization. */
static size_t synchronization_value(const std::string* ident);
/* Adds a substitution to the current substitution map. */
static void add_substitution(const std::string* ident1,
			     const std::string* ident2);
/* Declares an integer constant. */
static void declare_constant(const std::string* ident,
                             const Expression* value_expr);
/* Declares a rate constant. */
static void declare_rate(const std::string* ident,
                         const Expression* value_expr);
/* Declares a variable. */
static bool declare_variable(const std::string* ident,
                             const Expression* low,
                             const Expression* high,
                             const Expression* start,
                             bool delayed_addition = false);
/* Adds a command to the current module. */
static void add_command();
/* Prepares a command for parsing. */
static void prepare_command(int synch,
                            std::unique_ptr<const Expression>&& guard,
			    const Distribution* delay);
/* Adds a module to the current model defined by renaming. */
static void add_module(const std::string* ident1, const std::string* ident2);
/* Adds a module to the current model. */
static void add_module();
/* Prepares a module for parsing. */
static void prepare_module(const std::string* ident);
/* Prepares a model for parsing. */
static void prepare_model(ModelType model_type);
/* Compiles the current model. */
static void compile_model();

namespace {

// Wrapper for lexical analyzer function called by generated code.
int yylex(void* scanner) {
  return yylex(&yylval, &yylloc, scanner);
}

// Error reporting function.
void yyerror(const std::string& msg) {
  std::cerr << PACKAGE ":" << current_file << ':' << yylloc.first_line << ':'
            << msg << std::endl;
  success = false;
}

// Wrapper for error reporting function called by generated code.
void yyerror(void* scanner, const std::string& msg) {
  yyerror(msg);
}

void yywarning(const std::string& msg) {
  std::cerr << PACKAGE ":" << current_file << ':' << yylloc.first_line << ':'
            << msg << std::endl;
}

template <typename T>
std::unique_ptr<T> WrapUnique(T* ptr) {
  return std::unique_ptr<T>(ptr);
}

Function MakeFunction(const std::string& name) {
  if (name == "min") {
    return Function::MIN;
  } else if (name == "max") {
    return Function::MAX;
  } else if (name == "floor") {
    return Function::FLOOR;
  } else if (name == "ceil") {
    return Function::CEIL;
  } else if (name == "pow") {
    return Function::POW;
  } else if (name == "log") {
    return Function::LOG;
  } else if (name == "mod") {
    return Function::MOD;
  } else {
    yyerror("unknown function");
    return Function::UNKNOWN;
  }
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
  double value = typed_value->value<double>();
  delete typed_value;
  return value;
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

const Exponential* NewExponential(const Expression* rate) {
  return Exponential::make(WrapUnique(rate));
}

const Distribution* NewWeibull(const Expression* scale,
                               const Expression* shape) {
  return Weibull::make(WrapUnique(scale), WrapUnique(shape));
}

const Lognormal* NewLognormal(const Expression* scale,
                              const Expression* shape) {
  return Lognormal::make(WrapUnique(scale), WrapUnique(shape));
}

const Uniform* NewUniform(const Expression* low,
                          const Expression* high) {
  return Uniform::make(WrapUnique(low), WrapUnique(high));
}

}  // namespace

#line 397 "parser.cc" /* yacc.c:339  */

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
    EXP = 312,
    L = 313,
    IFF_TOKEN = 314,
    IMPLY_TOKEN = 315,
    NEQ = 316,
    LEQ = 317,
    GEQ = 318,
    UMINUS = 319
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
#define EXP 312
#define L 313
#define IFF_TOKEN 314
#define IMPLY_TOKEN 315
#define NEQ 316
#define LEQ 317
#define GEQ 318
#define UMINUS 319

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 394 "parser.yy" /* yacc.c:355  */

  ModelType model_type;
  size_t synch;
  const PathProperty* path;
  const Distribution* dist;
  const Expression* expr;
  int nat;
  const std::string* str;
  const TypedValue* number;
  Function function;
  UniquePtrVector<const Expression>* arguments;

#line 578 "parser.cc" /* yacc.c:355  */
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


extern YYSTYPE yylval;
extern YYLTYPE yylloc;
int yyparse (void* scanner);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 607 "parser.cc" /* yacc.c:358  */

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
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   657

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  84
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  40
/* YYNRULES -- Number of rules.  */
#define YYNRULES  142
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  332

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   319

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    77,     2,     2,     2,     2,    64,     2,
      82,    83,    73,    71,    81,    72,     2,    74,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    60,    78,
      67,    65,    70,    59,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    79,     2,    80,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    75,    63,     2,     2,     2,     2,     2,
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
      55,    56,    57,    58,    61,    62,    66,    68,    69,    76
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   425,   425,   425,   429,   430,   437,   437,   441,   443,
     445,   452,   453,   456,   458,   460,   462,   464,   466,   468,
     470,   472,   474,   483,   484,   487,   487,   490,   494,   495,
     498,   499,   502,   505,   506,   509,   511,   516,   517,   521,
     520,   525,   526,   529,   530,   531,   538,   539,   542,   545,
     546,   549,   550,   551,   554,   558,   566,   568,   570,   572,
     574,   581,   583,   585,   587,   589,   591,   593,   595,   597,
     599,   601,   603,   605,   607,   609,   611,   613,   615,   617,
     619,   621,   623,   625,   627,   631,   633,   635,   637,   639,
     641,   643,   645,   649,   651,   653,   655,   657,   661,   665,
     667,   675,   677,   679,   681,   683,   685,   687,   691,   698,
     699,   702,   704,   708,   710,   712,   714,   716,   718,   720,
     722,   724,   726,   728,   730,   732,   734,   736,   738,   740,
     742,   744,   746,   748,   750,   752,   754,   756,   758,   760,
     762,   766,   768
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
  "X", "EXP", "L", "'?'", "':'", "IFF_TOKEN", "IMPLY_TOKEN", "'|'", "'&'",
  "'='", "NEQ", "'<'", "LEQ", "GEQ", "'>'", "'+'", "'-'", "'*'", "'/'",
  "'{'", "UMINUS", "'!'", "';'", "'['", "']'", "','", "'('", "')'",
  "$accept", "file", "$@1", "model_or_properties", "model", "$@2",
  "model_type", "declarations", "declaration", "modules", "module_decl",
  "$@3", "substitutions", "subst_list", "subst", "variables",
  "variable_decl", "commands", "command", "$@4", "synchronization",
  "update", "rewards", "rewards_decl", "rewards_label", "reward_rules",
  "state_reward", "transition_reward", "distribution", "expr", "rate_expr",
  "const_rate_expr", "function", "arguments", "const_expr", "integer",
  "properties", "property_list", "property", "path_property", YY_NULLPTR
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
     305,   306,   307,   308,   309,   310,   311,   312,   313,    63,
      58,   314,   315,   124,    38,    61,   316,    60,   317,   318,
      62,    43,    45,    42,    47,   123,   319,    33,    59,    91,
      93,    44,    40,    41
};
# endif

#define YYPACT_NINF -297

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-297)))

#define YYTABLE_NINF -99

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -297,    29,   138,  -297,  -297,  -297,  -297,  -297,  -297,   -47,
     -13,  -297,   313,   151,   151,   151,  -297,  -297,  -297,   -11,
    -297,    47,   405,    89,   139,   142,   152,   160,  -297,  -297,
     268,  -297,   173,   151,   151,   151,   151,   151,   151,   151,
     151,   151,   151,   151,   151,   151,   151,   151,   151,  -297,
      88,   133,   145,   146,   157,  -297,   257,  -297,  -297,   158,
     -13,  -297,   173,   173,   173,   421,   164,   -51,   405,   341,
     450,   475,   498,   519,   276,   276,   553,   553,   553,   553,
       1,     1,  -297,  -297,   173,   151,   151,   151,   151,    69,
     184,   194,  -297,   230,    89,  -297,  -297,   302,   173,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   173,  -297,   151,   -22,   325,   213,
     216,   222,   240,   286,   288,    33,    34,   212,   290,  -297,
     303,   279,  -297,   357,   463,   487,   509,   529,   285,   285,
     557,   557,   557,   557,   186,   186,  -297,  -297,    64,   421,
     405,  -297,   -54,  -297,  -297,  -297,  -297,    35,    36,    -6,
    -297,   -27,  -297,   245,   312,   343,  -297,   173,   173,  -297,
     431,   447,    -6,  -297,   -27,  -297,  -297,  -297,    -6,    -6,
     192,  -297,  -297,  -297,   -27,   175,    -6,   464,  -297,  -297,
    -297,   211,   421,   151,   420,   537,   183,  -297,    44,    -6,
      -6,    -6,  -297,   115,   -27,   -27,  -297,     6,   542,   587,
     -12,  -297,   405,   588,  -297,  -297,  -297,   563,   563,  -297,
    -297,  -297,  -297,    -6,   604,   578,  -297,   -21,  -297,   606,
    -297,  -297,   373,   560,    48,   576,   562,   564,  -297,   565,
    -297,   -24,  -297,   173,   566,   144,   151,   -10,   610,   622,
     604,    -6,   568,  -297,   215,   173,  -297,  -297,   144,   144,
     226,   405,    -6,  -297,  -297,  -297,  -297,    37,  -297,   180,
     389,  -297,   239,   144,   144,   144,   144,  -297,   541,    -6,
     567,   569,   570,   571,   590,   561,   144,  -297,   195,   195,
    -297,  -297,  -297,   119,   -27,   -27,   144,   -27,  -297,   533,
      -5,    59,   130,   243,   170,   -25,  -297,    -6,  -297,   -27,
     -27,  -297,   -27,   627,   -25,   -59,   545,   135,   148,   156,
     591,   -52,   -25,  -297,  -297,  -297,  -297,  -297,   173,  -297,
    -297,   437
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     0,     1,     9,    10,     8,   114,   115,     0,
     116,   113,     0,     0,     0,     0,     3,     4,     6,     0,
       5,   109,   111,     0,     0,     0,     0,     0,   123,   124,
       0,    11,     0,   110,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    98,
       0,     0,     0,     0,     0,   140,    23,    62,    63,     0,
      64,    61,     0,     0,     0,    99,     0,     0,   112,     0,
     132,   131,   130,   129,   137,   138,   133,   134,   135,   136,
     125,   126,   127,   128,     0,     0,     0,     0,     0,     0,
       0,     0,    12,    46,     0,    67,    68,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   117,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    24,
       7,     0,    84,     0,    76,    75,    74,    73,    81,    82,
      77,    78,    79,    80,    69,    70,    71,    72,     0,   100,
     139,   118,     0,   119,   120,   121,   122,     0,     0,     0,
      13,     0,    17,     0,    25,    49,    47,     0,     0,    65,
       0,     0,     0,    15,     0,    19,   102,   108,     0,     0,
       0,   101,    94,    93,     0,     0,     0,     0,    33,    50,
      51,     0,    83,     0,     0,     0,     0,   103,     0,     0,
       0,     0,    14,     0,     0,     0,    18,     0,     0,    37,
       0,    66,   141,     0,    16,    20,   107,   104,   105,   106,
      97,    95,    96,     0,    28,     0,    34,     0,    48,     0,
      52,    53,     0,     0,     0,     0,     0,    29,    30,     0,
      26,     0,    38,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    41,     0,     0,    86,    85,     0,     0,
       0,   142,     0,    21,    32,    27,    31,     0,    42,     0,
       0,    87,     0,     0,     0,     0,     0,    54,     0,     0,
       0,     0,     0,     0,     0,    56,     0,    92,    88,    89,
      90,    91,    22,     0,     0,     0,     0,     0,    39,     0,
       0,     0,     0,     0,     0,     0,    55,     0,    35,     0,
       0,    57,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    40,    36,    60,    58,    59,     0,    45,
      44,    43
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -297,  -297,  -297,  -297,  -297,  -297,  -297,  -297,  -297,  -297,
    -297,  -297,  -297,  -297,   407,  -297,  -297,  -297,  -297,  -297,
    -297,  -296,  -297,  -297,  -297,  -297,  -297,  -297,  -297,   -62,
      32,  -171,    49,   -60,    -4,  -297,  -297,  -297,   116,    17
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    16,    17,    31,    18,    56,    92,    93,
     129,   188,   236,   237,   238,   209,   226,   227,   242,   305,
     243,   315,   130,   166,   190,   210,   230,   231,   284,    65,
     260,   185,    66,    67,   180,   181,    20,    21,   118,   119
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      95,    96,    97,   196,   240,   322,   182,   183,   313,   252,
     262,   228,   322,   203,   170,   307,    57,    58,   321,   323,
      59,    60,    61,   223,   117,   171,   330,   176,   177,     3,
     114,   329,   115,   221,   222,    23,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,    19,   149,   148,   279,   184,   253,   314,   241,   114,
      62,   151,    19,    19,    19,    63,   178,   229,   263,   -98,
      64,    32,    50,   308,    47,    48,   179,   199,   200,   201,
     123,   124,    19,    19,    19,    19,    19,    19,    19,    19,
      19,    19,    19,    19,    19,    19,    19,    19,   159,   161,
     172,   174,   125,   120,   121,   122,   192,   191,   199,   200,
     201,   160,   162,   173,   175,   199,   200,   201,    22,   199,
     200,   201,    49,   301,   302,    33,   304,   216,   247,    28,
      29,    30,   204,   205,    19,    19,    19,    19,   317,   318,
     309,   319,     4,   131,     5,   114,     6,   169,   232,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    19,     7,     8,   195,    84,
       9,    10,    11,    51,   197,   198,    52,   256,   257,     7,
       8,   254,   207,     9,    10,    11,    53,    12,   204,   205,
     199,   200,   201,   270,    54,   217,   218,   219,   220,   300,
      12,    57,    58,   204,   205,    59,    60,    61,   204,   205,
      13,   310,    85,   256,   257,    14,   258,   126,   325,   234,
      15,   204,   205,    13,    86,    87,   259,   127,    14,   204,
     205,   326,   150,    15,   280,   281,    88,   282,   283,   327,
      94,   269,    19,   204,   205,    62,   113,   267,   204,   205,
      63,   312,   258,   206,   128,    64,   204,   205,   278,   111,
     112,   215,   259,   199,   200,   201,   331,    89,   275,   276,
     202,    90,   163,    91,    98,   293,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     271,   272,   114,   153,   211,    19,   154,   273,   274,   275,
     276,   285,   155,   316,   277,   288,   289,   290,   291,   212,
     273,   274,   275,   276,   273,   274,   275,   276,   299,   157,
     156,   158,   287,   164,   186,   165,   311,    34,   303,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    41,    42,    43,    44,    45,    46,    47,
      48,    55,   105,   106,   107,   108,   109,   110,   111,   112,
     167,    98,   261,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   187,   189,   152,
      24,    25,    26,    27,    34,   132,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      34,   116,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    98,   168,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,    98,   245,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,    98,   286,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,    34,   193,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      98,   194,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,    98,   208,    99,   100,
     101,   213,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   273,   274,   275,   276,   199,   200,
     201,   306,   199,   200,   201,   214,   199,   200,   201,   292,
     225,   224,   233,   324,    45,    46,    47,    48,   109,   110,
     111,   112,   273,   274,   275,   276,   201,   235,   239,   244,
     246,   248,   249,   264,   251,   250,   255,   265,   268,   294,
     298,   295,   296,   297,   320,     0,   328,   266
};

static const yytype_int16 yycheck[] =
{
      62,    63,    64,   174,    25,    64,    33,    34,    33,    33,
      20,    23,    64,   184,    68,    20,    28,    29,   314,    78,
      32,    33,    34,    17,    84,    79,   322,    33,    34,     0,
      81,    83,    83,   204,   205,    82,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,     2,   114,   113,    17,    82,    80,    82,    79,    81,
      72,    83,    13,    14,    15,    77,    72,    79,    78,    82,
      82,    82,    23,    78,    73,    74,    82,    71,    72,    73,
      11,    12,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    65,    65,
      65,    65,    33,    86,    87,    88,   168,   167,    71,    72,
      73,    78,    78,    78,    78,    71,    72,    73,     2,    71,
      72,    73,    33,   294,   295,    78,   297,    83,    80,    13,
      14,    15,    73,    74,    85,    86,    87,    88,   309,   310,
      81,   312,     4,    94,     6,    81,     8,    83,   210,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,   116,    28,    29,   172,    81,
      32,    33,    34,    34,   178,   179,    34,    33,    34,    28,
      29,   243,   186,    32,    33,    34,    34,    49,    73,    74,
      71,    72,    73,   255,    34,   199,   200,   201,    83,    80,
      49,    28,    29,    73,    74,    32,    33,    34,    73,    74,
      72,    81,    79,    33,    34,    77,    72,    33,    83,   223,
      82,    73,    74,    72,    79,    79,    82,    33,    77,    73,
      74,    83,   116,    82,    54,    55,    79,    57,    58,    83,
      82,    26,   193,    73,    74,    72,    82,   251,    73,    74,
      77,    81,    72,    78,    24,    82,    73,    74,   262,    73,
      74,    78,    82,    71,    72,    73,   328,    10,    73,    74,
      78,    14,    60,    16,    59,   279,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
     258,   259,    81,    80,    83,   246,    80,    71,    72,    73,
      74,   269,    80,   307,    78,   273,   274,   275,   276,   193,
      71,    72,    73,    74,    71,    72,    73,    74,   286,    33,
      80,    33,    83,    33,    79,    22,    83,    59,   296,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    67,    68,    69,    70,    71,    72,    73,
      74,    83,    67,    68,    69,    70,    71,    72,    73,    74,
      81,    59,   246,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    65,    35,    54,
      67,    68,    69,    70,    59,    83,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    59,    34,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      59,    34,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    59,    33,    61,    62,
      63,    81,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    71,    72,    73,    74,    71,    72,
      73,    78,    71,    72,    73,    78,    71,    72,    73,    78,
      33,    79,    34,    78,    71,    72,    73,    74,    71,    72,
      73,    74,    71,    72,    73,    74,    73,    33,    60,    33,
      80,    65,    80,    33,    79,    81,    80,    25,    80,    82,
      60,    82,    82,    82,    27,    -1,    65,   250
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    85,    86,     0,     4,     6,     8,    28,    29,    32,
      33,    34,    49,    72,    77,    82,    87,    88,    90,   116,
     120,   121,   122,    82,    67,    68,    69,    70,   122,   122,
     122,    89,    82,    78,    59,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    33,
     116,    34,    34,    34,    34,    83,    91,    28,    29,    32,
      33,    34,    72,    77,    82,   113,   116,   117,   122,   122,
     122,   122,   122,   122,   122,   122,   122,   122,   122,   122,
     122,   122,   122,   122,    81,    79,    79,    79,    79,    10,
      14,    16,    92,    93,    82,   113,   113,   113,    59,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    82,    81,    83,    60,   117,   122,   123,
     123,   123,   123,    11,    12,    33,    33,    33,    24,    94,
     106,   116,    83,   113,   113,   113,   113,   113,   113,   113,
     113,   113,   113,   113,   113,   113,   113,   113,   117,   113,
     122,    83,    54,    80,    80,    80,    80,    33,    33,    65,
      78,    65,    78,    60,    33,    22,   107,    81,    60,    83,
      68,    79,    65,    78,    65,    78,    33,    34,    72,    82,
     118,   119,    33,    34,    82,   115,    79,    65,    95,    35,
     108,   117,   113,    34,    34,   118,   115,   118,   118,    71,
      72,    73,    78,   115,    73,    74,    78,   118,    33,    99,
     109,    83,   122,    81,    78,    78,    83,   118,   118,   118,
      83,   115,   115,    17,    79,    33,   100,   101,    23,    79,
     110,   111,   113,    34,   118,    33,    96,    97,    98,    60,
      25,    79,   102,   104,    33,    60,    80,    80,    65,    80,
      81,    79,    33,    80,   113,    80,    33,    34,    72,    82,
     114,   122,    20,    78,    33,    25,    98,   118,    80,    26,
     113,   114,   114,    71,    72,    73,    74,    78,   118,    17,
      54,    55,    57,    58,   112,   114,    60,    83,   114,   114,
     114,   114,    78,   118,    82,    82,    82,    82,    60,   114,
      80,   115,   115,   114,   115,   103,    78,    20,    78,    81,
      81,    83,    81,    33,    82,   105,   118,   115,   115,   115,
      27,   105,    64,    78,    78,    83,    83,    83,    65,    83,
     105,   113
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    84,    86,    85,    87,    87,    89,    88,    90,    90,
      90,    91,    91,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    93,    93,    95,    94,    94,    96,    96,
      97,    97,    98,    99,    99,   100,   100,   101,   101,   103,
     102,   104,   104,   105,   105,   105,   106,   106,   107,   108,
     108,   109,   109,   109,   110,   111,   112,   112,   112,   112,
     112,   113,   113,   113,   113,   113,   113,   113,   113,   113,
     113,   113,   113,   113,   113,   113,   113,   113,   113,   113,
     113,   113,   113,   113,   113,   114,   114,   114,   114,   114,
     114,   114,   114,   115,   115,   115,   115,   115,   116,   117,
     117,   118,   118,   118,   118,   118,   118,   118,   119,   120,
     120,   121,   121,   122,   122,   122,   122,   122,   122,   122,
     122,   122,   122,   122,   122,   122,   122,   122,   122,   122,
     122,   122,   122,   122,   122,   122,   122,   122,   122,   122,
     122,   123,   123
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     0,     5,     1,     1,
       1,     0,     2,     3,     5,     4,     6,     3,     5,     4,
       6,     9,    11,     0,     2,     0,     6,     8,     0,     1,
       1,     3,     3,     0,     2,     8,    10,     0,     2,     0,
       8,     2,     3,     4,     3,     3,     0,     2,     4,     0,
       1,     0,     2,     2,     4,     7,     1,     4,     6,     6,
       6,     1,     1,     1,     1,     4,     6,     2,     2,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     5,     3,     1,     1,     2,     3,     3,
       3,     3,     3,     1,     1,     3,     3,     3,     1,     1,
       3,     1,     1,     2,     3,     3,     3,     3,     1,     1,
       2,     1,     3,     1,     1,     1,     1,     4,     6,     6,
       6,     6,     6,     2,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     5,
       3,     5,     8
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
      yyerror (scanner, YY_("syntax error: cannot back up")); \
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
                  Type, Value, Location, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void* scanner)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (yylocationp);
  YYUSE (scanner);
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void* scanner)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, scanner);
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
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, void* scanner)
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
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       , scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, scanner); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, void* scanner)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (scanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yytype)
    {
          case 34: /* NUMBER  */
#line 420 "parser.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).number); }
#line 1800 "parser.cc" /* yacc.c:1257  */
        break;

    case 113: /* expr  */
#line 419 "parser.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).expr); }
#line 1806 "parser.cc" /* yacc.c:1257  */
        break;

    case 114: /* rate_expr  */
#line 419 "parser.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).expr); }
#line 1812 "parser.cc" /* yacc.c:1257  */
        break;

    case 115: /* const_rate_expr  */
#line 419 "parser.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).expr); }
#line 1818 "parser.cc" /* yacc.c:1257  */
        break;

    case 117: /* arguments  */
#line 421 "parser.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).arguments); }
#line 1824 "parser.cc" /* yacc.c:1257  */
        break;

    case 118: /* const_expr  */
#line 419 "parser.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).expr); }
#line 1830 "parser.cc" /* yacc.c:1257  */
        break;

    case 122: /* property  */
#line 419 "parser.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).expr); }
#line 1836 "parser.cc" /* yacc.c:1257  */
        break;


      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void* scanner)
{
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
      yychar = yylex (scanner);
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
#line 425 "parser.yy" /* yacc.c:1646  */
    { success = true; }
#line 2124 "parser.cc" /* yacc.c:1646  */
    break;

  case 3:
#line 426 "parser.yy" /* yacc.c:1646  */
    { check_undeclared(); if (!success) YYERROR; }
#line 2130 "parser.cc" /* yacc.c:1646  */
    break;

  case 6:
#line 437 "parser.yy" /* yacc.c:1646  */
    { prepare_model((yyvsp[0].model_type)); }
#line 2136 "parser.cc" /* yacc.c:1646  */
    break;

  case 7:
#line 438 "parser.yy" /* yacc.c:1646  */
    { compile_model(); }
#line 2142 "parser.cc" /* yacc.c:1646  */
    break;

  case 8:
#line 442 "parser.yy" /* yacc.c:1646  */
    { (yyval.model_type) = ModelType::CTMC; }
#line 2148 "parser.cc" /* yacc.c:1646  */
    break;

  case 9:
#line 444 "parser.yy" /* yacc.c:1646  */
    { (yyval.model_type) = ModelType::CTMC; }
#line 2154 "parser.cc" /* yacc.c:1646  */
    break;

  case 10:
#line 446 "parser.yy" /* yacc.c:1646  */
    { (yyval.model_type) = ModelType::GSMP; }
#line 2160 "parser.cc" /* yacc.c:1646  */
    break;

  case 13:
#line 457 "parser.yy" /* yacc.c:1646  */
    { declare_constant((yyvsp[-1].str), nullptr); }
#line 2166 "parser.cc" /* yacc.c:1646  */
    break;

  case 14:
#line 459 "parser.yy" /* yacc.c:1646  */
    { declare_constant((yyvsp[-3].str), (yyvsp[-1].expr)); }
#line 2172 "parser.cc" /* yacc.c:1646  */
    break;

  case 15:
#line 461 "parser.yy" /* yacc.c:1646  */
    { declare_constant((yyvsp[-1].str), nullptr); }
#line 2178 "parser.cc" /* yacc.c:1646  */
    break;

  case 16:
#line 463 "parser.yy" /* yacc.c:1646  */
    { declare_constant((yyvsp[-3].str), (yyvsp[-1].expr)); }
#line 2184 "parser.cc" /* yacc.c:1646  */
    break;

  case 17:
#line 465 "parser.yy" /* yacc.c:1646  */
    { declare_rate((yyvsp[-1].str), nullptr); }
#line 2190 "parser.cc" /* yacc.c:1646  */
    break;

  case 18:
#line 467 "parser.yy" /* yacc.c:1646  */
    { declare_rate((yyvsp[-3].str), (yyvsp[-1].expr)); }
#line 2196 "parser.cc" /* yacc.c:1646  */
    break;

  case 19:
#line 469 "parser.yy" /* yacc.c:1646  */
    { declare_rate((yyvsp[-1].str), nullptr); }
#line 2202 "parser.cc" /* yacc.c:1646  */
    break;

  case 20:
#line 471 "parser.yy" /* yacc.c:1646  */
    { declare_rate((yyvsp[-3].str), (yyvsp[-1].expr)); }
#line 2208 "parser.cc" /* yacc.c:1646  */
    break;

  case 21:
#line 473 "parser.yy" /* yacc.c:1646  */
    { declare_variable((yyvsp[-7].str), (yyvsp[-4].expr), (yyvsp[-2].expr), nullptr); }
#line 2214 "parser.cc" /* yacc.c:1646  */
    break;

  case 22:
#line 476 "parser.yy" /* yacc.c:1646  */
    { declare_variable((yyvsp[-9].str), (yyvsp[-6].expr), (yyvsp[-4].expr), (yyvsp[-1].expr)); }
#line 2220 "parser.cc" /* yacc.c:1646  */
    break;

  case 25:
#line 487 "parser.yy" /* yacc.c:1646  */
    { prepare_module((yyvsp[0].str)); }
#line 2226 "parser.cc" /* yacc.c:1646  */
    break;

  case 26:
#line 489 "parser.yy" /* yacc.c:1646  */
    { add_module(); }
#line 2232 "parser.cc" /* yacc.c:1646  */
    break;

  case 27:
#line 491 "parser.yy" /* yacc.c:1646  */
    { add_module((yyvsp[-6].str), (yyvsp[-4].str)); }
#line 2238 "parser.cc" /* yacc.c:1646  */
    break;

  case 32:
#line 502 "parser.yy" /* yacc.c:1646  */
    { add_substitution((yyvsp[-2].str), (yyvsp[0].str)); }
#line 2244 "parser.cc" /* yacc.c:1646  */
    break;

  case 35:
#line 510 "parser.yy" /* yacc.c:1646  */
    { declare_variable((yyvsp[-7].str), (yyvsp[-4].expr), (yyvsp[-2].expr), nullptr); }
#line 2250 "parser.cc" /* yacc.c:1646  */
    break;

  case 36:
#line 513 "parser.yy" /* yacc.c:1646  */
    { declare_variable((yyvsp[-9].str), (yyvsp[-6].expr), (yyvsp[-4].expr), (yyvsp[-1].expr)); }
#line 2256 "parser.cc" /* yacc.c:1646  */
    break;

  case 39:
#line 521 "parser.yy" /* yacc.c:1646  */
    { prepare_command((yyvsp[-4].synch), WrapUnique((yyvsp[-3].expr)), (yyvsp[-1].dist)); }
#line 2262 "parser.cc" /* yacc.c:1646  */
    break;

  case 40:
#line 522 "parser.yy" /* yacc.c:1646  */
    { add_command(); }
#line 2268 "parser.cc" /* yacc.c:1646  */
    break;

  case 41:
#line 525 "parser.yy" /* yacc.c:1646  */
    { (yyval.synch) = 0; }
#line 2274 "parser.cc" /* yacc.c:1646  */
    break;

  case 42:
#line 526 "parser.yy" /* yacc.c:1646  */
    { (yyval.synch) = synchronization_value((yyvsp[-1].str)); }
#line 2280 "parser.cc" /* yacc.c:1646  */
    break;

  case 43:
#line 529 "parser.yy" /* yacc.c:1646  */
    { add_update((yyvsp[-3].str), (yyvsp[0].expr)); }
#line 2286 "parser.cc" /* yacc.c:1646  */
    break;

  case 54:
#line 555 "parser.yy" /* yacc.c:1646  */
    { delete (yyvsp[-3].expr); delete (yyvsp[-1].expr); }
#line 2292 "parser.cc" /* yacc.c:1646  */
    break;

  case 55:
#line 559 "parser.yy" /* yacc.c:1646  */
    { delete (yyvsp[-5].str); delete (yyvsp[-3].expr); delete (yyvsp[-1].expr); }
#line 2298 "parser.cc" /* yacc.c:1646  */
    break;

  case 56:
#line 567 "parser.yy" /* yacc.c:1646  */
    { (yyval.dist) = NewExponential((yyvsp[0].expr)); }
#line 2304 "parser.cc" /* yacc.c:1646  */
    break;

  case 57:
#line 569 "parser.yy" /* yacc.c:1646  */
    { (yyval.dist) = NewExponential((yyvsp[-1].expr)); }
#line 2310 "parser.cc" /* yacc.c:1646  */
    break;

  case 58:
#line 571 "parser.yy" /* yacc.c:1646  */
    { (yyval.dist) = NewWeibull((yyvsp[-3].expr), (yyvsp[-1].expr)); }
#line 2316 "parser.cc" /* yacc.c:1646  */
    break;

  case 59:
#line 573 "parser.yy" /* yacc.c:1646  */
    { (yyval.dist) = NewLognormal((yyvsp[-3].expr), (yyvsp[-1].expr)); }
#line 2322 "parser.cc" /* yacc.c:1646  */
    break;

  case 60:
#line 575 "parser.yy" /* yacc.c:1646  */
    { (yyval.dist) = NewUniform((yyvsp[-3].expr), (yyvsp[-1].expr)); }
#line 2328 "parser.cc" /* yacc.c:1646  */
    break;

  case 61:
#line 582 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = new Literal(*(yyvsp[0].number)); delete (yyvsp[0].number); }
#line 2334 "parser.cc" /* yacc.c:1646  */
    break;

  case 62:
#line 584 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = new Literal(true); }
#line 2340 "parser.cc" /* yacc.c:1646  */
    break;

  case 63:
#line 586 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = new Literal(true); }
#line 2346 "parser.cc" /* yacc.c:1646  */
    break;

  case 64:
#line 588 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = find_variable((yyvsp[0].str)); }
#line 2352 "parser.cc" /* yacc.c:1646  */
    break;

  case 65:
#line 590 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = new FunctionCall((yyvsp[-3].function), std::move(*(yyvsp[-1].arguments))); delete (yyvsp[-1].arguments); }
#line 2358 "parser.cc" /* yacc.c:1646  */
    break;

  case 66:
#line 592 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = new FunctionCall((yyvsp[-3].function), std::move(*(yyvsp[-1].arguments))); delete (yyvsp[-1].arguments); }
#line 2364 "parser.cc" /* yacc.c:1646  */
    break;

  case 67:
#line 594 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNegate((yyvsp[0].expr)); }
#line 2370 "parser.cc" /* yacc.c:1646  */
    break;

  case 68:
#line 596 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNot((yyvsp[0].expr)); }
#line 2376 "parser.cc" /* yacc.c:1646  */
    break;

  case 69:
#line 598 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewPlus((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2382 "parser.cc" /* yacc.c:1646  */
    break;

  case 70:
#line 600 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewMinus((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2388 "parser.cc" /* yacc.c:1646  */
    break;

  case 71:
#line 602 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewMultiply((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2394 "parser.cc" /* yacc.c:1646  */
    break;

  case 72:
#line 604 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewDivide((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2400 "parser.cc" /* yacc.c:1646  */
    break;

  case 73:
#line 606 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewAnd((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2406 "parser.cc" /* yacc.c:1646  */
    break;

  case 74:
#line 608 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewOr((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2412 "parser.cc" /* yacc.c:1646  */
    break;

  case 75:
#line 610 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewImply((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2418 "parser.cc" /* yacc.c:1646  */
    break;

  case 76:
#line 612 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewIff((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2424 "parser.cc" /* yacc.c:1646  */
    break;

  case 77:
#line 614 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLess((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2430 "parser.cc" /* yacc.c:1646  */
    break;

  case 78:
#line 616 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLessEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2436 "parser.cc" /* yacc.c:1646  */
    break;

  case 79:
#line 618 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewGreaterEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2442 "parser.cc" /* yacc.c:1646  */
    break;

  case 80:
#line 620 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewGreater((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2448 "parser.cc" /* yacc.c:1646  */
    break;

  case 81:
#line 622 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2454 "parser.cc" /* yacc.c:1646  */
    break;

  case 82:
#line 624 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNotEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2460 "parser.cc" /* yacc.c:1646  */
    break;

  case 83:
#line 626 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewConditional((yyvsp[-4].expr), (yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2466 "parser.cc" /* yacc.c:1646  */
    break;

  case 84:
#line 628 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[-1].expr); }
#line 2472 "parser.cc" /* yacc.c:1646  */
    break;

  case 85:
#line 632 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = make_literal((yyvsp[0].number)); }
#line 2478 "parser.cc" /* yacc.c:1646  */
    break;

  case 86:
#line 634 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = find_rate_or_variable((yyvsp[0].str)); }
#line 2484 "parser.cc" /* yacc.c:1646  */
    break;

  case 87:
#line 636 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNegate((yyvsp[0].expr)); }
#line 2490 "parser.cc" /* yacc.c:1646  */
    break;

  case 88:
#line 638 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewPlus((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2496 "parser.cc" /* yacc.c:1646  */
    break;

  case 89:
#line 640 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewMinus((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2502 "parser.cc" /* yacc.c:1646  */
    break;

  case 90:
#line 642 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewMultiply((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2508 "parser.cc" /* yacc.c:1646  */
    break;

  case 91:
#line 644 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewDivide((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2514 "parser.cc" /* yacc.c:1646  */
    break;

  case 92:
#line 646 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[-1].expr); }
#line 2520 "parser.cc" /* yacc.c:1646  */
    break;

  case 93:
#line 650 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = make_literal((yyvsp[0].number)); }
#line 2526 "parser.cc" /* yacc.c:1646  */
    break;

  case 94:
#line 652 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = find_rate((yyvsp[0].str)); }
#line 2532 "parser.cc" /* yacc.c:1646  */
    break;

  case 95:
#line 654 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewMultiply((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2538 "parser.cc" /* yacc.c:1646  */
    break;

  case 96:
#line 656 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewDivide((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2544 "parser.cc" /* yacc.c:1646  */
    break;

  case 97:
#line 658 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[-1].expr); }
#line 2550 "parser.cc" /* yacc.c:1646  */
    break;

  case 98:
#line 662 "parser.yy" /* yacc.c:1646  */
    { (yyval.function) = MakeFunction(*(yyvsp[0].str)); delete (yyvsp[0].str); }
#line 2556 "parser.cc" /* yacc.c:1646  */
    break;

  case 99:
#line 666 "parser.yy" /* yacc.c:1646  */
    { (yyval.arguments) = new UniquePtrVector<const Expression>(WrapUnique((yyvsp[0].expr))); }
#line 2562 "parser.cc" /* yacc.c:1646  */
    break;

  case 100:
#line 668 "parser.yy" /* yacc.c:1646  */
    { (yyval.arguments) = (yyvsp[-2].arguments); (yyval.arguments)->push_back(WrapUnique((yyvsp[0].expr))); }
#line 2568 "parser.cc" /* yacc.c:1646  */
    break;

  case 101:
#line 676 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = make_literal((yyvsp[0].nat)); }
#line 2574 "parser.cc" /* yacc.c:1646  */
    break;

  case 102:
#line 678 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = find_constant((yyvsp[0].str)); }
#line 2580 "parser.cc" /* yacc.c:1646  */
    break;

  case 103:
#line 680 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNegate((yyvsp[0].expr)); }
#line 2586 "parser.cc" /* yacc.c:1646  */
    break;

  case 104:
#line 682 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewPlus((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2592 "parser.cc" /* yacc.c:1646  */
    break;

  case 105:
#line 684 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewMinus((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2598 "parser.cc" /* yacc.c:1646  */
    break;

  case 106:
#line 686 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewMultiply((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2604 "parser.cc" /* yacc.c:1646  */
    break;

  case 107:
#line 688 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[-1].expr); }
#line 2610 "parser.cc" /* yacc.c:1646  */
    break;

  case 108:
#line 691 "parser.yy" /* yacc.c:1646  */
    { (yyval.nat) = integer_value((yyvsp[0].number)); }
#line 2616 "parser.cc" /* yacc.c:1646  */
    break;

  case 111:
#line 703 "parser.yy" /* yacc.c:1646  */
    { properties.push_back(WrapUnique((yyvsp[0].expr))); }
#line 2622 "parser.cc" /* yacc.c:1646  */
    break;

  case 112:
#line 705 "parser.yy" /* yacc.c:1646  */
    { properties.push_back(WrapUnique((yyvsp[0].expr))); }
#line 2628 "parser.cc" /* yacc.c:1646  */
    break;

  case 113:
#line 709 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = new Literal(*(yyvsp[0].number)); delete (yyvsp[0].number); }
#line 2634 "parser.cc" /* yacc.c:1646  */
    break;

  case 114:
#line 711 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = new Literal(true); }
#line 2640 "parser.cc" /* yacc.c:1646  */
    break;

  case 115:
#line 713 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = new Literal(false); }
#line 2646 "parser.cc" /* yacc.c:1646  */
    break;

  case 116:
#line 715 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = value_or_variable((yyvsp[0].str)); }
#line 2652 "parser.cc" /* yacc.c:1646  */
    break;

  case 117:
#line 717 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = new FunctionCall((yyvsp[-3].function), std::move(*(yyvsp[-1].arguments))); delete (yyvsp[-1].arguments); }
#line 2658 "parser.cc" /* yacc.c:1646  */
    break;

  case 118:
#line 719 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = new FunctionCall((yyvsp[-3].function), std::move(*(yyvsp[-1].arguments))); delete (yyvsp[-1].arguments); }
#line 2664 "parser.cc" /* yacc.c:1646  */
    break;

  case 119:
#line 721 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewProbabilityLess(Double((yyvsp[-3].number)), (yyvsp[-1].path)); }
#line 2670 "parser.cc" /* yacc.c:1646  */
    break;

  case 120:
#line 723 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewProbabilityLessEqual(Double((yyvsp[-3].number)), (yyvsp[-1].path)); }
#line 2676 "parser.cc" /* yacc.c:1646  */
    break;

  case 121:
#line 725 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewProbabilityGreaterEqual(Double((yyvsp[-3].number)), (yyvsp[-1].path)); }
#line 2682 "parser.cc" /* yacc.c:1646  */
    break;

  case 122:
#line 727 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewProbabilityGreater(Double((yyvsp[-3].number)), (yyvsp[-1].path)); }
#line 2688 "parser.cc" /* yacc.c:1646  */
    break;

  case 123:
#line 729 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNegate((yyvsp[0].expr)); }
#line 2694 "parser.cc" /* yacc.c:1646  */
    break;

  case 124:
#line 731 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNot((yyvsp[0].expr)); }
#line 2700 "parser.cc" /* yacc.c:1646  */
    break;

  case 125:
#line 733 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewPlus((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2706 "parser.cc" /* yacc.c:1646  */
    break;

  case 126:
#line 735 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewMinus((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2712 "parser.cc" /* yacc.c:1646  */
    break;

  case 127:
#line 737 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewMultiply((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2718 "parser.cc" /* yacc.c:1646  */
    break;

  case 128:
#line 739 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewDivide((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2724 "parser.cc" /* yacc.c:1646  */
    break;

  case 129:
#line 741 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewAnd((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2730 "parser.cc" /* yacc.c:1646  */
    break;

  case 130:
#line 743 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewOr((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2736 "parser.cc" /* yacc.c:1646  */
    break;

  case 131:
#line 745 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewImply((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2742 "parser.cc" /* yacc.c:1646  */
    break;

  case 132:
#line 747 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewIff((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2748 "parser.cc" /* yacc.c:1646  */
    break;

  case 133:
#line 749 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLess((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2754 "parser.cc" /* yacc.c:1646  */
    break;

  case 134:
#line 751 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLessEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2760 "parser.cc" /* yacc.c:1646  */
    break;

  case 135:
#line 753 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewGreaterEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2766 "parser.cc" /* yacc.c:1646  */
    break;

  case 136:
#line 755 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewGreater((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2772 "parser.cc" /* yacc.c:1646  */
    break;

  case 137:
#line 757 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2778 "parser.cc" /* yacc.c:1646  */
    break;

  case 138:
#line 759 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNotEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2784 "parser.cc" /* yacc.c:1646  */
    break;

  case 139:
#line 761 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewConditional((yyvsp[-4].expr), (yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2790 "parser.cc" /* yacc.c:1646  */
    break;

  case 140:
#line 763 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[-1].expr); }
#line 2796 "parser.cc" /* yacc.c:1646  */
    break;

  case 141:
#line 767 "parser.yy" /* yacc.c:1646  */
    { (yyval.path) = NewUntil(0, Double((yyvsp[-1].number)), (yyvsp[-4].expr), (yyvsp[0].expr)); }
#line 2802 "parser.cc" /* yacc.c:1646  */
    break;

  case 142:
#line 769 "parser.yy" /* yacc.c:1646  */
    { (yyval.path) = NewUntil(Double((yyvsp[-4].number)), Double((yyvsp[-2].number)), (yyvsp[-7].expr), (yyvsp[0].expr)); }
#line 2808 "parser.cc" /* yacc.c:1646  */
    break;


#line 2812 "parser.cc" /* yacc.c:1646  */
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
      yyerror (scanner, YY_("syntax error"));
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
        yyerror (scanner, yymsgp);
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
                      yytoken, &yylval, &yylloc, scanner);
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
                  yystos[yystate], yyvsp, yylsp, scanner);
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
  yyerror (scanner, YY_("memory exhausted"));
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
                  yytoken, &yylval, &yylloc, scanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, yylsp, scanner);
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
#line 772 "parser.yy" /* yacc.c:1906  */


namespace {

class ConstantExpressionEvaluator : public ExpressionVisitor {
 public:
  explicit ConstantExpressionEvaluator(
      const std::map<std::string, TypedValue>* constant_values);

  TypedValue value() const { return value_; }

 private:
  virtual void DoVisitLiteral(const Literal& expr);
  virtual void DoVisitIdentifier(const Identifier& expr);
  virtual void DoVisitFunctionCall(const FunctionCall& expr);
  virtual void DoVisitUnaryOperation(const UnaryOperation& expr);
  virtual void DoVisitBinaryOperation(const BinaryOperation& expr);
  virtual void DoVisitConditional(const Conditional& expr);
  virtual void DoVisitProbabilityThresholdOperation(
      const ProbabilityThresholdOperation& expr);

  const std::map<std::string, TypedValue>* constant_values_;
  TypedValue value_;
};

ConstantExpressionEvaluator::ConstantExpressionEvaluator(
    const std::map<std::string, TypedValue>* constant_values)
    : constant_values_(constant_values), value_(0) {
}

void ConstantExpressionEvaluator::DoVisitLiteral(const Literal& expr) {
  value_ = expr.value();
}

void ConstantExpressionEvaluator::DoVisitIdentifier(const Identifier& expr) {
  auto i = constant_values_->find(expr.name());
  CHECK(i != constant_values_->end());
  value_ = i->second;
}

void ConstantExpressionEvaluator::DoVisitFunctionCall(
    const FunctionCall& expr) {
  std::vector<TypedValue> arguments;
  for (const Expression& argument : expr.arguments()) {
    argument.Accept(this);
    arguments.push_back(value_);
  }
  switch (expr.function()) {
    case Function::UNKNOWN:
      LOG(FATAL) << "bad function call";
    case Function::MIN:
      CHECK(!arguments.empty());
      value_ = arguments[0];
      for (size_t i = 1; i < arguments.size(); ++i) {
        value_ = std::min(value_, arguments[i]);
      }
      break;
    case Function::MAX:
      CHECK(!arguments.empty());
      value_ = arguments[0];
      for (size_t i = 1; i < arguments.size(); ++i) {
        value_ = std::max(value_, arguments[i]);
      }
      break;
    case Function::FLOOR:
      CHECK(arguments.size() == 1);
      value_ = floor(arguments[0]);
      break;
    case Function::CEIL:
      CHECK(arguments.size() == 2);
      value_ = ceil(arguments[0]);
      break;
    case Function::POW:
      CHECK(arguments.size() == 2);
      value_ = pow(arguments[0], arguments[1]);
      break;
    case Function::LOG:
      CHECK(arguments.size() == 2);
      value_ = log(arguments[0]) / log(arguments[1]);
      break;
    case Function::MOD:
      CHECK(arguments.size() == 2);
      value_ = arguments[0] % arguments[1];
      break;
  }
}

void ConstantExpressionEvaluator::DoVisitUnaryOperation(
    const UnaryOperation& expr) {
  expr.operand().Accept(this);
  switch (expr.op()) {
    case UnaryOperator::NEGATE:
      value_ = -value_;
      break;
    case UnaryOperator::NOT:
      value_ = !value_;
      break;
  }
}

void ConstantExpressionEvaluator::DoVisitBinaryOperation(
    const BinaryOperation& expr) {
  expr.operand1().Accept(this);
  TypedValue operand1 = value_;
  expr.operand2().Accept(this);
  switch (expr.op()) {
    case BinaryOperator::PLUS:
      value_ = operand1 + value_;
      break;
    case BinaryOperator::MINUS:
      value_ = operand1 - value_;
      break;
    case BinaryOperator::MULTIPLY:
      value_ = operand1 * value_;
      break;
    case BinaryOperator::DIVIDE:
      value_ = operand1 / value_;
      break;
    case BinaryOperator::AND:
      value_ = operand1.value<bool>() && value_.value<bool>();
      break;
    case BinaryOperator::OR:
      value_ = operand1.value<bool>() || value_.value<bool>();
      break;
    case BinaryOperator::IMPLY:
      value_ = !operand1.value<bool>() || value_.value<bool>();
      break;
    case BinaryOperator::IFF:
      value_ = operand1.value<bool>() == value_.value<bool>();
      break;
    case BinaryOperator::LESS:
      value_ = operand1 < value_;
      break;
    case BinaryOperator::LESS_EQUAL:
      value_ = operand1 <= value_;
      break;
    case BinaryOperator::GREATER_EQUAL:
      value_ = operand1 >= value_;
      break;
    case BinaryOperator::GREATER:
      value_ = operand1 > value_;
      break;
    case BinaryOperator::EQUAL:
      value_ = operand1 == value_;
      break;
    case BinaryOperator::NOT_EQUAL:
      value_ = operand1 != value_;
      break;
  }
}

void ConstantExpressionEvaluator::DoVisitConditional(const Conditional& expr) {
  expr.condition().Accept(this);
  if (value_.value<bool>()) {
    expr.if_branch().Accept(this);
  } else {
    expr.else_branch().Accept(this);
  }
}

void ConstantExpressionEvaluator::DoVisitProbabilityThresholdOperation(
    const ProbabilityThresholdOperation& expr) {
  LOG(FATAL) << "not a constant expression";
}

TypedValue EvaluateConstantExpression(
    const Expression& expr,
    const std::map<std::string, TypedValue>& constant_values) {
  ConstantExpressionEvaluator evaluator(&constant_values);
  expr.Accept(&evaluator);
  return evaluator.value();
}

bool InVariableSet(
    const Model& model, const std::set<int> variables, const std::string name) {
  for (std::set<int>::const_iterator i = variables.begin();
       i != variables.end(); ++i) {
    if (name == model.variables()[*i].name()) {
      return true;
    }
  }
  return false;
}

}  // namespace

void clear_declarations() {
  constants.clear();
  constant_values.clear();
  variables.clear();
}

static void check_undeclared() {
  if (!undeclared.empty()) {
    yyerror("undeclared variables used in expressions");
    for (std::set<std::string>::const_iterator si = undeclared.begin();
	 si != undeclared.end(); si++) {
      std::cerr << "  " << *si << std::endl;
    }
  }
  undeclared.clear();
  rates.clear();
  rate_values.clear();
  modules.clear();
  synchronizations.clear();
  variable_lows.clear();
  variable_highs.clear();
  variable_starts.clear();
}

static int integer_value(const TypedValue* q) {
  int n;
  if (q->type() != Type::INT) {
    yyerror("expecting integer");
    n = 0;
  } else {
    n = q->value<int>();
  }
  delete q;
  return n;
}

static const Identifier* find_constant(const std::string* ident) {
  if (constants.find(*ident) == constants.end()) {
    if (variables.find(*ident) != variables.end()) {
      yyerror("variable `" + *ident + "' used where expecting constant");
    } else if (rates.find(*ident) != rates.end()) {
      yyerror("rate `" + *ident + "' used where expecting constant");
    } else {
      yyerror("undeclared constant `" + *ident + "'");
    }
    variables.insert(*ident);
    rates.insert(*ident);
    constants.insert(*ident);
    rate_values.insert({*ident, 0});
    constant_values.insert({*ident, 0});
  }
  const Identifier* identifier = new Identifier(*ident);
  delete ident;
  return identifier;
}

static const Identifier* find_rate(const std::string* ident) {
  if (rates.find(*ident) == rates.end()) {
    if (variables.find(*ident) != variables.end()) {
      yyerror("variable `" + *ident + "' used where expecting rate");
    } else {
      yyerror("undeclared rate `" + *ident + "'");
    }
    variables.insert(*ident);
    rates.insert(*ident);
    rate_values.insert({*ident, 0});
  }
  const Identifier* identifier = new Identifier(*ident);
  delete ident;
  return identifier;
}

static const Identifier* find_rate_or_variable(const std::string* ident) {
  if (rates.find(*ident) != rates.end()) {
    const Identifier* identifier = new Identifier(*ident);
    delete ident;
    return identifier;
  } else {
    return find_variable(ident);
  }
}

static const Literal* make_literal(int n) {
  return new Literal(n);
}

static const Literal* make_literal(const TypedValue* q) {
  const Literal* v = new Literal(*q);
  delete q;
  return v;
}

static const Expression* value_or_variable(const std::string* ident) {
  if (constants.find(*ident) != constants.end()) {
    const Literal* value = new Literal(constant_values.find(*ident)->second);
    delete ident;
    return value;
  } else {
    if (variables.find(*ident) == variables.end()) {
      variables.insert(*ident);
      undeclared.insert(*ident);
    }
    const Identifier* identifier = new Identifier(*ident);
    delete ident;
    return identifier;
  }
}

static const Identifier* find_variable(const std::string* ident) {
  if (constants.find(*ident) == constants.end()) {
    if (variables.find(*ident) == variables.end()) {
      variables.insert(*ident);
      undeclared.insert(*ident);
    }
  }
  const Identifier* identifier = new Identifier(*ident);
  delete ident;
  return identifier;
}

static void add_update(const std::string* ident, const Expression* expr) {
  if (variables.find(*ident) == variables.end()) {
    yyerror("updating undeclared variable `" + *ident + "'");
  }
  const int module_index = model->modules().size();
  if (InVariableSet(*model, model->global_variables(), *ident)) {
    if (command->synch() != 0) {
      yywarning("updating global variable in synchronized command");
    }
  } else if (!InVariableSet(*model, model->module_variables(module_index),
                            *ident)) {
    yyerror("updating variable belonging to other module");
  }
  command->add_update(new Update(*ident,
                                 std::unique_ptr<const Expression>(expr)));
  delete ident;
}

static size_t synchronization_value(const std::string* ident) {
  size_t s;
  std::map<std::string, size_t>::const_iterator si =
    synchronizations.find(*ident);
  if (si == synchronizations.end()) {
    s = synchronizations.size() + 1;
    synchronizations.insert(std::make_pair(*ident, s));
  } else {
    s = si->second;
  }
  delete ident;
  return s;
}

static void add_substitution(const std::string* ident1,
			     const std::string* ident2) {
  if (variables.find(*ident1) != variables.end()) {
    /* Variable substitution. */
    subst.insert(std::make_pair(*ident1, *ident2));
    delete ident2;
  } else {
    std::map<std::string, size_t>::const_iterator si =
      synchronizations.find(*ident1);
    if (si != synchronizations.end()) {
      /* Synchronization substitution. */
      size_t s = synchronization_value(ident2);
      synch_subst.insert({si->second, s});
    } else {
      yyerror("illegal substitution `" + *ident1 + "=" + *ident2 + "'");
      delete ident2;
    }
  }
  delete ident1;
}

static void declare_constant(const std::string* ident,
                             const Expression* value_expr) {
  if (constants.find(*ident) != constants.end()) {
    yyerror("ignoring repeated declaration of constant `" + *ident + "'");
  } else if (rates.find(*ident) != rates.end()) {
    yyerror("ignoring declaration of constant `" + *ident
	    + "' previously declared as rate");
  } else if (variables.find(*ident) != variables.end()) {
    yyerror("ignoring declaration of constant `" + *ident
	    + "' previously declared as variable");
  } else if (synchronizations.find(*ident) != synchronizations.end()) {
    yyerror("ignoring declaration of constant `" + *ident
	    + "' previously declared as synchronization");
  } else {
    std::map<std::string, TypedValue>::iterator override =
        const_overrides.find(*ident);
    if (value_expr == nullptr && override == const_overrides.end()) {
      yyerror("uninitialized constant `" + *ident + "'");
      value_expr = make_literal(0);
    }
    variables.insert(*ident);
    rates.insert(*ident);
    constants.insert(*ident);
    const int value =
        ((override != const_overrides.end()) ?
         override->second :
         EvaluateConstantExpression(*value_expr, constant_values)).value<int>();
    rate_values.insert({*ident, value});
    constant_values.insert({*ident, value});
  }
  delete ident;
  delete value_expr;
}

static void declare_rate(const std::string* ident,
                         const Expression* value_expr) {
  if (rates.find(*ident) != rates.end()) {
    yyerror("ignoring repeated declaration of rate `" + *ident + "'");
  } else if (constants.find(*ident) != constants.end()) {
    yyerror("ignoring declaration of rate `" + *ident
	    + "' previously declared as constant");
  } else if (variables.find(*ident) != variables.end()) {
    yyerror("ignoring declaration of rate `" + *ident
	    + "' previously declared as variable");
  } else if (synchronizations.find(*ident) != synchronizations.end()) {
    yyerror("ignoring declaration of rate `" + *ident
	    + "' previously declared as synchronization");
  } else {
    std::map<std::string, TypedValue>::iterator override =
        const_overrides.find(*ident);
    if (value_expr == nullptr && override == const_overrides.end()) {
      yyerror("uninitialized rate `" + *ident + "'");
      value_expr = make_literal(0);
    }
    variables.insert(*ident);
    rates.insert(*ident);
    const TypedValue value = (override != const_overrides.end()) ?
        override->second : EvaluateConstantExpression(*value_expr, rate_values);
    rate_values.insert({*ident, value});
  }
  delete ident;
  delete value_expr;
}

static bool declare_variable(const std::string* ident,
                             const Expression* low,
                             const Expression* high,
                             const Expression* start,
                             bool delayed_addition) {
  bool declared = false;
  if (constants.find(*ident) != constants.end()) {
    yyerror("ignoring declaration of variable `" + *ident
	    + "' previously declared as constant");
  } else if (rates.find(*ident) != rates.end()) {
    yyerror("ignoring declaration of variable `" + *ident
	    + "' previously declared as rate");
  } else if (synchronizations.find(*ident) != synchronizations.end()) {
    yyerror("ignoring declaration of variable `" + *ident
	    + "' previously declared as synchronization");
  } else {
    declared = true;
    if (variables.find(*ident) != variables.end()) {
      if (undeclared.find(*ident) != undeclared.end()) {
	undeclared.erase(*ident);
      } else {
	yyerror("ignoring repeated declaration of variable `" + *ident + "'");
      }
    } else {
      variables.insert(*ident);
    }
  }
  if (declared) {
    int l =
        EvaluateConstantExpression(*low, constant_values).value<int>();
    int h =
        EvaluateConstantExpression(*high, constant_values).value<int>();
    int s = ((start != nullptr)
             ? EvaluateConstantExpression(*start, constant_values).value<int>()
             : l);
    variable_lows.insert(
        std::make_pair(*ident, std::unique_ptr<const Expression>(low)));
    variable_highs.insert(
        std::make_pair(*ident, std::unique_ptr<const Expression>(high)));
    variable_starts.insert(
        std::make_pair(*ident, std::unique_ptr<const Expression>(start)));
    model->AddIntVariable(*ident, l, h, s);
    if (!delayed_addition) {
      if (module != nullptr) {
	module->add_variable(*ident);
      }
    }
  } else {
    delete low;
    delete high;
    delete start;
  }
  delete ident;
  return declared;
}


/* Adds a command to the current module. */
static void add_command() {
  module->add_command(command);
}


/* Prepares a command for parsing. */
static void prepare_command(int synch,
                            std::unique_ptr<const Expression>&& guard,
			    const Distribution* delay) {
  command = new Command(synch, std::move(guard), delay);
}

namespace {

std::string SubstituteName(
    const std::string& name,
    const std::map<std::string, std::string>& substitutions) {
  auto i = substitutions.find(name);
  return (i == substitutions.end()) ? name : i->second;
}

class ExpressionIdentifierSubstituter : public ExpressionVisitor,
                                        public PathPropertyVisitor {
 public:
  explicit ExpressionIdentifierSubstituter(
      const std::map<std::string, std::string>& substitutions);

  std::unique_ptr<const Expression> release_expr() { return std::move(expr_); }

 private:
  virtual void DoVisitLiteral(const Literal& expr);
  virtual void DoVisitIdentifier(const Identifier& expr);
  virtual void DoVisitFunctionCall(const FunctionCall& expr);
  virtual void DoVisitUnaryOperation(const UnaryOperation& expr);
  virtual void DoVisitBinaryOperation(const BinaryOperation& expr);
  virtual void DoVisitConditional(const Conditional& expr);
  virtual void DoVisitProbabilityThresholdOperation(
      const ProbabilityThresholdOperation& expr);
  virtual void DoVisitUntilProperty(const UntilProperty& path_property);

  std::map<std::string, std::string> substitutions_;
  std::unique_ptr<const Expression> expr_;
  std::unique_ptr<const PathProperty> path_property_;
};

std::unique_ptr<const Expression> SubstituteIdentifiers(
    const Expression& expr,
    const std::map<std::string, std::string>& substitutions) {
  ExpressionIdentifierSubstituter substituter(substitutions);
  expr.Accept(&substituter);
  return substituter.release_expr();
}

class DistributionIdentifierSubstituter : public DistributionVisitor {
 public:
  explicit DistributionIdentifierSubstituter(
      const std::map<std::string, std::string>& substitutions);

  ~DistributionIdentifierSubstituter();

  const Distribution* release_distribution();

 private:
  virtual void DoVisitExponential(const Exponential& distribution);
  virtual void DoVisitWeibull(const Weibull& distribution);
  virtual void DoVisitLognormal(const Lognormal& distribution);
  virtual void DoVisitUniform(const Uniform& distribution);

  std::map<std::string, std::string> substitutions_;
  const Distribution* distribution_;
};

const Distribution* SubstituteIdentifiers(
    const Distribution& distribution,
    const std::map<std::string, std::string>& substitutions) {
  DistributionIdentifierSubstituter substituter(substitutions);
  distribution.Accept(&substituter);
  return substituter.release_distribution();
}

const Update* SubstituteIdentifiers(
    const Update& update,
    const std::map<std::string, std::string>& substitutions) {
  return new Update(SubstituteName(update.variable(), substitutions),
                    SubstituteIdentifiers(update.expr(), substitutions));
}

const Command* SubstituteIdentifiers(
    const Command& command,
    const std::map<std::string, std::string>& substitutions,
    const std::map<size_t, size_t>& synchs) {
  size_t s;
  std::map<size_t, size_t>::const_iterator si = synchs.find(command.synch());
  if (si == synchs.end()) {
    s = command.synch();
  } else {
    s = si->second;
  }
  Command* subst_comm = new Command(
      s,
      SubstituteIdentifiers(command.guard(), substitutions),
      SubstituteIdentifiers(command.delay(), substitutions));
  for (const Update* update : command.updates()) {
    subst_comm->add_update(SubstituteIdentifiers(*update, substitutions));
  }
  return subst_comm;
}

Module* SubstituteIdentifiers(
    const Module& module,
    const std::map<std::string, std::string>& substitutions,
    const std::map<size_t, size_t>& syncs) {
  Module* subst_mod = new Module();
  for (const std::string& variable : module.variables()) {
    subst_mod->add_variable(SubstituteName(variable, substitutions));
  }
  for (const Command* command : module.commands()) {
    subst_mod->add_command(
        SubstituteIdentifiers(*command, substitutions, syncs));
  }
  return subst_mod;
}

ExpressionIdentifierSubstituter::ExpressionIdentifierSubstituter(
    const std::map<std::string, std::string>& substitutions)
    : substitutions_(substitutions) {
}

void ExpressionIdentifierSubstituter::DoVisitLiteral(const Literal& expr) {
  expr_.reset(new Literal(expr.value()));
}

void ExpressionIdentifierSubstituter::DoVisitIdentifier(
    const Identifier& expr) {
  auto i = substitutions_.find(expr.name());
  if (i == substitutions_.end()) {
    expr_.reset(new Identifier(expr.name()));
  } else {
    expr_.reset(new Identifier(i->second));
  }
}

void ExpressionIdentifierSubstituter::DoVisitFunctionCall(
    const FunctionCall& expr) {
  UniquePtrVector<const Expression> arguments;
  for (const Expression& argument : expr.arguments()) {
    argument.Accept(this);
    arguments.push_back(release_expr());
  }
  expr_ = FunctionCall::New(expr.function(), std::move(arguments));
}

void ExpressionIdentifierSubstituter::DoVisitUnaryOperation(
    const UnaryOperation& expr) {
  expr.operand().Accept(this);
  expr_ = UnaryOperation::New(expr.op(), release_expr());
}

void ExpressionIdentifierSubstituter::DoVisitBinaryOperation(
    const BinaryOperation& expr) {
  expr.operand1().Accept(this);
  std::unique_ptr<const Expression> operand1 = release_expr();
  expr.operand2().Accept(this);
  expr_ = BinaryOperation::New(expr.op(), std::move(operand1), release_expr());
}

void ExpressionIdentifierSubstituter::DoVisitConditional(
    const Conditional& expr) {
  expr.condition().Accept(this);
  std::unique_ptr<const Expression> condition = release_expr();
  expr.if_branch().Accept(this);
  std::unique_ptr<const Expression> if_branch = release_expr();
  expr.else_branch().Accept(this);
  expr_ = Conditional::New(std::move(condition),
                           std::move(if_branch), release_expr());
}

void ExpressionIdentifierSubstituter::DoVisitProbabilityThresholdOperation(
    const ProbabilityThresholdOperation& expr) {
  expr.path_property().Accept(this);
  expr_ = ProbabilityThresholdOperation::New(expr.op(), expr.threshold(),
                                             std::move(path_property_));
}

void ExpressionIdentifierSubstituter::DoVisitUntilProperty(
    const UntilProperty& path_property) {
  path_property.pre_expr().Accept(this);
  auto pre_expr = std::move(expr_);
  path_property.post_expr().Accept(this);
  path_property_ =
      UntilProperty::New(path_property.min_time(), path_property.max_time(),
                         std::move(pre_expr), std::move(expr_));
}

DistributionIdentifierSubstituter::DistributionIdentifierSubstituter(
    const std::map<std::string, std::string>& substitutions)
    : substitutions_(substitutions), distribution_(nullptr) {
}

DistributionIdentifierSubstituter::~DistributionIdentifierSubstituter() {
  delete distribution_;
}

const Distribution* DistributionIdentifierSubstituter::release_distribution() {
  const Distribution* distribution = distribution_;
  distribution_ = nullptr;
  return distribution;
}

void DistributionIdentifierSubstituter::DoVisitExponential(
    const Exponential& distribution) {
  distribution_ = Exponential::make(
      SubstituteIdentifiers(distribution.rate(), substitutions_));
}

void DistributionIdentifierSubstituter::DoVisitWeibull(
    const Weibull& distribution) {
  distribution_ = Weibull::make(
      SubstituteIdentifiers(distribution.scale(), substitutions_),
      SubstituteIdentifiers(distribution.shape(), substitutions_));
}

void DistributionIdentifierSubstituter::DoVisitLognormal(
    const Lognormal& distribution) {
  distribution_ = Lognormal::make(
      SubstituteIdentifiers(distribution.scale(), substitutions_),
      SubstituteIdentifiers(distribution.shape(), substitutions_));
}

void DistributionIdentifierSubstituter::DoVisitUniform(
    const Uniform& distribution) {
  distribution_ = Uniform::make(
      SubstituteIdentifiers(distribution.low(), substitutions_),
      SubstituteIdentifiers(distribution.high(), substitutions_));
}

}  // namespace

static void add_module(const std::string* ident1, const std::string* ident2) {
  std::map<std::string, Module*>::const_iterator mi = modules.find(*ident1);
  if (mi != modules.end()) {
    yyerror("ignoring repeated declaration of module `" + *ident1 + "'");
  } else {
    mi = modules.find(*ident2);
    if (mi == modules.end()) {
      yyerror("ignoring renaming of undeclared module `" + *ident2 + "'");
    } else {
      const Module& src_module = *mi->second;
      std::map<std::string, std::string> c_subst;
      for (const std::string& constant : constants) {
	auto si = subst.find(constant);
	if (si != subst.end()) {
	  auto cj = constants.find(si->second);
	  if (cj == constants.end()) {
	    yyerror("substituting constant with non-constant " + si->second);
	  } else {
	    c_subst.insert(*si);
	  }
	}
      }
      std::map<std::string, std::string> v_subst;
      model->OpenModuleScope();
      for (const std::string& variable : src_module.variables()) {
	auto si = subst.find(variable);
	if (si == subst.end()) {
	  yyerror("missing substitution for module variable");
	} else {
          std::unique_ptr<const Expression> low =
              SubstituteIdentifiers(*variable_lows.find(variable)->second,
                                    c_subst);
          std::unique_ptr<const Expression> high =
              SubstituteIdentifiers(*variable_highs.find(variable)->second,
                                    c_subst);
          std::unique_ptr<const Expression> start;
	  auto i = variable_starts.find(variable);
	  if (i->second != nullptr) {
	    start = SubstituteIdentifiers(*i->second, c_subst);
	  }
	  if (declare_variable(new std::string(si->second), low.release(),
                               high.release(), start.release(), true)) {
	    v_subst.insert(*si);
	  }
	}
      }
      for (auto si = subst.begin(); si != subst.end(); si++) {
        std::unique_ptr<const Identifier> v1(
            find_variable(new std::string(si->first)));
	if (find(src_module.variables().begin(), src_module.variables().end(),
                 si->first) != src_module.variables().end()) {
          std::unique_ptr<const Identifier> v2(
              find_variable(new std::string(si->second)));
          v_subst.insert(*si);
	}
      }
      Module* mod = SubstituteIdentifiers(src_module, v_subst, synch_subst);
      modules.insert(std::make_pair(*ident1, mod));
      model->add_module(*mod);
      model->CloseModuleScope();
    }
  }
  subst.clear();
  synch_subst.clear();
  delete ident1;
  delete ident2;
}


/* Adds a module to the current model. */
static void add_module() {
  model->add_module(*module);
  module = nullptr;
  model->CloseModuleScope();
}


/* Prepares a module for parsing. */
static void prepare_module(const std::string* ident) {
  std::map<std::string, Module*>::const_iterator mi = modules.find(*ident);
  if (mi != modules.end()) {
    yyerror("ignoring repeated declaration of module `" + *ident + "'");
  } else {
    module = new Module();
    modules.insert(std::make_pair(*ident, module));
    model->OpenModuleScope();
  }
  delete ident;
}


/* Prepares a model for parsing. */
static void prepare_model(ModelType model_type) {
  clear_declarations();
  properties.clear();
  if (model != nullptr) {
    delete model;
  }
  model = new Model();
  model->set_type(model_type);
}


/* Compiles the current model. */
static void compile_model() {
  for (std::map<std::string, Module*>::const_iterator mi = modules.begin();
       mi != modules.end(); mi++) {
    mi->second->compile(constant_values, rate_values);
  }
  model->compile();
  global_model = model;
}

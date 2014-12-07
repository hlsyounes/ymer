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

#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <string>

#include "models.h"
#include "src/distribution.h"
#include "src/strutil.h"

#include "glog/logging.h"

#include "parser.hh"

// Lexical analyzer function.
extern int yylex(YYSTYPE* lvalp, YYLTYPE* llocp, void* scanner);
/* Name of current file. */
extern std::string current_file;

/* Last model parsed. */
const Model* global_model = nullptr;
/* Parsed properties. */
UniquePtrVector<const Expression> properties;

/* Current model. */
static Model* model;
/* Current identifier substitutions. */
static std::map<std::string, std::string> subst;
/* Whether the last parsing attempt succeeded. */
static bool success = true;

/* Returns a variable for the given identifier. */
static const Identifier* find_variable(const std::string* ident);
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
static void declare_variable(const std::string* ident, const Expression* low,
                             const Expression* high, const Expression* start);
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

void AddConstant(const std::string* name, Type type, const Expression* init) {
  std::vector<std::string> errors;
  if (!model->AddConstant(*WrapUnique(name), type, WrapUnique(init), &errors)) {
    for (const auto& error : errors) {
      yyerror(error);
    }
  }
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

void AddCommand(const std::string* action, const Expression* guard,
                std::vector<Outcome>* outcomes) {
        std::vector<std::string> errors;
        if (!model->AddCommand(Command(*WrapUnique(action), WrapUnique(guard),
                                       std::move(*WrapUnique(outcomes))),
                               &errors)) {
    for (const auto& error : errors) {
      yyerror(error);
    }
  }
}

}  // namespace

#line 380 "parser.cc" /* yacc.c:339  */

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
#line 377 "parser.yy" /* yacc.c:355  */

  ModelType model_type;
  const PathProperty* path;
  std::vector<Outcome>* outcomes;
  Outcome* outcome;
  const Distribution* dist;
  std::vector<Update>* updates;
  Update* update;
  const Expression* expr;
  const std::string* str;
  const TypedValue* number;
  Function function;
  UniquePtrVector<const Expression>* arguments;

#line 561 "parser.cc" /* yacc.c:355  */
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

#line 590 "parser.cc" /* yacc.c:358  */

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
#define YYLAST   1001

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  83
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  40
/* YYNRULES -- Number of rules.  */
#define YYNRULES  132
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  305

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
       0,   412,   412,   412,   416,   417,   424,   424,   428,   430,
     432,   434,   436,   438,   440,   447,   448,   451,   453,   455,
     457,   459,   461,   463,   465,   467,   469,   477,   478,   481,
     481,   484,   488,   489,   492,   493,   496,   499,   500,   503,
     505,   509,   510,   513,   518,   519,   522,   524,   527,   529,
     533,   537,   539,   542,   544,   548,   556,   557,   560,   563,
     564,   567,   568,   569,   572,   576,   584,   586,   588,   590,
     597,   599,   601,   603,   605,   607,   609,   611,   613,   615,
     617,   619,   621,   623,   625,   627,   629,   631,   633,   635,
     637,   639,   641,   643,   647,   649,   651,   655,   657,   665,
     666,   669,   671,   675,   677,   679,   681,   683,   685,   687,
     689,   691,   693,   695,   697,   699,   701,   703,   705,   707,
     709,   711,   713,   715,   717,   719,   721,   723,   725,   727,
     729,   733,   735
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
  "file", "$@1", "model_or_properties", "model", "$@2", "model_type",
  "declarations", "declaration", "modules", "module_decl", "$@3",
  "substitutions", "subst_list", "subst", "variables", "variable_decl",
  "commands", "command", "action", "outcomes", "update_distribution",
  "distribution_and_updates", "true_or_updates", "updates", "update",
  "rewards", "rewards_decl", "rewards_label", "reward_rules",
  "state_reward", "transition_reward", "distribution", "expr", "function",
  "arguments", "properties", "property_list", "property", "path_property", YY_NULLPTR
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

#define YYPACT_NINF -88

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-88)))

#define YYTABLE_NINF -95

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -88,    17,   244,   -88,   -88,   -88,   -88,   -88,   -88,   -88,
     -88,   -88,   -88,   -88,   -88,   -54,   -24,   -88,    16,   329,
     329,   329,   -88,   -88,   -88,    33,   -88,   -51,   832,    70,
       1,    64,    84,   103,   -88,   -88,   365,   -88,   336,   329,
     329,   329,   329,   329,   329,   329,   329,   329,   329,   329,
     329,   329,   329,   329,   329,   -88,    81,    88,    89,    92,
      94,   -88,     9,   -88,   -88,    93,   -24,   -88,   336,   336,
     336,   848,    95,   -26,   832,   768,   861,   886,   909,    23,
     129,   129,    63,    63,    63,    63,    32,    32,   -88,   -88,
     336,   329,   329,   329,   329,    18,   136,   140,   -88,   151,
      70,   -88,   -88,   388,   336,   336,   336,   336,   336,   336,
     336,   336,   336,   336,   336,   336,   336,   336,   336,   336,
     336,   -88,   329,    31,   606,    99,   101,   105,   106,   146,
     149,   -55,   -46,   128,   155,   -88,   167,   110,   -88,   784,
     874,   898,   920,    58,   928,   928,    85,    85,    85,    85,
      91,    91,   -88,   -88,    35,   848,   832,   -88,   -12,   -88,
     -88,   -88,   -88,   -44,   -43,   336,   -88,   336,   -88,   113,
     144,   157,   -88,   336,   336,   -88,   170,   175,   336,   -88,
     336,   -88,   622,   640,   336,   182,   -88,   -88,   -88,    80,
     848,   329,   137,   658,   676,   -88,   -88,     7,   138,   185,
     -18,   -88,   832,   186,   -88,   -88,   336,   190,   160,   -88,
       3,   -88,   195,   -88,   -88,   800,   150,   566,   166,   177,
     174,   -88,   193,   -88,   246,   -88,   201,   336,   329,   -16,
     248,   257,   190,   336,   -88,   208,   336,   694,   832,   336,
     -88,   -88,   -88,   -88,   197,   336,   816,   -88,   712,   336,
     324,   336,   -88,   586,   267,   730,   -13,   211,   209,   213,
     221,   343,   212,   240,   -88,   -88,   249,   -88,   252,   848,
     -88,   336,   -88,   336,   336,   336,   -19,   -88,   275,   232,
     -22,   748,   503,   524,   545,   253,   -88,   283,   -88,   -88,
     -88,   -88,   336,   336,   336,   336,   291,   411,   434,   457,
     480,   -88,   -88,   -88,   -88
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     0,     1,    11,    13,     9,    14,    10,    12,
       8,   104,   105,    96,    95,     0,   106,   103,     0,     0,
       0,     0,     3,     4,     6,     0,     5,    99,   101,     0,
       0,     0,     0,     0,   113,   114,     0,    15,     0,   100,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    94,     0,     0,     0,     0,
       0,   130,    27,    71,    72,     0,    73,    70,     0,     0,
       0,    97,     0,     0,   102,     0,   122,   121,   120,   119,
     127,   128,   123,   124,   125,   126,   115,   116,   117,   118,
       0,     0,     0,     0,     0,     0,     0,     0,    16,    56,
       0,    76,    77,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   107,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    28,     7,     0,    93,     0,
      85,    84,    83,    82,    90,    91,    86,    87,    88,    89,
      78,    79,    80,    81,     0,    98,   129,   108,     0,   109,
     110,   111,   112,     0,     0,     0,    17,     0,    21,     0,
      29,    59,    57,     0,     0,    74,     0,     0,     0,    19,
       0,    23,     0,     0,     0,     0,    37,    60,    61,     0,
      92,     0,     0,     0,     0,    18,    22,     0,     0,    41,
       0,    75,   131,     0,    20,    24,     0,    32,     0,    38,
       0,    58,     0,    62,    63,     0,     0,     0,     0,     0,
      33,    34,     0,    30,    44,    42,     0,     0,     0,     0,
       0,     0,     0,     0,    45,     0,     0,     0,   132,     0,
      25,    36,    31,    35,     0,     0,     0,    64,     0,     0,
       0,     0,    26,     0,     0,     0,     0,    71,     0,     0,
       0,     0,     0,    47,    48,    46,    52,    53,     0,    66,
      65,     0,    39,     0,     0,     0,    73,    43,     0,     0,
       0,     0,     0,     0,     0,     0,    49,     0,    54,    51,
      50,    40,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    69,    67,    68,    55
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -88,   -88,   -88,   -88,   -88,   -88,   -88,   -88,   -88,   -88,
     -88,   -88,   -88,   -88,    87,   -88,   -88,   -88,   -88,   -88,
     -88,   -88,    45,    46,   -88,    48,   -88,   -88,   -88,   -88,
     -88,   -88,   -88,   -68,   192,   -87,   -88,   -88,   100,    15
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    22,    23,    37,    24,    62,    98,    99,
     135,   186,   219,   220,   221,   199,   209,   210,   225,   235,
     262,   263,   264,   265,   266,   267,   136,   172,   188,   200,
     213,   214,   268,    71,    72,    73,    26,    27,   124,   125
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     101,   102,   103,   123,   239,   211,   289,   271,   285,   165,
      63,    64,    13,    14,    65,    66,    67,     3,   167,    95,
     178,   180,   166,    96,   206,    97,    39,    29,   223,   129,
     130,   168,   154,   179,   181,    57,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   152,
     153,   131,   155,    68,   120,   176,   121,   -94,    69,   287,
     212,   240,   -94,    70,   272,   104,   177,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   224,    30,    31,    32,    33,   189,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,   182,    58,   183,
      13,    14,    28,    55,    53,    54,   190,   126,   127,   128,
     193,   120,   194,   157,    38,   120,   197,   175,    59,    34,
      35,    36,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   215,    51,    52,    53,    54,    60,   217,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,   115,   116,   117,   118,   237,
     120,    90,   201,   117,   118,   244,    91,    92,   246,   132,
      93,   248,    94,   133,   100,   134,   119,   250,   159,   163,
     160,   253,   164,   255,   161,   162,   269,   169,   170,   171,
     173,   184,   187,   103,    25,    47,    48,    49,    50,    51,
      52,    53,    54,   281,   191,   282,   283,   284,   185,   192,
     269,    25,    25,    25,   249,   198,   207,   203,   208,   222,
     216,    56,   156,   218,   297,   298,   299,   300,   226,   228,
     230,    25,    25,    25,    25,    25,    25,    25,    25,    25,
      25,    25,    25,    25,    25,    25,    25,     4,     5,     6,
       7,     8,     9,    10,   232,   104,   231,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   233,    11,    12,    13,    14,    15,    16,    17,   234,
     236,   241,   242,    25,    25,    25,    25,   245,   -51,   277,
     273,   202,   137,    18,   274,   257,    64,    13,    14,    65,
      66,    67,   275,    63,    64,    13,    14,    65,    66,    67,
     278,   280,   279,   287,    25,    19,   296,   295,   285,   243,
      20,   258,   259,   286,   260,    21,   290,   288,   238,   258,
     259,     0,   260,     0,     0,     0,     0,     0,    68,     0,
       0,     0,     0,    69,     0,     0,    68,     0,   261,     0,
     254,    69,     0,     0,     0,     0,    70,    11,    12,    13,
      14,    15,    16,    17,    63,    64,    13,    14,    65,    66,
      67,    63,    64,    13,    14,    65,   276,    67,    18,     0,
       0,     0,   104,    25,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,     0,     0,
      19,     0,     0,     0,     0,    20,     0,    68,     0,     0,
      21,     0,    69,     0,    68,     0,     0,    70,     0,    69,
      25,     0,     0,    40,    70,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,     0,
       0,     0,     0,     0,     0,     0,   104,    61,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,     0,     0,     0,     0,     0,     0,     0,   104,
     138,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,     0,     0,     0,     0,     0,
       0,     0,   104,   301,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,     0,     0,
       0,     0,     0,     0,     0,   104,   302,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,     0,     0,     0,     0,     0,     0,     0,   104,   303,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,     0,     0,     0,     0,     0,     0,
       0,   104,   304,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,     0,     0,     0,
       0,     0,   104,   292,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,     0,     0,
       0,     0,     0,   104,   293,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,     0,
       0,     0,     0,     0,   104,   294,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
       0,     0,     0,     0,   104,   229,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     158,     0,     0,     0,    40,   256,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
     104,     0,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,     0,     0,   104,   195,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,     0,     0,   104,   196,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,     0,     0,   104,   204,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
       0,     0,   104,   205,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,     0,     0,
     104,   247,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,     0,     0,   104,   252,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,     0,     0,   104,   270,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,     0,     0,     0,   291,    40,   122,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,   104,   174,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   104,   227,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   104,   251,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
      40,     0,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,   104,     0,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   111,   112,   113,   114,   115,   116,
     117,   118
};

static const yytype_int16 yycheck[] =
{
      68,    69,    70,    90,    20,    23,    28,    20,    27,    64,
      28,    29,    30,    31,    32,    33,    34,     0,    64,    10,
      64,    64,    77,    14,    17,    16,    77,    81,    25,    11,
      12,    77,   119,    77,    77,    34,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,    33,   120,    71,    80,    67,    82,    81,    76,    81,
      78,    77,    81,    81,    77,    58,    78,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    78,    66,    67,    68,    69,   173,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,   165,    34,   167,
      30,    31,     2,    33,    72,    73,   174,    92,    93,    94,
     178,    80,   180,    82,    81,    80,   184,    82,    34,    19,
      20,    21,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,   200,    70,    71,    72,    73,    34,   206,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    70,    71,    72,    73,   227,
      80,    80,    82,    72,    73,   233,    78,    78,   236,    33,
      78,   239,    78,    33,    81,    24,    81,   245,    79,    33,
      79,   249,    33,   251,    79,    79,   254,    59,    33,    22,
      80,    78,    35,   261,     2,    66,    67,    68,    69,    70,
      71,    72,    73,   271,    34,   273,   274,   275,    64,    34,
     278,    19,    20,    21,    17,    33,    78,    80,    33,    59,
      34,    29,   122,    33,   292,   293,   294,   295,    33,    79,
      64,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,     3,     4,     5,
       6,     7,     8,     9,    80,    58,    79,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    78,    28,    29,    30,    31,    32,    33,    34,    33,
      79,    33,    25,    91,    92,    93,    94,    79,    77,    77,
      81,   191,   100,    49,    81,    28,    29,    30,    31,    32,
      33,    34,    81,    28,    29,    30,    31,    32,    33,    34,
      70,    59,    63,    81,   122,    71,    33,    64,    27,   232,
      76,    54,    55,   278,    57,    81,   280,   279,   228,    54,
      55,    -1,    57,    -1,    -1,    -1,    -1,    -1,    71,    -1,
      -1,    -1,    -1,    76,    -1,    -1,    71,    -1,    81,    -1,
      26,    76,    -1,    -1,    -1,    -1,    81,    28,    29,    30,
      31,    32,    33,    34,    28,    29,    30,    31,    32,    33,
      34,    28,    29,    30,    31,    32,    33,    34,    49,    -1,
      -1,    -1,    58,   191,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    -1,    -1,
      71,    -1,    -1,    -1,    -1,    76,    -1,    71,    -1,    -1,
      81,    -1,    76,    -1,    71,    -1,    -1,    81,    -1,    76,
     228,    -1,    -1,    58,    81,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    58,    82,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,
      82,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    58,    82,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    58,    82,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,    82,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    58,    82,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    -1,    -1,    -1,
      -1,    -1,    58,    80,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    -1,    -1,
      -1,    -1,    -1,    58,    80,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    -1,
      -1,    -1,    -1,    -1,    58,    80,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      -1,    -1,    -1,    -1,    58,    79,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      54,    -1,    -1,    -1,    58,    79,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      58,    -1,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    -1,    -1,    58,    77,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    -1,    -1,    58,    77,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    -1,    -1,    58,    77,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      -1,    -1,    58,    77,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    -1,    -1,
      58,    77,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    -1,    -1,    58,    77,
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
      70,    71,    72,    73,    66,    67,    68,    69,    70,    71,
      72,    73
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    84,    85,     0,     3,     4,     5,     6,     7,     8,
       9,    28,    29,    30,    31,    32,    33,    34,    49,    71,
      76,    81,    86,    87,    89,   117,   119,   120,   121,    81,
      66,    67,    68,    69,   121,   121,   121,    88,    81,    77,
      58,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    33,   117,    34,    34,    34,
      34,    82,    90,    28,    29,    32,    33,    34,    71,    76,
      81,   116,   117,   118,   121,   121,   121,   121,   121,   121,
     121,   121,   121,   121,   121,   121,   121,   121,   121,   121,
      80,    78,    78,    78,    78,    10,    14,    16,    91,    92,
      81,   116,   116,   116,    58,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    81,
      80,    82,    59,   118,   121,   122,   122,   122,   122,    11,
      12,    33,    33,    33,    24,    93,   109,   117,    82,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   118,   116,   121,    82,    54,    79,
      79,    79,    79,    33,    33,    64,    77,    64,    77,    59,
      33,    22,   110,    80,    59,    82,    67,    78,    64,    77,
      64,    77,   116,   116,    78,    64,    94,    35,   111,   118,
     116,    34,    34,   116,   116,    77,    77,   116,    33,    98,
     112,    82,   121,    80,    77,    77,    17,    78,    33,    99,
     100,    23,    78,   113,   114,   116,    34,   116,    33,    95,
      96,    97,    59,    25,    78,   101,    33,    59,    79,    79,
      64,    79,    80,    78,    33,   102,    79,   116,   121,    20,
      77,    33,    25,    97,   116,    79,   116,    77,   116,    17,
     116,    59,    77,   116,    26,   116,    79,    28,    54,    55,
      57,    81,   103,   104,   105,   106,   107,   108,   115,   116,
      77,    20,    77,    81,    81,    81,    33,    77,    70,    63,
      59,   116,   116,   116,   116,    27,   105,    81,   108,    28,
     106,    77,    80,    80,    80,    64,    33,   116,   116,   116,
     116,    82,    82,    82,    82
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    83,    85,    84,    86,    86,    88,    87,    89,    89,
      89,    89,    89,    89,    89,    90,    90,    91,    91,    91,
      91,    91,    91,    91,    91,    91,    91,    92,    92,    94,
      93,    93,    95,    95,    96,    96,    97,    98,    98,    99,
      99,   100,   100,   101,   102,   102,   103,   103,   104,   104,
     105,   106,   106,   107,   107,   108,   109,   109,   110,   111,
     111,   112,   112,   112,   113,   114,   115,   115,   115,   115,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   117,   117,   117,   118,   118,   119,
     119,   120,   120,   121,   121,   121,   121,   121,   121,   121,
     121,   121,   121,   121,   121,   121,   121,   121,   121,   121,
     121,   121,   121,   121,   121,   121,   121,   121,   121,   121,
     121,   122,   122
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     0,     5,     1,     1,
       1,     1,     1,     1,     1,     0,     2,     3,     5,     4,
       6,     3,     5,     4,     6,     9,    11,     0,     2,     0,
       6,     8,     0,     1,     1,     3,     3,     0,     2,     8,
      10,     0,     2,     7,     0,     1,     1,     1,     1,     3,
       3,     1,     1,     1,     3,     6,     0,     2,     4,     0,
       1,     0,     2,     2,     4,     7,     1,     6,     6,     6,
       1,     1,     1,     1,     4,     6,     2,     2,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     5,     3,     1,     1,     1,     1,     3,     1,
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
#line 407 "parser.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).number); }
#line 1841 "parser.cc" /* yacc.c:1257  */
        break;

    case 116: /* expr  */
#line 406 "parser.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).expr); }
#line 1847 "parser.cc" /* yacc.c:1257  */
        break;

    case 118: /* arguments  */
#line 408 "parser.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).arguments); }
#line 1853 "parser.cc" /* yacc.c:1257  */
        break;

    case 121: /* property  */
#line 406 "parser.yy" /* yacc.c:1257  */
      { delete ((*yyvaluep).expr); }
#line 1859 "parser.cc" /* yacc.c:1257  */
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
#line 412 "parser.yy" /* yacc.c:1646  */
    { success = true; }
#line 2147 "parser.cc" /* yacc.c:1646  */
    break;

  case 3:
#line 413 "parser.yy" /* yacc.c:1646  */
    { if (!success) YYERROR; }
#line 2153 "parser.cc" /* yacc.c:1646  */
    break;

  case 6:
#line 424 "parser.yy" /* yacc.c:1646  */
    { prepare_model((yyvsp[0].model_type)); }
#line 2159 "parser.cc" /* yacc.c:1646  */
    break;

  case 7:
#line 425 "parser.yy" /* yacc.c:1646  */
    { compile_model(); }
#line 2165 "parser.cc" /* yacc.c:1646  */
    break;

  case 8:
#line 429 "parser.yy" /* yacc.c:1646  */
    { (yyval.model_type) = ModelType::MDP; }
#line 2171 "parser.cc" /* yacc.c:1646  */
    break;

  case 9:
#line 431 "parser.yy" /* yacc.c:1646  */
    { (yyval.model_type) = ModelType::MDP; }
#line 2177 "parser.cc" /* yacc.c:1646  */
    break;

  case 10:
#line 433 "parser.yy" /* yacc.c:1646  */
    { (yyval.model_type) = ModelType::DTMC; }
#line 2183 "parser.cc" /* yacc.c:1646  */
    break;

  case 11:
#line 435 "parser.yy" /* yacc.c:1646  */
    { (yyval.model_type) = ModelType::DTMC; }
#line 2189 "parser.cc" /* yacc.c:1646  */
    break;

  case 12:
#line 437 "parser.yy" /* yacc.c:1646  */
    { (yyval.model_type) = ModelType::CTMC; }
#line 2195 "parser.cc" /* yacc.c:1646  */
    break;

  case 13:
#line 439 "parser.yy" /* yacc.c:1646  */
    { (yyval.model_type) = ModelType::CTMC; }
#line 2201 "parser.cc" /* yacc.c:1646  */
    break;

  case 14:
#line 441 "parser.yy" /* yacc.c:1646  */
    { (yyval.model_type) = ModelType::GSMP; }
#line 2207 "parser.cc" /* yacc.c:1646  */
    break;

  case 17:
#line 452 "parser.yy" /* yacc.c:1646  */
    { declare_constant((yyvsp[-1].str), nullptr); }
#line 2213 "parser.cc" /* yacc.c:1646  */
    break;

  case 18:
#line 454 "parser.yy" /* yacc.c:1646  */
    { declare_constant((yyvsp[-3].str), (yyvsp[-1].expr)); }
#line 2219 "parser.cc" /* yacc.c:1646  */
    break;

  case 19:
#line 456 "parser.yy" /* yacc.c:1646  */
    { declare_constant((yyvsp[-1].str), nullptr); }
#line 2225 "parser.cc" /* yacc.c:1646  */
    break;

  case 20:
#line 458 "parser.yy" /* yacc.c:1646  */
    { declare_constant((yyvsp[-3].str), (yyvsp[-1].expr)); }
#line 2231 "parser.cc" /* yacc.c:1646  */
    break;

  case 21:
#line 460 "parser.yy" /* yacc.c:1646  */
    { declare_rate((yyvsp[-1].str), nullptr); }
#line 2237 "parser.cc" /* yacc.c:1646  */
    break;

  case 22:
#line 462 "parser.yy" /* yacc.c:1646  */
    { declare_rate((yyvsp[-3].str), (yyvsp[-1].expr)); }
#line 2243 "parser.cc" /* yacc.c:1646  */
    break;

  case 23:
#line 464 "parser.yy" /* yacc.c:1646  */
    { declare_rate((yyvsp[-1].str), nullptr); }
#line 2249 "parser.cc" /* yacc.c:1646  */
    break;

  case 24:
#line 466 "parser.yy" /* yacc.c:1646  */
    { declare_rate((yyvsp[-3].str), (yyvsp[-1].expr)); }
#line 2255 "parser.cc" /* yacc.c:1646  */
    break;

  case 25:
#line 468 "parser.yy" /* yacc.c:1646  */
    { declare_variable((yyvsp[-7].str), (yyvsp[-4].expr), (yyvsp[-2].expr), nullptr); }
#line 2261 "parser.cc" /* yacc.c:1646  */
    break;

  case 26:
#line 470 "parser.yy" /* yacc.c:1646  */
    { declare_variable((yyvsp[-9].str), (yyvsp[-6].expr), (yyvsp[-4].expr), (yyvsp[-1].expr)); }
#line 2267 "parser.cc" /* yacc.c:1646  */
    break;

  case 29:
#line 481 "parser.yy" /* yacc.c:1646  */
    { prepare_module((yyvsp[0].str)); }
#line 2273 "parser.cc" /* yacc.c:1646  */
    break;

  case 30:
#line 483 "parser.yy" /* yacc.c:1646  */
    { add_module(); }
#line 2279 "parser.cc" /* yacc.c:1646  */
    break;

  case 31:
#line 485 "parser.yy" /* yacc.c:1646  */
    { add_module((yyvsp[-6].str), (yyvsp[-4].str)); }
#line 2285 "parser.cc" /* yacc.c:1646  */
    break;

  case 36:
#line 496 "parser.yy" /* yacc.c:1646  */
    { add_substitution((yyvsp[-2].str), (yyvsp[0].str)); }
#line 2291 "parser.cc" /* yacc.c:1646  */
    break;

  case 39:
#line 504 "parser.yy" /* yacc.c:1646  */
    { declare_variable((yyvsp[-7].str), (yyvsp[-4].expr), (yyvsp[-2].expr), nullptr); }
#line 2297 "parser.cc" /* yacc.c:1646  */
    break;

  case 40:
#line 506 "parser.yy" /* yacc.c:1646  */
    { declare_variable((yyvsp[-9].str), (yyvsp[-6].expr), (yyvsp[-4].expr), (yyvsp[-1].expr)); }
#line 2303 "parser.cc" /* yacc.c:1646  */
    break;

  case 43:
#line 514 "parser.yy" /* yacc.c:1646  */
    { AddCommand((yyvsp[-5].str), (yyvsp[-3].expr), (yyvsp[-1].outcomes)); }
#line 2309 "parser.cc" /* yacc.c:1646  */
    break;

  case 44:
#line 518 "parser.yy" /* yacc.c:1646  */
    { (yyval.str) = new std::string(); }
#line 2315 "parser.cc" /* yacc.c:1646  */
    break;

  case 46:
#line 523 "parser.yy" /* yacc.c:1646  */
    { (yyval.outcomes) = AddOutcome(NewOutcome(nullptr, (yyvsp[0].updates)), nullptr); }
#line 2321 "parser.cc" /* yacc.c:1646  */
    break;

  case 48:
#line 528 "parser.yy" /* yacc.c:1646  */
    { (yyval.outcomes) = AddOutcome((yyvsp[0].outcome), nullptr); }
#line 2327 "parser.cc" /* yacc.c:1646  */
    break;

  case 49:
#line 530 "parser.yy" /* yacc.c:1646  */
    { (yyval.outcomes) = AddOutcome((yyvsp[0].outcome), (yyvsp[-2].outcomes)); }
#line 2333 "parser.cc" /* yacc.c:1646  */
    break;

  case 50:
#line 534 "parser.yy" /* yacc.c:1646  */
    { (yyval.outcome) = NewOutcome((yyvsp[-2].dist), (yyvsp[0].updates)); }
#line 2339 "parser.cc" /* yacc.c:1646  */
    break;

  case 51:
#line 538 "parser.yy" /* yacc.c:1646  */
    { (yyval.updates) = new std::vector<Update>(); }
#line 2345 "parser.cc" /* yacc.c:1646  */
    break;

  case 53:
#line 543 "parser.yy" /* yacc.c:1646  */
    { (yyval.updates) = AddUpdate((yyvsp[0].update), nullptr); }
#line 2351 "parser.cc" /* yacc.c:1646  */
    break;

  case 54:
#line 545 "parser.yy" /* yacc.c:1646  */
    { (yyval.updates) = AddUpdate((yyvsp[0].update), (yyvsp[-2].updates)); }
#line 2357 "parser.cc" /* yacc.c:1646  */
    break;

  case 55:
#line 549 "parser.yy" /* yacc.c:1646  */
    { (yyval.update) = NewUpdate((yyvsp[-4].str), (yyvsp[-1].expr)); }
#line 2363 "parser.cc" /* yacc.c:1646  */
    break;

  case 64:
#line 573 "parser.yy" /* yacc.c:1646  */
    { delete (yyvsp[-3].expr); delete (yyvsp[-1].expr); }
#line 2369 "parser.cc" /* yacc.c:1646  */
    break;

  case 65:
#line 577 "parser.yy" /* yacc.c:1646  */
    { delete (yyvsp[-5].str); delete (yyvsp[-3].expr); delete (yyvsp[-1].expr); }
#line 2375 "parser.cc" /* yacc.c:1646  */
    break;

  case 66:
#line 585 "parser.yy" /* yacc.c:1646  */
    { (yyval.dist) = NewMemoryless((yyvsp[0].expr)); }
#line 2381 "parser.cc" /* yacc.c:1646  */
    break;

  case 67:
#line 587 "parser.yy" /* yacc.c:1646  */
    { (yyval.dist) = NewWeibull((yyvsp[-3].expr), (yyvsp[-1].expr)); }
#line 2387 "parser.cc" /* yacc.c:1646  */
    break;

  case 68:
#line 589 "parser.yy" /* yacc.c:1646  */
    { (yyval.dist) = NewLognormal((yyvsp[-3].expr), (yyvsp[-1].expr)); }
#line 2393 "parser.cc" /* yacc.c:1646  */
    break;

  case 69:
#line 591 "parser.yy" /* yacc.c:1646  */
    { (yyval.dist) = NewUniform((yyvsp[-3].expr), (yyvsp[-1].expr)); }
#line 2399 "parser.cc" /* yacc.c:1646  */
    break;

  case 70:
#line 598 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = new Literal(*(yyvsp[0].number)); delete (yyvsp[0].number); }
#line 2405 "parser.cc" /* yacc.c:1646  */
    break;

  case 71:
#line 600 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = new Literal(true); }
#line 2411 "parser.cc" /* yacc.c:1646  */
    break;

  case 72:
#line 602 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = new Literal(true); }
#line 2417 "parser.cc" /* yacc.c:1646  */
    break;

  case 73:
#line 604 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = find_variable((yyvsp[0].str)); }
#line 2423 "parser.cc" /* yacc.c:1646  */
    break;

  case 74:
#line 606 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = new FunctionCall((yyvsp[-3].function), std::move(*(yyvsp[-1].arguments))); delete (yyvsp[-1].arguments); }
#line 2429 "parser.cc" /* yacc.c:1646  */
    break;

  case 75:
#line 608 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = new FunctionCall((yyvsp[-3].function), std::move(*(yyvsp[-1].arguments))); delete (yyvsp[-1].arguments); }
#line 2435 "parser.cc" /* yacc.c:1646  */
    break;

  case 76:
#line 610 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNegate((yyvsp[0].expr)); }
#line 2441 "parser.cc" /* yacc.c:1646  */
    break;

  case 77:
#line 612 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNot((yyvsp[0].expr)); }
#line 2447 "parser.cc" /* yacc.c:1646  */
    break;

  case 78:
#line 614 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewPlus((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2453 "parser.cc" /* yacc.c:1646  */
    break;

  case 79:
#line 616 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewMinus((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2459 "parser.cc" /* yacc.c:1646  */
    break;

  case 80:
#line 618 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewMultiply((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2465 "parser.cc" /* yacc.c:1646  */
    break;

  case 81:
#line 620 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewDivide((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2471 "parser.cc" /* yacc.c:1646  */
    break;

  case 82:
#line 622 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewAnd((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2477 "parser.cc" /* yacc.c:1646  */
    break;

  case 83:
#line 624 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewOr((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2483 "parser.cc" /* yacc.c:1646  */
    break;

  case 84:
#line 626 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewImply((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2489 "parser.cc" /* yacc.c:1646  */
    break;

  case 85:
#line 628 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewIff((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2495 "parser.cc" /* yacc.c:1646  */
    break;

  case 86:
#line 630 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLess((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2501 "parser.cc" /* yacc.c:1646  */
    break;

  case 87:
#line 632 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLessEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2507 "parser.cc" /* yacc.c:1646  */
    break;

  case 88:
#line 634 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewGreaterEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2513 "parser.cc" /* yacc.c:1646  */
    break;

  case 89:
#line 636 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewGreater((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2519 "parser.cc" /* yacc.c:1646  */
    break;

  case 90:
#line 638 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2525 "parser.cc" /* yacc.c:1646  */
    break;

  case 91:
#line 640 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNotEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2531 "parser.cc" /* yacc.c:1646  */
    break;

  case 92:
#line 642 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewConditional((yyvsp[-4].expr), (yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2537 "parser.cc" /* yacc.c:1646  */
    break;

  case 93:
#line 644 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[-1].expr); }
#line 2543 "parser.cc" /* yacc.c:1646  */
    break;

  case 94:
#line 648 "parser.yy" /* yacc.c:1646  */
    { (yyval.function) = MakeFunction(*(yyvsp[0].str)); delete (yyvsp[0].str); }
#line 2549 "parser.cc" /* yacc.c:1646  */
    break;

  case 95:
#line 650 "parser.yy" /* yacc.c:1646  */
    { (yyval.function) = Function::MIN; }
#line 2555 "parser.cc" /* yacc.c:1646  */
    break;

  case 96:
#line 652 "parser.yy" /* yacc.c:1646  */
    { (yyval.function) = Function::MAX; }
#line 2561 "parser.cc" /* yacc.c:1646  */
    break;

  case 97:
#line 656 "parser.yy" /* yacc.c:1646  */
    { (yyval.arguments) = new UniquePtrVector<const Expression>(WrapUnique((yyvsp[0].expr))); }
#line 2567 "parser.cc" /* yacc.c:1646  */
    break;

  case 98:
#line 658 "parser.yy" /* yacc.c:1646  */
    { (yyval.arguments) = (yyvsp[-2].arguments); (yyval.arguments)->push_back(WrapUnique((yyvsp[0].expr))); }
#line 2573 "parser.cc" /* yacc.c:1646  */
    break;

  case 101:
#line 670 "parser.yy" /* yacc.c:1646  */
    { properties.push_back(WrapUnique((yyvsp[0].expr))); }
#line 2579 "parser.cc" /* yacc.c:1646  */
    break;

  case 102:
#line 672 "parser.yy" /* yacc.c:1646  */
    { properties.push_back(WrapUnique((yyvsp[0].expr))); }
#line 2585 "parser.cc" /* yacc.c:1646  */
    break;

  case 103:
#line 676 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = new Literal(*(yyvsp[0].number)); delete (yyvsp[0].number); }
#line 2591 "parser.cc" /* yacc.c:1646  */
    break;

  case 104:
#line 678 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = new Literal(true); }
#line 2597 "parser.cc" /* yacc.c:1646  */
    break;

  case 105:
#line 680 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = new Literal(false); }
#line 2603 "parser.cc" /* yacc.c:1646  */
    break;

  case 106:
#line 682 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = find_variable((yyvsp[0].str)); }
#line 2609 "parser.cc" /* yacc.c:1646  */
    break;

  case 107:
#line 684 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = new FunctionCall((yyvsp[-3].function), std::move(*(yyvsp[-1].arguments))); delete (yyvsp[-1].arguments); }
#line 2615 "parser.cc" /* yacc.c:1646  */
    break;

  case 108:
#line 686 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = new FunctionCall((yyvsp[-3].function), std::move(*(yyvsp[-1].arguments))); delete (yyvsp[-1].arguments); }
#line 2621 "parser.cc" /* yacc.c:1646  */
    break;

  case 109:
#line 688 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewProbabilityLess(Double((yyvsp[-3].number)), (yyvsp[-1].path)); }
#line 2627 "parser.cc" /* yacc.c:1646  */
    break;

  case 110:
#line 690 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewProbabilityLessEqual(Double((yyvsp[-3].number)), (yyvsp[-1].path)); }
#line 2633 "parser.cc" /* yacc.c:1646  */
    break;

  case 111:
#line 692 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewProbabilityGreaterEqual(Double((yyvsp[-3].number)), (yyvsp[-1].path)); }
#line 2639 "parser.cc" /* yacc.c:1646  */
    break;

  case 112:
#line 694 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewProbabilityGreater(Double((yyvsp[-3].number)), (yyvsp[-1].path)); }
#line 2645 "parser.cc" /* yacc.c:1646  */
    break;

  case 113:
#line 696 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNegate((yyvsp[0].expr)); }
#line 2651 "parser.cc" /* yacc.c:1646  */
    break;

  case 114:
#line 698 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNot((yyvsp[0].expr)); }
#line 2657 "parser.cc" /* yacc.c:1646  */
    break;

  case 115:
#line 700 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewPlus((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2663 "parser.cc" /* yacc.c:1646  */
    break;

  case 116:
#line 702 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewMinus((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2669 "parser.cc" /* yacc.c:1646  */
    break;

  case 117:
#line 704 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewMultiply((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2675 "parser.cc" /* yacc.c:1646  */
    break;

  case 118:
#line 706 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewDivide((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2681 "parser.cc" /* yacc.c:1646  */
    break;

  case 119:
#line 708 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewAnd((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2687 "parser.cc" /* yacc.c:1646  */
    break;

  case 120:
#line 710 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewOr((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2693 "parser.cc" /* yacc.c:1646  */
    break;

  case 121:
#line 712 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewImply((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2699 "parser.cc" /* yacc.c:1646  */
    break;

  case 122:
#line 714 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewIff((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2705 "parser.cc" /* yacc.c:1646  */
    break;

  case 123:
#line 716 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLess((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2711 "parser.cc" /* yacc.c:1646  */
    break;

  case 124:
#line 718 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewLessEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2717 "parser.cc" /* yacc.c:1646  */
    break;

  case 125:
#line 720 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewGreaterEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2723 "parser.cc" /* yacc.c:1646  */
    break;

  case 126:
#line 722 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewGreater((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2729 "parser.cc" /* yacc.c:1646  */
    break;

  case 127:
#line 724 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2735 "parser.cc" /* yacc.c:1646  */
    break;

  case 128:
#line 726 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewNotEqual((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2741 "parser.cc" /* yacc.c:1646  */
    break;

  case 129:
#line 728 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = NewConditional((yyvsp[-4].expr), (yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2747 "parser.cc" /* yacc.c:1646  */
    break;

  case 130:
#line 730 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[-1].expr); }
#line 2753 "parser.cc" /* yacc.c:1646  */
    break;

  case 131:
#line 734 "parser.yy" /* yacc.c:1646  */
    { (yyval.path) = NewUntil(0, Double((yyvsp[-1].number)), (yyvsp[-4].expr), (yyvsp[0].expr)); }
#line 2759 "parser.cc" /* yacc.c:1646  */
    break;

  case 132:
#line 736 "parser.yy" /* yacc.c:1646  */
    { (yyval.path) = NewUntil(Double((yyvsp[-4].number)), Double((yyvsp[-2].number)), (yyvsp[-7].expr), (yyvsp[0].expr)); }
#line 2765 "parser.cc" /* yacc.c:1646  */
    break;


#line 2769 "parser.cc" /* yacc.c:1646  */
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
#line 739 "parser.yy" /* yacc.c:1906  */


static const Identifier* find_variable(const std::string* ident) {
  const Identifier* identifier = new Identifier(*ident);
  delete ident;
  return identifier;
}

static void add_substitution(const std::string* ident1,
			     const std::string* ident2) {
  subst.insert(std::make_pair(*ident1, *ident2));
  delete ident1;
  delete ident2;
}

static void declare_constant(const std::string* ident,
                             const Expression* value_expr) {
  AddConstant(ident, Type::INT, value_expr);
}

static void declare_rate(const std::string* ident,
                         const Expression* value_expr) {
  AddConstant(ident, Type::DOUBLE, value_expr);
}

static void declare_variable(const std::string* ident, const Expression* low,
                             const Expression* high, const Expression* start) {
  std::vector<std::string> errors;
  if (!model->AddIntVariable(*ident, WrapUnique(low), WrapUnique(high),
                             WrapUnique(start), &errors)) {
    for (const auto& error : errors) {
      yyerror(error);
    }
  }
  delete ident;
}

static void add_module(const std::string* ident1, const std::string* ident2) {
  if (!model->StartModule(*ident1)) {
    yyerror(StrCat("duplicate module ", *ident1));
  } else {
    std::vector<std::string> errors;
    if (!model->AddFromModule(*ident2, subst, &errors)) {
      for (const auto& error : errors) {
        yyerror(error);
      }
    }
    model->EndModule();
  }
  subst.clear();
  delete ident1;
  delete ident2;
}


/* Adds a module to the current model. */
static void add_module() {
  model->EndModule();
}


/* Prepares a module for parsing. */
static void prepare_module(const std::string* ident) {
  if (!model->StartModule(*ident)) {
    yyerror(StrCat("duplicate module ", *ident));
  }
  delete ident;
}


/* Prepares a model for parsing. */
static void prepare_model(ModelType model_type) {
  properties.clear();
  if (model != nullptr) {
    delete model;
  }
  model = new Model();
  model->SetType(model_type);
}


/* Compiles the current model. */
static void compile_model() {
  model->compile();
  global_model = model;
}

/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
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
#define YYBISON_VERSION "2.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 1



/* Copy the first part of user declarations.  */

/* Line 268 of yacc.c  */
#line 24 "parser.yy"

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

#include "parser.h"

// Lexical analyzer function.
extern int yylex(YYSTYPE* lvalp, YYLTYPE* llocp, void* scanner);
/* Name of current file. */
extern std::string current_file;
/* Constant overrides. */
extern std::map<std::string, TypedValue> const_overrides;

/* Last model parsed. */
const Model* global_model = nullptr;
/* Parsed properties. */
std::vector<const StateFormula*> properties;

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
/* Returns a conjunction. */
static Conjunction* make_conjunction(StateFormula* f1,
				     const StateFormula* f2);
/* Returns a disjunction. */
static Disjunction* make_disjunction(StateFormula* f1,
				     const StateFormula* f2);
/* Returns a probabilistic path quantification. */
static StateFormula* make_probabilistic(const TypedValue* p,
					bool strict, bool negate,
					const PathFormula* f);
/* Returns an until formula. */
static const Until* make_until(const StateFormula* f1, const StateFormula* f2,
			       const TypedValue* t1, const TypedValue* t2);
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
static void prepare_model();
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
std::unique_ptr<T> MakeUnique(T* ptr) {
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

const UnaryOperation* NewNegate(std::unique_ptr<const Expression>&& operand) {
  return new UnaryOperation(UnaryOperator::NEGATE, std::move(operand));
}

const UnaryOperation* NewNot(std::unique_ptr<const Expression>&& operand) {
  return new UnaryOperation(UnaryOperator::NOT, std::move(operand));
}

const BinaryOperation* NewPlus(std::unique_ptr<const Expression>&& operand1,
                               std::unique_ptr<const Expression>&& operand2) {
  return new BinaryOperation(BinaryOperator::PLUS,
                             std::move(operand1), std::move(operand2));
}

const BinaryOperation* NewMinus(std::unique_ptr<const Expression>&& operand1,
                                std::unique_ptr<const Expression>&& operand2) {
  return new BinaryOperation(BinaryOperator::MINUS,
                             std::move(operand1), std::move(operand2));
}

const BinaryOperation* NewMultiply(
    std::unique_ptr<const Expression>&& operand1,
    std::unique_ptr<const Expression>&& operand2) {
  return new BinaryOperation(BinaryOperator::MULTIPLY,
                             std::move(operand1), std::move(operand2));
}

const BinaryOperation* NewDivide(std::unique_ptr<const Expression>&& operand1,
                                 std::unique_ptr<const Expression>&& operand2) {
  return new BinaryOperation(BinaryOperator::DIVIDE,
                             std::move(operand1), std::move(operand2));
}

const BinaryOperation* NewAnd(std::unique_ptr<const Expression>&& operand1,
                              std::unique_ptr<const Expression>&& operand2) {
  return new BinaryOperation(BinaryOperator::AND,
                             std::move(operand1), std::move(operand2));
}

const BinaryOperation* NewOr(std::unique_ptr<const Expression>&& operand1,
                             std::unique_ptr<const Expression>&& operand2) {
  return new BinaryOperation(BinaryOperator::OR,
                             std::move(operand1), std::move(operand2));
}

const BinaryOperation* NewImply(std::unique_ptr<const Expression>&& operand1,
                                std::unique_ptr<const Expression>&& operand2) {
  return new BinaryOperation(BinaryOperator::IMPLY,
                             std::move(operand1), std::move(operand2));
}

const BinaryOperation* NewIff(std::unique_ptr<const Expression>&& operand1,
                              std::unique_ptr<const Expression>&& operand2) {
  return new BinaryOperation(BinaryOperator::IFF,
                             std::move(operand1), std::move(operand2));
}

const BinaryOperation* NewLess(std::unique_ptr<const Expression>&& operand1,
                               std::unique_ptr<const Expression>&& operand2) {
  return new BinaryOperation(BinaryOperator::LESS,
                             std::move(operand1), std::move(operand2));
}

const BinaryOperation* NewLessEqual(
    std::unique_ptr<const Expression>&& operand1,
    std::unique_ptr<const Expression>&& operand2) {
  return new BinaryOperation(BinaryOperator::LESS_EQUAL,
                             std::move(operand1), std::move(operand2));
}

const BinaryOperation* NewGreaterEqual(
    std::unique_ptr<const Expression>&& operand1,
    std::unique_ptr<const Expression>&& operand2) {
  return new BinaryOperation(BinaryOperator::GREATER_EQUAL,
                             std::move(operand1), std::move(operand2));
}

const BinaryOperation* NewGreater(
    std::unique_ptr<const Expression>&& operand1,
    std::unique_ptr<const Expression>&& operand2) {
  return new BinaryOperation(BinaryOperator::GREATER,
                             std::move(operand1), std::move(operand2));
}

const BinaryOperation* NewEqual(std::unique_ptr<const Expression>&& operand1,
                                std::unique_ptr<const Expression>&& operand2) {
  return new BinaryOperation(BinaryOperator::EQUAL,
                             std::move(operand1), std::move(operand2));
}

const BinaryOperation* NewNotEqual(
    std::unique_ptr<const Expression>&& operand1,
    std::unique_ptr<const Expression>&& operand2) {
  return new BinaryOperation(BinaryOperator::NOT_EQUAL,
                             std::move(operand1), std::move(operand2));
}

LessThan* NewLessThan(std::unique_ptr<const Expression>&& expr1,
                      std::unique_ptr<const Expression>&& expr2) {
  return new LessThan(std::move(expr1), std::move(expr2));
}

LessThanOrEqual* NewLessThanOrEqual(std::unique_ptr<const Expression>&& expr1,
                                    std::unique_ptr<const Expression>&& expr2) {
  return new LessThanOrEqual(std::move(expr1), std::move(expr2));
}

GreaterThanOrEqual* NewGreaterThanOrEqual(
    std::unique_ptr<const Expression>&& expr1,
    std::unique_ptr<const Expression>&& expr2) {
  return new GreaterThanOrEqual(std::move(expr1), std::move(expr2));
}

GreaterThan* NewGreaterThan(std::unique_ptr<const Expression>&& expr1,
                            std::unique_ptr<const Expression>&& expr2) {
  return new GreaterThan(std::move(expr1), std::move(expr2));
}

Equality* NewEquality(std::unique_ptr<const Expression>&& expr1,
                      std::unique_ptr<const Expression>&& expr2) {
  return new Equality(std::move(expr1), std::move(expr2));
}

Inequality* NewInequality(std::unique_ptr<const Expression>&& expr1,
                          std::unique_ptr<const Expression>&& expr2) {
  return new Inequality(std::move(expr1), std::move(expr2));
}

const Exponential* NewExponential(std::unique_ptr<const Expression>&& rate) {
  return Exponential::make(std::move(rate));
}

const Distribution* NewWeibull(std::unique_ptr<const Expression>&& scale,
                               std::unique_ptr<const Expression>&& shape) {
  return Weibull::make(std::move(scale), std::move(shape));
}

const Lognormal* NewLognormal(std::unique_ptr<const Expression>&& scale,
                              std::unique_ptr<const Expression>&& shape) {
  return Lognormal::make(std::move(scale), std::move(shape));
}

const Uniform* NewUniform(std::unique_ptr<const Expression>&& low,
                          std::unique_ptr<const Expression>&& high) {
  return Uniform::make(std::move(low), std::move(high));
}

}  // namespace


/* Line 268 of yacc.c  */
#line 404 "parser.cc"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     DTMC_TOKEN = 258,
     CTMC_TOKEN = 259,
     MDP_TOKEN = 260,
     PROBABILISTIC = 261,
     STOCHASTIC = 262,
     NONDETERMINISTIC = 263,
     CONST = 264,
     INT_TOKEN = 265,
     DOUBLE_TOKEN = 266,
     BOOL_TOKEN = 267,
     RATE = 268,
     PROB = 269,
     GLOBAL = 270,
     DOTDOT = 271,
     FORMULA = 272,
     LABEL = 273,
     INIT = 274,
     ENDINIT = 275,
     REWARDS = 276,
     ENDREWARDS = 277,
     MODULE = 278,
     ENDMODULE = 279,
     ARROW = 280,
     PRIME = 281,
     TRUE = 282,
     FALSE = 283,
     MAX_TOKEN = 284,
     MIN_TOKEN = 285,
     FUNC = 286,
     IDENTIFIER = 287,
     NUMBER = 288,
     LABEL_NAME = 289,
     SYSTEM = 290,
     ENDSYSTEM = 291,
     DOUBLE_BAR = 292,
     TRIPLE_BAR = 293,
     BACK_ARROW = 294,
     A = 295,
     C = 296,
     E = 297,
     F = 298,
     G = 299,
     I = 300,
     PMAX = 301,
     PMIN = 302,
     P = 303,
     RMAX = 304,
     RMIN = 305,
     R = 306,
     S = 307,
     U = 308,
     W = 309,
     X = 310,
     EXP = 311,
     L = 312,
     IFF_TOKEN = 313,
     IMPLY_TOKEN = 314,
     NEQ = 315,
     GEQ = 316,
     LEQ = 317,
     UMINUS = 318
   };
#endif
/* Tokens.  */
#define DTMC_TOKEN 258
#define CTMC_TOKEN 259
#define MDP_TOKEN 260
#define PROBABILISTIC 261
#define STOCHASTIC 262
#define NONDETERMINISTIC 263
#define CONST 264
#define INT_TOKEN 265
#define DOUBLE_TOKEN 266
#define BOOL_TOKEN 267
#define RATE 268
#define PROB 269
#define GLOBAL 270
#define DOTDOT 271
#define FORMULA 272
#define LABEL 273
#define INIT 274
#define ENDINIT 275
#define REWARDS 276
#define ENDREWARDS 277
#define MODULE 278
#define ENDMODULE 279
#define ARROW 280
#define PRIME 281
#define TRUE 282
#define FALSE 283
#define MAX_TOKEN 284
#define MIN_TOKEN 285
#define FUNC 286
#define IDENTIFIER 287
#define NUMBER 288
#define LABEL_NAME 289
#define SYSTEM 290
#define ENDSYSTEM 291
#define DOUBLE_BAR 292
#define TRIPLE_BAR 293
#define BACK_ARROW 294
#define A 295
#define C 296
#define E 297
#define F 298
#define G 299
#define I 300
#define PMAX 301
#define PMIN 302
#define P 303
#define RMAX 304
#define RMIN 305
#define R 306
#define S 307
#define U 308
#define W 309
#define X 310
#define EXP 311
#define L 312
#define IFF_TOKEN 313
#define IMPLY_TOKEN 314
#define NEQ 315
#define GEQ 316
#define LEQ 317
#define UMINUS 318




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 293 of yacc.c  */
#line 394 "parser.yy"

  size_t synch;
  StateFormula* formula;
  const PathFormula* path;
  const Distribution* dist;
  const Expression* expr;
  int nat;
  const std::string* str;
  const TypedValue* number;
  Function function;
  UniquePtrVector<const Expression>* arguments;



/* Line 293 of yacc.c  */
#line 581 "parser.cc"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


/* Copy the second part of user declarations.  */


/* Line 343 of yacc.c  */
#line 606 "parser.cc"

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
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
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
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
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
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
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
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
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
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   515

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  83
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  40
/* YYNRULES -- Number of rules.  */
#define YYNRULES  134
/* YYNRULES -- Number of states.  */
#define YYNSTATES  311

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   318

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    75,     2,     2,     2,     2,    63,     2,
      81,    82,    72,    70,    80,    71,     2,    73,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    59,    77,
      66,    64,    67,    58,     2,     2,     2,     2,     2,     2,
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
      55,    56,    57,    60,    61,    65,    68,    69,    76
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,     9,    11,    12,    18,    20,
      22,    23,    26,    30,    36,    41,    48,    52,    58,    63,
      70,    80,    92,    93,    96,    97,   104,   113,   114,   116,
     118,   122,   126,   127,   130,   139,   150,   151,   154,   155,
     164,   167,   171,   176,   180,   184,   185,   188,   193,   194,
     196,   197,   200,   203,   208,   216,   218,   223,   230,   237,
     244,   246,   248,   250,   252,   257,   264,   267,   270,   274,
     278,   282,   286,   290,   294,   298,   302,   306,   310,   314,
     318,   322,   326,   330,   332,   334,   337,   341,   345,   349,
     353,   357,   359,   361,   365,   369,   373,   375,   377,   381,
     383,   385,   388,   392,   396,   400,   404,   406,   407,   410,
     412,   414,   421,   428,   435,   442,   446,   450,   454,   457,
     461,   465,   469,   473,   477,   481,   485,   491,   500,   502,
     504,   507,   511,   515,   519
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      84,     0,    -1,    -1,    85,    86,    -1,    87,    -1,   119,
      -1,    -1,    89,    88,    90,    92,   105,    -1,     7,    -1,
       4,    -1,    -1,    90,    91,    -1,     9,    32,    77,    -1,
       9,    32,    64,   117,    77,    -1,     9,    10,    32,    77,
      -1,     9,    10,    32,    64,   117,    77,    -1,    13,    32,
      77,    -1,    13,    32,    64,   114,    77,    -1,     9,    11,
      32,    77,    -1,     9,    11,    32,    64,   114,    77,    -1,
      15,    32,    59,    78,   117,    16,   117,    79,    77,    -1,
      15,    32,    59,    78,   117,    16,   117,    79,    19,   117,
      77,    -1,    -1,    92,    93,    -1,    -1,    23,    32,    94,
      98,   100,    24,    -1,    23,    32,    64,    32,    78,    95,
      79,    24,    -1,    -1,    96,    -1,    97,    -1,    96,    80,
      97,    -1,    32,    64,    32,    -1,    -1,    98,    99,    -1,
      32,    59,    78,   117,    16,   117,    79,    77,    -1,    32,
      59,    78,   117,    16,   117,    79,    19,   117,    77,    -1,
      -1,   100,   101,    -1,    -1,   103,   112,    25,   111,    59,
     102,   104,    77,    -1,    78,    79,    -1,    78,    32,    79,
      -1,    32,    26,    64,   112,    -1,   104,    63,   104,    -1,
      81,   104,    82,    -1,    -1,   105,   106,    -1,    21,   107,
     108,    22,    -1,    -1,    34,    -1,    -1,   108,   109,    -1,
     108,   110,    -1,   112,    59,   113,    77,    -1,    78,    32,
      79,   112,    59,   113,    77,    -1,   113,    -1,    56,    81,
     113,    82,    -1,    54,    81,   114,    80,   114,    82,    -1,
      57,    81,   114,    80,   114,    82,    -1,    53,    81,   114,
      80,   114,    82,    -1,    33,    -1,    27,    -1,    28,    -1,
      32,    -1,   115,    81,   116,    82,    -1,    31,    81,   115,
      80,   116,    82,    -1,    71,   112,    -1,    75,   112,    -1,
     112,    70,   112,    -1,   112,    71,   112,    -1,   112,    72,
     112,    -1,   112,    73,   112,    -1,   112,    63,   112,    -1,
     112,    62,   112,    -1,   112,    61,   112,    -1,   112,    60,
     112,    -1,   112,    66,   112,    -1,   112,    69,   112,    -1,
     112,    68,   112,    -1,   112,    67,   112,    -1,   112,    64,
     112,    -1,   112,    65,   112,    -1,    81,   112,    82,    -1,
      33,    -1,    32,    -1,    71,   113,    -1,   113,    70,   113,
      -1,   113,    71,   113,    -1,   113,    72,   113,    -1,   113,
      73,   113,    -1,    81,   113,    82,    -1,    33,    -1,    32,
      -1,   114,    72,   114,    -1,   114,    73,   114,    -1,    81,
     114,    82,    -1,    32,    -1,   112,    -1,   116,    80,   112,
      -1,   118,    -1,    32,    -1,    71,   117,    -1,   117,    70,
     117,    -1,   117,    71,   117,    -1,   117,    72,   117,    -1,
      81,   117,    82,    -1,    33,    -1,    -1,   119,   120,    -1,
      27,    -1,    28,    -1,    48,    66,    33,    78,   121,    79,
      -1,    48,    69,    33,    78,   121,    79,    -1,    48,    68,
      33,    78,   121,    79,    -1,    48,    67,    33,    78,   121,
      79,    -1,   120,    61,   120,    -1,   120,    63,   120,    -1,
     120,    62,   120,    -1,    75,   120,    -1,   122,    66,   122,
      -1,   122,    69,   122,    -1,   122,    68,   122,    -1,   122,
      67,   122,    -1,   122,    64,   122,    -1,   122,    65,   122,
      -1,    81,   120,    82,    -1,   120,    53,    69,    33,   120,
      -1,   120,    53,    78,    33,    80,    33,    79,   120,    -1,
     118,    -1,    32,    -1,    71,   122,    -1,   122,    70,   122,
      -1,   122,    71,   122,    -1,   122,    72,   122,    -1,    81,
     122,    82,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   424,   424,   424,   428,   429,   436,   436,   440,   440,
     446,   447,   450,   452,   454,   456,   458,   460,   462,   464,
     466,   468,   477,   478,   481,   481,   484,   488,   489,   492,
     493,   496,   499,   500,   503,   505,   510,   511,   515,   514,
     519,   520,   523,   524,   525,   532,   533,   536,   539,   540,
     543,   544,   545,   548,   552,   560,   562,   564,   566,   568,
     575,   577,   579,   581,   583,   585,   587,   589,   591,   593,
     595,   597,   599,   601,   603,   605,   607,   609,   611,   613,
     615,   617,   619,   623,   625,   627,   629,   631,   633,   635,
     637,   641,   643,   645,   647,   649,   653,   657,   659,   667,
     669,   671,   673,   675,   677,   679,   683,   690,   691,   695,
     696,   697,   699,   701,   703,   705,   707,   708,   709,   710,
     712,   714,   716,   718,   720,   722,   726,   728,   733,   735,
     737,   739,   741,   743,   745
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "DTMC_TOKEN", "CTMC_TOKEN", "MDP_TOKEN",
  "PROBABILISTIC", "STOCHASTIC", "NONDETERMINISTIC", "CONST", "INT_TOKEN",
  "DOUBLE_TOKEN", "BOOL_TOKEN", "RATE", "PROB", "GLOBAL", "DOTDOT",
  "FORMULA", "LABEL", "INIT", "ENDINIT", "REWARDS", "ENDREWARDS", "MODULE",
  "ENDMODULE", "ARROW", "PRIME", "TRUE", "FALSE", "MAX_TOKEN", "MIN_TOKEN",
  "FUNC", "IDENTIFIER", "NUMBER", "LABEL_NAME", "SYSTEM", "ENDSYSTEM",
  "DOUBLE_BAR", "TRIPLE_BAR", "BACK_ARROW", "A", "C", "E", "F", "G", "I",
  "PMAX", "PMIN", "P", "RMAX", "RMIN", "R", "S", "U", "W", "X", "EXP", "L",
  "'?'", "':'", "IFF_TOKEN", "IMPLY_TOKEN", "'|'", "'&'", "'='", "NEQ",
  "'<'", "'>'", "GEQ", "LEQ", "'+'", "'-'", "'*'", "'/'", "'{'", "'!'",
  "UMINUS", "';'", "'['", "']'", "','", "'('", "')'", "$accept", "file",
  "$@1", "model_or_properties", "model", "$@2", "model_type",
  "declarations", "declaration", "modules", "module_decl", "$@3",
  "substitutions", "subst_list", "subst", "variables", "variable_decl",
  "commands", "command", "$@4", "synchronization", "update", "rewards",
  "rewards_decl", "rewards_label", "reward_rules", "state_reward",
  "transition_reward", "distribution", "expr", "rate_expr",
  "const_rate_expr", "function", "arguments", "const_expr", "integer",
  "properties", "csl_formula", "path_formula", "csl_expr", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,    63,    58,
     313,   314,   124,    38,    61,   315,    60,    62,   316,   317,
      43,    45,    42,    47,   123,    33,   318,    59,    91,    93,
      44,    40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    83,    85,    84,    86,    86,    88,    87,    89,    89,
      90,    90,    91,    91,    91,    91,    91,    91,    91,    91,
      91,    91,    92,    92,    94,    93,    93,    95,    95,    96,
      96,    97,    98,    98,    99,    99,   100,   100,   102,   101,
     103,   103,   104,   104,   104,   105,   105,   106,   107,   107,
     108,   108,   108,   109,   110,   111,   111,   111,   111,   111,
     112,   112,   112,   112,   112,   112,   112,   112,   112,   112,
     112,   112,   112,   112,   112,   112,   112,   112,   112,   112,
     112,   112,   112,   113,   113,   113,   113,   113,   113,   113,
     113,   114,   114,   114,   114,   114,   115,   116,   116,   117,
     117,   117,   117,   117,   117,   117,   118,   119,   119,   120,
     120,   120,   120,   120,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   120,   120,   120,   121,   121,   122,   122,
     122,   122,   122,   122,   122
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     0,     5,     1,     1,
       0,     2,     3,     5,     4,     6,     3,     5,     4,     6,
       9,    11,     0,     2,     0,     6,     8,     0,     1,     1,
       3,     3,     0,     2,     8,    10,     0,     2,     0,     8,
       2,     3,     4,     3,     3,     0,     2,     4,     0,     1,
       0,     2,     2,     4,     7,     1,     4,     6,     6,     6,
       1,     1,     1,     1,     4,     6,     2,     2,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     1,     1,     2,     3,     3,     3,     3,
       3,     1,     1,     3,     3,     3,     1,     1,     3,     1,
       1,     2,     3,     3,     3,     3,     1,     0,     2,     1,
       1,     6,     6,     6,     6,     3,     3,     3,     2,     3,
       3,     3,     3,     3,     3,     3,     5,     8,     1,     1,
       2,     3,     3,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,   107,     1,     9,     8,     3,     4,     6,     5,
      10,   109,   110,   129,   106,     0,     0,     0,     0,   128,
     108,     0,    22,     0,     0,     0,     0,     0,   130,   118,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    11,    45,     0,
       0,     0,     0,     0,   125,   134,   115,   117,   116,   123,
     124,   119,   122,   121,   120,   131,   132,   133,     0,     0,
       0,     0,     0,     0,    23,     7,     0,     0,     0,     0,
       0,     0,     0,    12,     0,    16,     0,    24,    48,    46,
       0,     0,     0,     0,     0,     0,    14,     0,    18,   100,
       0,     0,     0,    99,    92,    91,     0,     0,     0,     0,
      32,    49,    50,     0,   111,   114,   113,   112,     0,     0,
     101,     0,     0,     0,     0,    13,     0,     0,     0,    17,
       0,     0,    36,     0,     0,     0,    15,    19,   105,   102,
     103,   104,    95,    93,    94,     0,    27,     0,    33,     0,
      47,    61,    62,     0,    63,    60,     0,     0,     0,     0,
      51,    52,     0,     0,     0,     0,     0,     0,     0,    28,
      29,     0,    25,     0,    37,     0,     0,    66,    67,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   126,     0,     0,
       0,     0,     0,     0,     0,    40,     0,    96,     0,     0,
      82,    84,    83,     0,     0,     0,    75,    74,    73,    72,
      80,    81,    76,    79,    78,    77,    68,    69,    70,    71,
      97,     0,     0,     0,    20,    31,    26,    30,     0,    41,
       0,     0,     0,    85,     0,     0,     0,     0,     0,    53,
       0,    64,     0,     0,     0,     0,     0,     0,     0,     0,
      55,     0,     0,    90,    86,    87,    88,    89,    98,   127,
      21,     0,     0,     0,     0,     0,    38,    65,     0,     0,
       0,     0,     0,     0,     0,    54,     0,    34,     0,     0,
      56,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    39,    35,    59,    57,    58,     0,    44,    43,
      42
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     6,     7,    10,     8,    22,    47,    48,
      74,   110,   168,   169,   170,   132,   148,   149,   174,   284,
     175,   294,    75,    89,   112,   133,   160,   161,   259,   230,
     215,   107,   163,   231,   102,    19,     9,    90,    91,    21
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -284
static const yytype_int16 yypact[] =
{
    -284,    17,    43,  -284,  -284,  -284,  -284,  -284,  -284,    82,
    -284,  -284,  -284,  -284,  -284,   408,    79,    82,    82,  -284,
     141,   212,   458,     7,    24,    32,    41,    79,  -284,  -284,
     138,   203,    82,    82,    82,    79,    79,    79,    79,    79,
      79,    79,    79,    79,    -4,    14,    47,  -284,    63,    30,
      35,    51,    56,   291,  -284,  -284,   377,    91,  -284,   169,
     169,   169,   169,   169,   169,    73,    73,  -284,    87,   127,
     -26,    16,   102,   147,  -284,   155,    82,    82,    82,    82,
      60,    61,    85,  -284,    59,  -284,   105,   133,   157,  -284,
     347,   136,   139,   254,   353,    85,  -284,    59,  -284,  -284,
      85,    85,   335,  -284,  -284,  -284,    59,   144,    85,   342,
    -284,  -284,  -284,   -39,  -284,  -284,  -284,  -284,   385,   165,
    -284,   294,    85,    85,    85,  -284,    69,    59,    59,  -284,
      13,   359,   417,    45,   439,   445,  -284,  -284,  -284,   369,
     369,  -284,  -284,  -284,  -284,    85,   455,   429,  -284,   -12,
    -284,  -284,  -284,   410,   411,  -284,   100,   100,   457,   100,
    -284,  -284,   227,   412,    82,   414,   354,   426,   416,   418,
    -284,   419,  -284,   -27,  -284,   100,   464,  -284,  -284,   420,
     184,   103,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   141,   467,   -16,
     469,   478,   455,    85,   424,  -284,   163,  -284,   425,   100,
    -284,  -284,  -284,   103,   103,   373,   283,   315,   326,   350,
     192,   192,   409,   409,   409,   409,   375,   375,  -284,  -284,
     256,   -67,   427,    85,  -284,  -284,  -284,  -284,    18,  -284,
     111,   100,   242,  -284,    76,   103,   103,   103,   103,  -284,
     100,  -284,    82,   389,    85,   423,   428,   430,   431,   448,
     413,    90,   103,  -284,   396,   396,  -284,  -284,   256,   141,
    -284,   357,    59,    59,   103,    59,  -284,  -284,   381,   -15,
     295,   358,   287,   362,    -6,  -284,    85,  -284,    59,    59,
    -284,    59,   482,    -6,   -14,   393,   113,   329,   331,   446,
     -47,    -6,  -284,  -284,  -284,  -284,  -284,   100,  -284,  -284,
     270
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -284,  -284,  -284,  -284,  -284,  -284,  -284,  -284,  -284,  -284,
    -284,  -284,  -284,  -284,   311,  -284,  -284,  -284,  -284,  -284,
    -284,  -283,  -284,  -284,  -284,  -284,  -284,  -284,  -284,   -88,
    -192,   -95,   338,   274,   -64,   -81,  -284,    -9,   293,   171
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -97
static const yytype_int16 yytable[] =
{
      20,   103,   119,   233,   286,   204,    68,    69,    29,    30,
     300,   126,   172,   250,   103,   251,   301,     3,   309,   103,
     103,   243,   244,    56,    57,    58,   292,   103,    70,   145,
     134,   118,   143,   144,   254,   308,   120,   121,    82,   135,
      49,   103,   103,   103,   130,   162,    71,     4,   260,   301,
       5,    83,   205,   264,   265,   266,   267,    50,   139,   140,
     141,   234,   287,   302,   103,    51,   173,   150,   177,   178,
     278,   180,   151,   152,    52,   293,   153,   154,   155,    72,
      84,   166,   282,   122,   123,   124,    73,   206,   122,   123,
     124,   104,   105,    85,   216,   217,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,   228,   229,    76,    11,
      12,    13,    14,    77,    13,    14,   156,    99,    14,    80,
     157,   242,   103,   158,    95,    97,   159,   151,   152,    78,
      15,   153,   154,   155,    79,   211,   212,    96,    98,   238,
     106,   127,   128,   211,   212,    43,   245,   246,   247,   248,
      16,   142,   103,    16,    34,   197,   100,    17,   263,    81,
      27,    86,   268,    18,   255,   256,   101,   257,   258,   253,
     250,   156,   277,   103,   213,   157,    88,   280,   281,    87,
     283,   159,   213,   108,   214,   127,   128,    28,   240,    31,
     271,   111,   214,   296,   297,   304,   298,   109,    53,    32,
      33,    34,    32,    33,    34,   103,    59,    60,    61,    62,
      63,    64,    65,    66,    67,   114,   127,   128,   115,   310,
      54,   129,   295,   182,   183,   184,   185,   186,   187,   188,
     189,   190,   191,   192,   193,   194,   195,   127,   128,    41,
      42,    43,   137,   269,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   195,   188,   189,
     190,   191,   192,   193,   194,   195,   210,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    55,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   262,   182,   183,   184,   185,   186,   187,   188,   189,
     190,   191,   192,   193,   194,   195,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     182,   183,   184,   116,   186,   187,   188,   189,   190,   191,
     192,   193,   194,   195,   183,   184,   185,   186,   187,   188,
     189,   190,   191,   192,   193,   194,   195,   245,   246,   247,
     248,    41,    42,    43,   122,   123,   124,   127,   128,   290,
      92,    93,    94,    55,   131,   288,   138,   184,   185,   186,
     187,   188,   189,   190,   191,   192,   193,   194,   195,   185,
     186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     113,   127,   128,   127,   128,   122,   123,   124,    32,    33,
      34,   305,   125,   306,   186,   187,   188,   189,   190,   191,
     192,   193,   194,   195,   122,   123,   124,   122,   123,   124,
     127,   128,   117,   199,   127,   128,   279,   146,   289,    33,
      34,   124,   291,   245,   246,   247,   248,   194,   195,   147,
     249,   245,   246,   247,   248,   122,   123,   124,   285,   122,
     123,   124,   136,   122,   123,   124,   270,    44,   247,   248,
     303,    45,   164,    46,    23,    24,    25,    26,   165,   192,
     193,   194,   195,   245,   246,   247,   248,   167,   171,   179,
     200,   176,   -96,   196,   198,   201,   207,   203,   202,   209,
     232,   235,   236,   239,   272,   241,   252,   276,   299,   273,
     307,   274,   275,   237,   208,   261
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-284))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_uint16 yycheck[] =
{
       9,    82,    97,    19,    19,    32,    10,    11,    17,    18,
     293,   106,    24,    80,    95,    82,    63,     0,   301,   100,
     101,   213,   214,    32,    33,    34,    32,   108,    32,    16,
      69,    95,   127,   128,    16,    82,   100,   101,    64,    78,
      33,   122,   123,   124,   108,   133,    32,     4,   240,    63,
       7,    77,    79,   245,   246,   247,   248,    33,   122,   123,
     124,    77,    77,    77,   145,    33,    78,    22,   156,   157,
     262,   159,    27,    28,    33,    81,    31,    32,    33,    32,
      64,   145,   274,    70,    71,    72,    23,   175,    70,    71,
      72,    32,    33,    77,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   195,    78,    27,
      28,    32,    33,    78,    32,    33,    71,    32,    33,    32,
      75,   209,   203,    78,    64,    64,    81,    27,    28,    78,
      48,    31,    32,    33,    78,    32,    33,    77,    77,   203,
      81,    72,    73,    32,    33,    72,    70,    71,    72,    73,
      71,    82,   233,    71,    63,   164,    71,    75,    82,    32,
      81,    59,   250,    81,    53,    54,    81,    56,    57,   233,
      80,    71,    82,   254,    71,    75,    21,   272,   273,    32,
     275,    81,    71,    78,    81,    72,    73,    16,    25,    18,
     254,    34,    81,   288,   289,    82,   291,    64,    27,    61,
      62,    63,    61,    62,    63,   286,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    79,    72,    73,    79,   307,
      82,    77,   286,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    72,    73,    70,
      71,    72,    77,   252,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    66,    67,
      68,    69,    70,    71,    72,    73,    82,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    82,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      60,    61,    62,    79,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    70,    71,    72,
      73,    70,    71,    72,    70,    71,    72,    72,    73,    82,
      77,    78,    79,    82,    32,    80,    82,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      53,    72,    73,    72,    73,    70,    71,    72,    61,    62,
      63,    82,    77,    82,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    70,    71,    72,    70,    71,    72,
      72,    73,    79,    79,    72,    73,    79,    78,    80,    62,
      63,    72,    80,    70,    71,    72,    73,    72,    73,    32,
      77,    70,    71,    72,    73,    70,    71,    72,    77,    70,
      71,    72,    77,    70,    71,    72,    77,     9,    72,    73,
      77,    13,    33,    15,    66,    67,    68,    69,    33,    70,
      71,    72,    73,    70,    71,    72,    73,    32,    59,    32,
      64,    81,    81,    81,    80,    79,    32,    78,    80,    79,
      33,    32,    24,    79,    81,    80,    79,    59,    26,    81,
      64,    81,    81,   202,   176,   241
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    84,    85,     0,     4,     7,    86,    87,    89,   119,
      88,    27,    28,    32,    33,    48,    71,    75,    81,   118,
     120,   122,    90,    66,    67,    68,    69,    81,   122,   120,
     120,   122,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,     9,    13,    15,    91,    92,    33,
      33,    33,    33,   122,    82,    82,   120,   120,   120,   122,
     122,   122,   122,   122,   122,   122,   122,   122,    10,    11,
      32,    32,    32,    23,    93,   105,    78,    78,    78,    78,
      32,    32,    64,    77,    64,    77,    59,    32,    21,   106,
     120,   121,   121,   121,   121,    64,    77,    64,    77,    32,
      71,    81,   117,   118,    32,    33,    81,   114,    78,    64,
      94,    34,   107,    53,    79,    79,    79,    79,   117,   114,
     117,   117,    70,    71,    72,    77,   114,    72,    73,    77,
     117,    32,    98,   108,    69,    78,    77,    77,    82,   117,
     117,   117,    82,   114,   114,    16,    78,    32,    99,   100,
      22,    27,    28,    31,    32,    33,    71,    75,    78,    81,
     109,   110,   112,   115,    33,    33,   117,    32,    95,    96,
      97,    59,    24,    78,   101,   103,    81,   112,   112,    32,
     112,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    81,   120,    80,    79,
      64,    79,    80,    78,    32,    79,   112,    32,   115,    79,
      82,    32,    33,    71,    81,   113,   112,   112,   112,   112,
     112,   112,   112,   112,   112,   112,   112,   112,   112,   112,
     112,   116,    33,    19,    77,    32,    24,    97,   117,    79,
      25,    80,   112,   113,   113,    70,    71,    72,    73,    77,
      80,    82,    79,   117,    16,    53,    54,    56,    57,   111,
     113,   116,    59,    82,   113,   113,   113,   113,   112,   120,
      77,   117,    81,    81,    81,    81,    59,    82,   113,    79,
     114,   114,   113,   114,   102,    77,    19,    77,    80,    80,
      82,    80,    32,    81,   104,   117,   114,   114,   114,    26,
     104,    63,    77,    77,    82,    82,    82,    64,    82,   104,
     112
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (scanner, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex (scanner)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location, scanner); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void* scanner)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, scanner)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    void* scanner;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
  YYUSE (scanner);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void* scanner)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp, scanner)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    void* scanner;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, scanner);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, void* scanner)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule, scanner)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
    void* scanner;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       , scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule, scanner); \
} while (YYID (0))

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
#ifndef	YYINITDEPTH
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
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
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
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
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
  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  YYSIZE_T yysize1;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = 0;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
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
                yysize1 = yysize + yytnamerr (0, yytname[yyx]);
                if (! (yysize <= yysize1
                       && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                  return 2;
                yysize = yysize1;
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

  yysize1 = yysize + yystrlen (yyformat);
  if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
    return 2;
  yysize = yysize1;

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

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, void* scanner)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp, scanner)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
    void* scanner;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (scanner);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {
      case 33: /* "NUMBER" */

/* Line 1391 of yacc.c  */
#line 419 "parser.yy"
	{ delete (yyvaluep->number); };

/* Line 1391 of yacc.c  */
#line 1882 "parser.cc"
	break;
      case 112: /* "expr" */

/* Line 1391 of yacc.c  */
#line 418 "parser.yy"
	{ delete (yyvaluep->expr); };

/* Line 1391 of yacc.c  */
#line 1891 "parser.cc"
	break;
      case 113: /* "rate_expr" */

/* Line 1391 of yacc.c  */
#line 418 "parser.yy"
	{ delete (yyvaluep->expr); };

/* Line 1391 of yacc.c  */
#line 1900 "parser.cc"
	break;
      case 114: /* "const_rate_expr" */

/* Line 1391 of yacc.c  */
#line 418 "parser.yy"
	{ delete (yyvaluep->expr); };

/* Line 1391 of yacc.c  */
#line 1909 "parser.cc"
	break;
      case 116: /* "arguments" */

/* Line 1391 of yacc.c  */
#line 420 "parser.yy"
	{ delete (yyvaluep->arguments); };

/* Line 1391 of yacc.c  */
#line 1918 "parser.cc"
	break;
      case 117: /* "const_expr" */

/* Line 1391 of yacc.c  */
#line 418 "parser.yy"
	{ delete (yyvaluep->expr); };

/* Line 1391 of yacc.c  */
#line 1927 "parser.cc"
	break;
      case 122: /* "csl_expr" */

/* Line 1391 of yacc.c  */
#line 418 "parser.yy"
	{ delete (yyvaluep->expr); };

/* Line 1391 of yacc.c  */
#line 1936 "parser.cc"
	break;

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void* scanner);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;

/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void* scanner)
#else
int
yyparse (scanner)
    void* scanner;
#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks thru separate pointers, to allow yyoverflow
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
  int yytoken;
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

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;

#if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 1;
#endif

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
      yychar = YYLEX;
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
  *++yyvsp = yylval;
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
     `$$ = $1'.

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

/* Line 1806 of yacc.c  */
#line 424 "parser.yy"
    { success = true; }
    break;

  case 3:

/* Line 1806 of yacc.c  */
#line 425 "parser.yy"
    { check_undeclared(); if (!success) YYERROR; }
    break;

  case 6:

/* Line 1806 of yacc.c  */
#line 436 "parser.yy"
    { prepare_model(); }
    break;

  case 7:

/* Line 1806 of yacc.c  */
#line 437 "parser.yy"
    { compile_model(); }
    break;

  case 12:

/* Line 1806 of yacc.c  */
#line 451 "parser.yy"
    { declare_constant((yyvsp[(2) - (3)].str), nullptr); }
    break;

  case 13:

/* Line 1806 of yacc.c  */
#line 453 "parser.yy"
    { declare_constant((yyvsp[(2) - (5)].str), (yyvsp[(4) - (5)].expr)); }
    break;

  case 14:

/* Line 1806 of yacc.c  */
#line 455 "parser.yy"
    { declare_constant((yyvsp[(3) - (4)].str), nullptr); }
    break;

  case 15:

/* Line 1806 of yacc.c  */
#line 457 "parser.yy"
    { declare_constant((yyvsp[(3) - (6)].str), (yyvsp[(5) - (6)].expr)); }
    break;

  case 16:

/* Line 1806 of yacc.c  */
#line 459 "parser.yy"
    { declare_rate((yyvsp[(2) - (3)].str), nullptr); }
    break;

  case 17:

/* Line 1806 of yacc.c  */
#line 461 "parser.yy"
    { declare_rate((yyvsp[(2) - (5)].str), (yyvsp[(4) - (5)].expr)); }
    break;

  case 18:

/* Line 1806 of yacc.c  */
#line 463 "parser.yy"
    { declare_rate((yyvsp[(3) - (4)].str), nullptr); }
    break;

  case 19:

/* Line 1806 of yacc.c  */
#line 465 "parser.yy"
    { declare_rate((yyvsp[(3) - (6)].str), (yyvsp[(5) - (6)].expr)); }
    break;

  case 20:

/* Line 1806 of yacc.c  */
#line 467 "parser.yy"
    { declare_variable((yyvsp[(2) - (9)].str), (yyvsp[(5) - (9)].expr), (yyvsp[(7) - (9)].expr), nullptr); }
    break;

  case 21:

/* Line 1806 of yacc.c  */
#line 470 "parser.yy"
    { declare_variable((yyvsp[(2) - (11)].str), (yyvsp[(5) - (11)].expr), (yyvsp[(7) - (11)].expr), (yyvsp[(10) - (11)].expr)); }
    break;

  case 24:

/* Line 1806 of yacc.c  */
#line 481 "parser.yy"
    { prepare_module((yyvsp[(2) - (2)].str)); }
    break;

  case 25:

/* Line 1806 of yacc.c  */
#line 483 "parser.yy"
    { add_module(); }
    break;

  case 26:

/* Line 1806 of yacc.c  */
#line 485 "parser.yy"
    { add_module((yyvsp[(2) - (8)].str), (yyvsp[(4) - (8)].str)); }
    break;

  case 31:

/* Line 1806 of yacc.c  */
#line 496 "parser.yy"
    { add_substitution((yyvsp[(1) - (3)].str), (yyvsp[(3) - (3)].str)); }
    break;

  case 34:

/* Line 1806 of yacc.c  */
#line 504 "parser.yy"
    { declare_variable((yyvsp[(1) - (8)].str), (yyvsp[(4) - (8)].expr), (yyvsp[(6) - (8)].expr), nullptr); }
    break;

  case 35:

/* Line 1806 of yacc.c  */
#line 507 "parser.yy"
    { declare_variable((yyvsp[(1) - (10)].str), (yyvsp[(4) - (10)].expr), (yyvsp[(6) - (10)].expr), (yyvsp[(9) - (10)].expr)); }
    break;

  case 38:

/* Line 1806 of yacc.c  */
#line 515 "parser.yy"
    { prepare_command((yyvsp[(1) - (5)].synch), MakeUnique((yyvsp[(2) - (5)].expr)), (yyvsp[(4) - (5)].dist)); }
    break;

  case 39:

/* Line 1806 of yacc.c  */
#line 516 "parser.yy"
    { add_command(); }
    break;

  case 40:

/* Line 1806 of yacc.c  */
#line 519 "parser.yy"
    { (yyval.synch) = 0; }
    break;

  case 41:

/* Line 1806 of yacc.c  */
#line 520 "parser.yy"
    { (yyval.synch) = synchronization_value((yyvsp[(2) - (3)].str)); }
    break;

  case 42:

/* Line 1806 of yacc.c  */
#line 523 "parser.yy"
    { add_update((yyvsp[(1) - (4)].str), (yyvsp[(4) - (4)].expr)); }
    break;

  case 53:

/* Line 1806 of yacc.c  */
#line 549 "parser.yy"
    { delete (yyvsp[(1) - (4)].expr); delete (yyvsp[(3) - (4)].expr); }
    break;

  case 54:

/* Line 1806 of yacc.c  */
#line 553 "parser.yy"
    { delete (yyvsp[(2) - (7)].str); delete (yyvsp[(4) - (7)].expr); delete (yyvsp[(6) - (7)].expr); }
    break;

  case 55:

/* Line 1806 of yacc.c  */
#line 561 "parser.yy"
    { (yyval.dist) = NewExponential(MakeUnique((yyvsp[(1) - (1)].expr))); }
    break;

  case 56:

/* Line 1806 of yacc.c  */
#line 563 "parser.yy"
    { (yyval.dist) = NewExponential(MakeUnique((yyvsp[(3) - (4)].expr))); }
    break;

  case 57:

/* Line 1806 of yacc.c  */
#line 565 "parser.yy"
    { (yyval.dist) = NewWeibull(MakeUnique((yyvsp[(3) - (6)].expr)), MakeUnique((yyvsp[(5) - (6)].expr))); }
    break;

  case 58:

/* Line 1806 of yacc.c  */
#line 567 "parser.yy"
    { (yyval.dist) = NewLognormal(MakeUnique((yyvsp[(3) - (6)].expr)), MakeUnique((yyvsp[(5) - (6)].expr))); }
    break;

  case 59:

/* Line 1806 of yacc.c  */
#line 569 "parser.yy"
    { (yyval.dist) = NewUniform(MakeUnique((yyvsp[(3) - (6)].expr)), MakeUnique((yyvsp[(5) - (6)].expr))); }
    break;

  case 60:

/* Line 1806 of yacc.c  */
#line 576 "parser.yy"
    { (yyval.expr) = new Literal(*(yyvsp[(1) - (1)].number)); delete (yyvsp[(1) - (1)].number); }
    break;

  case 61:

/* Line 1806 of yacc.c  */
#line 578 "parser.yy"
    { (yyval.expr) = new Literal(true); }
    break;

  case 62:

/* Line 1806 of yacc.c  */
#line 580 "parser.yy"
    { (yyval.expr) = new Literal(true); }
    break;

  case 63:

/* Line 1806 of yacc.c  */
#line 582 "parser.yy"
    { (yyval.expr) = find_variable((yyvsp[(1) - (1)].str)); }
    break;

  case 64:

/* Line 1806 of yacc.c  */
#line 584 "parser.yy"
    { (yyval.expr) = new FunctionCall((yyvsp[(1) - (4)].function), std::move(*(yyvsp[(3) - (4)].arguments))); delete (yyvsp[(3) - (4)].arguments); }
    break;

  case 65:

/* Line 1806 of yacc.c  */
#line 586 "parser.yy"
    { (yyval.expr) = new FunctionCall((yyvsp[(3) - (6)].function), std::move(*(yyvsp[(5) - (6)].arguments))); delete (yyvsp[(5) - (6)].arguments); }
    break;

  case 66:

/* Line 1806 of yacc.c  */
#line 588 "parser.yy"
    { (yyval.expr) = NewNegate(MakeUnique((yyvsp[(2) - (2)].expr))); }
    break;

  case 67:

/* Line 1806 of yacc.c  */
#line 590 "parser.yy"
    { (yyval.expr) = NewNot(MakeUnique((yyvsp[(2) - (2)].expr))); }
    break;

  case 68:

/* Line 1806 of yacc.c  */
#line 592 "parser.yy"
    { (yyval.expr) = NewPlus(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 69:

/* Line 1806 of yacc.c  */
#line 594 "parser.yy"
    { (yyval.expr) = NewMinus(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 70:

/* Line 1806 of yacc.c  */
#line 596 "parser.yy"
    { (yyval.expr) = NewMultiply(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 71:

/* Line 1806 of yacc.c  */
#line 598 "parser.yy"
    { (yyval.expr) = NewDivide(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 72:

/* Line 1806 of yacc.c  */
#line 600 "parser.yy"
    { (yyval.expr) = NewAnd(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 73:

/* Line 1806 of yacc.c  */
#line 602 "parser.yy"
    { (yyval.expr) = NewOr(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 74:

/* Line 1806 of yacc.c  */
#line 604 "parser.yy"
    { (yyval.expr) = NewImply(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 75:

/* Line 1806 of yacc.c  */
#line 606 "parser.yy"
    { (yyval.expr) = NewIff(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 76:

/* Line 1806 of yacc.c  */
#line 608 "parser.yy"
    { (yyval.expr) = NewLess(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 77:

/* Line 1806 of yacc.c  */
#line 610 "parser.yy"
    { (yyval.expr) = NewLessEqual(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 78:

/* Line 1806 of yacc.c  */
#line 612 "parser.yy"
    { (yyval.expr) = NewGreaterEqual(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 79:

/* Line 1806 of yacc.c  */
#line 614 "parser.yy"
    { (yyval.expr) = NewGreater(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 80:

/* Line 1806 of yacc.c  */
#line 616 "parser.yy"
    { (yyval.expr) = NewEqual(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 81:

/* Line 1806 of yacc.c  */
#line 618 "parser.yy"
    { (yyval.expr) = NewNotEqual(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 82:

/* Line 1806 of yacc.c  */
#line 620 "parser.yy"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;

  case 83:

/* Line 1806 of yacc.c  */
#line 624 "parser.yy"
    { (yyval.expr) = make_literal((yyvsp[(1) - (1)].number)); }
    break;

  case 84:

/* Line 1806 of yacc.c  */
#line 626 "parser.yy"
    { (yyval.expr) = find_rate_or_variable((yyvsp[(1) - (1)].str)); }
    break;

  case 85:

/* Line 1806 of yacc.c  */
#line 628 "parser.yy"
    { (yyval.expr) = NewNegate(MakeUnique((yyvsp[(2) - (2)].expr))); }
    break;

  case 86:

/* Line 1806 of yacc.c  */
#line 630 "parser.yy"
    { (yyval.expr) = NewPlus(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 87:

/* Line 1806 of yacc.c  */
#line 632 "parser.yy"
    { (yyval.expr) = NewMinus(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 88:

/* Line 1806 of yacc.c  */
#line 634 "parser.yy"
    { (yyval.expr) = NewMultiply(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 89:

/* Line 1806 of yacc.c  */
#line 636 "parser.yy"
    { (yyval.expr) = NewDivide(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 90:

/* Line 1806 of yacc.c  */
#line 638 "parser.yy"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;

  case 91:

/* Line 1806 of yacc.c  */
#line 642 "parser.yy"
    { (yyval.expr) = make_literal((yyvsp[(1) - (1)].number)); }
    break;

  case 92:

/* Line 1806 of yacc.c  */
#line 644 "parser.yy"
    { (yyval.expr) = find_rate((yyvsp[(1) - (1)].str)); }
    break;

  case 93:

/* Line 1806 of yacc.c  */
#line 646 "parser.yy"
    { (yyval.expr) = NewMultiply(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 94:

/* Line 1806 of yacc.c  */
#line 648 "parser.yy"
    { (yyval.expr) = NewDivide(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 95:

/* Line 1806 of yacc.c  */
#line 650 "parser.yy"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;

  case 96:

/* Line 1806 of yacc.c  */
#line 654 "parser.yy"
    { (yyval.function) = MakeFunction(*(yyvsp[(1) - (1)].str)); delete (yyvsp[(1) - (1)].str); }
    break;

  case 97:

/* Line 1806 of yacc.c  */
#line 658 "parser.yy"
    { (yyval.arguments) = new UniquePtrVector<const Expression>(MakeUnique((yyvsp[(1) - (1)].expr))); }
    break;

  case 98:

/* Line 1806 of yacc.c  */
#line 660 "parser.yy"
    { (yyval.arguments) = (yyvsp[(1) - (3)].arguments); (yyval.arguments)->push_back(MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 99:

/* Line 1806 of yacc.c  */
#line 668 "parser.yy"
    { (yyval.expr) = make_literal((yyvsp[(1) - (1)].nat)); }
    break;

  case 100:

/* Line 1806 of yacc.c  */
#line 670 "parser.yy"
    { (yyval.expr) = find_constant((yyvsp[(1) - (1)].str)); }
    break;

  case 101:

/* Line 1806 of yacc.c  */
#line 672 "parser.yy"
    { (yyval.expr) = NewNegate(MakeUnique((yyvsp[(2) - (2)].expr))); }
    break;

  case 102:

/* Line 1806 of yacc.c  */
#line 674 "parser.yy"
    { (yyval.expr) = NewPlus(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 103:

/* Line 1806 of yacc.c  */
#line 676 "parser.yy"
    { (yyval.expr) = NewMinus(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 104:

/* Line 1806 of yacc.c  */
#line 678 "parser.yy"
    { (yyval.expr) = NewMultiply(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 105:

/* Line 1806 of yacc.c  */
#line 680 "parser.yy"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;

  case 106:

/* Line 1806 of yacc.c  */
#line 683 "parser.yy"
    { (yyval.nat) = integer_value((yyvsp[(1) - (1)].number)); }
    break;

  case 108:

/* Line 1806 of yacc.c  */
#line 692 "parser.yy"
    { properties.push_back((yyvsp[(2) - (2)].formula)); }
    break;

  case 109:

/* Line 1806 of yacc.c  */
#line 695 "parser.yy"
    { (yyval.formula) = new Conjunction(); }
    break;

  case 110:

/* Line 1806 of yacc.c  */
#line 696 "parser.yy"
    { (yyval.formula) = new Disjunction(); }
    break;

  case 111:

/* Line 1806 of yacc.c  */
#line 698 "parser.yy"
    { (yyval.formula) = make_probabilistic((yyvsp[(3) - (6)].number), true, true, (yyvsp[(5) - (6)].path)); }
    break;

  case 112:

/* Line 1806 of yacc.c  */
#line 700 "parser.yy"
    { (yyval.formula) = make_probabilistic((yyvsp[(3) - (6)].number), false, true, (yyvsp[(5) - (6)].path)); }
    break;

  case 113:

/* Line 1806 of yacc.c  */
#line 702 "parser.yy"
    { (yyval.formula) = make_probabilistic((yyvsp[(3) - (6)].number), false, false, (yyvsp[(5) - (6)].path)); }
    break;

  case 114:

/* Line 1806 of yacc.c  */
#line 704 "parser.yy"
    { (yyval.formula) = make_probabilistic((yyvsp[(3) - (6)].number), true, false, (yyvsp[(5) - (6)].path)); }
    break;

  case 115:

/* Line 1806 of yacc.c  */
#line 706 "parser.yy"
    { (yyval.formula) = new Implication((yyvsp[(1) - (3)].formula), (yyvsp[(3) - (3)].formula)); }
    break;

  case 116:

/* Line 1806 of yacc.c  */
#line 707 "parser.yy"
    { (yyval.formula) = make_conjunction((yyvsp[(1) - (3)].formula), (yyvsp[(3) - (3)].formula)); }
    break;

  case 117:

/* Line 1806 of yacc.c  */
#line 708 "parser.yy"
    { (yyval.formula) = make_disjunction((yyvsp[(1) - (3)].formula), (yyvsp[(3) - (3)].formula)); }
    break;

  case 118:

/* Line 1806 of yacc.c  */
#line 709 "parser.yy"
    { (yyval.formula) = new Negation((yyvsp[(2) - (2)].formula)); }
    break;

  case 119:

/* Line 1806 of yacc.c  */
#line 711 "parser.yy"
    { (yyval.formula) = NewLessThan(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 120:

/* Line 1806 of yacc.c  */
#line 713 "parser.yy"
    { (yyval.formula) = NewLessThanOrEqual(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 121:

/* Line 1806 of yacc.c  */
#line 715 "parser.yy"
    { (yyval.formula) = NewGreaterThanOrEqual(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 122:

/* Line 1806 of yacc.c  */
#line 717 "parser.yy"
    { (yyval.formula) = NewGreaterThan(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 123:

/* Line 1806 of yacc.c  */
#line 719 "parser.yy"
    { (yyval.formula) = NewEquality(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 124:

/* Line 1806 of yacc.c  */
#line 721 "parser.yy"
    { (yyval.formula) = NewInequality(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 125:

/* Line 1806 of yacc.c  */
#line 723 "parser.yy"
    { (yyval.formula) = (yyvsp[(2) - (3)].formula); }
    break;

  case 126:

/* Line 1806 of yacc.c  */
#line 727 "parser.yy"
    { (yyval.path) = make_until((yyvsp[(1) - (5)].formula), (yyvsp[(5) - (5)].formula), nullptr, (yyvsp[(4) - (5)].number)); }
    break;

  case 127:

/* Line 1806 of yacc.c  */
#line 729 "parser.yy"
    { (yyval.path) = make_until((yyvsp[(1) - (8)].formula), (yyvsp[(8) - (8)].formula), (yyvsp[(4) - (8)].number), (yyvsp[(6) - (8)].number)); }
    break;

  case 128:

/* Line 1806 of yacc.c  */
#line 734 "parser.yy"
    { (yyval.expr) = make_literal((yyvsp[(1) - (1)].nat)); }
    break;

  case 129:

/* Line 1806 of yacc.c  */
#line 736 "parser.yy"
    { (yyval.expr) = value_or_variable((yyvsp[(1) - (1)].str)); }
    break;

  case 130:

/* Line 1806 of yacc.c  */
#line 738 "parser.yy"
    { (yyval.expr) = NewNegate(MakeUnique((yyvsp[(2) - (2)].expr))); }
    break;

  case 131:

/* Line 1806 of yacc.c  */
#line 740 "parser.yy"
    { (yyval.expr) = NewPlus(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 132:

/* Line 1806 of yacc.c  */
#line 742 "parser.yy"
    { (yyval.expr) = NewMinus(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 133:

/* Line 1806 of yacc.c  */
#line 744 "parser.yy"
    { (yyval.expr) = NewMultiply(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 134:

/* Line 1806 of yacc.c  */
#line 746 "parser.yy"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;



/* Line 1806 of yacc.c  */
#line 3008 "parser.cc"
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

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
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
  /* Do not reclaim the symbols of the rule which action triggered
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
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

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

  *++yyvsp = yylval;

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

#if !defined(yyoverflow) || YYERROR_VERBOSE
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
  /* Do not reclaim the symbols of the rule which action triggered
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
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 2067 of yacc.c  */
#line 749 "parser.yy"


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

static Conjunction* make_conjunction(StateFormula* f1,
				     const StateFormula* f2) {
  Conjunction* conj = dynamic_cast<Conjunction*>(f1);
  if (conj == nullptr) {
    conj = new Conjunction();
    conj->add_conjunct(f1);
  }
  conj->add_conjunct(f2);
  return conj;
}

static Disjunction* make_disjunction(StateFormula* f1,
				     const StateFormula* f2) {
  Disjunction* disj = dynamic_cast<Disjunction*>(f1);
  if (disj == nullptr) {
    disj = new Disjunction();
    disj->add_disjunct(f1);
  }
  disj->add_disjunct(f2);
  return disj;
}

static StateFormula* make_probabilistic(const TypedValue* p,
					bool strict, bool negate,
					const PathFormula* f) {
  if (*p < 0 || *p > 1) {
    yyerror("probability bound outside the interval [0,1]");
  }
  bool s = (strict && !negate) || (!strict && negate);
  StateFormula* pr = new Probabilistic(*p, s, f);
  if (negate) {
    pr = new Negation(pr);
  }
  delete p;
  return pr;
}

static const Until* make_until(const StateFormula* f1, const StateFormula* f2,
			       const TypedValue* t1, const TypedValue* t2) {
  const Until* until;
  if (t1 == nullptr) {
    if (*t2 < 0) {
      yyerror("negative time bound");
    }
    until = new Until(f1, f2, 0, *t2);
  } else {
    if (*t1 < 0) {
      yyerror("negative time bound");
    } else if (*t2 < *t1) {
      yyerror("empty time interval");
    }
    until = new Until(f1, f2, *t1, *t2);
  }
  delete t2;
  return until;
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

class ExpressionIdentifierSubstituter : public ExpressionVisitor {
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

  std::map<std::string, std::string> substitutions_;
  std::unique_ptr<const Expression> expr_;
};

std::unique_ptr<const Expression> SubstituteIdentifiers(
    const Expression& expr,
    const std::map<std::string, std::string>& substitutions) {
  ExpressionIdentifierSubstituter substituter(substitutions);
  expr.Accept(&substituter);
  return substituter.release_expr();
}

class StateFormulaIdentifierSubstituter : public StateFormulaVisitor {
 public:
  explicit StateFormulaIdentifierSubstituter(
      const std::map<std::string, std::string>& substitutions);

  ~StateFormulaIdentifierSubstituter();

  const StateFormula* release_formula();

 private:
  virtual void DoVisitConjunction(const Conjunction& formula);
  virtual void DoVisitDisjunction(const Disjunction& formula);
  virtual void DoVisitNegation(const Negation& formula);
  virtual void DoVisitImplication(const Implication& formula);
  virtual void DoVisitProbabilistic(const Probabilistic& formula);
  virtual void DoVisitComparison(const Comparison& formula);

  std::map<std::string, std::string> substitutions_;
  const StateFormula* formula_;
};

const StateFormula* SubstituteIdentifiers(
    const StateFormula& formula,
    const std::map<std::string, std::string>& substitutions) {
  StateFormulaIdentifierSubstituter substituter(substitutions);
  formula.Accept(&substituter);
  return substituter.release_formula();
}

class PathFormulaIdentifierSubstituter : public PathFormulaVisitor {
 public:
  explicit PathFormulaIdentifierSubstituter(
      const std::map<std::string, std::string>& substitutions);

  ~PathFormulaIdentifierSubstituter();

  const PathFormula* release_formula();

 private:
  virtual void DoVisitUntil(const Until& formula);

  std::map<std::string, std::string> substitutions_;
  const PathFormula* formula_;
};

const PathFormula* SubstituteIdentifiers(
    const PathFormula& formula,
    const std::map<std::string, std::string>& substitutions) {
  PathFormulaIdentifierSubstituter substituter(substitutions);
  formula.Accept(&substituter);
  return substituter.release_formula();
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
  LOG(FATAL) << "not an expression";
}

StateFormulaIdentifierSubstituter::StateFormulaIdentifierSubstituter(
    const std::map<std::string, std::string>& substitutions)
    : substitutions_(substitutions), formula_(nullptr) {
}

StateFormulaIdentifierSubstituter::~StateFormulaIdentifierSubstituter() {
  delete formula_;
}

const StateFormula* StateFormulaIdentifierSubstituter::release_formula() {
  const StateFormula* formula = formula_;
  formula_ = nullptr;
  return formula;
}

void StateFormulaIdentifierSubstituter::DoVisitConjunction(
    const Conjunction& formula) {
  Conjunction* conjunction = new Conjunction();
  for (const StateFormula* conjunct : formula.conjuncts()) {
    conjunct->Accept(this);
    conjunction->add_conjunct(release_formula());
  }
  formula_ = conjunction;
}

void StateFormulaIdentifierSubstituter::DoVisitDisjunction(
    const Disjunction& formula) {
  Disjunction* disjunction = new Disjunction();
  for (const StateFormula* disjunct : formula.disjuncts()) {
    disjunct->Accept(this);
    disjunction->add_disjunct(release_formula());
  }
  formula_ = disjunction;
}

void StateFormulaIdentifierSubstituter::DoVisitNegation(
    const Negation& formula) {
  formula.negand().Accept(this);
  formula_ = new Negation(release_formula());
}

void StateFormulaIdentifierSubstituter::DoVisitImplication(
    const Implication& formula) {
  formula.antecedent().Accept(this);
  const StateFormula* antecedent = release_formula();
  formula.consequent().Accept(this);
  formula_ = new Implication(antecedent, release_formula());
}

void StateFormulaIdentifierSubstituter::DoVisitProbabilistic(
    const Probabilistic& formula) {
  formula_ = new Probabilistic(
      formula.threshold(), formula.strict(),
      SubstituteIdentifiers(formula.formula(), substitutions_));
}

void StateFormulaIdentifierSubstituter::DoVisitComparison(
    const Comparison& formula) {
  switch (formula.op()) {
    case Comparison::LESS:
      formula_ = new LessThan(
          SubstituteIdentifiers(formula.expr1(), substitutions_),
          SubstituteIdentifiers(formula.expr2(), substitutions_));
      break;
    case Comparison::LESS_EQUAL:
      formula_ = new LessThanOrEqual(
          SubstituteIdentifiers(formula.expr1(), substitutions_),
          SubstituteIdentifiers(formula.expr2(), substitutions_));
      break;
    case Comparison::GREATER_EQUAL:
      formula_ = new GreaterThanOrEqual(
          SubstituteIdentifiers(formula.expr1(), substitutions_),
          SubstituteIdentifiers(formula.expr2(), substitutions_));
      break;
    case Comparison::GREATER:
      formula_ = new GreaterThan(
          SubstituteIdentifiers(formula.expr1(), substitutions_),
          SubstituteIdentifiers(formula.expr2(), substitutions_));
      break;
    case Comparison::EQUAL:
      formula_ = new Equality(
          SubstituteIdentifiers(formula.expr1(), substitutions_),
          SubstituteIdentifiers(formula.expr2(), substitutions_));
      break;
    case Comparison::NOT_EQUAL:
      formula_ = new Inequality(
          SubstituteIdentifiers(formula.expr1(), substitutions_),
          SubstituteIdentifiers(formula.expr2(), substitutions_));
      break;
  }
}

PathFormulaIdentifierSubstituter::PathFormulaIdentifierSubstituter(
    const std::map<std::string, std::string>& substitutions)
    : substitutions_(substitutions), formula_(nullptr) {
}

PathFormulaIdentifierSubstituter::~PathFormulaIdentifierSubstituter() {
  delete formula_;
}

const PathFormula* PathFormulaIdentifierSubstituter::release_formula() {
  const PathFormula* formula = formula_;
  formula_ = nullptr;
  return formula;
}

void PathFormulaIdentifierSubstituter::DoVisitUntil(const Until& formula) {
  formula_ = new Until(SubstituteIdentifiers(formula.pre(), substitutions_),
                       SubstituteIdentifiers(formula.post(), substitutions_),
                       formula.min_time(), formula.max_time());
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
static void prepare_model() {
  clear_declarations();
  for (const StateFormula* property : properties) {
    delete property;
  }
  properties.clear();
  if (model != nullptr) {
    delete model;
  }
  model = new Model();
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


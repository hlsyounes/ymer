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
#define YYLSP_NEEDED 0



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

/* Workaround for bug in Bison 1.35 that disables stack growth. */
#define YYLTYPE_IS_TRIVIAL 1


/* An integer range. */
struct Range {
  const Expression* l;
  const Expression* h;
};


/* The lexer. */
extern int yylex();
/* Current line number. */
extern size_t line_number;
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

/* Outputs an error message. */
static void yyerror(const std::string& s);
/* Outputs a warning message. */
static void yywarning(const std::string& s);
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
/* Returns a range with the given bounds, signaling an error if the
   range is empty. */
static Range make_range(const Expression* l, const Expression* h);
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
                             const Range& range,
                             const Expression* start,
                             bool delayed_addition = false);
/* Adds a command to the current module. */
static void add_command();
/* Prepares a command for parsing. */
static void prepare_command(int synch, const StateFormula* guard,
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

template <typename T>
std::unique_ptr<T> MakeUnique(T* ptr) {
  return std::unique_ptr<T>(ptr);
}

Function MakeFunction(std::unique_ptr<const std::string>&& name) {
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
    yyerror("unknown function");
    return Function::UNKNOWN;
  }
}

const FunctionCall* NewFunctionCall(
    Function function,
    std::unique_ptr<UniquePtrVector<const Expression>>&& arguments) {
  return FunctionCall::New(function, std::move(*arguments)).release();
}

const UnaryOperation* NewNegate(std::unique_ptr<const Expression>&& operand) {
  return UnaryOperation::New(UnaryOperator::NEGATE, std::move(operand))
      .release();
}

const BinaryOperation* NewPlus(std::unique_ptr<const Expression>&& operand1,
                               std::unique_ptr<const Expression>&& operand2) {
  return BinaryOperation::New(BinaryOperator::PLUS,
                              std::move(operand1), std::move(operand2))
      .release();
}

const BinaryOperation* NewMinus(std::unique_ptr<const Expression>&& operand1,
                                std::unique_ptr<const Expression>&& operand2) {
  return BinaryOperation::New(BinaryOperator::MINUS,
                              std::move(operand1), std::move(operand2))
      .release();
}

const BinaryOperation* NewMultiply(
    std::unique_ptr<const Expression>&& operand1,
    std::unique_ptr<const Expression>&& operand2) {
  return BinaryOperation::New(BinaryOperator::MULTIPLY,
                              std::move(operand1), std::move(operand2))
      .release();
}

const BinaryOperation* NewDivide(std::unique_ptr<const Expression>&& operand1,
                                 std::unique_ptr<const Expression>&& operand2) {
  return BinaryOperation::New(BinaryOperator::DIVIDE,
                              std::move(operand1), std::move(operand2))
      .release();
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
#line 342 "parser.cc"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
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
     STOCHASTIC = 258,
     CTMC = 259,
     CONST_TOKEN = 260,
     INT_TOKEN = 261,
     DOUBLE = 262,
     RATE = 263,
     GLOBAL = 264,
     INIT = 265,
     TRUE_TOKEN = 266,
     FALSE_TOKEN = 267,
     EXP = 268,
     FUNC = 269,
     REWARDS = 270,
     ENDREWARDS = 271,
     MODULE = 272,
     ENDMODULE = 273,
     PNAME = 274,
     IDENTIFIER = 275,
     LABEL_NAME = 276,
     NUMBER = 277,
     ARROW = 278,
     DOTDOT = 279,
     ILLEGAL_TOKEN = 280,
     IMPLY = 281,
     NEQ = 282,
     EQ = 283,
     GTE = 284,
     LTE = 285,
     UMINUS = 286
   };
#endif
/* Tokens.  */
#define STOCHASTIC 258
#define CTMC 259
#define CONST_TOKEN 260
#define INT_TOKEN 261
#define DOUBLE 262
#define RATE 263
#define GLOBAL 264
#define INIT 265
#define TRUE_TOKEN 266
#define FALSE_TOKEN 267
#define EXP 268
#define FUNC 269
#define REWARDS 270
#define ENDREWARDS 271
#define MODULE 272
#define ENDMODULE 273
#define PNAME 274
#define IDENTIFIER 275
#define LABEL_NAME 276
#define NUMBER 277
#define ARROW 278
#define DOTDOT 279
#define ILLEGAL_TOKEN 280
#define IMPLY 281
#define NEQ 282
#define EQ 283
#define GTE 284
#define LTE 285
#define UMINUS 286




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 293 of yacc.c  */
#line 312 "parser.yy"

  size_t synch;
  StateFormula* formula;
  const PathFormula* path;
  const Distribution* dist;
  const Expression* expr;
  Range range;
  int nat;
  const std::string* str;
  const TypedValue* num;
  Function function;
  UniquePtrVector<const Expression>* arguments;



/* Line 293 of yacc.c  */
#line 456 "parser.cc"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 343 of yacc.c  */
#line 468 "parser.cc"

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
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

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
#define YYLAST   420

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  53
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  42
/* YYNRULES -- Number of rules.  */
#define YYNRULES  133
/* YYNRULES -- Number of states.  */
#define YYNSTATES  306

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   286

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    29,     2,     2,     2,     2,    27,     2,
      47,    48,    38,    36,    46,    37,     2,    39,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    43,    41,
      30,    42,    31,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,    50,     2,     2,     2,
      52,     2,     2,     2,     2,    51,     2,    49,     2,     2,
       2,    44,     2,    45,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    28,     2,     2,     2,     2,     2,
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
      25,    26,    32,    33,    34,    35,    40
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,     9,    11,    12,    18,    20,
      22,    23,    26,    30,    36,    41,    48,    52,    58,    63,
      70,    76,    84,    85,    88,    89,    96,   105,   106,   108,
     110,   114,   118,   119,   122,   127,   134,   135,   138,   139,
     148,   151,   155,   159,   163,   167,   168,   171,   176,   177,
     179,   180,   183,   186,   191,   199,   201,   203,   207,   211,
     214,   218,   222,   226,   230,   234,   238,   242,   244,   249,
     256,   263,   270,   272,   274,   279,   286,   289,   293,   297,
     301,   305,   307,   309,   312,   316,   320,   324,   328,   332,
     334,   336,   340,   344,   348,   350,   352,   356,   362,   364,
     366,   369,   373,   377,   381,   385,   387,   388,   391,   393,
     395,   402,   409,   416,   423,   427,   431,   435,   438,   442,
     446,   450,   454,   458,   462,   466,   472,   481,   483,   485,
     488,   492,   496,   500
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      54,     0,    -1,    -1,    55,    56,    -1,    57,    -1,    91,
      -1,    -1,    59,    58,    60,    62,    75,    -1,     3,    -1,
       4,    -1,    -1,    60,    61,    -1,     5,    20,    41,    -1,
       5,    20,    42,    89,    41,    -1,     5,     6,    20,    41,
      -1,     5,     6,    20,    42,    89,    41,    -1,     8,    20,
      41,    -1,     8,    20,    42,    85,    41,    -1,     5,     7,
      20,    41,    -1,     5,     7,    20,    42,    85,    41,    -1,
       9,    20,    43,    88,    41,    -1,     9,    20,    43,    88,
      10,    89,    41,    -1,    -1,    62,    63,    -1,    -1,    17,
      20,    64,    68,    70,    18,    -1,    17,    20,    42,    20,
      44,    65,    45,    18,    -1,    -1,    66,    -1,    67,    -1,
      66,    46,    67,    -1,    20,    42,    20,    -1,    -1,    68,
      69,    -1,    20,    43,    88,    41,    -1,    20,    43,    88,
      10,    89,    41,    -1,    -1,    70,    71,    -1,    -1,    73,
      81,    23,    82,    43,    72,    74,    41,    -1,    44,    45,
      -1,    44,    20,    45,    -1,    19,    42,    83,    -1,    74,
      27,    74,    -1,    47,    74,    48,    -1,    -1,    75,    76,
      -1,    15,    77,    78,    16,    -1,    -1,    21,    -1,    -1,
      78,    79,    -1,    78,    80,    -1,    81,    43,    84,    41,
      -1,    44,    20,    45,    81,    43,    84,    41,    -1,    11,
      -1,    12,    -1,    81,    27,    81,    -1,    81,    28,    81,
      -1,    29,    81,    -1,    83,    30,    83,    -1,    83,    35,
      83,    -1,    83,    34,    83,    -1,    83,    31,    83,    -1,
      83,    42,    83,    -1,    83,    32,    83,    -1,    47,    81,
      48,    -1,    84,    -1,    13,    47,    84,    48,    -1,    49,
      47,    85,    46,    85,    48,    -1,    50,    47,    85,    46,
      85,    48,    -1,    51,    47,    85,    46,    85,    48,    -1,
      90,    -1,    20,    -1,    86,    47,    87,    48,    -1,    14,
      47,    86,    46,    87,    48,    -1,    37,    83,    -1,    83,
      36,    83,    -1,    83,    37,    83,    -1,    83,    38,    83,
      -1,    47,    83,    48,    -1,    22,    -1,    20,    -1,    37,
      84,    -1,    84,    36,    84,    -1,    84,    37,    84,    -1,
      84,    38,    84,    -1,    84,    39,    84,    -1,    47,    84,
      48,    -1,    22,    -1,    20,    -1,    85,    38,    85,    -1,
      85,    39,    85,    -1,    47,    85,    48,    -1,    20,    -1,
      83,    -1,    87,    46,    83,    -1,    44,    89,    24,    89,
      45,    -1,    90,    -1,    20,    -1,    37,    89,    -1,    89,
      36,    89,    -1,    89,    37,    89,    -1,    89,    38,    89,
      -1,    47,    89,    48,    -1,    22,    -1,    -1,    91,    92,
      -1,    11,    -1,    12,    -1,    52,    30,    22,    44,    93,
      45,    -1,    52,    35,    22,    44,    93,    45,    -1,    52,
      34,    22,    44,    93,    45,    -1,    52,    31,    22,    44,
      93,    45,    -1,    92,    26,    92,    -1,    92,    27,    92,
      -1,    92,    28,    92,    -1,    29,    92,    -1,    94,    30,
      94,    -1,    94,    35,    94,    -1,    94,    34,    94,    -1,
      94,    31,    94,    -1,    94,    42,    94,    -1,    94,    32,
      94,    -1,    47,    92,    48,    -1,    92,    51,    35,    22,
      92,    -1,    92,    51,    44,    22,    46,    22,    45,    92,
      -1,    90,    -1,    20,    -1,    37,    94,    -1,    94,    36,
      94,    -1,    94,    37,    94,    -1,    94,    38,    94,    -1,
      47,    94,    48,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   342,   342,   342,   346,   347,   354,   354,   358,   358,
     364,   365,   368,   370,   372,   374,   376,   378,   380,   382,
     384,   386,   394,   395,   398,   398,   401,   405,   406,   409,
     410,   413,   416,   417,   420,   421,   425,   426,   430,   429,
     433,   434,   437,   438,   439,   446,   447,   450,   453,   454,
     457,   458,   459,   462,   466,   474,   475,   476,   477,   478,
     479,   481,   483,   485,   487,   489,   491,   498,   500,   502,
     504,   506,   513,   515,   517,   519,   521,   523,   525,   527,
     529,   533,   535,   537,   539,   541,   543,   545,   547,   551,
     553,   555,   557,   559,   563,   567,   569,   577,   580,   582,
     584,   586,   588,   590,   592,   596,   603,   604,   608,   609,
     610,   612,   614,   616,   618,   619,   620,   621,   622,   624,
     626,   628,   630,   632,   634,   638,   640,   645,   647,   649,
     651,   653,   655,   657
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "STOCHASTIC", "CTMC", "CONST_TOKEN",
  "INT_TOKEN", "DOUBLE", "RATE", "GLOBAL", "INIT", "TRUE_TOKEN",
  "FALSE_TOKEN", "EXP", "FUNC", "REWARDS", "ENDREWARDS", "MODULE",
  "ENDMODULE", "PNAME", "IDENTIFIER", "LABEL_NAME", "NUMBER", "ARROW",
  "DOTDOT", "ILLEGAL_TOKEN", "IMPLY", "'&'", "'|'", "'!'", "'<'", "'>'",
  "NEQ", "EQ", "GTE", "LTE", "'+'", "'-'", "'*'", "'/'", "UMINUS", "';'",
  "'='", "':'", "'['", "']'", "','", "'('", "')'", "'W'", "'L'", "'U'",
  "'P'", "$accept", "file", "$@1", "model_or_properties", "model", "$@2",
  "model_type", "declarations", "declaration", "modules", "module_decl",
  "$@3", "substitutions", "subst_list", "subst", "variables",
  "variable_decl", "commands", "command", "$@4", "synchronization",
  "update", "rewards", "rewards_decl", "rewards_label", "reward_rules",
  "state_reward", "transition_reward", "formula", "distribution", "expr",
  "rate_expr", "const_rate_expr", "function", "arguments", "range",
  "const_expr", "integer", "properties", "csl_formula", "path_formula",
  "csl_expr", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,    38,   124,    33,
      60,    62,   282,   283,   284,   285,    43,    45,    42,    47,
     286,    59,    61,    58,    91,    93,    44,    40,    41,    87,
      76,    85,    80
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    53,    55,    54,    56,    56,    58,    57,    59,    59,
      60,    60,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    62,    62,    64,    63,    63,    65,    65,    66,
      66,    67,    68,    68,    69,    69,    70,    70,    72,    71,
      73,    73,    74,    74,    74,    75,    75,    76,    77,    77,
      78,    78,    78,    79,    80,    81,    81,    81,    81,    81,
      81,    81,    81,    81,    81,    81,    81,    82,    82,    82,
      82,    82,    83,    83,    83,    83,    83,    83,    83,    83,
      83,    84,    84,    84,    84,    84,    84,    84,    84,    85,
      85,    85,    85,    85,    86,    87,    87,    88,    89,    89,
      89,    89,    89,    89,    89,    90,    91,    91,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    93,    93,    94,    94,    94,
      94,    94,    94,    94
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     0,     5,     1,     1,
       0,     2,     3,     5,     4,     6,     3,     5,     4,     6,
       5,     7,     0,     2,     0,     6,     8,     0,     1,     1,
       3,     3,     0,     2,     4,     6,     0,     2,     0,     8,
       2,     3,     3,     3,     3,     0,     2,     4,     0,     1,
       0,     2,     2,     4,     7,     1,     1,     3,     3,     2,
       3,     3,     3,     3,     3,     3,     3,     1,     4,     6,
       6,     6,     1,     1,     4,     6,     2,     3,     3,     3,
       3,     1,     1,     2,     3,     3,     3,     3,     3,     1,
       1,     3,     3,     3,     1,     1,     3,     5,     1,     1,
       2,     3,     3,     3,     3,     1,     0,     2,     1,     1,
       6,     6,     6,     6,     3,     3,     3,     2,     3,     3,
       3,     3,     3,     3,     3,     5,     8,     1,     1,     2,
       3,     3,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,   106,     1,     8,     9,     3,     4,     6,     5,
      10,   108,   109,   128,   105,     0,     0,     0,     0,   127,
     107,     0,    22,   117,     0,   129,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    11,    45,     0,
     124,   133,     0,     0,     0,     0,   114,   115,   116,   118,
     121,   123,   120,   119,   130,   131,   132,   122,     0,     0,
       0,     0,     0,     0,    23,     7,     0,     0,     0,     0,
       0,     0,    12,     0,    16,     0,     0,    24,    48,    46,
       0,     0,     0,     0,     0,    14,     0,    18,     0,    99,
       0,     0,     0,    98,    90,    89,     0,     0,     0,     0,
       0,    32,    49,    50,     0,   110,   113,   112,   111,     0,
       0,   100,     0,     0,     0,     0,    13,     0,     0,     0,
      17,     0,     0,    20,     0,    36,     0,     0,     0,    15,
      19,   104,   101,   102,   103,    93,    91,    92,     0,     0,
      27,     0,    33,     0,    55,    56,     0,    47,    73,     0,
       0,     0,     0,    51,    52,     0,     0,     0,    72,     0,
       0,     0,    21,     0,     0,    28,    29,     0,    25,     0,
      37,     0,     0,    59,     0,    76,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   125,     0,    97,     0,     0,     0,     0,     0,
      40,     0,    94,     0,     0,     0,    66,    80,    57,    58,
      82,    81,     0,     0,     0,    60,    63,    65,    62,    61,
      77,    78,    79,    64,    95,     0,     0,    31,    26,    30,
       0,    34,    41,     0,     0,     0,    83,     0,     0,     0,
       0,     0,    53,     0,    74,     0,     0,     0,     0,     0,
       0,     0,    67,     0,     0,    88,    84,    85,    86,    87,
      96,   126,    35,     0,     0,     0,     0,    38,    75,     0,
       0,     0,     0,     0,     0,    54,    68,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    39,
      69,    70,    71,    42,    44,    43
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     6,     7,    10,     8,    22,    47,    48,
      74,   111,   174,   175,   176,   135,   152,   153,   180,   284,
     181,   292,    75,    89,   113,   136,   163,   164,   165,   261,
     166,   224,   107,   167,   235,   109,   102,    19,     9,    90,
      91,    21
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -189
static const yytype_int16 yypact[] =
{
    -189,    15,    94,  -189,  -189,  -189,  -189,  -189,  -189,   137,
    -189,  -189,  -189,  -189,  -189,   137,   108,   137,   304,  -189,
     358,   250,   366,  -189,   108,  -189,   184,   226,     5,    25,
      27,    30,   137,   137,   137,   108,   108,   108,   108,   108,
     108,   108,   108,   108,    84,    44,    47,  -189,    55,   185,
    -189,  -189,    31,    43,    45,    48,   148,  -189,  -189,   351,
     351,   351,   351,   351,    82,    82,  -189,   351,   106,   114,
     309,   315,   140,   174,  -189,   182,   137,   137,   137,   137,
     321,   355,  -189,   109,  -189,    36,   156,   160,   183,  -189,
      96,   161,   163,   175,   190,  -189,   109,  -189,    36,  -189,
     109,   109,   311,  -189,  -189,  -189,    36,   237,   109,    -5,
     193,  -189,  -189,  -189,    51,  -189,  -189,  -189,  -189,   317,
     342,  -189,   189,   109,   109,   109,  -189,   134,    36,    36,
    -189,   127,   109,  -189,   186,   218,   187,   222,   257,  -189,
    -189,  -189,   221,   221,  -189,  -189,  -189,  -189,   109,   323,
     293,   274,  -189,   -14,  -189,  -189,   279,  -189,   283,    59,
      37,   325,    59,  -189,  -189,    66,   259,   322,  -189,   137,
     291,   286,  -189,   359,   337,   356,  -189,   156,  -189,   -17,
    -189,    59,   380,  -189,    37,  -189,   361,    73,   235,    59,
      59,   130,    37,    37,    37,    37,    37,    37,    37,    37,
      37,    37,   358,   381,  -189,   384,   387,   293,    -3,   362,
    -189,   345,  -189,   363,   262,    59,  -189,  -189,  -189,  -189,
    -189,  -189,   130,   130,   204,   354,   354,   354,   354,   354,
     370,   370,  -189,   354,   354,   -35,   365,  -189,  -189,  -189,
     109,  -189,  -189,   168,    37,   126,  -189,   266,   130,   130,
     130,   130,  -189,    37,  -189,   137,   329,   364,   367,   368,
     369,   374,   340,   122,   130,  -189,   360,   360,  -189,  -189,
     354,   358,  -189,   130,    36,    36,    36,  -189,  -189,   305,
     270,   281,   290,   294,     1,  -189,  -189,    36,    36,    36,
     371,     1,    41,   147,   273,   277,    37,   -11,     1,  -189,
    -189,  -189,  -189,   354,  -189,  -189
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -189,  -189,  -189,  -189,  -189,  -189,  -189,  -189,  -189,  -189,
    -189,  -189,  -189,  -189,   205,  -189,  -189,  -189,  -189,  -189,
    -189,  -166,  -189,  -189,  -189,  -189,  -189,  -189,  -150,  -189,
     -57,  -188,   -96,   236,   176,   242,   -79,   -82,  -189,    -9,
     316,   212
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -95
static const yytype_int16 yytable[] =
{
      20,   103,   120,   209,   178,   132,    23,   240,    26,   183,
     127,   253,   187,   254,   103,     3,   298,   119,   103,   103,
     290,   121,   122,    56,    57,    58,   103,    52,   210,   131,
     179,   211,   146,   147,   246,   247,   133,   304,   241,   218,
     219,   103,   103,   103,   142,   143,   144,    53,   291,    54,
     103,   156,    55,   149,   168,   262,   104,   158,   105,    14,
     266,   267,   268,   269,    71,   245,   103,    72,   298,   171,
     154,   155,    73,   156,   160,    76,   279,   168,   168,   158,
     168,    14,   299,   106,   184,   280,   137,    77,   159,    78,
      68,    69,    79,   189,   190,   138,   160,     4,     5,   168,
     189,   190,   168,   185,    70,   188,   162,   168,   168,   191,
     168,   168,   168,   168,   168,   168,   168,   168,   168,   168,
      42,   216,    32,    33,    34,   297,    80,   214,    13,    99,
      14,    14,   305,   168,    81,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,    16,   100,   114,    11,    12,
     220,   148,   221,   189,   190,    24,   101,    13,   103,    14,
     202,   256,   168,   123,   124,   125,    15,   222,   253,   264,
     278,   168,   128,   129,    16,    33,    34,   223,   281,   282,
     283,   257,   145,    86,    17,   128,   129,   234,   220,    18,
     221,   293,   294,   295,    87,   300,   270,    88,   154,   155,
     108,   156,   110,   157,   112,   222,   115,   158,   116,    14,
      32,    33,    34,   134,   168,   223,   159,   258,   259,   260,
     117,    40,    41,    42,   160,   123,   124,   125,    25,    27,
     150,   161,    50,    51,   162,   118,    49,   141,   151,   303,
     248,   249,   250,   251,   169,   252,   271,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    35,    36,    37,   125,
      38,    39,    40,    41,    42,   192,   193,   194,    43,   195,
     196,   197,   198,   199,    51,   128,   129,   200,   130,   170,
      35,    36,    37,   217,    38,    39,    40,    41,    42,   192,
     193,   194,    43,   195,   196,   197,   198,   199,   197,   198,
     199,   200,   248,   249,   250,   251,   248,   249,   250,   251,
     217,   128,   129,   173,   265,   128,   129,   177,   286,   128,
     129,   301,   123,   124,   125,   302,   182,   287,   128,   129,
     -94,   204,   128,   129,    28,    29,   288,   203,    30,    31,
     289,   248,   249,   250,   251,   186,   285,   123,   124,   125,
      82,    83,   126,   123,   124,   125,    84,    85,   139,   123,
     124,   125,    95,    96,   172,   123,   124,   125,   243,   201,
     272,    44,   189,   190,    45,    46,   248,   249,   250,   251,
     128,   129,   206,   140,    32,    33,    34,    40,    41,    42,
     197,   198,   199,    92,    93,    94,    97,    98,   250,   251,
     212,   205,   207,   236,   237,   238,   215,   242,   199,   244,
     255,   273,   239,   296,   274,   275,   276,   277,   213,   208,
     263
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-189))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_uint16 yycheck[] =
{
       9,    83,    98,    20,    18,    10,    15,    10,    17,   159,
     106,    46,   162,    48,    96,     0,    27,    96,   100,   101,
      19,   100,   101,    32,    33,    34,   108,    22,    45,   108,
      44,   181,   128,   129,   222,   223,    41,    48,    41,   189,
     190,   123,   124,   125,   123,   124,   125,    22,    47,    22,
     132,    14,    22,   132,   136,   243,    20,    20,    22,    22,
     248,   249,   250,   251,    20,   215,   148,    20,    27,   148,
      11,    12,    17,    14,    37,    44,   264,   159,   160,    20,
     162,    22,    41,    47,    47,   273,    35,    44,    29,    44,
       6,     7,    44,    27,    28,    44,    37,     3,     4,   181,
      27,    28,   184,   160,    20,   162,    47,   189,   190,    43,
     192,   193,   194,   195,   196,   197,   198,   199,   200,   201,
      38,    48,    26,    27,    28,   291,    20,   184,    20,    20,
      22,    22,   298,   215,    20,   192,   193,   194,   195,   196,
     197,   198,   199,   200,   201,    37,    37,    51,    11,    12,
      20,    24,    22,    27,    28,    47,    47,    20,   240,    22,
     169,   240,   244,    36,    37,    38,    29,    37,    46,    43,
      48,   253,    38,    39,    37,    27,    28,    47,   274,   275,
     276,    13,    48,    43,    47,    38,    39,   244,    20,    52,
      22,   287,   288,   289,    20,    48,   253,    15,    11,    12,
      44,    14,    42,    16,    21,    37,    45,    20,    45,    22,
      26,    27,    28,    20,   296,    47,    29,    49,    50,    51,
      45,    36,    37,    38,    37,    36,    37,    38,    16,    17,
      44,    44,    48,    48,    47,    45,    24,    48,    20,   296,
      36,    37,    38,    39,    22,    41,   255,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    30,    31,    32,    38,
      34,    35,    36,    37,    38,    30,    31,    32,    42,    34,
      35,    36,    37,    38,    48,    38,    39,    42,    41,    22,
      30,    31,    32,    48,    34,    35,    36,    37,    38,    30,
      31,    32,    42,    34,    35,    36,    37,    38,    36,    37,
      38,    42,    36,    37,    38,    39,    36,    37,    38,    39,
      48,    38,    39,    20,    48,    38,    39,    43,    48,    38,
      39,    48,    36,    37,    38,    48,    47,    46,    38,    39,
      47,    45,    38,    39,    30,    31,    46,    46,    34,    35,
      46,    36,    37,    38,    39,    20,    41,    36,    37,    38,
      41,    42,    41,    36,    37,    38,    41,    42,    41,    36,
      37,    38,    41,    42,    41,    36,    37,    38,    23,    47,
      41,     5,    27,    28,     8,     9,    36,    37,    38,    39,
      38,    39,    45,    41,    26,    27,    28,    36,    37,    38,
      36,    37,    38,    77,    78,    79,    41,    42,    38,    39,
      20,    42,    46,    22,    20,    18,    45,    45,    38,    46,
      45,    47,   207,    42,    47,    47,    47,    43,   182,   177,
     244
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    54,    55,     0,     3,     4,    56,    57,    59,    91,
      58,    11,    12,    20,    22,    29,    37,    47,    52,    90,
      92,    94,    60,    92,    47,    94,    92,    94,    30,    31,
      34,    35,    26,    27,    28,    30,    31,    32,    34,    35,
      36,    37,    38,    42,     5,     8,     9,    61,    62,    94,
      48,    48,    22,    22,    22,    22,    92,    92,    92,    94,
      94,    94,    94,    94,    94,    94,    94,    94,     6,     7,
      20,    20,    20,    17,    63,    75,    44,    44,    44,    44,
      20,    20,    41,    42,    41,    42,    43,    20,    15,    76,
      92,    93,    93,    93,    93,    41,    42,    41,    42,    20,
      37,    47,    89,    90,    20,    22,    47,    85,    44,    88,
      42,    64,    21,    77,    51,    45,    45,    45,    45,    89,
      85,    89,    89,    36,    37,    38,    41,    85,    38,    39,
      41,    89,    10,    41,    20,    68,    78,    35,    44,    41,
      41,    48,    89,    89,    89,    48,    85,    85,    24,    89,
      44,    20,    69,    70,    11,    12,    14,    16,    20,    29,
      37,    44,    47,    79,    80,    81,    83,    86,    90,    22,
      22,    89,    41,    20,    65,    66,    67,    43,    18,    44,
      71,    73,    47,    81,    47,    83,    20,    81,    83,    27,
      28,    43,    30,    31,    32,    34,    35,    36,    37,    38,
      42,    47,    92,    46,    45,    42,    45,    46,    88,    20,
      45,    81,    20,    86,    83,    45,    48,    48,    81,    81,
      20,    22,    37,    47,    84,    83,    83,    83,    83,    83,
      83,    83,    83,    83,    83,    87,    22,    20,    18,    67,
      10,    41,    45,    23,    46,    81,    84,    84,    36,    37,
      38,    39,    41,    46,    48,    45,    89,    13,    49,    50,
      51,    82,    84,    87,    43,    48,    84,    84,    84,    84,
      83,    92,    41,    47,    47,    47,    47,    43,    48,    84,
      84,    85,    85,    85,    72,    41,    48,    46,    46,    46,
      19,    47,    74,    85,    85,    85,    42,    74,    27,    41,
      48,    48,    48,    83,    48,    74
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
      yyerror (YY_("syntax error: cannot back up")); \
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


/* This macro is provided for backward compatibility. */

#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
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
		  Type, Value); \
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
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
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
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
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {
      case 87: /* "arguments" */

/* Line 1391 of yacc.c  */
#line 338 "parser.yy"
	{ delete (yyvaluep->arguments); };

/* Line 1391 of yacc.c  */
#line 1689 "parser.cc"
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
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

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
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

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

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
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

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

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
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

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


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1806 of yacc.c  */
#line 342 "parser.yy"
    { success = true; line_number = 1; }
    break;

  case 3:

/* Line 1806 of yacc.c  */
#line 343 "parser.yy"
    { check_undeclared(); if (!success) YYERROR; }
    break;

  case 6:

/* Line 1806 of yacc.c  */
#line 354 "parser.yy"
    { prepare_model(); }
    break;

  case 7:

/* Line 1806 of yacc.c  */
#line 355 "parser.yy"
    { compile_model(); }
    break;

  case 12:

/* Line 1806 of yacc.c  */
#line 369 "parser.yy"
    { declare_constant((yyvsp[(2) - (3)].str), nullptr); }
    break;

  case 13:

/* Line 1806 of yacc.c  */
#line 371 "parser.yy"
    { declare_constant((yyvsp[(2) - (5)].str), (yyvsp[(4) - (5)].expr)); }
    break;

  case 14:

/* Line 1806 of yacc.c  */
#line 373 "parser.yy"
    { declare_constant((yyvsp[(3) - (4)].str), nullptr); }
    break;

  case 15:

/* Line 1806 of yacc.c  */
#line 375 "parser.yy"
    { declare_constant((yyvsp[(3) - (6)].str), (yyvsp[(5) - (6)].expr)); }
    break;

  case 16:

/* Line 1806 of yacc.c  */
#line 377 "parser.yy"
    { declare_rate((yyvsp[(2) - (3)].str), nullptr); }
    break;

  case 17:

/* Line 1806 of yacc.c  */
#line 379 "parser.yy"
    { declare_rate((yyvsp[(2) - (5)].str), (yyvsp[(4) - (5)].expr)); }
    break;

  case 18:

/* Line 1806 of yacc.c  */
#line 381 "parser.yy"
    { declare_rate((yyvsp[(3) - (4)].str), nullptr); }
    break;

  case 19:

/* Line 1806 of yacc.c  */
#line 383 "parser.yy"
    { declare_rate((yyvsp[(3) - (6)].str), (yyvsp[(5) - (6)].expr)); }
    break;

  case 20:

/* Line 1806 of yacc.c  */
#line 385 "parser.yy"
    { declare_variable((yyvsp[(2) - (5)].str), (yyvsp[(4) - (5)].range), nullptr); }
    break;

  case 21:

/* Line 1806 of yacc.c  */
#line 387 "parser.yy"
    { declare_variable((yyvsp[(2) - (7)].str), (yyvsp[(4) - (7)].range), (yyvsp[(6) - (7)].expr)); }
    break;

  case 24:

/* Line 1806 of yacc.c  */
#line 398 "parser.yy"
    { prepare_module((yyvsp[(2) - (2)].str)); }
    break;

  case 25:

/* Line 1806 of yacc.c  */
#line 400 "parser.yy"
    { add_module(); }
    break;

  case 26:

/* Line 1806 of yacc.c  */
#line 402 "parser.yy"
    { add_module((yyvsp[(2) - (8)].str), (yyvsp[(4) - (8)].str)); }
    break;

  case 31:

/* Line 1806 of yacc.c  */
#line 413 "parser.yy"
    { add_substitution((yyvsp[(1) - (3)].str), (yyvsp[(3) - (3)].str)); }
    break;

  case 34:

/* Line 1806 of yacc.c  */
#line 420 "parser.yy"
    { declare_variable((yyvsp[(1) - (4)].str), (yyvsp[(3) - (4)].range), nullptr); }
    break;

  case 35:

/* Line 1806 of yacc.c  */
#line 422 "parser.yy"
    { declare_variable((yyvsp[(1) - (6)].str), (yyvsp[(3) - (6)].range), (yyvsp[(5) - (6)].expr)); }
    break;

  case 38:

/* Line 1806 of yacc.c  */
#line 430 "parser.yy"
    { prepare_command((yyvsp[(1) - (5)].synch), (yyvsp[(2) - (5)].formula), (yyvsp[(4) - (5)].dist)); }
    break;

  case 39:

/* Line 1806 of yacc.c  */
#line 430 "parser.yy"
    { add_command(); }
    break;

  case 40:

/* Line 1806 of yacc.c  */
#line 433 "parser.yy"
    { (yyval.synch) = 0; }
    break;

  case 41:

/* Line 1806 of yacc.c  */
#line 434 "parser.yy"
    { (yyval.synch) = synchronization_value((yyvsp[(2) - (3)].str)); }
    break;

  case 42:

/* Line 1806 of yacc.c  */
#line 437 "parser.yy"
    { add_update((yyvsp[(1) - (3)].str), (yyvsp[(3) - (3)].expr)); }
    break;

  case 53:

/* Line 1806 of yacc.c  */
#line 463 "parser.yy"
    { delete (yyvsp[(1) - (4)].formula); delete (yyvsp[(3) - (4)].expr); }
    break;

  case 54:

/* Line 1806 of yacc.c  */
#line 467 "parser.yy"
    { delete (yyvsp[(2) - (7)].str); delete (yyvsp[(4) - (7)].formula); delete (yyvsp[(6) - (7)].expr); }
    break;

  case 55:

/* Line 1806 of yacc.c  */
#line 474 "parser.yy"
    { (yyval.formula) = new Conjunction(); }
    break;

  case 56:

/* Line 1806 of yacc.c  */
#line 475 "parser.yy"
    { (yyval.formula) = new Disjunction(); }
    break;

  case 57:

/* Line 1806 of yacc.c  */
#line 476 "parser.yy"
    { (yyval.formula) = make_conjunction((yyvsp[(1) - (3)].formula), (yyvsp[(3) - (3)].formula)); }
    break;

  case 58:

/* Line 1806 of yacc.c  */
#line 477 "parser.yy"
    { (yyval.formula) = make_disjunction((yyvsp[(1) - (3)].formula), (yyvsp[(3) - (3)].formula)); }
    break;

  case 59:

/* Line 1806 of yacc.c  */
#line 478 "parser.yy"
    { (yyval.formula) = new Negation((yyvsp[(2) - (2)].formula)); }
    break;

  case 60:

/* Line 1806 of yacc.c  */
#line 480 "parser.yy"
    { (yyval.formula) = NewLessThan(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 61:

/* Line 1806 of yacc.c  */
#line 482 "parser.yy"
    { (yyval.formula) = NewLessThanOrEqual(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 62:

/* Line 1806 of yacc.c  */
#line 484 "parser.yy"
    { (yyval.formula) = NewGreaterThanOrEqual(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 63:

/* Line 1806 of yacc.c  */
#line 486 "parser.yy"
    { (yyval.formula) = NewGreaterThan(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 64:

/* Line 1806 of yacc.c  */
#line 488 "parser.yy"
    { (yyval.formula) = NewEquality(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 65:

/* Line 1806 of yacc.c  */
#line 490 "parser.yy"
    { (yyval.formula) = NewInequality(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 66:

/* Line 1806 of yacc.c  */
#line 491 "parser.yy"
    { (yyval.formula) = (yyvsp[(2) - (3)].formula); }
    break;

  case 67:

/* Line 1806 of yacc.c  */
#line 499 "parser.yy"
    { (yyval.dist) = NewExponential(MakeUnique((yyvsp[(1) - (1)].expr))); }
    break;

  case 68:

/* Line 1806 of yacc.c  */
#line 501 "parser.yy"
    { (yyval.dist) = NewExponential(MakeUnique((yyvsp[(3) - (4)].expr))); }
    break;

  case 69:

/* Line 1806 of yacc.c  */
#line 503 "parser.yy"
    { (yyval.dist) = NewWeibull(MakeUnique((yyvsp[(3) - (6)].expr)), MakeUnique((yyvsp[(5) - (6)].expr))); }
    break;

  case 70:

/* Line 1806 of yacc.c  */
#line 505 "parser.yy"
    { (yyval.dist) = NewLognormal(MakeUnique((yyvsp[(3) - (6)].expr)), MakeUnique((yyvsp[(5) - (6)].expr))); }
    break;

  case 71:

/* Line 1806 of yacc.c  */
#line 507 "parser.yy"
    { (yyval.dist) = NewUniform(MakeUnique((yyvsp[(3) - (6)].expr)), MakeUnique((yyvsp[(5) - (6)].expr))); }
    break;

  case 72:

/* Line 1806 of yacc.c  */
#line 514 "parser.yy"
    { (yyval.expr) = make_literal((yyvsp[(1) - (1)].nat)); }
    break;

  case 73:

/* Line 1806 of yacc.c  */
#line 516 "parser.yy"
    { (yyval.expr) = find_variable((yyvsp[(1) - (1)].str)); }
    break;

  case 74:

/* Line 1806 of yacc.c  */
#line 518 "parser.yy"
    { (yyval.expr) = NewFunctionCall((yyvsp[(1) - (4)].function), MakeUnique((yyvsp[(3) - (4)].arguments))); }
    break;

  case 75:

/* Line 1806 of yacc.c  */
#line 520 "parser.yy"
    { (yyval.expr) = NewFunctionCall((yyvsp[(3) - (6)].function), MakeUnique((yyvsp[(5) - (6)].arguments))); }
    break;

  case 76:

/* Line 1806 of yacc.c  */
#line 522 "parser.yy"
    { (yyval.expr) = NewNegate(MakeUnique((yyvsp[(2) - (2)].expr))); }
    break;

  case 77:

/* Line 1806 of yacc.c  */
#line 524 "parser.yy"
    { (yyval.expr) = NewPlus(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 78:

/* Line 1806 of yacc.c  */
#line 526 "parser.yy"
    { (yyval.expr) = NewMinus(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 79:

/* Line 1806 of yacc.c  */
#line 528 "parser.yy"
    { (yyval.expr) = NewMultiply(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 80:

/* Line 1806 of yacc.c  */
#line 530 "parser.yy"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;

  case 81:

/* Line 1806 of yacc.c  */
#line 534 "parser.yy"
    { (yyval.expr) = make_literal((yyvsp[(1) - (1)].num)); }
    break;

  case 82:

/* Line 1806 of yacc.c  */
#line 536 "parser.yy"
    { (yyval.expr) = find_rate_or_variable((yyvsp[(1) - (1)].str)); }
    break;

  case 83:

/* Line 1806 of yacc.c  */
#line 538 "parser.yy"
    { (yyval.expr) = NewNegate(MakeUnique((yyvsp[(2) - (2)].expr))); }
    break;

  case 84:

/* Line 1806 of yacc.c  */
#line 540 "parser.yy"
    { (yyval.expr) = NewPlus(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 85:

/* Line 1806 of yacc.c  */
#line 542 "parser.yy"
    { (yyval.expr) = NewMinus(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 86:

/* Line 1806 of yacc.c  */
#line 544 "parser.yy"
    { (yyval.expr) = NewMultiply(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 87:

/* Line 1806 of yacc.c  */
#line 546 "parser.yy"
    { (yyval.expr) = NewDivide(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 88:

/* Line 1806 of yacc.c  */
#line 548 "parser.yy"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;

  case 89:

/* Line 1806 of yacc.c  */
#line 552 "parser.yy"
    { (yyval.expr) = make_literal((yyvsp[(1) - (1)].num)); }
    break;

  case 90:

/* Line 1806 of yacc.c  */
#line 554 "parser.yy"
    { (yyval.expr) = find_rate((yyvsp[(1) - (1)].str)); }
    break;

  case 91:

/* Line 1806 of yacc.c  */
#line 556 "parser.yy"
    { (yyval.expr) = NewMultiply(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 92:

/* Line 1806 of yacc.c  */
#line 558 "parser.yy"
    { (yyval.expr) = NewDivide(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 93:

/* Line 1806 of yacc.c  */
#line 560 "parser.yy"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;

  case 94:

/* Line 1806 of yacc.c  */
#line 564 "parser.yy"
    { (yyval.function) = MakeFunction(MakeUnique((yyvsp[(1) - (1)].str))); }
    break;

  case 95:

/* Line 1806 of yacc.c  */
#line 568 "parser.yy"
    { (yyval.arguments) = new UniquePtrVector<const Expression>(MakeUnique((yyvsp[(1) - (1)].expr))); }
    break;

  case 96:

/* Line 1806 of yacc.c  */
#line 570 "parser.yy"
    { (yyval.arguments) = (yyvsp[(1) - (3)].arguments); (yyval.arguments)->push_back(MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 97:

/* Line 1806 of yacc.c  */
#line 577 "parser.yy"
    { (yyval.range) = make_range((yyvsp[(2) - (5)].expr), (yyvsp[(4) - (5)].expr)); }
    break;

  case 98:

/* Line 1806 of yacc.c  */
#line 581 "parser.yy"
    { (yyval.expr) = make_literal((yyvsp[(1) - (1)].nat)); }
    break;

  case 99:

/* Line 1806 of yacc.c  */
#line 583 "parser.yy"
    { (yyval.expr) = find_constant((yyvsp[(1) - (1)].str)); }
    break;

  case 100:

/* Line 1806 of yacc.c  */
#line 585 "parser.yy"
    { (yyval.expr) = NewNegate(MakeUnique((yyvsp[(2) - (2)].expr))); }
    break;

  case 101:

/* Line 1806 of yacc.c  */
#line 587 "parser.yy"
    { (yyval.expr) = NewPlus(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 102:

/* Line 1806 of yacc.c  */
#line 589 "parser.yy"
    { (yyval.expr) = NewMinus(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 103:

/* Line 1806 of yacc.c  */
#line 591 "parser.yy"
    { (yyval.expr) = NewMultiply(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 104:

/* Line 1806 of yacc.c  */
#line 593 "parser.yy"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;

  case 105:

/* Line 1806 of yacc.c  */
#line 596 "parser.yy"
    { (yyval.nat) = integer_value((yyvsp[(1) - (1)].num)); }
    break;

  case 107:

/* Line 1806 of yacc.c  */
#line 605 "parser.yy"
    { properties.push_back((yyvsp[(2) - (2)].formula)); }
    break;

  case 108:

/* Line 1806 of yacc.c  */
#line 608 "parser.yy"
    { (yyval.formula) = new Conjunction(); }
    break;

  case 109:

/* Line 1806 of yacc.c  */
#line 609 "parser.yy"
    { (yyval.formula) = new Disjunction(); }
    break;

  case 110:

/* Line 1806 of yacc.c  */
#line 611 "parser.yy"
    { (yyval.formula) = make_probabilistic((yyvsp[(3) - (6)].num), true, true, (yyvsp[(5) - (6)].path)); }
    break;

  case 111:

/* Line 1806 of yacc.c  */
#line 613 "parser.yy"
    { (yyval.formula) = make_probabilistic((yyvsp[(3) - (6)].num), false, true, (yyvsp[(5) - (6)].path)); }
    break;

  case 112:

/* Line 1806 of yacc.c  */
#line 615 "parser.yy"
    { (yyval.formula) = make_probabilistic((yyvsp[(3) - (6)].num), false, false, (yyvsp[(5) - (6)].path)); }
    break;

  case 113:

/* Line 1806 of yacc.c  */
#line 617 "parser.yy"
    { (yyval.formula) = make_probabilistic((yyvsp[(3) - (6)].num), true, false, (yyvsp[(5) - (6)].path)); }
    break;

  case 114:

/* Line 1806 of yacc.c  */
#line 618 "parser.yy"
    { (yyval.formula) = new Implication((yyvsp[(1) - (3)].formula), (yyvsp[(3) - (3)].formula)); }
    break;

  case 115:

/* Line 1806 of yacc.c  */
#line 619 "parser.yy"
    { (yyval.formula) = make_conjunction((yyvsp[(1) - (3)].formula), (yyvsp[(3) - (3)].formula)); }
    break;

  case 116:

/* Line 1806 of yacc.c  */
#line 620 "parser.yy"
    { (yyval.formula) = make_disjunction((yyvsp[(1) - (3)].formula), (yyvsp[(3) - (3)].formula)); }
    break;

  case 117:

/* Line 1806 of yacc.c  */
#line 621 "parser.yy"
    { (yyval.formula) = new Negation((yyvsp[(2) - (2)].formula)); }
    break;

  case 118:

/* Line 1806 of yacc.c  */
#line 623 "parser.yy"
    { (yyval.formula) = NewLessThan(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 119:

/* Line 1806 of yacc.c  */
#line 625 "parser.yy"
    { (yyval.formula) = NewLessThanOrEqual(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 120:

/* Line 1806 of yacc.c  */
#line 627 "parser.yy"
    { (yyval.formula) = NewGreaterThanOrEqual(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 121:

/* Line 1806 of yacc.c  */
#line 629 "parser.yy"
    { (yyval.formula) = NewGreaterThan(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 122:

/* Line 1806 of yacc.c  */
#line 631 "parser.yy"
    { (yyval.formula) = NewEquality(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 123:

/* Line 1806 of yacc.c  */
#line 633 "parser.yy"
    { (yyval.formula) = NewInequality(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 124:

/* Line 1806 of yacc.c  */
#line 635 "parser.yy"
    { (yyval.formula) = (yyvsp[(2) - (3)].formula); }
    break;

  case 125:

/* Line 1806 of yacc.c  */
#line 639 "parser.yy"
    { (yyval.path) = make_until((yyvsp[(1) - (5)].formula), (yyvsp[(5) - (5)].formula), nullptr, (yyvsp[(4) - (5)].num)); }
    break;

  case 126:

/* Line 1806 of yacc.c  */
#line 641 "parser.yy"
    { (yyval.path) = make_until((yyvsp[(1) - (8)].formula), (yyvsp[(8) - (8)].formula), (yyvsp[(4) - (8)].num), (yyvsp[(6) - (8)].num)); }
    break;

  case 127:

/* Line 1806 of yacc.c  */
#line 646 "parser.yy"
    { (yyval.expr) = make_literal((yyvsp[(1) - (1)].nat)); }
    break;

  case 128:

/* Line 1806 of yacc.c  */
#line 648 "parser.yy"
    { (yyval.expr) = value_or_variable((yyvsp[(1) - (1)].str)); }
    break;

  case 129:

/* Line 1806 of yacc.c  */
#line 650 "parser.yy"
    { (yyval.expr) = NewNegate(MakeUnique((yyvsp[(2) - (2)].expr))); }
    break;

  case 130:

/* Line 1806 of yacc.c  */
#line 652 "parser.yy"
    { (yyval.expr) = NewPlus(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 131:

/* Line 1806 of yacc.c  */
#line 654 "parser.yy"
    { (yyval.expr) = NewMinus(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 132:

/* Line 1806 of yacc.c  */
#line 656 "parser.yy"
    { (yyval.expr) = NewMultiply(MakeUnique((yyvsp[(1) - (3)].expr)), MakeUnique((yyvsp[(3) - (3)].expr))); }
    break;

  case 133:

/* Line 1806 of yacc.c  */
#line 658 "parser.yy"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;



/* Line 1806 of yacc.c  */
#line 2727 "parser.cc"
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
      yyerror (YY_("syntax error"));
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
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



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
		      yytoken, &yylval);
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


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


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
  yyerror (YY_("memory exhausted"));
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
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
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
#line 661 "parser.yy"


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

static void yyerror(const std::string& s) {
  std::cerr << PACKAGE ":" << current_file << ':' << line_number << ": " << s
	    << std::endl;
  success = false;
}

static void yywarning(const std::string& s) {
  std::cerr << PACKAGE ":" << current_file << ':' << line_number << ": " << s
	    << std::endl;
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

static Range make_range(const Expression* l, const Expression* h) {
  Range r = { l, h };
  return r;
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
                             const Range& range,
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
    int low =
        EvaluateConstantExpression(*range.l, constant_values).value<int>();
    int high =
        EvaluateConstantExpression(*range.h, constant_values).value<int>();
    int s = ((start != nullptr)
             ? EvaluateConstantExpression(*start, constant_values).value<int>()
             : low);
    variable_lows.insert(
        std::make_pair(*ident, std::unique_ptr<const Expression>(range.l)));
    variable_highs.insert(
        std::make_pair(*ident, std::unique_ptr<const Expression>(range.h)));
    variable_starts.insert(
        std::make_pair(*ident, std::unique_ptr<const Expression>(start)));
    model->AddIntVariable(*ident, low, high, s);
    if (!delayed_addition) {
      if (module != nullptr) {
	module->add_variable(*ident);
      }
    }
  } else {
    delete range.l;
    delete range.h;
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
static void prepare_command(int synch, const StateFormula* guard,
			    const Distribution* delay) {
  command = new Command(synch, guard, delay);
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
	  Range r = { low.release(), high.release() };
	  if (declare_variable(new std::string(si->second), r, start.release(),
                               true)) {
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


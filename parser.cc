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

const UnaryOperation* NewUnaryOperation(UnaryOperator op,
                                        const Expression* operand) {
  return UnaryOperation::New(op, std::unique_ptr<const Expression>(operand))
      .release();
}

const BinaryOperation* NewBinaryOperation(BinaryOperator op,
                                          const Expression* operand1,
                                          const Expression* operand2) {
  return BinaryOperation::New(op,
                              std::unique_ptr<const Expression>(operand1),
                              std::unique_ptr<const Expression>(operand2))
      .release();
}

LessThan* NewLessThan(const Expression* expr1, const Expression* expr2) {
  return new LessThan(std::unique_ptr<const Expression>(expr1),
                      std::unique_ptr<const Expression>(expr2));
}

LessThanOrEqual* NewLessThanOrEqual(const Expression* expr1,
                                    const Expression* expr2) {
  return new LessThanOrEqual(std::unique_ptr<const Expression>(expr1),
                             std::unique_ptr<const Expression>(expr2));
}

GreaterThanOrEqual* NewGreaterThanOrEqual(const Expression* expr1,
                                          const Expression* expr2) {
  return new GreaterThanOrEqual(std::unique_ptr<const Expression>(expr1),
                                std::unique_ptr<const Expression>(expr2));
}

GreaterThan* NewGreaterThan(const Expression* expr1, const Expression* expr2) {
  return new GreaterThan(std::unique_ptr<const Expression>(expr1),
                         std::unique_ptr<const Expression>(expr2));
}

Equality* NewEquality(const Expression* expr1, const Expression* expr2) {
  return new Equality(std::unique_ptr<const Expression>(expr1),
                      std::unique_ptr<const Expression>(expr2));
}

Inequality* NewInequality(const Expression* expr1, const Expression* expr2) {
  return new Inequality(std::unique_ptr<const Expression>(expr1),
                        std::unique_ptr<const Expression>(expr2));
}

const Exponential* NewExponential(const Expression* rate) {
  return Exponential::make(std::unique_ptr<const Expression>(rate));
}

const Distribution* NewWeibull(const Expression* scale,
                               const Expression* shape) {
  return Weibull::make(std::unique_ptr<const Expression>(scale),
                       std::unique_ptr<const Expression>(shape));
}

const Lognormal* NewLognormal(const Expression* scale,
                               const Expression* shape) {
  return Lognormal::make(std::unique_ptr<const Expression>(scale),
                         std::unique_ptr<const Expression>(shape));
}

const Uniform* NewUniform(const Expression* low, const Expression* high) {
  return Uniform::make(std::unique_ptr<const Expression>(low),
                       std::unique_ptr<const Expression>(high));
}

}  // namespace



/* Line 268 of yacc.c  */
#line 294 "parser.cc"

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
     REWARDS = 269,
     ENDREWARDS = 270,
     MODULE = 271,
     ENDMODULE = 272,
     PNAME = 273,
     NAME = 274,
     LABEL_NAME = 275,
     NUMBER = 276,
     ARROW = 277,
     DOTDOT = 278,
     ILLEGAL_TOKEN = 279,
     IMPLY = 280,
     NEQ = 281,
     EQ = 282,
     GTE = 283,
     LTE = 284,
     UMINUS = 285
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
#define REWARDS 269
#define ENDREWARDS 270
#define MODULE 271
#define ENDMODULE 272
#define PNAME 273
#define NAME 274
#define LABEL_NAME 275
#define NUMBER 276
#define ARROW 277
#define DOTDOT 278
#define ILLEGAL_TOKEN 279
#define IMPLY 280
#define NEQ 281
#define EQ 282
#define GTE 283
#define LTE 284
#define UMINUS 285




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 293 of yacc.c  */
#line 264 "parser.yy"

  size_t synch;
  StateFormula* formula;
  const PathFormula* path;
  const Distribution* dist;
  const Expression* expr;
  Range range;
  int nat;
  const std::string* str;
  const TypedValue* num;



/* Line 293 of yacc.c  */
#line 404 "parser.cc"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 343 of yacc.c  */
#line 416 "parser.cc"

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
#define YYLAST   401

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  52
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  40
/* YYNRULES -- Number of rules.  */
#define YYNRULES  128
/* YYNRULES -- Number of states.  */
#define YYNSTATES  292

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   285

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    28,     2,     2,     2,     2,    26,     2,
      46,    47,    37,    35,    45,    36,     2,    38,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    42,    40,
      29,    41,    30,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,    49,     2,     2,     2,
      51,     2,     2,     2,     2,    50,     2,    48,     2,     2,
       2,    43,     2,    44,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    27,     2,     2,     2,     2,     2,
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
      25,    31,    32,    33,    34,    39
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
     256,   263,   270,   272,   274,   277,   281,   285,   289,   293,
     295,   297,   300,   304,   308,   312,   316,   320,   322,   324,
     328,   332,   336,   342,   344,   346,   349,   353,   357,   361,
     365,   367,   368,   371,   373,   375,   382,   389,   396,   403,
     407,   411,   415,   418,   422,   426,   430,   434,   438,   442,
     446,   452,   461,   463,   465,   468,   472,   476,   480
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      53,     0,    -1,    -1,    54,    55,    -1,    56,    -1,    88,
      -1,    -1,    58,    57,    59,    61,    74,    -1,     3,    -1,
       4,    -1,    -1,    59,    60,    -1,     5,    19,    40,    -1,
       5,    19,    41,    86,    40,    -1,     5,     6,    19,    40,
      -1,     5,     6,    19,    41,    86,    40,    -1,     8,    19,
      40,    -1,     8,    19,    41,    84,    40,    -1,     5,     7,
      19,    40,    -1,     5,     7,    19,    41,    84,    40,    -1,
       9,    19,    42,    85,    40,    -1,     9,    19,    42,    85,
      10,    86,    40,    -1,    -1,    61,    62,    -1,    -1,    16,
      19,    63,    67,    69,    17,    -1,    16,    19,    41,    19,
      43,    64,    44,    17,    -1,    -1,    65,    -1,    66,    -1,
      65,    45,    66,    -1,    19,    41,    19,    -1,    -1,    67,
      68,    -1,    19,    42,    85,    40,    -1,    19,    42,    85,
      10,    86,    40,    -1,    -1,    69,    70,    -1,    -1,    72,
      80,    22,    81,    42,    71,    73,    40,    -1,    43,    44,
      -1,    43,    19,    44,    -1,    18,    41,    82,    -1,    73,
      26,    73,    -1,    46,    73,    47,    -1,    -1,    74,    75,
      -1,    14,    76,    77,    15,    -1,    -1,    20,    -1,    -1,
      77,    78,    -1,    77,    79,    -1,    80,    42,    83,    40,
      -1,    43,    19,    44,    80,    42,    83,    40,    -1,    11,
      -1,    12,    -1,    80,    26,    80,    -1,    80,    27,    80,
      -1,    28,    80,    -1,    82,    29,    82,    -1,    82,    34,
      82,    -1,    82,    33,    82,    -1,    82,    30,    82,    -1,
      82,    41,    82,    -1,    82,    31,    82,    -1,    46,    80,
      47,    -1,    83,    -1,    13,    46,    83,    47,    -1,    48,
      46,    84,    45,    84,    47,    -1,    49,    46,    84,    45,
      84,    47,    -1,    50,    46,    84,    45,    84,    47,    -1,
      87,    -1,    19,    -1,    36,    82,    -1,    82,    35,    82,
      -1,    82,    36,    82,    -1,    82,    37,    82,    -1,    46,
      82,    47,    -1,    21,    -1,    19,    -1,    36,    83,    -1,
      83,    35,    83,    -1,    83,    36,    83,    -1,    83,    37,
      83,    -1,    83,    38,    83,    -1,    46,    83,    47,    -1,
      21,    -1,    19,    -1,    84,    37,    84,    -1,    84,    38,
      84,    -1,    46,    84,    47,    -1,    43,    86,    23,    86,
      44,    -1,    87,    -1,    19,    -1,    36,    86,    -1,    86,
      35,    86,    -1,    86,    36,    86,    -1,    86,    37,    86,
      -1,    46,    86,    47,    -1,    21,    -1,    -1,    88,    89,
      -1,    11,    -1,    12,    -1,    51,    29,    21,    43,    90,
      44,    -1,    51,    34,    21,    43,    90,    44,    -1,    51,
      33,    21,    43,    90,    44,    -1,    51,    30,    21,    43,
      90,    44,    -1,    89,    25,    89,    -1,    89,    26,    89,
      -1,    89,    27,    89,    -1,    28,    89,    -1,    91,    29,
      91,    -1,    91,    34,    91,    -1,    91,    33,    91,    -1,
      91,    30,    91,    -1,    91,    41,    91,    -1,    91,    31,
      91,    -1,    46,    89,    47,    -1,    89,    50,    34,    21,
      89,    -1,    89,    50,    43,    21,    45,    21,    44,    89,
      -1,    87,    -1,    19,    -1,    36,    91,    -1,    91,    35,
      91,    -1,    91,    36,    91,    -1,    91,    37,    91,    -1,
      46,    91,    47,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   288,   288,   288,   292,   293,   300,   300,   304,   304,
     310,   311,   314,   315,   316,   317,   319,   320,   321,   322,
     324,   325,   333,   334,   337,   337,   339,   343,   344,   347,
     348,   351,   354,   355,   358,   359,   363,   364,   368,   367,
     371,   372,   375,   376,   377,   384,   385,   388,   391,   392,
     395,   396,   397,   400,   404,   412,   413,   414,   415,   416,
     417,   418,   419,   420,   421,   422,   423,   430,   431,   432,
     434,   436,   443,   444,   445,   447,   449,   451,   453,   456,
     457,   458,   460,   462,   464,   466,   468,   471,   472,   473,
     475,   477,   484,   487,   488,   489,   491,   493,   495,   497,
     500,   507,   508,   512,   513,   514,   516,   518,   520,   522,
     523,   524,   525,   526,   527,   528,   529,   530,   531,   532,
     535,   537,   542,   543,   544,   546,   548,   550,   552
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "STOCHASTIC", "CTMC", "CONST_TOKEN",
  "INT_TOKEN", "DOUBLE", "RATE", "GLOBAL", "INIT", "TRUE_TOKEN",
  "FALSE_TOKEN", "EXP", "REWARDS", "ENDREWARDS", "MODULE", "ENDMODULE",
  "PNAME", "NAME", "LABEL_NAME", "NUMBER", "ARROW", "DOTDOT",
  "ILLEGAL_TOKEN", "IMPLY", "'&'", "'|'", "'!'", "'<'", "'>'", "NEQ", "EQ",
  "GTE", "LTE", "'+'", "'-'", "'*'", "'/'", "UMINUS", "';'", "'='", "':'",
  "'['", "']'", "','", "'('", "')'", "'W'", "'L'", "'U'", "'P'", "$accept",
  "file", "$@1", "model_or_properties", "model", "$@2", "model_type",
  "declarations", "declaration", "modules", "module_decl", "$@3",
  "substitutions", "subst_list", "subst", "variables", "variable_decl",
  "commands", "command", "$@4", "synchronization", "update", "rewards",
  "rewards_decl", "rewards_label", "reward_rules", "state_reward",
  "transition_reward", "formula", "distribution", "expr", "rate_expr",
  "const_rate_expr", "range", "const_expr", "integer", "properties",
  "csl_formula", "path_formula", "csl_expr", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,    38,   124,    33,    60,
      62,   281,   282,   283,   284,    43,    45,    42,    47,   285,
      59,    61,    58,    91,    93,    44,    40,    41,    87,    76,
      85,    80
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    52,    54,    53,    55,    55,    57,    56,    58,    58,
      59,    59,    60,    60,    60,    60,    60,    60,    60,    60,
      60,    60,    61,    61,    63,    62,    62,    64,    64,    65,
      65,    66,    67,    67,    68,    68,    69,    69,    71,    70,
      72,    72,    73,    73,    73,    74,    74,    75,    76,    76,
      77,    77,    77,    78,    79,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    81,    81,    81,
      81,    81,    82,    82,    82,    82,    82,    82,    82,    83,
      83,    83,    83,    83,    83,    83,    83,    84,    84,    84,
      84,    84,    85,    86,    86,    86,    86,    86,    86,    86,
      87,    88,    88,    89,    89,    89,    89,    89,    89,    89,
      89,    89,    89,    89,    89,    89,    89,    89,    89,    89,
      90,    90,    91,    91,    91,    91,    91,    91,    91
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
       6,     6,     1,     1,     2,     3,     3,     3,     3,     1,
       1,     2,     3,     3,     3,     3,     3,     1,     1,     3,
       3,     3,     5,     1,     1,     2,     3,     3,     3,     3,
       1,     0,     2,     1,     1,     6,     6,     6,     6,     3,
       3,     3,     2,     3,     3,     3,     3,     3,     3,     3,
       5,     8,     1,     1,     2,     3,     3,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,   101,     1,     8,     9,     3,     4,     6,     5,
      10,   103,   104,   123,   100,     0,     0,     0,     0,   122,
     102,     0,    22,   112,     0,   124,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    11,    45,     0,
     119,   128,     0,     0,     0,     0,   109,   110,   111,   113,
     116,   118,   115,   114,   125,   126,   127,   117,     0,     0,
       0,     0,     0,     0,    23,     7,     0,     0,     0,     0,
       0,     0,    12,     0,    16,     0,     0,    24,    48,    46,
       0,     0,     0,     0,     0,    14,     0,    18,     0,    94,
       0,     0,     0,    93,    88,    87,     0,     0,     0,     0,
       0,    32,    49,    50,     0,   105,   108,   107,   106,     0,
       0,    95,     0,     0,     0,     0,    13,     0,     0,     0,
      17,     0,     0,    20,     0,    36,     0,     0,     0,    15,
      19,    99,    96,    97,    98,    91,    89,    90,     0,     0,
      27,     0,    33,     0,    55,    56,    47,    73,     0,     0,
       0,     0,    51,    52,     0,     0,    72,     0,     0,     0,
      21,     0,     0,    28,    29,     0,    25,     0,    37,     0,
      59,     0,    74,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   120,     0,
      92,     0,     0,     0,     0,     0,    40,     0,     0,     0,
      66,    78,    57,    58,    80,    79,     0,     0,     0,    60,
      63,    65,    62,    61,    75,    76,    77,    64,     0,    31,
      26,    30,     0,    34,    41,     0,     0,    81,     0,     0,
       0,     0,     0,    53,     0,     0,     0,     0,     0,     0,
       0,    67,     0,    86,    82,    83,    84,    85,   121,    35,
       0,     0,     0,     0,    38,     0,     0,     0,     0,     0,
       0,    54,    68,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    39,    69,    70,    71,    42,
      44,    43
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     6,     7,    10,     8,    22,    47,    48,
      74,   111,   172,   173,   174,   135,   152,   153,   178,   270,
     179,   278,    75,    89,   113,   136,   162,   163,   164,   250,
     165,   218,   107,   109,   102,    19,     9,    90,    91,    21
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -265
static const yytype_int16 yypact[] =
{
    -265,    65,    12,  -265,  -265,  -265,  -265,  -265,  -265,   105,
    -265,  -265,  -265,  -265,  -265,   105,   128,   105,   294,  -265,
     340,   232,   179,  -265,   128,  -265,    59,   208,    38,    54,
      60,    66,   105,   105,   105,   128,   128,   128,   128,   128,
     128,   128,   128,   128,   116,    79,    84,  -265,   122,   161,
    -265,  -265,    97,   111,   114,   120,    44,  -265,  -265,   333,
     333,   333,   333,   333,   115,   115,  -265,   333,   127,   199,
     135,   164,   138,   210,  -265,   246,   105,   105,   105,   105,
     285,   293,  -265,   149,  -265,   -10,   255,   264,   289,  -265,
      75,   267,   272,   308,   310,  -265,   149,  -265,   -10,  -265,
     149,   149,   295,  -265,  -265,  -265,   -10,   219,   149,    -6,
     326,  -265,  -265,  -265,     6,  -265,  -265,  -265,  -265,   301,
     324,  -265,   244,   149,   149,   149,  -265,   156,   -10,   -10,
    -265,   125,   149,  -265,   286,   327,   171,   342,   356,  -265,
    -265,  -265,   341,   341,  -265,  -265,  -265,  -265,   149,   307,
     360,   338,  -265,    30,  -265,  -265,  -265,  -265,   109,   170,
     362,   109,  -265,  -265,    41,   241,  -265,   105,   337,   175,
    -265,   343,   339,   344,  -265,   255,  -265,    -7,  -265,   109,
    -265,   170,  -265,   346,    25,   217,   109,   109,   194,   170,
     170,   170,   170,   170,   170,   170,   170,   170,   340,   364,
    -265,   367,   370,   360,    -5,   347,  -265,   329,   257,   109,
    -265,  -265,  -265,  -265,  -265,  -265,   194,   194,   196,   336,
     336,   336,   336,   336,   351,   351,  -265,   336,   348,  -265,
    -265,  -265,   149,  -265,  -265,   123,    92,  -265,   248,   194,
     194,   194,   194,  -265,   105,   313,   349,   350,   352,   353,
     355,   322,   194,  -265,   302,   302,  -265,  -265,   340,  -265,
     194,   -10,   -10,   -10,  -265,   282,   252,    94,   270,   276,
     -15,  -265,  -265,   -10,   -10,   -10,   359,   -15,   103,   259,
     263,   265,   170,   -19,   -15,  -265,  -265,  -265,  -265,   336,
    -265,  -265
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -265,  -265,  -265,  -265,  -265,  -265,  -265,  -265,  -265,  -265,
    -265,  -265,  -265,  -265,   190,  -265,  -265,  -265,  -265,  -265,
    -265,  -264,  -265,  -265,  -265,  -265,  -265,  -265,  -131,  -265,
    -101,  -178,   -96,   226,   -79,   -82,  -265,    -9,   297,   185
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint16 yytable[] =
{
      20,   103,   120,   276,   132,   232,    23,   284,    26,   104,
     127,   105,   205,   283,   103,     4,     5,   119,   103,   103,
     291,   121,   122,    56,    57,    58,   103,   180,   290,   131,
     184,   277,   146,   147,   133,   233,   106,   206,   237,   238,
     137,   103,   103,   103,   142,   143,   144,   176,   207,   138,
     103,   186,   187,   149,   166,   212,   213,   251,   182,    52,
     185,   254,   255,   256,   257,     3,   103,   186,   187,   169,
      33,    34,   210,   177,   265,    53,   166,   166,   236,   166,
     208,    54,   266,   188,    32,    33,    34,    55,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   166,    71,   166,
      32,    33,    34,    72,   166,   166,    50,   166,   166,   166,
     166,   166,   166,   166,   166,   166,    11,    12,   186,   187,
     154,   155,    68,    69,    13,   114,    14,   166,   157,   284,
      14,   128,   129,    15,   252,    70,   246,   158,    73,   273,
      76,    16,   214,   285,   215,   159,    80,    13,   148,    14,
     103,    17,    42,   245,    77,   161,    18,    78,   198,   216,
     123,   124,   125,    79,    16,   267,   268,   269,    99,   217,
      14,   247,   248,   249,    24,    82,    83,   279,   280,   281,
      86,   289,   154,   155,    44,   100,   156,    45,    46,   157,
     157,    14,    14,   128,   129,   101,    40,    41,    42,   158,
     166,    25,    27,   145,    84,    85,   159,   159,    51,    49,
     123,   124,   125,   214,   160,   215,   181,   161,    81,   200,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    87,
     216,   239,   240,   241,   242,   258,   243,    35,    36,    37,
     217,    38,    39,    40,    41,    42,   189,   190,   191,    43,
     192,   193,   194,   195,   196,    51,   128,   129,   197,   130,
      88,    35,    36,    37,   211,    38,    39,    40,    41,    42,
     189,   190,   191,    43,   192,   193,   194,   195,   196,   123,
     124,   125,   197,   239,   240,   241,   242,   239,   240,   241,
     242,   141,   194,   195,   196,   253,   128,   129,   108,   272,
     128,   129,   128,   129,   211,   110,   286,   128,   129,   112,
     287,   115,   288,   128,   129,   274,   116,   239,   240,   241,
     242,   275,   271,    28,    29,    95,    96,    30,    31,   150,
     123,   124,   125,    97,    98,   126,   123,   124,   125,   241,
     242,   139,   123,   124,   125,   134,   151,   170,   123,   124,
     125,   235,   117,   259,   118,   186,   187,   239,   240,   241,
     242,   128,   129,   167,   140,    32,    33,    34,    40,    41,
      42,   194,   195,   196,    92,    93,    94,   168,   125,   171,
     175,   183,   199,   202,   201,   228,   229,   230,   196,   203,
     209,   234,   244,   231,     0,   260,   261,   264,   262,   263,
     282,   204
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-265))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
       9,    83,    98,    18,    10,    10,    15,    26,    17,    19,
     106,    21,    19,   277,    96,     3,     4,    96,   100,   101,
     284,   100,   101,    32,    33,    34,   108,   158,    47,   108,
     161,    46,   128,   129,    40,    40,    46,    44,   216,   217,
      34,   123,   124,   125,   123,   124,   125,    17,   179,    43,
     132,    26,    27,   132,   136,   186,   187,   235,   159,    21,
     161,   239,   240,   241,   242,     0,   148,    26,    27,   148,
      26,    27,    47,    43,   252,    21,   158,   159,   209,   161,
     181,    21,   260,    42,    25,    26,    27,    21,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   179,    19,   181,
      25,    26,    27,    19,   186,   187,    47,   189,   190,   191,
     192,   193,   194,   195,   196,   197,    11,    12,    26,    27,
      11,    12,     6,     7,    19,    50,    21,   209,    19,    26,
      21,    37,    38,    28,    42,    19,    13,    28,    16,    45,
      43,    36,    19,    40,    21,    36,    19,    19,    23,    21,
     232,    46,    37,   232,    43,    46,    51,    43,   167,    36,
      35,    36,    37,    43,    36,   261,   262,   263,    19,    46,
      21,    48,    49,    50,    46,    40,    41,   273,   274,   275,
      42,   282,    11,    12,     5,    36,    15,     8,     9,    19,
      19,    21,    21,    37,    38,    46,    35,    36,    37,    28,
     282,    16,    17,    47,    40,    41,    36,    36,    47,    24,
      35,    36,    37,    19,    43,    21,    46,    46,    19,    44,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    19,
      36,    35,    36,    37,    38,   244,    40,    29,    30,    31,
      46,    33,    34,    35,    36,    37,    29,    30,    31,    41,
      33,    34,    35,    36,    37,    47,    37,    38,    41,    40,
      14,    29,    30,    31,    47,    33,    34,    35,    36,    37,
      29,    30,    31,    41,    33,    34,    35,    36,    37,    35,
      36,    37,    41,    35,    36,    37,    38,    35,    36,    37,
      38,    47,    35,    36,    37,    47,    37,    38,    43,    47,
      37,    38,    37,    38,    47,    41,    47,    37,    38,    20,
      47,    44,    47,    37,    38,    45,    44,    35,    36,    37,
      38,    45,    40,    29,    30,    40,    41,    33,    34,    43,
      35,    36,    37,    40,    41,    40,    35,    36,    37,    37,
      38,    40,    35,    36,    37,    19,    19,    40,    35,    36,
      37,    22,    44,    40,    44,    26,    27,    35,    36,    37,
      38,    37,    38,    21,    40,    25,    26,    27,    35,    36,
      37,    35,    36,    37,    77,    78,    79,    21,    37,    19,
      42,    19,    45,    44,    41,    21,    19,    17,    37,    45,
      44,    44,    44,   203,    -1,    46,    46,    42,    46,    46,
      41,   175
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    53,    54,     0,     3,     4,    55,    56,    58,    88,
      57,    11,    12,    19,    21,    28,    36,    46,    51,    87,
      89,    91,    59,    89,    46,    91,    89,    91,    29,    30,
      33,    34,    25,    26,    27,    29,    30,    31,    33,    34,
      35,    36,    37,    41,     5,     8,     9,    60,    61,    91,
      47,    47,    21,    21,    21,    21,    89,    89,    89,    91,
      91,    91,    91,    91,    91,    91,    91,    91,     6,     7,
      19,    19,    19,    16,    62,    74,    43,    43,    43,    43,
      19,    19,    40,    41,    40,    41,    42,    19,    14,    75,
      89,    90,    90,    90,    90,    40,    41,    40,    41,    19,
      36,    46,    86,    87,    19,    21,    46,    84,    43,    85,
      41,    63,    20,    76,    50,    44,    44,    44,    44,    86,
      84,    86,    86,    35,    36,    37,    40,    84,    37,    38,
      40,    86,    10,    40,    19,    67,    77,    34,    43,    40,
      40,    47,    86,    86,    86,    47,    84,    84,    23,    86,
      43,    19,    68,    69,    11,    12,    15,    19,    28,    36,
      43,    46,    78,    79,    80,    82,    87,    21,    21,    86,
      40,    19,    64,    65,    66,    42,    17,    43,    70,    72,
      80,    46,    82,    19,    80,    82,    26,    27,    42,    29,
      30,    31,    33,    34,    35,    36,    37,    41,    89,    45,
      44,    41,    44,    45,    85,    19,    44,    80,    82,    44,
      47,    47,    80,    80,    19,    21,    36,    46,    83,    82,
      82,    82,    82,    82,    82,    82,    82,    82,    21,    19,
      17,    66,    10,    40,    44,    22,    80,    83,    83,    35,
      36,    37,    38,    40,    44,    86,    13,    48,    49,    50,
      81,    83,    42,    47,    83,    83,    83,    83,    89,    40,
      46,    46,    46,    46,    42,    83,    83,    84,    84,    84,
      71,    40,    47,    45,    45,    45,    18,    46,    73,    84,
      84,    84,    41,    73,    26,    40,    47,    47,    47,    82,
      47,    73
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
#line 288 "parser.yy"
    { success = true; line_number = 1; }
    break;

  case 3:

/* Line 1806 of yacc.c  */
#line 289 "parser.yy"
    { check_undeclared(); if (!success) YYERROR; }
    break;

  case 6:

/* Line 1806 of yacc.c  */
#line 300 "parser.yy"
    { prepare_model(); }
    break;

  case 7:

/* Line 1806 of yacc.c  */
#line 301 "parser.yy"
    { compile_model(); }
    break;

  case 12:

/* Line 1806 of yacc.c  */
#line 314 "parser.yy"
    { declare_constant((yyvsp[(2) - (3)].str), nullptr); }
    break;

  case 13:

/* Line 1806 of yacc.c  */
#line 315 "parser.yy"
    { declare_constant((yyvsp[(2) - (5)].str), (yyvsp[(4) - (5)].expr)); }
    break;

  case 14:

/* Line 1806 of yacc.c  */
#line 316 "parser.yy"
    { declare_constant((yyvsp[(3) - (4)].str), nullptr); }
    break;

  case 15:

/* Line 1806 of yacc.c  */
#line 318 "parser.yy"
    { declare_constant((yyvsp[(3) - (6)].str), (yyvsp[(5) - (6)].expr)); }
    break;

  case 16:

/* Line 1806 of yacc.c  */
#line 319 "parser.yy"
    { declare_rate((yyvsp[(2) - (3)].str), nullptr); }
    break;

  case 17:

/* Line 1806 of yacc.c  */
#line 320 "parser.yy"
    { declare_rate((yyvsp[(2) - (5)].str), (yyvsp[(4) - (5)].expr)); }
    break;

  case 18:

/* Line 1806 of yacc.c  */
#line 321 "parser.yy"
    { declare_rate((yyvsp[(3) - (4)].str), nullptr); }
    break;

  case 19:

/* Line 1806 of yacc.c  */
#line 323 "parser.yy"
    { declare_rate((yyvsp[(3) - (6)].str), (yyvsp[(5) - (6)].expr)); }
    break;

  case 20:

/* Line 1806 of yacc.c  */
#line 324 "parser.yy"
    { declare_variable((yyvsp[(2) - (5)].str), (yyvsp[(4) - (5)].range), nullptr); }
    break;

  case 21:

/* Line 1806 of yacc.c  */
#line 326 "parser.yy"
    { declare_variable((yyvsp[(2) - (7)].str), (yyvsp[(4) - (7)].range), (yyvsp[(6) - (7)].expr)); }
    break;

  case 24:

/* Line 1806 of yacc.c  */
#line 337 "parser.yy"
    { prepare_module((yyvsp[(2) - (2)].str)); }
    break;

  case 25:

/* Line 1806 of yacc.c  */
#line 338 "parser.yy"
    { add_module(); }
    break;

  case 26:

/* Line 1806 of yacc.c  */
#line 340 "parser.yy"
    { add_module((yyvsp[(2) - (8)].str), (yyvsp[(4) - (8)].str)); }
    break;

  case 31:

/* Line 1806 of yacc.c  */
#line 351 "parser.yy"
    { add_substitution((yyvsp[(1) - (3)].str), (yyvsp[(3) - (3)].str)); }
    break;

  case 34:

/* Line 1806 of yacc.c  */
#line 358 "parser.yy"
    { declare_variable((yyvsp[(1) - (4)].str), (yyvsp[(3) - (4)].range), nullptr); }
    break;

  case 35:

/* Line 1806 of yacc.c  */
#line 360 "parser.yy"
    { declare_variable((yyvsp[(1) - (6)].str), (yyvsp[(3) - (6)].range), (yyvsp[(5) - (6)].expr)); }
    break;

  case 38:

/* Line 1806 of yacc.c  */
#line 368 "parser.yy"
    { prepare_command((yyvsp[(1) - (5)].synch), (yyvsp[(2) - (5)].formula), (yyvsp[(4) - (5)].dist)); }
    break;

  case 39:

/* Line 1806 of yacc.c  */
#line 368 "parser.yy"
    { add_command(); }
    break;

  case 40:

/* Line 1806 of yacc.c  */
#line 371 "parser.yy"
    { (yyval.synch) = 0; }
    break;

  case 41:

/* Line 1806 of yacc.c  */
#line 372 "parser.yy"
    { (yyval.synch) = synchronization_value((yyvsp[(2) - (3)].str)); }
    break;

  case 42:

/* Line 1806 of yacc.c  */
#line 375 "parser.yy"
    { add_update((yyvsp[(1) - (3)].str), (yyvsp[(3) - (3)].expr)); }
    break;

  case 53:

/* Line 1806 of yacc.c  */
#line 401 "parser.yy"
    { delete (yyvsp[(1) - (4)].formula); delete (yyvsp[(3) - (4)].expr); }
    break;

  case 54:

/* Line 1806 of yacc.c  */
#line 405 "parser.yy"
    { delete (yyvsp[(2) - (7)].str); delete (yyvsp[(4) - (7)].formula); delete (yyvsp[(6) - (7)].expr); }
    break;

  case 55:

/* Line 1806 of yacc.c  */
#line 412 "parser.yy"
    { (yyval.formula) = new Conjunction(); }
    break;

  case 56:

/* Line 1806 of yacc.c  */
#line 413 "parser.yy"
    { (yyval.formula) = new Disjunction(); }
    break;

  case 57:

/* Line 1806 of yacc.c  */
#line 414 "parser.yy"
    { (yyval.formula) = make_conjunction((yyvsp[(1) - (3)].formula), (yyvsp[(3) - (3)].formula)); }
    break;

  case 58:

/* Line 1806 of yacc.c  */
#line 415 "parser.yy"
    { (yyval.formula) = make_disjunction((yyvsp[(1) - (3)].formula), (yyvsp[(3) - (3)].formula)); }
    break;

  case 59:

/* Line 1806 of yacc.c  */
#line 416 "parser.yy"
    { (yyval.formula) = new Negation((yyvsp[(2) - (2)].formula)); }
    break;

  case 60:

/* Line 1806 of yacc.c  */
#line 417 "parser.yy"
    { (yyval.formula) = NewLessThan((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 61:

/* Line 1806 of yacc.c  */
#line 418 "parser.yy"
    { (yyval.formula) = NewLessThanOrEqual((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 62:

/* Line 1806 of yacc.c  */
#line 419 "parser.yy"
    { (yyval.formula) = NewGreaterThanOrEqual((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 63:

/* Line 1806 of yacc.c  */
#line 420 "parser.yy"
    { (yyval.formula) = NewGreaterThan((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 64:

/* Line 1806 of yacc.c  */
#line 421 "parser.yy"
    { (yyval.formula) = NewEquality((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 65:

/* Line 1806 of yacc.c  */
#line 422 "parser.yy"
    { (yyval.formula) = NewInequality((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 66:

/* Line 1806 of yacc.c  */
#line 423 "parser.yy"
    { (yyval.formula) = (yyvsp[(2) - (3)].formula); }
    break;

  case 67:

/* Line 1806 of yacc.c  */
#line 430 "parser.yy"
    { (yyval.dist) = NewExponential((yyvsp[(1) - (1)].expr)); }
    break;

  case 68:

/* Line 1806 of yacc.c  */
#line 431 "parser.yy"
    { (yyval.dist) = NewExponential((yyvsp[(3) - (4)].expr)); }
    break;

  case 69:

/* Line 1806 of yacc.c  */
#line 433 "parser.yy"
    { (yyval.dist) = NewWeibull((yyvsp[(3) - (6)].expr), (yyvsp[(5) - (6)].expr)); }
    break;

  case 70:

/* Line 1806 of yacc.c  */
#line 435 "parser.yy"
    { (yyval.dist) = NewLognormal((yyvsp[(3) - (6)].expr), (yyvsp[(5) - (6)].expr)); }
    break;

  case 71:

/* Line 1806 of yacc.c  */
#line 437 "parser.yy"
    { (yyval.dist) = NewUniform((yyvsp[(3) - (6)].expr), (yyvsp[(5) - (6)].expr)); }
    break;

  case 72:

/* Line 1806 of yacc.c  */
#line 443 "parser.yy"
    { (yyval.expr) = make_literal((yyvsp[(1) - (1)].nat)); }
    break;

  case 73:

/* Line 1806 of yacc.c  */
#line 444 "parser.yy"
    { (yyval.expr) = find_variable((yyvsp[(1) - (1)].str)); }
    break;

  case 74:

/* Line 1806 of yacc.c  */
#line 446 "parser.yy"
    { (yyval.expr) = NewUnaryOperation(UnaryOperator::NEGATE, (yyvsp[(2) - (2)].expr)); }
    break;

  case 75:

/* Line 1806 of yacc.c  */
#line 448 "parser.yy"
    { (yyval.expr) = NewBinaryOperation(BinaryOperator::PLUS, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 76:

/* Line 1806 of yacc.c  */
#line 450 "parser.yy"
    { (yyval.expr) = NewBinaryOperation(BinaryOperator::MINUS, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 77:

/* Line 1806 of yacc.c  */
#line 452 "parser.yy"
    { (yyval.expr) = NewBinaryOperation(BinaryOperator::MULTIPLY, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 78:

/* Line 1806 of yacc.c  */
#line 453 "parser.yy"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;

  case 79:

/* Line 1806 of yacc.c  */
#line 456 "parser.yy"
    { (yyval.expr) = make_literal((yyvsp[(1) - (1)].num)); }
    break;

  case 80:

/* Line 1806 of yacc.c  */
#line 457 "parser.yy"
    { (yyval.expr) = find_rate_or_variable((yyvsp[(1) - (1)].str)); }
    break;

  case 81:

/* Line 1806 of yacc.c  */
#line 459 "parser.yy"
    { (yyval.expr) = NewUnaryOperation(UnaryOperator::NEGATE, (yyvsp[(2) - (2)].expr)); }
    break;

  case 82:

/* Line 1806 of yacc.c  */
#line 461 "parser.yy"
    { (yyval.expr) = NewBinaryOperation(BinaryOperator::PLUS, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 83:

/* Line 1806 of yacc.c  */
#line 463 "parser.yy"
    { (yyval.expr) = NewBinaryOperation(BinaryOperator::MINUS, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 84:

/* Line 1806 of yacc.c  */
#line 465 "parser.yy"
    { (yyval.expr) = NewBinaryOperation(BinaryOperator::MULTIPLY, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 85:

/* Line 1806 of yacc.c  */
#line 467 "parser.yy"
    { (yyval.expr) = NewBinaryOperation(BinaryOperator::DIVIDE, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 86:

/* Line 1806 of yacc.c  */
#line 468 "parser.yy"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;

  case 87:

/* Line 1806 of yacc.c  */
#line 471 "parser.yy"
    { (yyval.expr) = make_literal((yyvsp[(1) - (1)].num)); }
    break;

  case 88:

/* Line 1806 of yacc.c  */
#line 472 "parser.yy"
    { (yyval.expr) = find_rate((yyvsp[(1) - (1)].str)); }
    break;

  case 89:

/* Line 1806 of yacc.c  */
#line 474 "parser.yy"
    { (yyval.expr) = NewBinaryOperation(BinaryOperator::MULTIPLY, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 90:

/* Line 1806 of yacc.c  */
#line 476 "parser.yy"
    { (yyval.expr) = NewBinaryOperation(BinaryOperator::DIVIDE, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 91:

/* Line 1806 of yacc.c  */
#line 477 "parser.yy"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;

  case 92:

/* Line 1806 of yacc.c  */
#line 484 "parser.yy"
    { (yyval.range) = make_range((yyvsp[(2) - (5)].expr), (yyvsp[(4) - (5)].expr)); }
    break;

  case 93:

/* Line 1806 of yacc.c  */
#line 487 "parser.yy"
    { (yyval.expr) = make_literal((yyvsp[(1) - (1)].nat)); }
    break;

  case 94:

/* Line 1806 of yacc.c  */
#line 488 "parser.yy"
    { (yyval.expr) = find_constant((yyvsp[(1) - (1)].str)); }
    break;

  case 95:

/* Line 1806 of yacc.c  */
#line 490 "parser.yy"
    { (yyval.expr) = NewUnaryOperation(UnaryOperator::NEGATE, (yyvsp[(2) - (2)].expr)); }
    break;

  case 96:

/* Line 1806 of yacc.c  */
#line 492 "parser.yy"
    { (yyval.expr) = NewBinaryOperation(BinaryOperator::PLUS, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 97:

/* Line 1806 of yacc.c  */
#line 494 "parser.yy"
    { (yyval.expr) = NewBinaryOperation(BinaryOperator::MINUS, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 98:

/* Line 1806 of yacc.c  */
#line 496 "parser.yy"
    { (yyval.expr) = NewBinaryOperation(BinaryOperator::MULTIPLY, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 99:

/* Line 1806 of yacc.c  */
#line 497 "parser.yy"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;

  case 100:

/* Line 1806 of yacc.c  */
#line 500 "parser.yy"
    { (yyval.nat) = integer_value((yyvsp[(1) - (1)].num)); }
    break;

  case 102:

/* Line 1806 of yacc.c  */
#line 509 "parser.yy"
    { properties.push_back((yyvsp[(2) - (2)].formula)); }
    break;

  case 103:

/* Line 1806 of yacc.c  */
#line 512 "parser.yy"
    { (yyval.formula) = new Conjunction(); }
    break;

  case 104:

/* Line 1806 of yacc.c  */
#line 513 "parser.yy"
    { (yyval.formula) = new Disjunction(); }
    break;

  case 105:

/* Line 1806 of yacc.c  */
#line 515 "parser.yy"
    { (yyval.formula) = make_probabilistic((yyvsp[(3) - (6)].num), true, true, (yyvsp[(5) - (6)].path)); }
    break;

  case 106:

/* Line 1806 of yacc.c  */
#line 517 "parser.yy"
    { (yyval.formula) = make_probabilistic((yyvsp[(3) - (6)].num), false, true, (yyvsp[(5) - (6)].path)); }
    break;

  case 107:

/* Line 1806 of yacc.c  */
#line 519 "parser.yy"
    { (yyval.formula) = make_probabilistic((yyvsp[(3) - (6)].num), false, false, (yyvsp[(5) - (6)].path)); }
    break;

  case 108:

/* Line 1806 of yacc.c  */
#line 521 "parser.yy"
    { (yyval.formula) = make_probabilistic((yyvsp[(3) - (6)].num), true, false, (yyvsp[(5) - (6)].path)); }
    break;

  case 109:

/* Line 1806 of yacc.c  */
#line 522 "parser.yy"
    { (yyval.formula) = new Implication((yyvsp[(1) - (3)].formula), (yyvsp[(3) - (3)].formula)); }
    break;

  case 110:

/* Line 1806 of yacc.c  */
#line 523 "parser.yy"
    { (yyval.formula) = make_conjunction((yyvsp[(1) - (3)].formula), (yyvsp[(3) - (3)].formula)); }
    break;

  case 111:

/* Line 1806 of yacc.c  */
#line 524 "parser.yy"
    { (yyval.formula) = make_disjunction((yyvsp[(1) - (3)].formula), (yyvsp[(3) - (3)].formula)); }
    break;

  case 112:

/* Line 1806 of yacc.c  */
#line 525 "parser.yy"
    { (yyval.formula) = new Negation((yyvsp[(2) - (2)].formula)); }
    break;

  case 113:

/* Line 1806 of yacc.c  */
#line 526 "parser.yy"
    { (yyval.formula) = NewLessThan((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 114:

/* Line 1806 of yacc.c  */
#line 527 "parser.yy"
    { (yyval.formula) = NewLessThanOrEqual((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 115:

/* Line 1806 of yacc.c  */
#line 528 "parser.yy"
    { (yyval.formula) = NewGreaterThanOrEqual((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 116:

/* Line 1806 of yacc.c  */
#line 529 "parser.yy"
    { (yyval.formula) = NewGreaterThan((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 117:

/* Line 1806 of yacc.c  */
#line 530 "parser.yy"
    { (yyval.formula) = NewEquality((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 118:

/* Line 1806 of yacc.c  */
#line 531 "parser.yy"
    { (yyval.formula) = NewInequality((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 119:

/* Line 1806 of yacc.c  */
#line 532 "parser.yy"
    { (yyval.formula) = (yyvsp[(2) - (3)].formula); }
    break;

  case 120:

/* Line 1806 of yacc.c  */
#line 536 "parser.yy"
    { (yyval.path) = make_until((yyvsp[(1) - (5)].formula), (yyvsp[(5) - (5)].formula), nullptr, (yyvsp[(4) - (5)].num)); }
    break;

  case 121:

/* Line 1806 of yacc.c  */
#line 538 "parser.yy"
    { (yyval.path) = make_until((yyvsp[(1) - (8)].formula), (yyvsp[(8) - (8)].formula), (yyvsp[(4) - (8)].num), (yyvsp[(6) - (8)].num)); }
    break;

  case 122:

/* Line 1806 of yacc.c  */
#line 542 "parser.yy"
    { (yyval.expr) = make_literal((yyvsp[(1) - (1)].nat)); }
    break;

  case 123:

/* Line 1806 of yacc.c  */
#line 543 "parser.yy"
    { (yyval.expr) = value_or_variable((yyvsp[(1) - (1)].str)); }
    break;

  case 124:

/* Line 1806 of yacc.c  */
#line 545 "parser.yy"
    { (yyval.expr) = NewUnaryOperation(UnaryOperator::NEGATE, (yyvsp[(2) - (2)].expr)); }
    break;

  case 125:

/* Line 1806 of yacc.c  */
#line 547 "parser.yy"
    { (yyval.expr) = NewBinaryOperation(BinaryOperator::PLUS, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 126:

/* Line 1806 of yacc.c  */
#line 549 "parser.yy"
    { (yyval.expr) = NewBinaryOperation(BinaryOperator::MINUS, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 127:

/* Line 1806 of yacc.c  */
#line 551 "parser.yy"
    { (yyval.expr) = NewBinaryOperation(BinaryOperator::MULTIPLY, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 128:

/* Line 1806 of yacc.c  */
#line 552 "parser.yy"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;



/* Line 1806 of yacc.c  */
#line 2615 "parser.cc"
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
#line 556 "parser.yy"


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
  // TODO(hlsyounes): implement.
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
  PointerVector<const Expression> arguments;
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


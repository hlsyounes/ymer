
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
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
#define YYBISON_VERSION "2.4.1"

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

/* Line 189 of yacc.c  */
#line 24 "parser.yy"

#include <config.h>
#include "models.h"
#include "modules.h"
#include "distributions.h"
#include "formulas.h"
#include "expressions.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <stdexcept>


/* Workaround for bug in Bison 1.35 that disables stack growth. */
#define YYLTYPE_IS_TRIVIAL 1


/* An integer range. */
struct Range {
  const Expression<int>* l;
  const Expression<int>* h;
};


/* The lexer. */
extern int yylex();
/* Current line number. */
extern int line_number;
/* Name of current file. */
extern std::string current_file;

/* Last model parsed. */
const Model* global_model = 0;
/* Parsed properties. */
FormulaList properties;
/* Parsed path formula. */
const PathFormula* global_path_formula = 0;

/* Parser mode (0: standard; 1: model; 2: path formula) */
static int mode = 0;
/* Current model. */
static Model* model;
/* Current module. */
static Module* module;
/* Source module for substitutions. */
static Module* src_module;
/* Current constant and variable substitutions. */
static Substitutions subst;
/* Current synchronization substitutions. */
static SynchSubstitutions synch_subst;
/* Current command. */
static Command* command;
/* Declared integer constants. */
static std::map<std::string, const Constant<int>*> constants;
/* Declared rate constants. */
static std::map<std::string, const Constant<double>*> rates;
/* All state variables. */
static std::map<std::string, Variable<int>*> variables;
/* Declared modules. */
static std::map<std::string, Module*> modules;
/* Declared synchronizations. */
static std::map<std::string, int> synchronizations;
/* Yet undeclared state variables. */
static std::set<std::string> undeclared;
/* Whether to negate the current formula. */ 
static std::stack<bool> negate;
/* Whether the last parsing attempt succeeded. */
static bool success = true;

/* Outputs an error message. */
static void yyerror(const std::string& s); 
/* Outputs a warning message. */
static void yywarning(const std::string& s);
/* Checks if undeclared variables were used. */
static void check_undeclared();
/* Returns a pointer to a variable or constant with the given name, or 0. */
static const Expression<int>* find_named_expression(const std::string* ident,
						    bool constant);
/* Returns a pointer to a rate expression with the given name, or 0. */
static const Expression<double>* find_rate(const std::string* ident,
					   bool constant);
/* Returns a range with the given bounds, signaling an error if the
   range is empty. */
static Range make_range(const Expression<int>* l, const Expression<int>* h);
/* Returns a value expression. */
static const Value<int>* make_value(int n);
/* Returns a value expression. */
static const Value<double>* make_value(double x);
/* Returns a tautology. */ 
static StateFormula* make_tautology();
/* Returns a contradiction. */
static StateFormula* make_contradiction();
/* Returns a conjunction. */
static StateFormula* make_conjunction(StateFormula& f1,
				      const StateFormula& f2);
/* Returns a disjunction. */
static StateFormula* make_disjunction(StateFormula& f1,
				      const StateFormula& f2);
/* Returns an implication. */
static StateFormula* make_implication(const StateFormula& f1,
				      const StateFormula& f2);
/* Returns a probabilistic path quantification. */
static StateFormula* make_probabilistic(bool negate, double p, bool strict,
					const PathFormula& f);
/* Returns an until formula. */
static const Until* make_until(const StateFormula& f1, const StateFormula& f2,
			       double t1, double t2);
/* Adds an update to the current command. */
static void add_update(const std::string* ident, const Expression<int>& expr);
/* Returns the value of the given synchronization. */
static int synchronization_value(const std::string* ident);
/* Prepares a module substitution for parsing. */
static void prepare_substitutions(const std::string& ident);
/* Adds a substitution to the current substitution map. */
static void add_substitution(const std::string* ident1,
			     const std::string* ident2);
/* Declares an integer constant. */
static void declare_constant(const std::string* ident, int value);
/* Declares a rate constant. */
static void declare_rate(const std::string* ident, double value);
/* Declares a variable. */
static const Variable<int>* declare_variable(const std::string* ident,
					     const Range& range,
					     const Expression<int>* init);
/* Adds a command to the current module. */
static void add_command();
/* Prepares a command for parsing. */
static void prepare_command(int synch, const StateFormula& guard,
			    const Distribution& delay);
/* Adds a module to the current model defined by renaming. */
static void add_module(const std::string* ident1, const std::string* ident2);
/* Adds a module to the current model. */
static void add_module();
/* Prepares a module for parsing. */
static void prepare_module(const std::string* ident);
/* Prepares a model for parsing. */
static void prepare_model();
/* Adds a parsed property. */
static void add_property(const StateFormula* f);


/* Line 189 of yacc.c  */
#line 218 "parser.cc"

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
     STOCHASTIC = 258,
     CONST = 259,
     RATE = 260,
     GLOBAL = 261,
     INIT = 262,
     TRUE_TOKEN = 263,
     FALSE_TOKEN = 264,
     EXP = 265,
     MODULE = 266,
     ENDMODULE = 267,
     PNAME = 268,
     NAME = 269,
     INTEGER = 270,
     REAL = 271,
     ARROW = 272,
     DOTDOT = 273,
     IMPLY = 274,
     NEQ = 275,
     EQ = 276,
     GTE = 277,
     LTE = 278
   };
#endif
/* Tokens.  */
#define STOCHASTIC 258
#define CONST 259
#define RATE 260
#define GLOBAL 261
#define INIT 262
#define TRUE_TOKEN 263
#define FALSE_TOKEN 264
#define EXP 265
#define MODULE 266
#define ENDMODULE 267
#define PNAME 268
#define NAME 269
#define INTEGER 270
#define REAL 271
#define ARROW 272
#define DOTDOT 273
#define IMPLY 274
#define NEQ 275
#define EQ 276
#define GTE 277
#define LTE 278




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 185 "parser.yy"

  int synch;
  StateFormula* formula;
  const PathFormula* path;
  const Distribution* dist;
  const Expression<int>* iexpr;
  const Expression<double>* dexpr;
  Range range;
  const std::string* str;
  int nat;
  double real;



/* Line 214 of yacc.c  */
#line 315 "parser.cc"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 327 "parser.cc"

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
# if YYENABLE_NLS
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
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
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
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
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

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   288

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  45
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  41
/* YYNRULES -- Number of rules.  */
#define YYNRULES  105
/* YYNRULES -- Number of states.  */
#define YYNSTATES  240

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   278

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    22,     2,     2,     2,     2,    20,     2,
      39,    40,    31,    29,    38,    30,     2,    32,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    35,    34,
      23,    33,    24,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,    42,     2,     2,     2,
      44,     2,     2,     2,     2,    43,     2,    41,     2,     2,
       2,    36,     2,    37,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    21,     2,     2,     2,     2,     2,
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
      15,    16,    17,    18,    19,    25,    26,    27,    28
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,     9,    11,    13,    14,    19,
      21,    22,    25,    31,    37,    43,    51,    52,    55,    56,
      63,    64,    74,    75,    77,    79,    83,    87,    88,    91,
      96,   103,   104,   107,   108,   117,   120,   124,   128,   132,
     136,   140,   144,   145,   149,   153,   157,   161,   165,   169,
     173,   177,   179,   184,   191,   198,   205,   207,   209,   213,
     217,   221,   225,   227,   229,   233,   237,   241,   247,   249,
     251,   255,   259,   263,   267,   268,   271,   272,   277,   279,
     281,   282,   290,   291,   299,   300,   308,   309,   317,   321,
     325,   329,   330,   334,   338,   342,   346,   350,   354,   358,
     362,   368,   374,   383,   387,   389
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      46,     0,    -1,    -1,    47,    48,    -1,    49,    -1,    75,
      -1,    76,    -1,    -1,    51,    50,    52,    54,    -1,     3,
      -1,    -1,    52,    53,    -1,     4,    14,    33,    15,    34,
      -1,     5,    14,    33,    85,    34,    -1,     6,    14,    35,
      73,    34,    -1,     6,    14,    35,    73,     7,    74,    34,
      -1,    -1,    54,    55,    -1,    -1,    11,    14,    56,    61,
      63,    12,    -1,    -1,    11,    14,    33,    14,    57,    36,
      58,    37,    12,    -1,    -1,    59,    -1,    60,    -1,    59,
      38,    60,    -1,    14,    33,    14,    -1,    -1,    61,    62,
      -1,    14,    35,    73,    34,    -1,    14,    35,    73,     7,
      74,    34,    -1,    -1,    63,    64,    -1,    -1,    66,    68,
      17,    70,    35,    65,    67,    34,    -1,    36,    37,    -1,
      36,    14,    37,    -1,    13,    33,    71,    -1,    67,    20,
      67,    -1,    39,    67,    40,    -1,    68,    20,    68,    -1,
      68,    21,    68,    -1,    -1,    22,    69,    68,    -1,    71,
      23,    71,    -1,    71,    28,    71,    -1,    71,    27,    71,
      -1,    71,    24,    71,    -1,    71,    33,    71,    -1,    71,
      25,    71,    -1,    39,    68,    40,    -1,    72,    -1,    10,
      39,    72,    40,    -1,    41,    39,    72,    38,    72,    40,
      -1,    42,    39,    72,    38,    72,    40,    -1,    43,    39,
      72,    38,    72,    40,    -1,    15,    -1,    14,    -1,    71,
      29,    71,    -1,    71,    30,    71,    -1,    71,    31,    71,
      -1,    39,    71,    40,    -1,    85,    -1,    14,    -1,    72,
      31,    72,    -1,    72,    32,    72,    -1,    39,    72,    40,
      -1,    36,    74,    18,    74,    37,    -1,    15,    -1,    14,
      -1,    74,    29,    74,    -1,    74,    30,    74,    -1,    74,
      31,    74,    -1,    39,    74,    40,    -1,    -1,    75,    78,
      -1,    -1,    36,    77,    84,    37,    -1,     8,    -1,     9,
      -1,    -1,    44,    23,    85,    36,    79,    84,    37,    -1,
      -1,    44,    28,    85,    36,    80,    84,    37,    -1,    -1,
      44,    27,    85,    36,    81,    84,    37,    -1,    -1,    44,
      24,    85,    36,    82,    84,    37,    -1,    78,    19,    78,
      -1,    78,    20,    78,    -1,    78,    21,    78,    -1,    -1,
      22,    83,    78,    -1,    71,    23,    71,    -1,    71,    28,
      71,    -1,    71,    27,    71,    -1,    71,    24,    71,    -1,
      71,    33,    71,    -1,    71,    25,    71,    -1,    39,    78,
      40,    -1,    78,    43,    28,    85,    78,    -1,    78,    43,
      27,    85,    78,    -1,    78,    43,    36,    85,    38,    85,
      37,    78,    -1,    78,    43,    78,    -1,    15,    -1,    16,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   211,   211,   211,   215,   216,   217,   224,   224,   228,
     234,   235,   238,   239,   240,   241,   249,   250,   253,   253,
     255,   255,   259,   260,   263,   264,   267,   270,   271,   274,
     275,   279,   280,   284,   283,   287,   288,   291,   292,   293,
     300,   301,   302,   302,   304,   305,   306,   307,   308,   310,
     311,   318,   319,   320,   322,   324,   331,   332,   333,   334,
     335,   336,   339,   340,   341,   343,   344,   351,   354,   355,
     356,   357,   359,   361,   368,   369,   373,   373,   377,   378,
     379,   379,   384,   384,   389,   389,   394,   394,   399,   401,
     402,   403,   403,   405,   406,   408,   410,   412,   414,   416,
     419,   421,   423,   425,   430,   430
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "STOCHASTIC", "CONST", "RATE", "GLOBAL",
  "INIT", "TRUE_TOKEN", "FALSE_TOKEN", "EXP", "MODULE", "ENDMODULE",
  "PNAME", "NAME", "INTEGER", "REAL", "ARROW", "DOTDOT", "IMPLY", "'&'",
  "'|'", "'!'", "'<'", "'>'", "NEQ", "EQ", "GTE", "LTE", "'+'", "'-'",
  "'*'", "'/'", "'='", "';'", "':'", "'['", "']'", "','", "'('", "')'",
  "'W'", "'L'", "'U'", "'P'", "$accept", "file", "$@1",
  "model_or_properties", "model", "$@2", "model_type", "declarations",
  "declaration", "modules", "module_decl", "$@3", "$@4", "substitutions",
  "subst_list", "subst", "variables", "variable_decl", "commands",
  "command", "$@5", "synchronization", "update", "formula", "$@6",
  "distribution", "expr", "rate_expr", "range", "const_expr", "properties",
  "sampling_formula", "$@7", "csl_formula", "$@8", "$@9", "$@10", "$@11",
  "$@12", "path_formula", "real", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
      38,   124,    33,    60,    62,   275,   276,   277,   278,    43,
      45,    42,    47,    61,    59,    58,    91,    93,    44,    40,
      41,    87,    76,    85,    80
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    45,    47,    46,    48,    48,    48,    50,    49,    51,
      52,    52,    53,    53,    53,    53,    54,    54,    56,    55,
      57,    55,    58,    58,    59,    59,    60,    61,    61,    62,
      62,    63,    63,    65,    64,    66,    66,    67,    67,    67,
      68,    68,    69,    68,    68,    68,    68,    68,    68,    68,
      68,    70,    70,    70,    70,    70,    71,    71,    71,    71,
      71,    71,    72,    72,    72,    72,    72,    73,    74,    74,
      74,    74,    74,    74,    75,    75,    77,    76,    78,    78,
      79,    78,    80,    78,    81,    78,    82,    78,    78,    78,
      78,    83,    78,    78,    78,    78,    78,    78,    78,    78,
      84,    84,    84,    84,    85,    85
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     0,     4,     1,
       0,     2,     5,     5,     5,     7,     0,     2,     0,     6,
       0,     9,     0,     1,     1,     3,     3,     0,     2,     4,
       6,     0,     2,     0,     8,     2,     3,     3,     3,     3,
       3,     3,     0,     3,     3,     3,     3,     3,     3,     3,
       3,     1,     4,     6,     6,     6,     1,     1,     3,     3,
       3,     3,     1,     1,     3,     3,     3,     5,     1,     1,
       3,     3,     3,     3,     0,     2,     0,     4,     1,     1,
       0,     7,     0,     7,     0,     7,     0,     7,     3,     3,
       3,     0,     3,     3,     3,     3,     3,     3,     3,     3,
       5,     5,     8,     3,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,    74,     1,     9,    76,     3,     4,     7,     5,
       6,     0,    10,    78,    79,    57,    56,    91,     0,     0,
       0,    75,     0,     0,    16,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    77,     0,     0,     0,    11,
       8,    92,    61,    99,   104,   105,     0,     0,     0,     0,
       0,    93,    96,    98,    95,    94,    58,    59,    60,    97,
      88,    89,    90,     0,     0,     0,   103,     0,     0,     0,
       0,    17,    80,    86,    84,    82,     0,     0,     0,     0,
       0,     0,     0,    18,     0,     0,     0,     0,   101,   100,
       0,     0,     0,     0,     0,     0,    27,     0,     0,     0,
       0,     0,    12,    13,    69,    68,     0,     0,     0,    14,
      20,    31,    81,    87,    85,    83,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    28,     0,   102,    73,     0,
      70,    71,    72,    15,    22,     0,    19,     0,    32,     0,
      67,     0,     0,    23,    24,     0,     0,    35,    42,     0,
       0,     0,     0,     0,     0,     0,    29,    36,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      26,    21,    25,     0,    43,    50,     0,    63,     0,     0,
       0,     0,     0,    51,    62,    40,    41,    44,    47,    49,
      46,    45,    48,    30,     0,     0,     0,     0,     0,    33,
       0,     0,     0,    66,     0,     0,     0,     0,    64,    65,
      52,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    34,    53,    54,    55,    37,    39,    38
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     6,     7,    12,     8,    24,    49,    50,
      81,   106,   133,   152,   153,   154,   121,   135,   136,   148,
     217,   149,   226,   160,   168,   192,    20,   193,   104,   117,
       9,    10,    11,    22,    94,    97,    96,    95,    25,    23,
     194
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -141
static const yytype_int16 yypact[] =
{
    -141,     9,     5,  -141,  -141,  -141,  -141,  -141,  -141,    54,
    -141,    54,  -141,  -141,  -141,  -141,  -141,  -141,    54,   124,
     172,   113,    63,   -11,   202,    54,   139,    95,    28,    28,
      28,    28,    10,    10,    10,    10,    10,    10,    10,    10,
      10,    54,    54,    54,    52,  -141,    58,    61,    67,  -141,
      17,  -141,  -141,  -141,  -141,  -141,    64,    90,   117,   129,
      10,   161,   161,   161,   161,   161,   112,   112,  -141,   161,
     234,  -141,  -141,    28,    28,    28,   113,   116,   140,    59,
     171,  -141,  -141,  -141,  -141,  -141,    99,    54,    54,   156,
     203,    28,   168,   197,    54,    54,    54,    54,   113,   113,
      28,   194,   198,    56,    -1,   220,  -141,   230,   231,   232,
     233,   235,  -141,  -141,  -141,  -141,    56,   107,    56,  -141,
    -141,   257,  -141,  -141,  -141,  -141,    54,   158,    56,    56,
      56,    56,   222,   211,   214,  -141,    74,   113,  -141,   209,
     242,   242,  -141,  -141,   260,   168,  -141,    22,  -141,    15,
    -141,   243,   238,   239,  -141,     0,   241,  -141,  -141,    15,
     154,   186,   265,   268,   260,    56,  -141,  -141,    15,    57,
     153,   103,    15,    15,    10,    10,    10,    10,    10,    10,
    -141,  -141,  -141,   228,  -141,  -141,   244,  -141,    -4,   245,
     246,   247,   252,   229,  -141,  -141,  -141,   161,   161,   161,
     161,   161,   161,  -141,    -4,   189,    -4,    -4,    -4,  -141,
      -4,    -4,   191,  -141,    89,   210,   212,    -8,  -141,  -141,
    -141,    -4,    -4,    -4,   248,    -8,    -7,   193,   195,   205,
      10,    25,    -8,  -141,  -141,  -141,  -141,   161,  -141,  -141
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -141,  -141,  -141,  -141,  -141,  -141,  -141,  -141,  -141,  -141,
    -141,  -141,  -141,  -141,  -141,   118,  -141,  -141,  -141,  -141,
    -141,  -141,  -140,   -69,  -141,  -141,   -18,   -99,   143,   -78,
    -141,  -141,  -141,    14,  -141,  -141,  -141,  -141,  -141,   169,
     -27
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      26,    56,    57,    58,    59,   224,   118,   165,     4,     3,
     187,    54,    55,   232,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    21,    15,    16,    45,   233,    80,    15,
      16,   225,    27,   119,   166,   188,   156,   158,   127,    51,
     132,     5,    86,    54,    55,   232,    87,    88,    89,    60,
     139,   140,   141,   142,   159,    70,    71,    72,    76,   157,
      13,    14,    13,    14,   102,   238,    15,    16,    15,    16,
     114,   115,    77,   111,    17,    78,    17,   172,   173,    73,
      74,    79,    41,    42,    43,   231,   146,   183,    75,   205,
     169,    18,   239,    18,    92,   116,    19,   185,    19,   184,
      82,    98,    99,   195,   196,   212,    44,   214,   215,   216,
     147,   218,   219,   186,    41,    42,    43,   187,    54,    55,
     210,   211,   227,   228,   229,   128,    83,   221,    37,    38,
      39,   161,    41,    42,    43,    53,   129,   130,   131,    52,
     137,   170,   188,    39,   189,   190,   191,    28,    29,    90,
     161,    30,    31,    84,   161,   161,   197,   198,   199,   200,
     201,   202,    32,    33,    34,    85,    35,    36,    37,    38,
      39,   171,    40,    91,   172,   173,   174,   175,   176,    52,
     177,   178,    37,    38,    39,    93,   179,   129,   130,   131,
      37,    38,    39,    52,   100,    32,    33,    34,   138,    35,
      36,    37,    38,    39,   103,    40,    46,    47,    48,   174,
     175,   176,   237,   177,   178,    37,    38,    39,   101,   179,
     210,   211,   210,   211,   210,   211,   210,   211,   112,   213,
     105,   220,   113,   234,   120,   235,   210,   211,   129,   130,
     131,   210,   211,   210,   211,   236,   150,   144,   222,   145,
     223,   129,   130,   131,    42,    43,   143,   129,   130,   131,
     210,   211,   203,   107,   108,   109,   110,   122,   123,   124,
     125,   134,   126,   131,   151,   163,   162,   164,   167,   180,
     181,   230,   182,   204,   206,   207,   208,   209,   155
};

static const yytype_uint8 yycheck[] =
{
      18,    28,    29,    30,    31,    13,     7,     7,     3,     0,
      14,    15,    16,    20,    32,    33,    34,    35,    36,    37,
      38,    39,    40,     9,    14,    15,    37,    34,    11,    14,
      15,    39,    18,    34,    34,    39,    14,    22,   116,    25,
     118,    36,    60,    15,    16,    20,    73,    74,    75,    39,
     128,   129,   130,   131,    39,    41,    42,    43,    44,    37,
       8,     9,     8,     9,    91,    40,    14,    15,    14,    15,
      14,    15,    14,   100,    22,    14,    22,    20,    21,    27,
      28,    14,    19,    20,    21,   225,    12,   165,    36,   188,
     159,    39,   232,    39,    35,    39,    44,    40,    44,   168,
      36,    87,    88,   172,   173,   204,    43,   206,   207,   208,
      36,   210,   211,    10,    19,    20,    21,    14,    15,    16,
      31,    32,   221,   222,   223,    18,    36,    38,    29,    30,
      31,   149,    19,    20,    21,    40,    29,    30,    31,    40,
     126,   159,    39,    31,    41,    42,    43,    23,    24,    33,
     168,    27,    28,    36,   172,   173,   174,   175,   176,   177,
     178,   179,    23,    24,    25,    36,    27,    28,    29,    30,
      31,    17,    33,    33,    20,    21,    23,    24,    25,    40,
      27,    28,    29,    30,    31,    14,    33,    29,    30,    31,
      29,    30,    31,    40,    38,    23,    24,    25,    40,    27,
      28,    29,    30,    31,    36,    33,     4,     5,     6,    23,
      24,    25,   230,    27,    28,    29,    30,    31,    15,    33,
      31,    32,    31,    32,    31,    32,    31,    32,    34,    40,
      33,    40,    34,    40,    14,    40,    31,    32,    29,    30,
      31,    31,    32,    31,    32,    40,    37,    36,    38,    35,
      38,    29,    30,    31,    20,    21,    34,    29,    30,    31,
      31,    32,    34,    94,    95,    96,    97,    37,    37,    37,
      37,    14,    37,    31,    14,    37,    33,    38,    37,    14,
      12,    33,   164,    39,    39,    39,    39,    35,   145
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    46,    47,     0,     3,    36,    48,    49,    51,    75,
      76,    77,    50,     8,     9,    14,    15,    22,    39,    44,
      71,    78,    78,    84,    52,    83,    71,    78,    23,    24,
      27,    28,    23,    24,    25,    27,    28,    29,    30,    31,
      33,    19,    20,    21,    43,    37,     4,     5,     6,    53,
      54,    78,    40,    40,    15,    16,    85,    85,    85,    85,
      39,    71,    71,    71,    71,    71,    71,    71,    71,    71,
      78,    78,    78,    27,    28,    36,    78,    14,    14,    14,
      11,    55,    36,    36,    36,    36,    71,    85,    85,    85,
      33,    33,    35,    14,    79,    82,    81,    80,    78,    78,
      38,    15,    85,    36,    73,    33,    56,    84,    84,    84,
      84,    85,    34,    34,    14,    15,    39,    74,     7,    34,
      14,    61,    37,    37,    37,    37,    37,    74,    18,    29,
      30,    31,    74,    57,    14,    62,    63,    78,    40,    74,
      74,    74,    74,    34,    36,    35,    12,    36,    64,    66,
      37,    14,    58,    59,    60,    73,    14,    37,    22,    39,
      68,    71,    33,    37,    38,     7,    34,    37,    69,    68,
      71,    17,    20,    21,    23,    24,    25,    27,    28,    33,
      14,    12,    60,    74,    68,    40,    10,    14,    39,    41,
      42,    43,    70,    72,    85,    68,    68,    71,    71,    71,
      71,    71,    71,    34,    39,    72,    39,    39,    39,    35,
      31,    32,    72,    40,    72,    72,    72,    65,    72,    72,
      40,    38,    38,    38,    13,    39,    67,    72,    72,    72,
      33,    67,    20,    34,    40,    40,    40,    71,    40,    67
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
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
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


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
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

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
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



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

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
  if (yyn == YYPACT_NINF)
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
      if (yyn == 0 || yyn == YYTABLE_NINF)
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

/* Line 1455 of yacc.c  */
#line 211 "parser.yy"
    { success = true; line_number = 1; negate.push(false); }
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 212 "parser.yy"
    { check_undeclared(); if (!success) YYERROR; }
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 224 "parser.yy"
    { prepare_model(); }
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 225 "parser.yy"
    { global_model = model; }
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 238 "parser.yy"
    { declare_constant((yyvsp[(2) - (5)].str), (yyvsp[(4) - (5)].nat)); }
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 239 "parser.yy"
    { declare_rate((yyvsp[(2) - (5)].str), (yyvsp[(4) - (5)].real)); }
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 240 "parser.yy"
    { declare_variable((yyvsp[(2) - (5)].str), (yyvsp[(4) - (5)].range), 0); }
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 242 "parser.yy"
    { declare_variable((yyvsp[(2) - (7)].str), (yyvsp[(4) - (7)].range), (yyvsp[(6) - (7)].iexpr)); }
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 253 "parser.yy"
    { prepare_module((yyvsp[(2) - (2)].str)); }
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 254 "parser.yy"
    { add_module(); }
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 255 "parser.yy"
    { prepare_substitutions(*(yyvsp[(4) - (4)].str)); }
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 256 "parser.yy"
    { add_module((yyvsp[(2) - (9)].str), (yyvsp[(4) - (9)].str)); }
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 267 "parser.yy"
    { add_substitution((yyvsp[(1) - (3)].str), (yyvsp[(3) - (3)].str)); }
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 274 "parser.yy"
    { declare_variable((yyvsp[(1) - (4)].str), (yyvsp[(3) - (4)].range), 0); }
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 276 "parser.yy"
    { declare_variable((yyvsp[(1) - (6)].str), (yyvsp[(3) - (6)].range), (yyvsp[(5) - (6)].iexpr)); }
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 284 "parser.yy"
    { prepare_command((yyvsp[(1) - (5)].synch), *(yyvsp[(2) - (5)].formula), *(yyvsp[(4) - (5)].dist)); }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 284 "parser.yy"
    { add_command(); }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 287 "parser.yy"
    { (yyval.synch) = 0; }
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 288 "parser.yy"
    { (yyval.synch) = synchronization_value((yyvsp[(2) - (3)].str)); }
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 291 "parser.yy"
    { add_update((yyvsp[(1) - (3)].str), *(yyvsp[(3) - (3)].iexpr)); }
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 300 "parser.yy"
    { (yyval.formula) = make_conjunction(*(yyvsp[(1) - (3)].formula), *(yyvsp[(3) - (3)].formula)); }
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 301 "parser.yy"
    { (yyval.formula) = make_disjunction(*(yyvsp[(1) - (3)].formula), *(yyvsp[(3) - (3)].formula)); }
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 302 "parser.yy"
    { negate.top() = !negate.top(); }
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 303 "parser.yy"
    { (yyval.formula) = (yyvsp[(3) - (3)].formula); negate.top() = !negate.top(); }
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 304 "parser.yy"
    { (yyval.formula) = new LessThan<int>(*(yyvsp[(1) - (3)].iexpr), *(yyvsp[(3) - (3)].iexpr), negate.top()); }
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 305 "parser.yy"
    { (yyval.formula) = new GreaterThan<int>(*(yyvsp[(1) - (3)].iexpr), *(yyvsp[(3) - (3)].iexpr), !negate.top()); }
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 306 "parser.yy"
    { (yyval.formula) = new LessThan<int>(*(yyvsp[(1) - (3)].iexpr), *(yyvsp[(3) - (3)].iexpr), !negate.top()); }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 307 "parser.yy"
    { (yyval.formula) = new GreaterThan<int>(*(yyvsp[(1) - (3)].iexpr), *(yyvsp[(3) - (3)].iexpr), negate.top()); }
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 309 "parser.yy"
    { (yyval.formula) = new Equality<int>(*(yyvsp[(1) - (3)].iexpr), *(yyvsp[(3) - (3)].iexpr), negate.top()); }
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 310 "parser.yy"
    { (yyval.formula) = new Equality<int>(*(yyvsp[(1) - (3)].iexpr), *(yyvsp[(3) - (3)].iexpr), !negate.top()); }
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 311 "parser.yy"
    { (yyval.formula) = (yyvsp[(2) - (3)].formula); }
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 318 "parser.yy"
    { (yyval.dist) = &Exponential::make(*(yyvsp[(1) - (1)].dexpr)); }
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 319 "parser.yy"
    { (yyval.dist) = &Exponential::make(*(yyvsp[(3) - (4)].dexpr)); }
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 321 "parser.yy"
    { (yyval.dist) = &Weibull::make(*(yyvsp[(3) - (6)].dexpr), *(yyvsp[(5) - (6)].dexpr)); }
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 323 "parser.yy"
    { (yyval.dist) = &Lognormal::make(*(yyvsp[(3) - (6)].dexpr), *(yyvsp[(5) - (6)].dexpr)); }
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 325 "parser.yy"
    { (yyval.dist) = &Uniform::make(*(yyvsp[(3) - (6)].dexpr), *(yyvsp[(5) - (6)].dexpr)); }
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 331 "parser.yy"
    { (yyval.iexpr) = make_value((yyvsp[(1) - (1)].nat)); }
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 332 "parser.yy"
    { (yyval.iexpr) = find_named_expression((yyvsp[(1) - (1)].str), false); }
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 333 "parser.yy"
    { (yyval.iexpr) = &Addition<int>::make(*(yyvsp[(1) - (3)].iexpr), *(yyvsp[(3) - (3)].iexpr)); }
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 334 "parser.yy"
    { (yyval.iexpr) = &Subtraction<int>::make(*(yyvsp[(1) - (3)].iexpr), *(yyvsp[(3) - (3)].iexpr)); }
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 335 "parser.yy"
    { (yyval.iexpr) = &Multiplication<int>::make(*(yyvsp[(1) - (3)].iexpr), *(yyvsp[(3) - (3)].iexpr)); }
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 336 "parser.yy"
    { (yyval.iexpr) = (yyvsp[(2) - (3)].iexpr); }
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 339 "parser.yy"
    { (yyval.dexpr) = make_value((yyvsp[(1) - (1)].real)); }
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 340 "parser.yy"
    { (yyval.dexpr) = find_rate((yyvsp[(1) - (1)].str), false); }
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 342 "parser.yy"
    { (yyval.dexpr) = &Multiplication<double>::make(*(yyvsp[(1) - (3)].dexpr), *(yyvsp[(3) - (3)].dexpr)); }
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 343 "parser.yy"
    { (yyval.dexpr) = &Division::make(*(yyvsp[(1) - (3)].dexpr), *(yyvsp[(3) - (3)].dexpr)); }
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 344 "parser.yy"
    { (yyval.dexpr) = (yyvsp[(2) - (3)].dexpr); }
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 351 "parser.yy"
    { (yyval.range) = make_range((yyvsp[(2) - (5)].iexpr), (yyvsp[(4) - (5)].iexpr)); }
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 354 "parser.yy"
    { (yyval.iexpr) = make_value((yyvsp[(1) - (1)].nat)); }
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 355 "parser.yy"
    { (yyval.iexpr) = find_named_expression((yyvsp[(1) - (1)].str), true); }
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 356 "parser.yy"
    { (yyval.iexpr) = &Addition<int>::make(*(yyvsp[(1) - (3)].iexpr), *(yyvsp[(3) - (3)].iexpr)); }
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 358 "parser.yy"
    { (yyval.iexpr) = &Subtraction<int>::make(*(yyvsp[(1) - (3)].iexpr), *(yyvsp[(3) - (3)].iexpr)); }
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 360 "parser.yy"
    { (yyval.iexpr) = &Multiplication<int>::make(*(yyvsp[(1) - (3)].iexpr), *(yyvsp[(3) - (3)].iexpr)); }
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 361 "parser.yy"
    { (yyval.iexpr) = (yyvsp[(2) - (3)].iexpr); }
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 370 "parser.yy"
    { add_property((yyvsp[(2) - (2)].formula)); }
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 373 "parser.yy"
    { if (mode != 2) yyerror("unexpected path formula"); }
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 374 "parser.yy"
    { global_path_formula = (yyvsp[(3) - (4)].path); }
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 377 "parser.yy"
    { (yyval.formula) = make_tautology(); }
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 378 "parser.yy"
    { (yyval.formula) = make_contradiction(); }
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 379 "parser.yy"
    { negate.push(false); }
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 380 "parser.yy"
    {
		  negate.pop();
		  (yyval.formula) = make_probabilistic(!negate.top(), (yyvsp[(3) - (7)].real), false, *(yyvsp[(6) - (7)].path));
		}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 384 "parser.yy"
    { negate.push(false); }
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 385 "parser.yy"
    {
		  negate.pop();
		  (yyval.formula) = make_probabilistic(!negate.top(), (yyvsp[(3) - (7)].real), true, *(yyvsp[(6) - (7)].path));
		}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 389 "parser.yy"
    { negate.push(false); }
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 390 "parser.yy"
    {
		  negate.pop();
		  (yyval.formula) = make_probabilistic(negate.top(), (yyvsp[(3) - (7)].real), false, *(yyvsp[(6) - (7)].path));
		}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 394 "parser.yy"
    { negate.push(false); }
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 395 "parser.yy"
    {
		  negate.pop();
		  (yyval.formula) = make_probabilistic(negate.top(), (yyvsp[(3) - (7)].real), true, *(yyvsp[(6) - (7)].path));
		}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 400 "parser.yy"
    { (yyval.formula) = make_implication(*(yyvsp[(1) - (3)].formula), *(yyvsp[(3) - (3)].formula)); }
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 401 "parser.yy"
    { (yyval.formula) = make_conjunction(*(yyvsp[(1) - (3)].formula), *(yyvsp[(3) - (3)].formula)); }
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 402 "parser.yy"
    { (yyval.formula) = make_disjunction(*(yyvsp[(1) - (3)].formula), *(yyvsp[(3) - (3)].formula)); }
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 403 "parser.yy"
    { negate.top() = !negate.top(); }
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 404 "parser.yy"
    { (yyval.formula) = (yyvsp[(3) - (3)].formula); negate.top() = !negate.top(); }
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 405 "parser.yy"
    { (yyval.formula) = new LessThan<int>(*(yyvsp[(1) - (3)].iexpr), *(yyvsp[(3) - (3)].iexpr), negate.top()); }
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 407 "parser.yy"
    { (yyval.formula) = new GreaterThan<int>(*(yyvsp[(1) - (3)].iexpr), *(yyvsp[(3) - (3)].iexpr), !negate.top()); }
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 409 "parser.yy"
    { (yyval.formula) = new LessThan<int>(*(yyvsp[(1) - (3)].iexpr), *(yyvsp[(3) - (3)].iexpr), !negate.top()); }
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 411 "parser.yy"
    { (yyval.formula) = new GreaterThan<int>(*(yyvsp[(1) - (3)].iexpr), *(yyvsp[(3) - (3)].iexpr), negate.top()); }
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 413 "parser.yy"
    { (yyval.formula) = new Equality<int>(*(yyvsp[(1) - (3)].iexpr), *(yyvsp[(3) - (3)].iexpr), negate.top()); }
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 415 "parser.yy"
    { (yyval.formula) = new Equality<int>(*(yyvsp[(1) - (3)].iexpr), *(yyvsp[(3) - (3)].iexpr), !negate.top()); }
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 416 "parser.yy"
    { (yyval.formula) = (yyvsp[(2) - (3)].formula); }
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 420 "parser.yy"
    { (yyval.path) = make_until(*(yyvsp[(1) - (5)].formula), *(yyvsp[(5) - (5)].formula), 0, (yyvsp[(4) - (5)].real)); }
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 422 "parser.yy"
    { (yyval.path) = make_until(*(yyvsp[(1) - (5)].formula), *(yyvsp[(5) - (5)].formula), (yyvsp[(4) - (5)].real), HUGE_VAL); }
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 424 "parser.yy"
    { (yyval.path) = make_until(*(yyvsp[(1) - (8)].formula), *(yyvsp[(8) - (8)].formula), (yyvsp[(4) - (8)].real), (yyvsp[(6) - (8)].real)); }
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 426 "parser.yy"
    { (yyval.path) = new Until(*(yyvsp[(1) - (3)].formula), *(yyvsp[(3) - (3)].formula), 0.0, HUGE_VAL); }
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 430 "parser.yy"
    { (yyval.real) = (yyvsp[(1) - (1)].nat); }
    break;



/* Line 1455 of yacc.c  */
#line 2333 "parser.cc"
      default: break;
    }
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
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
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
      if (yyn != YYPACT_NINF)
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
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
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



/* Line 1675 of yacc.c  */
#line 433 "parser.yy"


/* Clears all previously parsed declarations. */
void clear_declarations() {
  constants.clear();
  rates.clear();
  variables.clear();
}


/* Tests if the given variables is a member of the given list. */
template<typename T>
static bool member(const VariableList<T>& vars, const Variable<T>* v) {
  return find(vars.begin(), vars.end(), v) != vars.end();
}


/* Outputs an error message. */
static void yyerror(const std::string& s) {
  std::cerr << PACKAGE ":" << current_file << ':' << line_number << ": " << s
	    << std::endl;
  success = false;
}


/* Outputs a warning. */
static void yywarning(const std::string& s) {
  std::cerr << PACKAGE ":" << current_file << ':' << line_number << ": " << s
	    << std::endl;
}


/* Checks if undeclared variables were used. */
static void check_undeclared() {
  negate.pop();
  if (!undeclared.empty()) {
    yyerror("undeclared variables used in expressions");
    for (std::set<std::string>::const_iterator si = undeclared.begin();
	 si != undeclared.end(); si++) {
      std::cerr << "  " << *si << std::endl;
    }
  }
  undeclared.clear();
  modules.clear();
  synchronizations.clear();
}


/* Returns a pointer to a variable or constant with the given name, or 0. */
static const Expression<int>* find_named_expression(const std::string* ident,
						    bool constant) {
  std::map<std::string, const Constant<int>*>::const_iterator ci =
    constants.find(*ident);
  if (ci != constants.end()) {
    delete ident;
    return (*ci).second;
  } else {
    if (constant) {
      if (variables.find(*ident) != variables.end()) {
	yyerror("variable `" + *ident + "' used where expecting constant");
      } else if (rates.find(*ident) != rates.end()) {
	yyerror("rate `" + *ident + "' used where expecting constant");
      } else {
	yyerror("undeclared constant `" + *ident + "'");
      }
      Constant<int>* c = new Constant<int>(*ident, 0);
      constants.insert(std::make_pair(*ident, c));
      delete ident;
      return c;
    } else {
      Variable<int>* v;
      std::map<std::string, Variable<int>*>::const_iterator vi =
	variables.find(*ident);
      if (vi == variables.end()) {
	v = new Variable<int>(*ident, *new Value<int>(0), *new Value<int>(1),
			      *new Value<int>(0));
	variables.insert(std::make_pair(*ident, v));
	undeclared.insert(*ident);
      } else {
	v = (*vi).second;
      }
      delete ident;
      return v;
    }
  }
}


/* Returns a pointer to the rate constant with the given name, or 0. */
static const Expression<double>* find_rate(const std::string* ident,
					   bool constant) {
  std::map<std::string, const Constant<double>*>::const_iterator ri =
    rates.find(*ident);
  if (ri != rates.end()) {
    delete ident;
    return (*ri).second;
  } else {
    return &TypeCast<int, double>::make(*find_named_expression(ident,
							       constant));
  }
}


/* Returns a range with the given bounds. */
static Range make_range(const Expression<int>* l, const Expression<int>* h) {
  Range r = { l, h };
  return r;
}


/* Returns a value expression. */
static const Value<int>* make_value(int n) {
  return new Value<int>(n);
}


/* Returns a value expression. */
static const Value<double>* make_value(double x) {
  return new Value<double>(x);
}


/* Returns a tautology. */ 
static StateFormula* make_tautology() {
  if (negate.top()) {
    return new Disjunction();
  } else {
    return new Conjunction();
  }
}


/* Returns a contradiction. */
static StateFormula* make_contradiction() {
  if (negate.top()) {
    return new Conjunction();
  } else {
    return new Disjunction();
  }
}


/* Returns a conjunction. */
static StateFormula* make_conjunction(StateFormula& f1,
				      const StateFormula& f2) {
  if (negate.top()) {
    Disjunction* disj = dynamic_cast<Disjunction*>(&f1);
    if (disj == 0) {
      disj = new Disjunction();
      disj->add_disjunct(f1);
    }
    disj->add_disjunct(f2);
    return disj;
  } else {
    Conjunction* conj = dynamic_cast<Conjunction*>(&f1);
    if (conj == 0) {
      conj = new Conjunction();
      conj->add_conjunct(f1);
    }
    conj->add_conjunct(f2);
    return conj;
  }
}


/* Returns a disjunction. */
static StateFormula* make_disjunction(StateFormula& f1,
				      const StateFormula& f2) {
  if (negate.top()) {
    Conjunction* conj = dynamic_cast<Conjunction*>(&f1);
    if (conj == 0) {
      conj = new Conjunction();
      conj->add_conjunct(f1);
    }
    conj->add_conjunct(f2);
    return conj;
  } else {
    Disjunction* disj = dynamic_cast<Disjunction*>(&f1);
    if (disj == 0) {
      disj = new Disjunction();
      disj->add_disjunct(f1);
    }
    disj->add_disjunct(f2);
    return disj;
  }
}


/* Negates the given state formula. */
static StateFormula* negation(const StateFormula& f) {
  const Conjunction* cp = dynamic_cast<const Conjunction*>(&f);
  if (cp != 0) {
    Disjunction* disj = new Disjunction();
    for (FormulaList::const_iterator fi = cp->conjuncts().begin();
	 fi != cp->conjuncts().end(); fi++) {
      disj->add_disjunct(*negation(**fi));
    }
    return disj;
  }
  const Disjunction* dp = dynamic_cast<const Disjunction*>(&f);
  if (dp != 0) {
    Conjunction* conj = new Conjunction();
    for (FormulaList::const_iterator fi = dp->disjuncts().begin();
	 fi != dp->disjuncts().end(); fi++) {
      conj->add_conjunct(*negation(**fi));
    }
    return conj;
  }
  const Probabilistic* pp = dynamic_cast<const Probabilistic*>(&f);
  if (pp != 0) {
    return new Probabilistic(!pp->negated(), pp->threshold(), pp->strict(),
			     pp->formula());
  }
  const LessThan<int>* lp = dynamic_cast<const LessThan<int>*>(&f);
  if (lp != 0) {
    return new LessThan<int>(lp->expr1(), lp->expr2(), !lp->negated());
  }
  const GreaterThan<int>* gp = dynamic_cast<const GreaterThan<int>*>(&f);
  if (gp != 0) {
    return new GreaterThan<int>(gp->expr1(), gp->expr2(), !gp->negated());
  }
  const Equality<int>* ep = dynamic_cast<const Equality<int>*>(&f);
  if (ep != 0) {
    return new Equality<int>(ep->expr1(), ep->expr2(), !ep->negated());
  }
  throw std::logic_error("unexpected formula in `negation'");
}


/* Returns an implication. */
static StateFormula* make_implication(const StateFormula& f1,
				      const StateFormula& f2) {
  StateFormula* result;
  if (negate.top()) {
    result = make_conjunction(*negation(f1), f2);
  } else {
    result = make_disjunction(*negation(f1), f2);
  }
  RCObject::ref(&f1);
  RCObject::deref(&f1);
  return result;
}


/* Returns a probabilistic path quantification. */
static StateFormula* make_probabilistic(bool negate, double p, bool strict,
					const PathFormula& f) {
  if (p < 0.0 || p > 1.0) {
    yyerror("probability bound outside the interval [0,1]");
  }
  return new Probabilistic(negate, p, strict, f);
}


/* Returns an until formula. */
static const Until* make_until(const StateFormula& f1, const StateFormula& f2,
			       double t1, double t2) {
  if (t1 < 0) {
    yyerror("negative time bound");
  } else if (t2 < t1) {
    yyerror("empty time interval");
  }
  return new Until(f1, f2, t1, t2);
}


/* Adds an update to the current command. */
static void add_update(const std::string* ident, const Expression<int>& expr) {
  const Variable<int>* v;
  std::map<std::string, Variable<int>*>::const_iterator vi =
    variables.find(*ident);
  if (vi == variables.end()) {
    yyerror("updating undeclared variable `" + *ident + "'");
    v = new Variable<int>(*ident, *new Value<int>(0), *new Value<int>(1),
			  *new Value<int>(0));
  } else {
    v = (*vi).second;
  }
  if (member(model->int_variables(), v)) {
    if (command->synch() != 0) {
      yywarning("updating global variable in synchronized command");
    }
  } else if (!member(module->int_variables(), v)) {
    yyerror("updating variable belonging to other module");
  }
  command->add_update(*new Update<int>(*v, expr));
  delete ident;
}


/* Returns the value of the given synchronization. */
static int synchronization_value(const std::string* ident) {
  int s;
  std::map<std::string, int>::const_iterator si =
    synchronizations.find(*ident);
  if (si == synchronizations.end()) {
    s = synchronizations.size() + 1;
    synchronizations.insert(std::make_pair(*ident, s));
  } else {
    s = (*si).second;
  }
  delete ident;
  return s;
}


/* Prepares a module substitution for parsing. */
static void prepare_substitutions(const std::string& ident) {
  std::map<std::string, Module*>::const_iterator mi = modules.find(ident);
  if (mi != modules.end()) {
    src_module = (*mi).second;
  } else {
    yyerror("ignoring renaming of undeclared module `" + ident + "'");
    src_module = 0;
  }
}


/* Adds a substitution to the current substitution map. */
static void add_substitution(const std::string* ident1,
			     const std::string* ident2) {
  std::map<std::string, Variable<int>*>::const_iterator vi =
    variables.find(*ident1);
  if (vi != variables.end()) {
    /* Variable substitution. */
    const Variable<int>& v1 = *(*vi).second;
    const Variable<int>* v2;
    std::map<std::string, Variable<int>*>::const_iterator vj =
      variables.find(*ident2);
    bool module_v = (src_module != 0
		     && (find(src_module->int_variables().begin(),
			      src_module->int_variables().end(), &v1)
			 != src_module->int_variables().end()));
    /* If v1 is a module variable for src_module, then v2 must be new. */
    if (vj != variables.end()) {
      if (module_v && undeclared.find(*ident2) == undeclared.end()) {
	yyerror("module variable " + v1.name()
		+ " must be substituted with fresh variable");
      }
      v2 = (*vj).second;
    } else {
      if (constants.find(*ident2) != constants.end()
	  || rates.find(*ident2) != rates.end()
	  || synchronizations.find(*ident2) != synchronizations.end()) {
	yyerror("illegal substitution `" + *ident1 + "=" + *ident2 + "'");
      } else if (!module_v) {
	yyerror("fresh variable " + *ident2
		+ " must be substituting module variable");
      }
      Variable<int>* vp2 = new Variable<int>(*ident2,
					     v1.low(), v1.high(), v1.init());
      variables.insert(std::make_pair(*ident2, vp2));
      v2 = vp2;
    }
    subst.insert(v1, *v2);
    delete ident2;
  } else {
    std::map<std::string, const Constant<int>*>::const_iterator ci =
      constants.find(*ident1);
    if (ci != constants.end()) {
      /* Constant substitution. */
      const Constant<int>& c1 = *(*ci).second;
      const Constant<int>* c2;
      std::map<std::string, const Constant<int>*>::const_iterator cj =
	constants.find(*ident2);
      if (cj != constants.end()) {
	c2 = (*cj).second;
      } else {
	yyerror("undeclared constant `" + *ident2 + "'");
	c2 = new Constant<int>(*ident2, c1.value());
	constants.insert(std::make_pair(*ident2, c2));
      }
      subst.insert(c1, *c2);
      delete ident2;
    } else {
      std::map<std::string, const Constant<double>*>::const_iterator ri =
	rates.find(*ident1);
      if (ri != rates.end()) {
	/* Rate substitution. */
	const Constant<double>& r1 = *(*ri).second;
	const Constant<double>* r2;
	std::map<std::string, const Constant<double>*>::const_iterator rj =
	  rates.find(*ident2);
	if (rj != rates.end()) {
	  r2 = (*rj).second;
	} else {
	  yyerror("undeclared rate `" + *ident2 + "'");
	  r2 = new Constant<double>(*ident2, r1.value());
	  rates.insert(std::make_pair(*ident2, r2));
	}
	subst.insert(r1, *r2);
	delete ident2;
      } else {
	std::map<std::string, int>::const_iterator si =
	  synchronizations.find(*ident1);
	if (si != synchronizations.end()) {
	  /* Synchronization substitution. */
	  int s = synchronization_value(ident2);
	  synch_subst.insert(std::make_pair((*si).second, s));
	} else {
	  yyerror("illegal substitution `" + *ident1 + "=" + *ident2 + "'");
	  delete ident2;
	}
      }
    }
  }
  delete ident1;
}


/* Declares an integer constant. */
static void declare_constant(const std::string* ident, int value) {
  if (constants.find(*ident) != constants.end()) {
    yyerror("ignoring repeated declaration of constant `" + *ident + "'");
  } else if (rates.find(*ident) != rates.end()) {
    yyerror("ignoring declaration of constant `" + *ident
	    + "' previously declared as rate");
  } else if (variables.find(*ident) != variables.end()) {
    yyerror("ignoring declaration of constant `" + *ident
	    + "' previously declared as variable");
  } else {
    const Constant<int>* c = new Constant<int>(*ident, value);
    constants.insert(std::make_pair(*ident, c));
    model->add_constant(*c);
  }
  delete ident;
}


/* Declares a rate constant. */
static void declare_rate(const std::string* ident, double value) {
  if (rates.find(*ident) != rates.end()) {
    yyerror("ignoring repeated declaration of rate `" + *ident + "'");
  } else if (constants.find(*ident) != constants.end()) {
    yyerror("ignoring declaration of rate `" + *ident
	    + "' previously declared as constant");
  } else if (variables.find(*ident) != variables.end()) {
    yyerror("ignoring declaration of rate `" + *ident
	    + "' previously declared as variable");
  } else {
    const Constant<double>* r = new Constant<double>(*ident, value);
    rates.insert(std::make_pair(*ident, r));
    model->add_constant(*r);
  }
  delete ident;
}


/* Declares a variable. */
static const Variable<int>* declare_variable(const std::string* ident,
					     const Range& range,
					     const Expression<int>* init) {
  Variable<int>* v = 0;
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
    std::map<std::string, Variable<int>*>::const_iterator vi =
      variables.find(*ident);
    if (vi != variables.end()) {
      if (undeclared.find(*ident) != undeclared.end()) {
	v = (*vi).second;
	v->set_low(*range.l);
	v->set_high(*range.h);
	if (init != 0) {
	  v->set_init(*init);
	} else {
	  v->set_init(*range.l);
	}
	undeclared.erase(*ident);
      } else {
	yyerror("ignoring repeated declaration of variable `" + *ident + "'");
      }
    } else {
      v = new Variable<int>(*ident, *range.l, *range.h,
			    ((init != 0) ? *init : *range.l));
      variables.insert(std::make_pair(*ident, v));
    }
  }
  if (v != 0) {
    if (module != 0) {
      module->add_variable(*v);
    } else {
      model->add_variable(*v);
    }
  } else {
    RCObject::ref(range.l);
    RCObject::deref(range.l);
    RCObject::ref(range.h);
    RCObject::deref(range.h);
    if (init != 0) {
      RCObject::ref(init);
      RCObject::deref(init);
    }
  }
  delete ident;
  return v;
}


/* Adds a command to the current module. */
static void add_command() {
  module->add_command(*command);
}


/* Prepares a command for parsing. */
static void prepare_command(int synch, const StateFormula& guard,
			    const Distribution& delay) {
  command = new Command(synch, guard, delay);
}


/* Adds a module to the current model defined by renaming. */
static void add_module(const std::string* ident1, const std::string* ident2) {
  std::map<std::string, Module*>::const_iterator mi = modules.find(*ident1);
  if (mi != modules.end()) {
    yyerror("ignoring repeated declaration of module `" + *ident1 + "'");
  } else {
    if (src_module == 0) {
      yyerror("ignoring renaming of undeclared module `" + *ident2 + "'");
    } else {
      for (VariableList<int>::const_iterator vi =
	     src_module->int_variables().begin();
	   vi != src_module->int_variables().end(); vi++) {
	const Variable<int>* vj = subst.find(**vi);
	if (vj == 0) {
	  yyerror("missing substitution for module variable `"
		  + (*vi)->name() + "'");
	} else {
	  std::map<std::string, Variable<int>*>::const_iterator vk =
	    variables.find(vj->name());
	  if (vk == variables.end()) {
	    throw std::logic_error("missing variable `" + vj->name() + "'");
	  }
	  Variable<int>& sv = *(*vk).second;
	  sv.set_low((*vi)->low().substitution(subst));
	  sv.set_high((*vi)->high().substitution(subst));
	  sv.set_init((*vi)->init().substitution(subst));
	}
      }
      Module* mod = &src_module->substitution(*ident1, subst, synch_subst);
      modules.insert(std::make_pair(*ident1, mod));
      model->add_module(*mod);
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
  module = 0;
}


/* Prepares a module for parsing. */
static void prepare_module(const std::string* ident) {
  std::map<std::string, Module*>::const_iterator mi = modules.find(*ident);
  if (mi != modules.end()) {
    yyerror("ignoring repeated declaration of module `" + *ident + "'");
  } else {
    module = new Module(*ident);
    modules.insert(std::make_pair(*ident, module));
  }
  delete ident;
}


/* Prepares a model for parsing. */
static void prepare_model() {
  if (mode == 2) {
    yyerror("expecting path formula");
  }
  if (model != 0) {
    delete model;
  }
  for (FormulaList::const_iterator fi = properties.begin();
       fi != properties.end(); fi++) {
    StateFormula::deref(*fi);
  }
  properties.clear();
  clear_declarations();
  model = new Model();
}


/* Adds a parsed property. */
static void add_property(const StateFormula* f) {
  if (mode != 0) {
    yyerror("expecting model or path formula");
  }
  properties.push_back(f);
  StateFormula::ref(f);
}


/* Parses a model. */
int parse_model() {
  mode = 1;
  int result = yyparse();
  mode = 0;
  return result;
}


/* Parses a path formula. */
int parse_path_formula() {
  mode = 2;
  RCObject::deref(global_path_formula);
  int result = yyparse();
  RCObject::ref(global_path_formula);
  mode = 0;
  return result;
}


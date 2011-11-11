
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
#line 25 "parser.yy"

#include <config.h>
#include "models.h"
#include "distributions.h"
#include "formulas.h"
#include <algorithm>
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

/* Last model parsed. */
const Model* global_model = NULL;
/* Number of bits required by binary encoding of state space. */
int num_model_bits;
/* Parsed properties. */
FormulaList properties;

/* Current model. */
static Model* model;
/* Current module. */
static Module* module;
/* Current variable substitutions. */
static std::map<const Variable*, const std::string*> subst;
/* Current synchronization substitutions. */
static SynchSubstitutionMap synch_subst;
/* Current command. */
static Command* command;
/* Declared integer constants. */
static std::map<std::string, const Variable*> constants;
/* Constant values. */
static ValueMap constant_values;
/* Declared rate constants. */
static std::map<std::string, const Variable*> rates;
/* Rate values. */
static ValueMap rate_values;
/* All state variables. */
static std::map<std::string, Variable*> variables;
/* Variables lows. */
static std::map<const Variable*, const Expression*> variable_lows;
/* Variables highs. */
static std::map<const Variable*, const Expression*> variable_highs;
/* Variables starts. */
static std::map<const Variable*, const Expression*> variable_starts;
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
/* Returns the numerator of the given rational, signaling an error if
   the denominator is not 1. */
static int integer_value(const Rational* q);
/* Returns a variable representing an integer constant. */
static const Variable* find_constant(const std::string* ident);
/* Returns a variable representing a rate constant. */
static const Variable* find_rate(const std::string* ident);
/* Returns a range with the given bounds, signaling an error if the
   range is empty. */
static Range make_range(const Expression* l, const Expression* h);
/* Returns a value expression. */
static const Value* make_value(int n);
/* Returns a value expression. */
static const Value* make_value(const Rational* q); 
/* Returns a constant value or a variable for the given identifier. */
static const Expression* value_or_variable(const std::string* ident);
/* Returns a variable for the given identifier. */
static const Variable* find_variable(const std::string* ident);
/* Returns a conjunction. */
static Conjunction* make_conjunction(StateFormula& f1,
				     const StateFormula& f2);
/* Returns a disjunction. */
static Disjunction* make_disjunction(StateFormula& f1,
				     const StateFormula& f2);
/* Returns a probabilistic path quantification. */
static StateFormula* make_probabilistic(const Rational* p,
					bool strict, bool negate,
					const PathFormula& f);
/* Returns an until formula. */
static const Until* make_until(const StateFormula& f1, const StateFormula& f2,
			       const Rational* t1, const Rational* t2);
/* Adds an update to the current command. */
static void add_update(const std::string* ident, const Expression& expr);
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
static const Variable* declare_variable(const std::string* ident,
					const Range& range,
					const Expression* start,
					bool delayed_addition = false);
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
/* Compiles the current model. */
static void compile_model();


/* Line 189 of yacc.c  */
#line 220 "parser.cc"

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
     INT = 261,
     DOUBLE = 262,
     RATE = 263,
     GLOBAL = 264,
     INIT = 265,
     TRUE_TOKEN = 266,
     FALSE_TOKEN = 267,
     EXP = 268,
     MODULE = 269,
     ENDMODULE = 270,
     PNAME = 271,
     NAME = 272,
     NUMBER = 273,
     ARROW = 274,
     DOTDOT = 275,
     ILLEGAL_TOKEN = 276,
     IMPLY = 277,
     NEQ = 278,
     EQ = 279,
     GTE = 280,
     LTE = 281
   };
#endif
/* Tokens.  */
#define STOCHASTIC 258
#define CTMC 259
#define CONST_TOKEN 260
#define INT 261
#define DOUBLE 262
#define RATE 263
#define GLOBAL 264
#define INIT 265
#define TRUE_TOKEN 266
#define FALSE_TOKEN 267
#define EXP 268
#define MODULE 269
#define ENDMODULE 270
#define PNAME 271
#define NAME 272
#define NUMBER 273
#define ARROW 274
#define DOTDOT 275
#define ILLEGAL_TOKEN 276
#define IMPLY 277
#define NEQ 278
#define EQ 279
#define GTE 280
#define LTE 281




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 187 "parser.yy"

  size_t synch;
  StateFormula* formula;
  const PathFormula* path;
  const Distribution* dist;
  const Expression* expr;
  Range range;
  int nat;
  const std::string* str;
  const Rational* num;



/* Line 214 of yacc.c  */
#line 322 "parser.cc"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 334 "parser.cc"

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
#define YYLAST   336

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  48
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  34
/* YYNRULES -- Number of rules.  */
#define YYNRULES  106
/* YYNRULES -- Number of states.  */
#define YYNSTATES  254

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   281

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    25,     2,     2,     2,     2,    23,     2,
      42,    43,    34,    32,    41,    33,     2,    35,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    38,    37,
      26,    36,    27,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,    45,     2,     2,     2,
      47,     2,     2,     2,     2,    46,     2,    44,     2,     2,
       2,    39,     2,    40,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    24,     2,     2,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20,    21,    22,    28,    29,
      30,    31
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,     9,    11,    12,    17,    19,
      21,    22,    25,    31,    38,    44,    51,    57,    65,    66,
      69,    70,    77,    86,    87,    89,    91,    95,    99,   100,
     103,   108,   115,   116,   119,   120,   129,   132,   136,   140,
     144,   148,   152,   156,   159,   163,   167,   171,   175,   179,
     183,   187,   189,   194,   201,   208,   215,   217,   219,   223,
     227,   231,   235,   237,   239,   243,   247,   251,   253,   255,
     259,   263,   267,   273,   275,   277,   281,   285,   289,   293,
     295,   296,   299,   301,   303,   310,   317,   324,   331,   335,
     339,   343,   346,   350,   354,   358,   362,   366,   370,   374,
     380,   389,   391,   393,   397,   401,   405
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      49,     0,    -1,    -1,    50,    51,    -1,    52,    -1,    78,
      -1,    -1,    54,    53,    55,    57,    -1,     3,    -1,     4,
      -1,    -1,    55,    56,    -1,     5,    17,    36,    76,    37,
      -1,     5,     6,    17,    36,    76,    37,    -1,     8,    17,
      36,    74,    37,    -1,     5,     7,    17,    36,    74,    37,
      -1,     9,    17,    38,    75,    37,    -1,     9,    17,    38,
      75,    10,    76,    37,    -1,    -1,    57,    58,    -1,    -1,
      14,    17,    59,    63,    65,    15,    -1,    14,    17,    36,
      17,    39,    60,    40,    15,    -1,    -1,    61,    -1,    62,
      -1,    61,    41,    62,    -1,    17,    36,    17,    -1,    -1,
      63,    64,    -1,    17,    38,    75,    37,    -1,    17,    38,
      75,    10,    76,    37,    -1,    -1,    65,    66,    -1,    -1,
      68,    70,    19,    71,    38,    67,    69,    37,    -1,    39,
      40,    -1,    39,    17,    40,    -1,    16,    36,    72,    -1,
      69,    23,    69,    -1,    42,    69,    43,    -1,    70,    23,
      70,    -1,    70,    24,    70,    -1,    25,    70,    -1,    72,
      26,    72,    -1,    72,    31,    72,    -1,    72,    30,    72,
      -1,    72,    27,    72,    -1,    72,    36,    72,    -1,    72,
      28,    72,    -1,    42,    70,    43,    -1,    73,    -1,    13,
      42,    73,    43,    -1,    44,    42,    74,    41,    74,    43,
      -1,    45,    42,    74,    41,    74,    43,    -1,    46,    42,
      74,    41,    74,    43,    -1,    77,    -1,    17,    -1,    72,
      32,    72,    -1,    72,    33,    72,    -1,    72,    34,    72,
      -1,    42,    72,    43,    -1,    18,    -1,    17,    -1,    73,
      34,    73,    -1,    73,    35,    73,    -1,    42,    73,    43,
      -1,    18,    -1,    17,    -1,    74,    34,    74,    -1,    74,
      35,    74,    -1,    42,    74,    43,    -1,    39,    76,    20,
      76,    40,    -1,    77,    -1,    17,    -1,    76,    32,    76,
      -1,    76,    33,    76,    -1,    76,    34,    76,    -1,    42,
      76,    43,    -1,    18,    -1,    -1,    78,    79,    -1,    11,
      -1,    12,    -1,    47,    26,    18,    39,    80,    40,    -1,
      47,    31,    18,    39,    80,    40,    -1,    47,    30,    18,
      39,    80,    40,    -1,    47,    27,    18,    39,    80,    40,
      -1,    79,    22,    79,    -1,    79,    23,    79,    -1,    79,
      24,    79,    -1,    25,    79,    -1,    81,    26,    81,    -1,
      81,    31,    81,    -1,    81,    30,    81,    -1,    81,    27,
      81,    -1,    81,    36,    81,    -1,    81,    28,    81,    -1,
      42,    79,    43,    -1,    79,    46,    31,    18,    79,    -1,
      79,    46,    39,    18,    41,    18,    40,    79,    -1,    77,
      -1,    17,    -1,    81,    32,    81,    -1,    81,    33,    81,
      -1,    81,    34,    81,    -1,    42,    81,    43,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   211,   211,   211,   215,   216,   223,   223,   227,   227,
     233,   234,   237,   238,   240,   241,   243,   244,   252,   253,
     256,   256,   258,   262,   263,   266,   267,   270,   273,   274,
     277,   278,   282,   283,   287,   286,   290,   291,   294,   295,
     296,   303,   304,   305,   306,   307,   308,   309,   310,   311,
     312,   319,   320,   321,   323,   325,   332,   333,   334,   335,
     336,   337,   340,   341,   342,   343,   344,   347,   348,   349,
     351,   353,   360,   363,   364,   365,   366,   367,   369,   372,
     379,   380,   384,   385,   386,   388,   390,   392,   394,   395,
     396,   397,   398,   399,   400,   401,   402,   403,   404,   407,
     409,   414,   415,   416,   417,   418,   419
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "STOCHASTIC", "CTMC", "CONST_TOKEN",
  "INT", "DOUBLE", "RATE", "GLOBAL", "INIT", "TRUE_TOKEN", "FALSE_TOKEN",
  "EXP", "MODULE", "ENDMODULE", "PNAME", "NAME", "NUMBER", "ARROW",
  "DOTDOT", "ILLEGAL_TOKEN", "IMPLY", "'&'", "'|'", "'!'", "'<'", "'>'",
  "NEQ", "EQ", "GTE", "LTE", "'+'", "'-'", "'*'", "'/'", "'='", "';'",
  "':'", "'['", "']'", "','", "'('", "')'", "'W'", "'L'", "'U'", "'P'",
  "$accept", "file", "$@1", "model_or_properties", "model", "$@2",
  "model_type", "declarations", "declaration", "modules", "module_decl",
  "$@3", "substitutions", "subst_list", "subst", "variables",
  "variable_decl", "commands", "command", "$@4", "synchronization",
  "update", "formula", "distribution", "expr", "rate_expr",
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
     275,   276,   277,    38,   124,    33,    60,    62,   278,   279,
     280,   281,    43,    45,    42,    47,    61,    59,    58,    91,
      93,    44,    40,    41,    87,    76,    85,    80
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    48,    50,    49,    51,    51,    53,    52,    54,    54,
      55,    55,    56,    56,    56,    56,    56,    56,    57,    57,
      59,    58,    58,    60,    60,    61,    61,    62,    63,    63,
      64,    64,    65,    65,    67,    66,    68,    68,    69,    69,
      69,    70,    70,    70,    70,    70,    70,    70,    70,    70,
      70,    71,    71,    71,    71,    71,    72,    72,    72,    72,
      72,    72,    73,    73,    73,    73,    73,    74,    74,    74,
      74,    74,    75,    76,    76,    76,    76,    76,    76,    77,
      78,    78,    79,    79,    79,    79,    79,    79,    79,    79,
      79,    79,    79,    79,    79,    79,    79,    79,    79,    80,
      80,    81,    81,    81,    81,    81,    81
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     0,     4,     1,     1,
       0,     2,     5,     6,     5,     6,     5,     7,     0,     2,
       0,     6,     8,     0,     1,     1,     3,     3,     0,     2,
       4,     6,     0,     2,     0,     8,     2,     3,     3,     3,
       3,     3,     3,     2,     3,     3,     3,     3,     3,     3,
       3,     1,     4,     6,     6,     6,     1,     1,     3,     3,
       3,     3,     1,     1,     3,     3,     3,     1,     1,     3,
       3,     3,     5,     1,     1,     3,     3,     3,     3,     1,
       0,     2,     1,     1,     6,     6,     6,     6,     3,     3,
       3,     2,     3,     3,     3,     3,     3,     3,     3,     5,
       8,     1,     1,     3,     3,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,    80,     1,     8,     9,     3,     4,     6,     5,
      10,    82,    83,   102,    79,     0,     0,     0,   101,    81,
       0,    18,    91,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    11,     7,    98,   106,     0,     0,
       0,     0,    88,    89,    90,     0,    92,    95,    97,    94,
      93,   103,   104,   105,    96,     0,     0,     0,     0,     0,
       0,    19,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    20,     0,     0,     0,     0,     0,     0,     0,
      74,     0,     0,    73,    68,    67,     0,     0,     0,     0,
       0,    28,     0,    84,    87,    86,    85,     0,     0,     0,
       0,     0,     0,    12,     0,     0,     0,    14,     0,     0,
      16,     0,    32,     0,     0,    13,    15,    78,    75,    76,
      77,    71,    69,    70,     0,     0,    23,     0,    29,     0,
       0,     0,     0,    17,     0,     0,    24,    25,     0,    21,
       0,    33,     0,    99,     0,    72,     0,     0,     0,     0,
       0,    36,    57,     0,     0,     0,     0,    56,     0,    27,
      22,    26,     0,    30,    37,    43,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    50,    61,     0,    63,    62,     0,     0,     0,
       0,     0,    51,    41,    42,     0,    44,    47,    49,    46,
      45,    58,    59,    60,    48,   100,    31,     0,     0,     0,
       0,     0,    34,     0,     0,     0,     0,    66,     0,     0,
       0,     0,    64,    65,    52,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    35,    53,    54,
      55,    38,    40,    39
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     6,     7,    10,     8,    21,    44,    45,
      71,   101,   145,   146,   147,   122,   138,   139,   151,   231,
     152,   240,   165,   201,   166,   202,    97,    99,    92,    18,
       9,    83,    84,    20
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -149
static const yytype_int16 yypact[] =
{
    -149,     3,    96,  -149,  -149,  -149,  -149,  -149,  -149,   104,
    -149,  -149,  -149,  -149,  -149,   104,   104,   246,  -149,   287,
     183,   295,  -149,   102,   134,    -6,    -4,    20,    63,   104,
     104,   104,    38,    38,    38,    38,    38,    38,    38,    38,
      38,   195,    30,    66,  -149,    73,  -149,  -149,   105,   108,
     118,   124,   181,  -149,  -149,    38,   280,   280,   280,   280,
     280,    83,    83,  -149,   280,   170,   191,   138,   182,   185,
     212,  -149,   104,   104,   104,   104,   157,   205,   207,    40,
      61,   200,   209,   130,   217,   226,   234,   235,    40,    61,
    -149,    40,   247,  -149,  -149,  -149,    61,   233,    40,     5,
     243,  -149,    59,  -149,  -149,  -149,  -149,   253,   271,   160,
      40,    40,    40,  -149,   197,    61,    61,  -149,   139,    40,
    -149,   239,   266,   264,   270,  -149,  -149,  -149,   255,   255,
    -149,  -149,  -149,  -149,    40,   259,   277,   257,  -149,    -5,
     104,   260,   219,  -149,   285,   267,   281,  -149,   200,  -149,
       0,  -149,     8,   287,   305,  -149,   307,   310,   277,    14,
     286,  -149,  -149,     8,     8,    29,   194,  -149,   288,  -149,
    -149,  -149,    40,  -149,  -149,  -149,   132,   152,    28,     8,
       8,    76,    76,    76,    76,    76,    76,    76,    76,    76,
     104,   265,  -149,  -149,   289,  -149,  -149,    78,   290,   291,
     292,   297,    93,  -149,  -149,    76,   283,   283,   283,   283,
     283,   293,   293,  -149,   283,   287,  -149,    78,   199,    61,
      61,    61,  -149,    78,    78,   164,   201,  -149,   220,   228,
     230,   -15,  -149,  -149,  -149,    61,    61,    61,   294,   -15,
     -18,   203,   213,   215,    76,    48,   -15,  -149,  -149,  -149,
    -149,   283,  -149,  -149
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,
    -149,  -149,  -149,  -149,   171,  -149,  -149,  -149,  -149,  -149,
    -149,   -40,   -78,  -149,  -121,  -148,   -87,   188,   -80,   -75,
    -149,    -9,   245,   103
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      19,   238,   108,     3,    93,   246,    22,    23,   107,   114,
     149,   109,    48,    93,    49,   119,    93,   160,   118,   247,
      52,    53,    54,    93,   172,   162,    14,   239,   132,   133,
     128,   129,   130,   163,   150,    93,    93,    93,    50,   135,
     161,   194,   120,   177,    93,   195,   196,    68,   178,   218,
     164,   173,   179,   180,   142,    13,    14,    90,    14,    93,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   226,
     197,   246,   198,   199,   200,   232,   233,   167,    94,    95,
      55,    51,    91,    69,   225,   175,   176,    70,   167,   167,
     123,   252,   191,   162,    14,   195,   196,    93,   124,     4,
       5,   203,   204,    96,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,    11,    12,    39,   205,    24,
     197,    13,    14,   251,    29,    30,    31,   223,   224,    15,
     167,   153,   228,   229,   230,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    72,    46,    16,    73,   241,   242,
     243,    17,    29,    30,    31,   179,   180,    74,    76,   134,
      32,    33,    34,    75,    35,    36,    37,    38,    39,   167,
      40,   110,   111,   112,    79,   192,   102,    47,   181,   182,
     183,   215,   184,   185,   186,   187,   188,    77,   189,    37,
      38,    39,   110,   111,   112,   193,   186,   187,   188,   245,
      47,    65,    66,   127,    30,    31,   253,   193,    78,    32,
      33,    34,    67,    35,    36,    37,    38,    39,    80,    40,
     181,   182,   183,    81,   184,   185,   186,   187,   188,    82,
     189,   115,   116,   223,   224,   223,   224,   115,   116,    98,
     131,    88,   227,    89,   234,   100,   248,   115,   116,   115,
     116,   110,   111,   112,   115,   116,   249,   103,   250,   155,
     121,   235,   115,   116,   115,   116,   104,   115,   116,   236,
     117,   237,    25,    26,   105,   106,    27,    28,   136,   110,
     111,   112,   140,   137,   113,   110,   111,   112,   141,   112,
     125,   110,   111,   112,   144,   148,   143,   110,   111,   112,
      41,   154,   216,    42,    43,   115,   116,   157,   126,    29,
      30,    31,    37,    38,    39,   186,   187,   188,    85,    86,
      87,   156,   158,   168,   169,   170,   174,   188,   190,   171,
     244,   217,   219,   220,   221,   222,   159
};

static const yytype_uint8 yycheck[] =
{
       9,    16,    89,     0,    79,    23,    15,    16,    88,    96,
      15,    91,    18,    88,    18,    10,    91,    17,    98,    37,
      29,    30,    31,    98,    10,    17,    18,    42,   115,   116,
     110,   111,   112,    25,    39,   110,   111,   112,    18,   119,
      40,    13,    37,   164,   119,    17,    18,    17,    19,   197,
      42,    37,    23,    24,   134,    17,    18,    17,    18,   134,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   217,
      42,    23,    44,    45,    46,   223,   224,   152,    17,    18,
      42,    18,    42,    17,   205,   163,   164,    14,   163,   164,
      31,    43,   172,    17,    18,    17,    18,   172,    39,     3,
       4,   179,   180,    42,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,    11,    12,    34,    42,    16,
      42,    17,    18,   244,    22,    23,    24,    34,    35,    25,
     205,   140,   219,   220,   221,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    39,    43,    42,    39,   235,   236,
     237,    47,    22,    23,    24,    23,    24,    39,    55,    20,
      26,    27,    28,    39,    30,    31,    32,    33,    34,   244,
      36,    32,    33,    34,    36,    43,    46,    43,    26,    27,
      28,   190,    30,    31,    32,    33,    34,    17,    36,    32,
      33,    34,    32,    33,    34,    43,    32,    33,    34,   239,
      43,     6,     7,    43,    23,    24,   246,    43,    17,    26,
      27,    28,    17,    30,    31,    32,    33,    34,    36,    36,
      26,    27,    28,    38,    30,    31,    32,    33,    34,    17,
      36,    34,    35,    34,    35,    34,    35,    34,    35,    39,
      43,    36,    43,    36,    43,    36,    43,    34,    35,    34,
      35,    32,    33,    34,    34,    35,    43,    40,    43,    40,
      17,    41,    34,    35,    34,    35,    40,    34,    35,    41,
      37,    41,    26,    27,    40,    40,    30,    31,    39,    32,
      33,    34,    18,    17,    37,    32,    33,    34,    18,    34,
      37,    32,    33,    34,    17,    38,    37,    32,    33,    34,
       5,    41,    37,     8,     9,    34,    35,    40,    37,    22,
      23,    24,    32,    33,    34,    32,    33,    34,    73,    74,
      75,    36,    41,    18,    17,    15,    40,    34,    40,   158,
      36,    42,    42,    42,    42,    38,   148
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    49,    50,     0,     3,     4,    51,    52,    54,    78,
      53,    11,    12,    17,    18,    25,    42,    47,    77,    79,
      81,    55,    79,    79,    81,    26,    27,    30,    31,    22,
      23,    24,    26,    27,    28,    30,    31,    32,    33,    34,
      36,     5,     8,     9,    56,    57,    43,    43,    18,    18,
      18,    18,    79,    79,    79,    42,    81,    81,    81,    81,
      81,    81,    81,    81,    81,     6,     7,    17,    17,    17,
      14,    58,    39,    39,    39,    39,    81,    17,    17,    36,
      36,    38,    17,    79,    80,    80,    80,    80,    36,    36,
      17,    42,    76,    77,    17,    18,    42,    74,    39,    75,
      36,    59,    46,    40,    40,    40,    40,    76,    74,    76,
      32,    33,    34,    37,    74,    34,    35,    37,    76,    10,
      37,    17,    63,    31,    39,    37,    37,    43,    76,    76,
      76,    43,    74,    74,    20,    76,    39,    17,    64,    65,
      18,    18,    76,    37,    17,    60,    61,    62,    38,    15,
      39,    66,    68,    79,    41,    40,    36,    40,    41,    75,
      17,    40,    17,    25,    42,    70,    72,    77,    18,    17,
      15,    62,    10,    37,    40,    70,    70,    72,    19,    23,
      24,    26,    27,    28,    30,    31,    32,    33,    34,    36,
      40,    76,    43,    43,    13,    17,    18,    42,    44,    45,
      46,    71,    73,    70,    70,    42,    72,    72,    72,    72,
      72,    72,    72,    72,    72,    79,    37,    42,    73,    42,
      42,    42,    38,    34,    35,    72,    73,    43,    74,    74,
      74,    67,    73,    73,    43,    41,    41,    41,    16,    42,
      69,    74,    74,    74,    36,    69,    23,    37,    43,    43,
      43,    72,    43,    69
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
    { success = true; line_number = 1; }
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 212 "parser.yy"
    { check_undeclared(); if (!success) YYERROR; }
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 223 "parser.yy"
    { prepare_model(); }
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 224 "parser.yy"
    { compile_model(); }
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 237 "parser.yy"
    { declare_constant((yyvsp[(2) - (5)].str), (yyvsp[(4) - (5)].expr)); }
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 239 "parser.yy"
    { declare_constant((yyvsp[(3) - (6)].str), (yyvsp[(5) - (6)].expr)); }
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 240 "parser.yy"
    { declare_rate((yyvsp[(2) - (5)].str), (yyvsp[(4) - (5)].expr)); }
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 242 "parser.yy"
    { declare_rate((yyvsp[(3) - (6)].str), (yyvsp[(5) - (6)].expr)); }
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 243 "parser.yy"
    { declare_variable((yyvsp[(2) - (5)].str), (yyvsp[(4) - (5)].range), NULL); }
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 245 "parser.yy"
    { declare_variable((yyvsp[(2) - (7)].str), (yyvsp[(4) - (7)].range), (yyvsp[(6) - (7)].expr)); }
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 256 "parser.yy"
    { prepare_module((yyvsp[(2) - (2)].str)); }
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 257 "parser.yy"
    { add_module(); }
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 259 "parser.yy"
    { add_module((yyvsp[(2) - (8)].str), (yyvsp[(4) - (8)].str)); }
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 270 "parser.yy"
    { add_substitution((yyvsp[(1) - (3)].str), (yyvsp[(3) - (3)].str)); }
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 277 "parser.yy"
    { declare_variable((yyvsp[(1) - (4)].str), (yyvsp[(3) - (4)].range), NULL); }
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 279 "parser.yy"
    { declare_variable((yyvsp[(1) - (6)].str), (yyvsp[(3) - (6)].range), (yyvsp[(5) - (6)].expr)); }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 287 "parser.yy"
    { prepare_command((yyvsp[(1) - (5)].synch), *(yyvsp[(2) - (5)].formula), *(yyvsp[(4) - (5)].dist)); }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 287 "parser.yy"
    { add_command(); }
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 290 "parser.yy"
    { (yyval.synch) = 0; }
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 291 "parser.yy"
    { (yyval.synch) = synchronization_value((yyvsp[(2) - (3)].str)); }
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 294 "parser.yy"
    { add_update((yyvsp[(1) - (3)].str), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 303 "parser.yy"
    { (yyval.formula) = make_conjunction(*(yyvsp[(1) - (3)].formula), *(yyvsp[(3) - (3)].formula)); }
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 304 "parser.yy"
    { (yyval.formula) = make_disjunction(*(yyvsp[(1) - (3)].formula), *(yyvsp[(3) - (3)].formula)); }
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 305 "parser.yy"
    { (yyval.formula) = new Negation(*(yyvsp[(2) - (2)].formula)); }
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 306 "parser.yy"
    { (yyval.formula) = new LessThan(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 307 "parser.yy"
    { (yyval.formula) = new LessThanOrEqual(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 308 "parser.yy"
    { (yyval.formula) = new GreaterThanOrEqual(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 309 "parser.yy"
    { (yyval.formula) = new GreaterThan(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 310 "parser.yy"
    { (yyval.formula) = new Equality(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 311 "parser.yy"
    { (yyval.formula) = new Inequality(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 312 "parser.yy"
    { (yyval.formula) = (yyvsp[(2) - (3)].formula); }
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 319 "parser.yy"
    { (yyval.dist) = &Exponential::make(*(yyvsp[(1) - (1)].expr)); }
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 320 "parser.yy"
    { (yyval.dist) = &Exponential::make(*(yyvsp[(3) - (4)].expr)); }
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 322 "parser.yy"
    { (yyval.dist) = &Weibull::make(*(yyvsp[(3) - (6)].expr), *(yyvsp[(5) - (6)].expr)); }
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 324 "parser.yy"
    { (yyval.dist) = &Lognormal::make(*(yyvsp[(3) - (6)].expr), *(yyvsp[(5) - (6)].expr)); }
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 326 "parser.yy"
    { (yyval.dist) = &Uniform::make(*(yyvsp[(3) - (6)].expr), *(yyvsp[(5) - (6)].expr)); }
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 332 "parser.yy"
    { (yyval.expr) = make_value((yyvsp[(1) - (1)].nat)); }
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 333 "parser.yy"
    { (yyval.expr) = find_variable((yyvsp[(1) - (1)].str)); }
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 334 "parser.yy"
    { (yyval.expr) = &Addition::make(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 335 "parser.yy"
    { (yyval.expr) = &Subtraction::make(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 336 "parser.yy"
    { (yyval.expr) = &Multiplication::make(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 337 "parser.yy"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 340 "parser.yy"
    { (yyval.expr) = make_value((yyvsp[(1) - (1)].num)); }
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 341 "parser.yy"
    { (yyval.expr) = find_rate((yyvsp[(1) - (1)].str)); }
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 342 "parser.yy"
    { (yyval.expr) = &Multiplication::make(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 343 "parser.yy"
    { (yyval.expr) = &Division::make(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 344 "parser.yy"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 347 "parser.yy"
    { (yyval.expr) = make_value((yyvsp[(1) - (1)].num)); }
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 348 "parser.yy"
    { (yyval.expr) = find_rate((yyvsp[(1) - (1)].str)); }
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 350 "parser.yy"
    { (yyval.expr) = &Multiplication::make(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 352 "parser.yy"
    { (yyval.expr) = &Division::make(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 353 "parser.yy"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 360 "parser.yy"
    { (yyval.range) = make_range((yyvsp[(2) - (5)].expr), (yyvsp[(4) - (5)].expr)); }
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 363 "parser.yy"
    { (yyval.expr) = make_value((yyvsp[(1) - (1)].nat)); }
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 364 "parser.yy"
    { (yyval.expr) = find_constant((yyvsp[(1) - (1)].str)); }
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 365 "parser.yy"
    { (yyval.expr) = &Addition::make(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 366 "parser.yy"
    { (yyval.expr) = &Subtraction::make(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 368 "parser.yy"
    { (yyval.expr) = &Multiplication::make(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 369 "parser.yy"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 372 "parser.yy"
    { (yyval.nat) = integer_value((yyvsp[(1) - (1)].num)); }
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 381 "parser.yy"
    { properties.push_back((yyvsp[(2) - (2)].formula)); StateFormula::ref((yyvsp[(2) - (2)].formula)); }
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 384 "parser.yy"
    { (yyval.formula) = new Conjunction(); }
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 385 "parser.yy"
    { (yyval.formula) = new Disjunction(); }
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 387 "parser.yy"
    { (yyval.formula) = make_probabilistic((yyvsp[(3) - (6)].num), true, true, *(yyvsp[(5) - (6)].path)); }
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 389 "parser.yy"
    { (yyval.formula) = make_probabilistic((yyvsp[(3) - (6)].num), false, true, *(yyvsp[(5) - (6)].path)); }
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 391 "parser.yy"
    { (yyval.formula) = make_probabilistic((yyvsp[(3) - (6)].num), false, false, *(yyvsp[(5) - (6)].path)); }
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 393 "parser.yy"
    { (yyval.formula) = make_probabilistic((yyvsp[(3) - (6)].num), true, false, *(yyvsp[(5) - (6)].path)); }
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 394 "parser.yy"
    { (yyval.formula) = new Implication(*(yyvsp[(1) - (3)].formula), *(yyvsp[(3) - (3)].formula)); }
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 395 "parser.yy"
    { (yyval.formula) = make_conjunction(*(yyvsp[(1) - (3)].formula), *(yyvsp[(3) - (3)].formula)); }
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 396 "parser.yy"
    { (yyval.formula) = make_disjunction(*(yyvsp[(1) - (3)].formula), *(yyvsp[(3) - (3)].formula)); }
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 397 "parser.yy"
    { (yyval.formula) = new Negation(*(yyvsp[(2) - (2)].formula)); }
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 398 "parser.yy"
    { (yyval.formula) = new LessThan(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 399 "parser.yy"
    { (yyval.formula) = new LessThanOrEqual(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 400 "parser.yy"
    { (yyval.formula) = new GreaterThanOrEqual(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 401 "parser.yy"
    { (yyval.formula) = new GreaterThan(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 402 "parser.yy"
    { (yyval.formula) = new Equality(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 403 "parser.yy"
    { (yyval.formula) = new Inequality(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 404 "parser.yy"
    { (yyval.formula) = (yyvsp[(2) - (3)].formula); }
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 408 "parser.yy"
    { (yyval.path) = make_until(*(yyvsp[(1) - (5)].formula), *(yyvsp[(5) - (5)].formula), NULL, (yyvsp[(4) - (5)].num)); }
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 410 "parser.yy"
    { (yyval.path) = make_until(*(yyvsp[(1) - (8)].formula), *(yyvsp[(8) - (8)].formula), (yyvsp[(4) - (8)].num), (yyvsp[(6) - (8)].num)); }
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 414 "parser.yy"
    { (yyval.expr) = make_value((yyvsp[(1) - (1)].nat)); }
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 415 "parser.yy"
    { (yyval.expr) = value_or_variable((yyvsp[(1) - (1)].str)); }
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 416 "parser.yy"
    { (yyval.expr) = &Addition::make(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 417 "parser.yy"
    { (yyval.expr) = &Subtraction::make(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 418 "parser.yy"
    { (yyval.expr) = &Multiplication::make(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 419 "parser.yy"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;



/* Line 1455 of yacc.c  */
#line 2358 "parser.cc"
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
#line 423 "parser.yy"


/* Clears all previously parsed declarations. */
void clear_declarations() {
  constants.clear();
  constant_values.clear();
  variables.clear();
}


/* Tests if the given variables is a member of the given list. */
static bool member(const VariableList& vars, const Variable* v) {
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
  for (std::map<const Variable*, const Expression*>::const_iterator vi =
	 variable_lows.begin();
       vi != variable_lows.end(); vi++) {
    Expression::destructive_deref((*vi).second);
  }
  variable_lows.clear();
  for (std::map<const Variable*, const Expression*>::const_iterator vi =
	 variable_highs.begin();
       vi != variable_highs.end(); vi++) {
    Expression::destructive_deref((*vi).second);
  }
  variable_highs.clear();
  for (std::map<const Variable*, const Expression*>::const_iterator vi =
	 variable_starts.begin();
       vi != variable_starts.end(); vi++) {
    Expression::destructive_deref((*vi).second);
  }
  variable_starts.clear();
}


/* Returns the numerator of the given rational, signaling an error if
   the denominator is not 1. */
static int integer_value(const Rational* q) {
  int n;
  if (q->denominator() != 1) {
    yyerror("expecting integer");
    n = 0;
  } else {
    n = q->numerator();
  }
  delete q;
  return n;
}


/* Returns a variable representing an integer constant. */
static const Variable* find_constant(const std::string* ident) {
  std::map<std::string, const Variable*>::const_iterator ci =
    constants.find(*ident);
  if (ci != constants.end()) {
    delete ident;
    return (*ci).second;
  } else {
    if (variables.find(*ident) != variables.end()) {
      yyerror("variable `" + *ident + "' used where expecting constant");
    } else if (rates.find(*ident) != rates.end()) {
      yyerror("rate `" + *ident + "' used where expecting constant");
    } else {
      yyerror("undeclared constant `" + *ident + "'");
    }
    Variable* v = new Variable();
    variables.insert(std::make_pair(*ident, v));
    constants.insert(std::make_pair(*ident, v));
    constant_values.insert(std::make_pair(v, 0));
    rate_values.insert(std::make_pair(v, 0));
    delete ident;
    return v;
  }
}


/* Returns a variable representing a rate constant. */
static const Variable* find_rate(const std::string* ident) {
  std::map<std::string, const Variable*>::const_iterator ri =
    rates.find(*ident);
  if (ri != rates.end()) {
    delete ident;
    return (*ri).second;
  } else {
    return find_variable(ident);
  }
}


/* Returns a range with the given bounds. */
static Range make_range(const Expression* l, const Expression* h) {
  Range r = { l, h };
  return r;
}


/* Returns a value expression. */
static const Value* make_value(int n) {
  return new Value(n);
}


/* Returns a value expression. */
static const Value* make_value(const Rational* q) {
  const Value* v = new Value(*q);
  delete q;
  return v;
}


/* Returns a constant value or a variable for the given identifier. */
static const Expression* value_or_variable(const std::string* ident) {
  std::map<std::string, const Variable*>::const_iterator ci =
    constants.find(*ident);
  if (ci != constants.end()) {
    delete ident;
    return new Value(constant_values[(*ci).second]);
  } else {
    Variable* v;
    std::map<std::string, Variable*>::const_iterator vi =
      variables.find(*ident);
    if (vi == variables.end()) {
      v = new Variable();
      variables.insert(std::make_pair(*ident, v));
      undeclared.insert(*ident);
    } else {
      v = (*vi).second;
    }
    delete ident;
    return v;
  }
}


/* Returns a variable for the given identifier. */
static const Variable* find_variable(const std::string* ident) {
  std::map<std::string, const Variable*>::const_iterator ci =
    constants.find(*ident);
  if (ci != constants.end()) {
    delete ident;
    return (*ci).second;
  } else {
    Variable* v;
    std::map<std::string, Variable*>::const_iterator vi =
      variables.find(*ident);
    if (vi == variables.end()) {
      v = new Variable();
      variables.insert(std::make_pair(*ident, v));
      undeclared.insert(*ident);
    } else {
      v = (*vi).second;
    }
    delete ident;
    return v;
  }
}


/* Returns a conjunction. */
static Conjunction* make_conjunction(StateFormula& f1,
				     const StateFormula& f2) {
  Conjunction* conj = dynamic_cast<Conjunction*>(&f1);
  if (conj == NULL) {
    conj = new Conjunction();
    conj->add_conjunct(f1);
  }
  conj->add_conjunct(f2);
  return conj;
}


/* Returns a disjunction. */
static Disjunction* make_disjunction(StateFormula& f1,
				     const StateFormula& f2) {
  Disjunction* disj = dynamic_cast<Disjunction*>(&f1);
  if (disj == NULL) {
    disj = new Disjunction();
    disj->add_disjunct(f1);
  }
  disj->add_disjunct(f2);
  return disj;
}


/* Returns a probabilistic path quantification. */
static StateFormula* make_probabilistic(const Rational* p,
					bool strict, bool negate,
					const PathFormula& f) {
  if (*p < 0 || *p > 1) {
    yyerror("probability bound outside the interval [0,1]");
  }
  bool s = (strict && !negate) || (!strict && negate);
  StateFormula* pr = new Probabilistic(*p, s, f);
  if (negate) {
    pr = new Negation(*pr);
  }
  delete p;
  return pr;
}


/* Returns an until formula. */
static const Until* make_until(const StateFormula& f1, const StateFormula& f2,
			       const Rational* t1, const Rational* t2) {
  const Until* until;
  if (t1 == NULL) {
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


/* Adds an update to the current command. */
static void add_update(const std::string* ident, const Expression& expr) {
  const Variable* v;
  std::map<std::string, Variable*>::const_iterator vi = variables.find(*ident);
  if (vi == variables.end()) {
    yyerror("updating undeclared variable `" + *ident + "'");
    v = new Variable();
  } else {
    v = (*vi).second;
  }
  if (member(model->variables(), v)) {
    if (command->synch() != 0) {
      yywarning("updating global variable in synchronized command");
    }
  } else if (!member(module->variables(), v)) {
    yyerror("updating variable belonging to other module");
  }
  command->add_update(*new Update(*v, expr));
  delete ident;
}


/* Returns the value of the given synchronization. */
static size_t synchronization_value(const std::string* ident) {
  size_t s;
  std::map<std::string, size_t>::const_iterator si =
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


/* Adds a substitution to the current substitution map. */
static void add_substitution(const std::string* ident1,
			     const std::string* ident2) {
  // TODO: must be able to substitute synchronizations as well as variables
  std::map<std::string, Variable*>::const_iterator vi =
    variables.find(*ident1);
  if (vi != variables.end()) {
    /* Variable substitution. */
    subst.insert(std::make_pair((*vi).second, ident2));
  } else {
    std::map<std::string, size_t>::const_iterator si =
      synchronizations.find(*ident1);
    if (si != synchronizations.end()) {
      /* Synchronization substitution. */
      size_t s = synchronization_value(ident2);
      synch_subst.insert(std::make_pair((*si).second, s));
    } else {
      yyerror("illegal substitution `" + *ident1 + "=" + *ident2 + "'");
      delete ident2;
    }
  }
  delete ident1;
}


/* Declares an integer constant. */
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
    Variable* v = new Variable();
    variables.insert(std::make_pair(*ident, v));
    constants.insert(std::make_pair(*ident, v));
    const int value = value_expr->value(constant_values).numerator();
    constant_values.insert(std::make_pair(v, value));
    rate_values.insert(std::make_pair(v, value));
  }
  delete ident;
  delete value_expr;
}


/* Declares a rate constant. */
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
    Variable* v = new Variable();
    variables.insert(std::make_pair(*ident, v));
    rates.insert(std::make_pair(*ident, v));
    rate_values.insert(std::make_pair(v, value_expr->value(rate_values)));
  }
  delete ident;
  delete value_expr;
}


/* Declares a variable. */
static const Variable* declare_variable(const std::string* ident,
					const Range& range,
					const Expression* start,
					bool delayed_addition) {
  Variable* v = NULL;
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
    std::map<std::string, Variable*>::const_iterator vi =
      variables.find(*ident);
    if (vi != variables.end()) {
      if (undeclared.find(*ident) != undeclared.end()) {
	v = (*vi).second;
	int low = range.l->value(constant_values).numerator();
	v->set_low(low);
	v->set_high(range.h->value(constant_values).numerator());
	if (start != NULL) {
	  v->set_start(start->value(constant_values).numerator());
	} else {
	  v->set_start(low);
	}
	v->set_low_bit(num_model_bits);
	undeclared.erase(*ident);
      } else {
	yyerror("ignoring repeated declaration of variable `" + *ident + "'");
      }
    } else {
      int low = range.l->value(constant_values).numerator();
      int s = ((start != NULL)
	       ? start->value(constant_values).numerator() : low);
      v = new Variable(low, range.h->value(constant_values).numerator(), s,
		       num_model_bits);
    }
  }
  if (v != NULL) {
    variable_lows.insert(std::make_pair(v, range.l));
    Expression::ref(range.l);
    variable_highs.insert(std::make_pair(v, range.h));
    Expression::ref(range.h);
    variable_starts.insert(std::make_pair(v, start));
    Expression::ref(start);
    num_model_bits = v->high_bit() + 1;
    variables.insert(std::make_pair(*ident, v));
    if (!delayed_addition) {
      if (module != NULL) {
	module->add_variable(*v);
      } else {
	model->add_variable(*v);
      }
    }
  } else {
    Expression::ref(range.l);
    Expression::destructive_deref(range.l);
    Expression::ref(range.h);
    Expression::destructive_deref(range.h);
    Expression::ref(start);
    Expression::destructive_deref(start);
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
    mi = modules.find(*ident2);
    if (mi == modules.end()) {
      yyerror("ignoring renaming of undeclared module `" + *ident2 + "'");
    } else {
      const Module& src_module = *(*mi).second;
      SubstitutionMap c_subst;
      for (ValueMap::const_iterator ci = constant_values.begin();
	   ci != constant_values.end(); ci++) {
	std::map<const Variable*, const std::string*>::const_iterator si =
	  subst.find((*ci).first);
	if (si != subst.end()) {
	  std::map<std::string, const Variable*>::const_iterator cj =
	    constants.find(*(*si).second);
	  if (cj != constants.end()) {
	    c_subst.insert(std::make_pair((*ci).first, (*cj).second));
	  } else {
	    yyerror("substituting constant with non-constant" + *(*si).second);
	  }
	}
      }
      SubstitutionMap v_subst;
      for (VariableList::const_iterator vi = src_module.variables().begin();
	   vi != src_module.variables().end(); vi++) {
	std::map<const Variable*, const std::string*>::const_iterator si =
	  subst.find(*vi);
	if (si == subst.end()) {
	  yyerror("missing substitution for module variable");
	} else {
	  const Expression* low =
	    &(*variable_lows.find(*vi)).second->substitution(c_subst);
	  const Expression* high =
	    &(*variable_highs.find(*vi)).second->substitution(c_subst);
	  const Expression* start;
	  std::map<const Variable*, const Expression*>::const_iterator i =
	    variable_starts.find(*vi);
	  if ((*i).second != NULL) {
	    start = &(*i).second->substitution(c_subst);
	  } else {
	    start = NULL;
	  }
	  Range r = { low, high };
	  const Variable* v =
	    declare_variable((*si).second, r, start, true);
	  if (v != NULL) {
	    v_subst.insert(std::make_pair(*vi, v));
	  }
	}
      }
      for (std::map<const Variable*, const std::string*>::const_iterator si =
	     subst.begin();
	   si != subst.end(); si++) {
	const Variable* v1 = (*si).first;
	if (!member(src_module.variables(), v1)) {
	  const Variable* v2 =
	    dynamic_cast<const Variable*>(find_variable((*si).second));
	  if (v2 == NULL) {
	    yyerror("substituting variable with constant");
	  } else {
	    v_subst.insert(std::make_pair(v1, v2));
	  }
	}
      }
      Module* mod = &src_module.substitution(v_subst, synch_subst);
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
  module = NULL;
}


/* Prepares a module for parsing. */
static void prepare_module(const std::string* ident) {
  std::map<std::string, Module*>::const_iterator mi = modules.find(*ident);
  if (mi != modules.end()) {
    yyerror("ignoring repeated declaration of module `" + *ident + "'");
  } else {
    module = new Module();
    modules.insert(std::make_pair(*ident, module));
  }
  delete ident;
}


/* Prepares a model for parsing. */
static void prepare_model() {
  clear_declarations();
  for (FormulaList::const_iterator fi = properties.begin();
       fi != properties.end(); fi++) {
    StateFormula::destructive_deref(*fi);
  }
  properties.clear();
  if (model != NULL) {
    delete model;
  }
  model = new Model();
  num_model_bits = 0;
}


/* Compiles the current model. */
static void compile_model() {
  for (std::map<std::string, Module*>::const_iterator mi = modules.begin();
       mi != modules.end(); mi++) {
    (*mi).second->compile(constant_values, rate_values);
  }
  model->compile();
  global_model = model;
}



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
#include "glog/logging.h"
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
/* Constant overrides. */
extern std::map<std::string, TypedValue> const_overrides;

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
static std::map<std::string, std::string> subst;
/* Current synchronization substitutions. */
static SynchSubstitutionMap synch_subst;
/* Current command. */
static Command* command;
/* Declared integer constants. */
static std::map<std::string, const Variable*> constants;
/* Constant values. */
static std::map<std::string, TypedValue> constant_values;
/* Declared rate constants. */
static std::map<std::string, const Variable*> rates;
/* Rate values. */
static std::map<std::string, TypedValue> rate_values;
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
/* Next variable index. */
static int next_variable_index;
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
/* Returns a variable representing an integer constant. */
static const Variable* find_constant(const std::string* ident);
/* Returns a variable representing a rate constant. */
static const Variable* find_rate(const std::string* ident);
/* Returns a variable representing a rate constant or an integer variable. */
static const Variable* find_rate_or_variable(const std::string* ident);
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
static const Variable* find_variable(const std::string* ident);
/* Returns a conjunction. */
static Conjunction* make_conjunction(StateFormula& f1,
				     const StateFormula& f2);
/* Returns a disjunction. */
static Disjunction* make_disjunction(StateFormula& f1,
				     const StateFormula& f2);
/* Returns a probabilistic path quantification. */
static StateFormula* make_probabilistic(const TypedValue* p,
					bool strict, bool negate,
					const PathFormula& f);
/* Returns an until formula. */
static const Until* make_until(const StateFormula& f1, const StateFormula& f2,
			       const TypedValue* t1, const TypedValue* t2);
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
#line 227 "parser.cc"

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
     LTE = 284
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




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 195 "parser.yy"

  size_t synch;
  StateFormula* formula;
  const PathFormula* path;
  const Distribution* dist;
  const Expression* expr;
  Range range;
  int nat;
  const std::string* str;
  const TypedValue* num;



/* Line 214 of yacc.c  */
#line 335 "parser.cc"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 347 "parser.cc"

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
#define YYLAST   387

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  51
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  40
/* YYNRULES -- Number of rules.  */
#define YYNRULES  124
/* YYNRULES -- Number of states.  */
#define YYNSTATES  284

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   284

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    28,     2,     2,     2,     2,    26,     2,
      45,    46,    37,    35,    44,    36,     2,    38,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    41,    39,
      29,    40,    30,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,    48,     2,     2,     2,
      50,     2,     2,     2,     2,    49,     2,    47,     2,     2,
       2,    42,     2,    43,     2,     2,     2,     2,     2,     2,
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
      25,    31,    32,    33,    34
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
     256,   263,   270,   272,   274,   278,   282,   286,   290,   292,
     294,   298,   302,   306,   310,   314,   316,   318,   322,   326,
     330,   336,   338,   340,   344,   348,   352,   356,   358,   359,
     362,   364,   366,   373,   380,   387,   394,   398,   402,   406,
     409,   413,   417,   421,   425,   429,   433,   437,   443,   452,
     454,   456,   460,   464,   468
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      52,     0,    -1,    -1,    53,    54,    -1,    55,    -1,    87,
      -1,    -1,    57,    56,    58,    60,    73,    -1,     3,    -1,
       4,    -1,    -1,    58,    59,    -1,     5,    19,    39,    -1,
       5,    19,    40,    85,    39,    -1,     5,     6,    19,    39,
      -1,     5,     6,    19,    40,    85,    39,    -1,     8,    19,
      39,    -1,     8,    19,    40,    83,    39,    -1,     5,     7,
      19,    39,    -1,     5,     7,    19,    40,    83,    39,    -1,
       9,    19,    41,    84,    39,    -1,     9,    19,    41,    84,
      10,    85,    39,    -1,    -1,    60,    61,    -1,    -1,    16,
      19,    62,    66,    68,    17,    -1,    16,    19,    40,    19,
      42,    63,    43,    17,    -1,    -1,    64,    -1,    65,    -1,
      64,    44,    65,    -1,    19,    40,    19,    -1,    -1,    66,
      67,    -1,    19,    41,    84,    39,    -1,    19,    41,    84,
      10,    85,    39,    -1,    -1,    68,    69,    -1,    -1,    71,
      79,    22,    80,    41,    70,    72,    39,    -1,    42,    43,
      -1,    42,    19,    43,    -1,    18,    40,    81,    -1,    72,
      26,    72,    -1,    45,    72,    46,    -1,    -1,    73,    74,
      -1,    14,    75,    76,    15,    -1,    -1,    20,    -1,    -1,
      76,    77,    -1,    76,    78,    -1,    79,    41,    82,    39,
      -1,    42,    19,    43,    79,    41,    82,    39,    -1,    11,
      -1,    12,    -1,    79,    26,    79,    -1,    79,    27,    79,
      -1,    28,    79,    -1,    81,    29,    81,    -1,    81,    34,
      81,    -1,    81,    33,    81,    -1,    81,    30,    81,    -1,
      81,    40,    81,    -1,    81,    31,    81,    -1,    45,    79,
      46,    -1,    82,    -1,    13,    45,    82,    46,    -1,    47,
      45,    83,    44,    83,    46,    -1,    48,    45,    83,    44,
      83,    46,    -1,    49,    45,    83,    44,    83,    46,    -1,
      86,    -1,    19,    -1,    81,    35,    81,    -1,    81,    36,
      81,    -1,    81,    37,    81,    -1,    45,    81,    46,    -1,
      21,    -1,    19,    -1,    82,    35,    82,    -1,    82,    36,
      82,    -1,    82,    37,    82,    -1,    82,    38,    82,    -1,
      45,    82,    46,    -1,    21,    -1,    19,    -1,    83,    37,
      83,    -1,    83,    38,    83,    -1,    45,    83,    46,    -1,
      42,    85,    23,    85,    43,    -1,    86,    -1,    19,    -1,
      85,    35,    85,    -1,    85,    36,    85,    -1,    85,    37,
      85,    -1,    45,    85,    46,    -1,    21,    -1,    -1,    87,
      88,    -1,    11,    -1,    12,    -1,    50,    29,    21,    42,
      89,    43,    -1,    50,    34,    21,    42,    89,    43,    -1,
      50,    33,    21,    42,    89,    43,    -1,    50,    30,    21,
      42,    89,    43,    -1,    88,    25,    88,    -1,    88,    26,
      88,    -1,    88,    27,    88,    -1,    28,    88,    -1,    90,
      29,    90,    -1,    90,    34,    90,    -1,    90,    33,    90,
      -1,    90,    30,    90,    -1,    90,    40,    90,    -1,    90,
      31,    90,    -1,    45,    88,    46,    -1,    88,    49,    34,
      21,    88,    -1,    88,    49,    42,    21,    44,    21,    43,
      88,    -1,    86,    -1,    19,    -1,    90,    35,    90,    -1,
      90,    36,    90,    -1,    90,    37,    90,    -1,    45,    90,
      46,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   219,   219,   219,   223,   224,   231,   231,   235,   235,
     241,   242,   245,   246,   247,   248,   250,   251,   252,   253,
     255,   256,   264,   265,   268,   268,   270,   274,   275,   278,
     279,   282,   285,   286,   289,   290,   294,   295,   299,   298,
     302,   303,   306,   307,   308,   315,   316,   319,   322,   323,
     326,   327,   328,   331,   335,   343,   344,   345,   346,   347,
     348,   349,   350,   351,   352,   353,   354,   361,   362,   363,
     365,   367,   374,   375,   376,   377,   378,   379,   382,   383,
     384,   385,   386,   387,   388,   391,   392,   393,   395,   397,
     404,   407,   408,   409,   410,   411,   413,   416,   423,   424,
     428,   429,   430,   432,   434,   436,   438,   439,   440,   441,
     442,   443,   444,   445,   446,   447,   448,   451,   453,   458,
     459,   460,   461,   462,   463
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
  "GTE", "LTE", "'+'", "'-'", "'*'", "'/'", "';'", "'='", "':'", "'['",
  "']'", "','", "'('", "')'", "'W'", "'L'", "'U'", "'P'", "$accept",
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
      62,   281,   282,   283,   284,    43,    45,    42,    47,    59,
      61,    58,    91,    93,    44,    40,    41,    87,    76,    85,
      80
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    51,    53,    52,    54,    54,    56,    55,    57,    57,
      58,    58,    59,    59,    59,    59,    59,    59,    59,    59,
      59,    59,    60,    60,    62,    61,    61,    63,    63,    64,
      64,    65,    66,    66,    67,    67,    68,    68,    70,    69,
      71,    71,    72,    72,    72,    73,    73,    74,    75,    75,
      76,    76,    76,    77,    78,    79,    79,    79,    79,    79,
      79,    79,    79,    79,    79,    79,    79,    80,    80,    80,
      80,    80,    81,    81,    81,    81,    81,    81,    82,    82,
      82,    82,    82,    82,    82,    83,    83,    83,    83,    83,
      84,    85,    85,    85,    85,    85,    85,    86,    87,    87,
      88,    88,    88,    88,    88,    88,    88,    88,    88,    88,
      88,    88,    88,    88,    88,    88,    88,    89,    89,    90,
      90,    90,    90,    90,    90
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
       6,     6,     1,     1,     3,     3,     3,     3,     1,     1,
       3,     3,     3,     3,     3,     1,     1,     3,     3,     3,
       5,     1,     1,     3,     3,     3,     3,     1,     0,     2,
       1,     1,     6,     6,     6,     6,     3,     3,     3,     2,
       3,     3,     3,     3,     3,     3,     3,     5,     8,     1,
       1,     3,     3,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,    98,     1,     8,     9,     3,     4,     6,     5,
      10,   100,   101,   120,    97,     0,     0,     0,   119,    99,
       0,    22,   109,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    11,    45,   116,   124,     0,     0,
       0,     0,   106,   107,   108,     0,   110,   113,   115,   112,
     111,   121,   122,   123,   114,     0,     0,     0,     0,     0,
       0,    23,     7,     0,     0,     0,     0,     0,     0,     0,
      12,     0,    16,     0,     0,    24,    48,    46,     0,     0,
       0,     0,     0,    14,     0,    18,     0,    92,     0,     0,
      91,    86,    85,     0,     0,     0,     0,     0,    32,    49,
      50,     0,   102,   105,   104,   103,     0,     0,     0,     0,
       0,     0,    13,     0,     0,     0,    17,     0,     0,    20,
       0,    36,     0,     0,     0,    15,    19,    96,    93,    94,
      95,    89,    87,    88,     0,     0,    27,     0,    33,     0,
      55,    56,    47,    73,     0,     0,     0,    51,    52,     0,
       0,    72,     0,     0,     0,    21,     0,     0,    28,    29,
       0,    25,     0,    37,     0,    59,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   117,     0,    90,     0,     0,     0,     0,     0,    40,
       0,     0,    66,    77,    57,    58,    79,    78,     0,     0,
       0,    60,    63,    65,    62,    61,    74,    75,    76,    64,
       0,    31,    26,    30,     0,    34,    41,     0,     0,     0,
       0,     0,     0,     0,    53,     0,     0,     0,     0,     0,
       0,     0,     0,    67,     0,    84,    80,    81,    82,    83,
     118,    35,     0,     0,     0,     0,    38,     0,     0,     0,
       0,     0,     0,    54,    68,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    39,    69,    70,
      71,    42,    44,    43
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     6,     7,    10,     8,    21,    44,    45,
      71,   108,   167,   168,   169,   131,   148,   149,   173,   262,
     174,   270,    72,    87,   110,   132,   157,   158,   159,   242,
     160,   209,   104,   106,    99,    18,     9,    88,    89,    20
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -260
static const yytype_int16 yypact[] =
{
    -260,    27,   227,  -260,  -260,  -260,  -260,  -260,  -260,   112,
    -260,  -260,  -260,  -260,  -260,   112,   112,   268,  -260,   166,
     210,   329,  -260,   143,   172,    11,    15,    26,    32,   112,
     112,   112,    47,    47,    47,    47,    47,    47,    47,    47,
      47,   130,    46,    51,  -260,    66,  -260,  -260,    53,    55,
      59,    87,   187,  -260,  -260,    47,   308,   308,   308,   308,
     308,   102,   102,  -260,   308,   126,   129,   209,   221,   114,
     145,  -260,   190,   112,   112,   112,   112,   151,   320,   322,
    -260,    48,  -260,   106,   183,   171,   234,  -260,   116,   238,
     244,   260,   293,  -260,    48,  -260,   106,  -260,    48,   281,
    -260,  -260,  -260,   106,   309,    48,     5,   300,  -260,  -260,
    -260,    38,  -260,  -260,  -260,  -260,   286,   312,   236,    48,
      48,    48,  -260,    17,   106,   106,  -260,   201,    48,  -260,
     287,   305,   135,   334,   342,  -260,  -260,  -260,   327,   327,
    -260,  -260,  -260,  -260,    48,   291,   346,   325,  -260,    29,
    -260,  -260,  -260,  -260,   155,   348,   155,  -260,  -260,   158,
     222,  -260,   112,   324,   248,  -260,   330,   326,   328,  -260,
     183,  -260,    79,  -260,   155,  -260,   331,   152,   186,   155,
     155,   107,   113,   113,   113,   113,   113,   113,   113,   113,
     113,   166,   350,  -260,   354,   358,   346,     6,   333,  -260,
     278,   155,  -260,  -260,  -260,  -260,  -260,  -260,   107,   271,
     113,   317,   317,   317,   317,   317,   340,   340,  -260,   317,
     335,  -260,  -260,  -260,    48,  -260,  -260,    30,   169,   228,
     107,   107,   107,   107,  -260,   240,   112,   296,   336,   337,
     338,   339,   344,   304,   107,  -260,   257,   257,  -260,  -260,
     166,  -260,   107,   106,   106,   106,  -260,   276,   232,   191,
     252,   255,    -7,  -260,  -260,   106,   106,   106,   347,    -7,
     -13,   144,   196,   242,   113,   -22,    -7,  -260,  -260,  -260,
    -260,   317,  -260,  -260
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -260,  -260,  -260,  -260,  -260,  -260,  -260,  -260,  -260,  -260,
    -260,  -260,  -260,  -260,   184,  -260,  -260,  -260,  -260,  -260,
    -260,  -259,  -260,  -260,  -260,  -260,  -260,  -260,  -151,  -260,
     -99,  -171,   -94,   216,   -86,   -80,  -260,    -9,   282,    80
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint16 yytable[] =
{
      19,   100,   117,   175,   276,   177,    22,    23,   116,   123,
     275,   268,   118,   276,   100,   128,   224,   283,   100,   127,
      52,    53,    54,   200,   282,   100,   277,     3,   204,   205,
     142,   143,    48,   138,   139,   140,    49,   229,   269,   100,
     100,   100,   145,   238,   129,   225,   171,    50,   100,   206,
     228,   207,   161,    51,   124,   125,   243,   178,   164,   246,
     247,   248,   249,   141,   100,    68,    13,    97,    14,    14,
      69,   172,   133,   257,   161,   208,   161,   239,   240,   241,
     134,   258,    70,   211,   212,   213,   214,   215,   216,   217,
     218,   219,    55,    98,   161,    73,    24,    74,   198,   161,
     161,    75,   161,   161,   161,   161,   161,   161,   161,   161,
     161,   235,    56,    57,    58,    59,    60,    61,    62,    63,
      64,   161,   199,    11,    12,   101,   206,   102,   207,    76,
     161,    13,   153,    14,    14,    77,    65,    66,   237,    39,
      15,    29,    30,    31,   100,    78,   150,   151,    79,    67,
     152,   103,   208,   191,   153,    84,    14,    16,   210,   259,
     260,   261,    17,   154,    85,   111,   150,   151,    29,    30,
      31,   271,   272,   273,   153,   281,    14,   155,   179,   180,
     156,   124,   125,   154,   179,   180,    37,    38,    39,    46,
     278,    29,    30,    31,   161,   179,   180,    47,   202,   181,
     156,    32,    33,    34,    86,    35,    36,    37,    38,    39,
     244,   107,    40,    30,    31,   182,   183,   184,    47,   185,
     186,   187,   188,   189,   144,   105,   190,   250,   124,   125,
       4,     5,   203,   124,   125,   265,   119,   120,   121,    32,
      33,    34,   279,    35,    36,    37,    38,    39,    80,    81,
      40,   182,   183,   184,   109,   185,   186,   187,   188,   189,
      82,    83,   190,   230,   231,   232,   233,   230,   231,   232,
     233,   119,   120,   121,   245,   187,   188,   189,   264,   124,
     125,   112,   137,   119,   120,   121,   203,   113,   280,   124,
     125,   193,   124,   125,   232,   233,   266,    25,    26,   267,
     227,    27,    28,   114,   179,   180,   230,   231,   232,   233,
     234,   230,   231,   232,   233,   263,   119,   120,   121,   130,
     122,   119,   120,   121,   147,   135,   119,   120,   121,   146,
     165,   119,   120,   121,    41,   251,   115,    42,    43,   230,
     231,   232,   233,    37,    38,    39,   124,   125,   126,   124,
     125,   136,   187,   188,   189,   162,    90,    91,    92,    93,
      94,    95,    96,   163,   121,   166,   170,   176,   192,   195,
     194,   220,   196,   221,   201,   222,   226,   189,   236,     0,
     223,   252,   253,   254,   255,   256,   197,   274
};

static const yytype_int16 yycheck[] =
{
       9,    81,    96,   154,    26,   156,    15,    16,    94,   103,
     269,    18,    98,    26,    94,    10,    10,   276,    98,   105,
      29,    30,    31,   174,    46,   105,    39,     0,   179,   180,
     124,   125,    21,   119,   120,   121,    21,   208,    45,   119,
     120,   121,   128,    13,    39,    39,    17,    21,   128,    19,
     201,    21,   132,    21,    37,    38,   227,   156,   144,   230,
     231,   232,   233,    46,   144,    19,    19,    19,    21,    21,
      19,    42,    34,   244,   154,    45,   156,    47,    48,    49,
      42,   252,    16,   182,   183,   184,   185,   186,   187,   188,
     189,   190,    45,    45,   174,    42,    16,    42,    19,   179,
     180,    42,   182,   183,   184,   185,   186,   187,   188,   189,
     190,   210,    32,    33,    34,    35,    36,    37,    38,    39,
      40,   201,    43,    11,    12,    19,    19,    21,    21,    42,
     210,    19,    19,    21,    21,    55,     6,     7,   224,    37,
      28,    25,    26,    27,   224,    19,    11,    12,    19,    19,
      15,    45,    45,   162,    19,    41,    21,    45,    45,   253,
     254,   255,    50,    28,    19,    49,    11,    12,    25,    26,
      27,   265,   266,   267,    19,   274,    21,    42,    26,    27,
      45,    37,    38,    28,    26,    27,    35,    36,    37,    46,
      46,    25,    26,    27,   274,    26,    27,    46,    46,    41,
      45,    29,    30,    31,    14,    33,    34,    35,    36,    37,
      41,    40,    40,    26,    27,    29,    30,    31,    46,    33,
      34,    35,    36,    37,    23,    42,    40,   236,    37,    38,
       3,     4,    46,    37,    38,    44,    35,    36,    37,    29,
      30,    31,    46,    33,    34,    35,    36,    37,    39,    40,
      40,    29,    30,    31,    20,    33,    34,    35,    36,    37,
      39,    40,    40,    35,    36,    37,    38,    35,    36,    37,
      38,    35,    36,    37,    46,    35,    36,    37,    46,    37,
      38,    43,    46,    35,    36,    37,    46,    43,    46,    37,
      38,    43,    37,    38,    37,    38,    44,    29,    30,    44,
      22,    33,    34,    43,    26,    27,    35,    36,    37,    38,
      39,    35,    36,    37,    38,    39,    35,    36,    37,    19,
      39,    35,    36,    37,    19,    39,    35,    36,    37,    42,
      39,    35,    36,    37,     5,    39,    43,     8,     9,    35,
      36,    37,    38,    35,    36,    37,    37,    38,    39,    37,
      38,    39,    35,    36,    37,    21,    74,    75,    76,    39,
      40,    39,    40,    21,    37,    19,    41,    19,    44,    43,
      40,    21,    44,    19,    43,    17,    43,    37,    43,    -1,
     196,    45,    45,    45,    45,    41,   170,    40
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    52,    53,     0,     3,     4,    54,    55,    57,    87,
      56,    11,    12,    19,    21,    28,    45,    50,    86,    88,
      90,    58,    88,    88,    90,    29,    30,    33,    34,    25,
      26,    27,    29,    30,    31,    33,    34,    35,    36,    37,
      40,     5,     8,     9,    59,    60,    46,    46,    21,    21,
      21,    21,    88,    88,    88,    45,    90,    90,    90,    90,
      90,    90,    90,    90,    90,     6,     7,    19,    19,    19,
      16,    61,    73,    42,    42,    42,    42,    90,    19,    19,
      39,    40,    39,    40,    41,    19,    14,    74,    88,    89,
      89,    89,    89,    39,    40,    39,    40,    19,    45,    85,
      86,    19,    21,    45,    83,    42,    84,    40,    62,    20,
      75,    49,    43,    43,    43,    43,    85,    83,    85,    35,
      36,    37,    39,    83,    37,    38,    39,    85,    10,    39,
      19,    66,    76,    34,    42,    39,    39,    46,    85,    85,
      85,    46,    83,    83,    23,    85,    42,    19,    67,    68,
      11,    12,    15,    19,    28,    42,    45,    77,    78,    79,
      81,    86,    21,    21,    85,    39,    19,    63,    64,    65,
      41,    17,    42,    69,    71,    79,    19,    79,    81,    26,
      27,    41,    29,    30,    31,    33,    34,    35,    36,    37,
      40,    88,    44,    43,    40,    43,    44,    84,    19,    43,
      79,    43,    46,    46,    79,    79,    19,    21,    45,    82,
      45,    81,    81,    81,    81,    81,    81,    81,    81,    81,
      21,    19,    17,    65,    10,    39,    43,    22,    79,    82,
      35,    36,    37,    38,    39,    81,    43,    85,    13,    47,
      48,    49,    80,    82,    41,    46,    82,    82,    82,    82,
      88,    39,    45,    45,    45,    45,    41,    82,    82,    83,
      83,    83,    70,    39,    46,    44,    44,    44,    18,    45,
      72,    83,    83,    83,    40,    72,    26,    39,    46,    46,
      46,    81,    46,    72
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
#line 219 "parser.yy"
    { success = true; line_number = 1; }
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 220 "parser.yy"
    { check_undeclared(); if (!success) YYERROR; }
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 231 "parser.yy"
    { prepare_model(); }
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 232 "parser.yy"
    { compile_model(); }
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 245 "parser.yy"
    { declare_constant((yyvsp[(2) - (3)].str), NULL); }
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 246 "parser.yy"
    { declare_constant((yyvsp[(2) - (5)].str), (yyvsp[(4) - (5)].expr)); }
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 247 "parser.yy"
    { declare_constant((yyvsp[(3) - (4)].str), NULL); }
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 249 "parser.yy"
    { declare_constant((yyvsp[(3) - (6)].str), (yyvsp[(5) - (6)].expr)); }
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 250 "parser.yy"
    { declare_rate((yyvsp[(2) - (3)].str), NULL); }
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 251 "parser.yy"
    { declare_rate((yyvsp[(2) - (5)].str), (yyvsp[(4) - (5)].expr)); }
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 252 "parser.yy"
    { declare_rate((yyvsp[(3) - (4)].str), NULL); }
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 254 "parser.yy"
    { declare_rate((yyvsp[(3) - (6)].str), (yyvsp[(5) - (6)].expr)); }
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 255 "parser.yy"
    { declare_variable((yyvsp[(2) - (5)].str), (yyvsp[(4) - (5)].range), NULL); }
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 257 "parser.yy"
    { declare_variable((yyvsp[(2) - (7)].str), (yyvsp[(4) - (7)].range), (yyvsp[(6) - (7)].expr)); }
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 268 "parser.yy"
    { prepare_module((yyvsp[(2) - (2)].str)); }
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 269 "parser.yy"
    { add_module(); }
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 271 "parser.yy"
    { add_module((yyvsp[(2) - (8)].str), (yyvsp[(4) - (8)].str)); }
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 282 "parser.yy"
    { add_substitution((yyvsp[(1) - (3)].str), (yyvsp[(3) - (3)].str)); }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 289 "parser.yy"
    { declare_variable((yyvsp[(1) - (4)].str), (yyvsp[(3) - (4)].range), NULL); }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 291 "parser.yy"
    { declare_variable((yyvsp[(1) - (6)].str), (yyvsp[(3) - (6)].range), (yyvsp[(5) - (6)].expr)); }
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 299 "parser.yy"
    { prepare_command((yyvsp[(1) - (5)].synch), *(yyvsp[(2) - (5)].formula), *(yyvsp[(4) - (5)].dist)); }
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 299 "parser.yy"
    { add_command(); }
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 302 "parser.yy"
    { (yyval.synch) = 0; }
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 303 "parser.yy"
    { (yyval.synch) = synchronization_value((yyvsp[(2) - (3)].str)); }
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 306 "parser.yy"
    { add_update((yyvsp[(1) - (3)].str), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 332 "parser.yy"
    { delete (yyvsp[(1) - (4)].formula); delete (yyvsp[(3) - (4)].expr); }
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 336 "parser.yy"
    { delete (yyvsp[(2) - (7)].str); delete (yyvsp[(4) - (7)].formula); delete (yyvsp[(6) - (7)].expr); }
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 343 "parser.yy"
    { (yyval.formula) = new Conjunction(); }
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 344 "parser.yy"
    { (yyval.formula) = new Disjunction(); }
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 345 "parser.yy"
    { (yyval.formula) = make_conjunction(*(yyvsp[(1) - (3)].formula), *(yyvsp[(3) - (3)].formula)); }
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 346 "parser.yy"
    { (yyval.formula) = make_disjunction(*(yyvsp[(1) - (3)].formula), *(yyvsp[(3) - (3)].formula)); }
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 347 "parser.yy"
    { (yyval.formula) = new Negation(*(yyvsp[(2) - (2)].formula)); }
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 348 "parser.yy"
    { (yyval.formula) = new LessThan(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 349 "parser.yy"
    { (yyval.formula) = new LessThanOrEqual(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 350 "parser.yy"
    { (yyval.formula) = new GreaterThanOrEqual(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 351 "parser.yy"
    { (yyval.formula) = new GreaterThan(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 352 "parser.yy"
    { (yyval.formula) = new Equality(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 353 "parser.yy"
    { (yyval.formula) = new Inequality(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 354 "parser.yy"
    { (yyval.formula) = (yyvsp[(2) - (3)].formula); }
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 361 "parser.yy"
    { (yyval.dist) = &Exponential::make(*(yyvsp[(1) - (1)].expr)); }
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 362 "parser.yy"
    { (yyval.dist) = &Exponential::make(*(yyvsp[(3) - (4)].expr)); }
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 364 "parser.yy"
    { (yyval.dist) = &Weibull::make(*(yyvsp[(3) - (6)].expr), *(yyvsp[(5) - (6)].expr)); }
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 366 "parser.yy"
    { (yyval.dist) = &Lognormal::make(*(yyvsp[(3) - (6)].expr), *(yyvsp[(5) - (6)].expr)); }
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 368 "parser.yy"
    { (yyval.dist) = &Uniform::make(*(yyvsp[(3) - (6)].expr), *(yyvsp[(5) - (6)].expr)); }
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 374 "parser.yy"
    { (yyval.expr) = make_literal((yyvsp[(1) - (1)].nat)); }
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 375 "parser.yy"
    { (yyval.expr) = find_variable((yyvsp[(1) - (1)].str)); }
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 376 "parser.yy"
    { (yyval.expr) = &Addition::make(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 377 "parser.yy"
    { (yyval.expr) = &Subtraction::make(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 378 "parser.yy"
    { (yyval.expr) = &Multiplication::make(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 379 "parser.yy"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 382 "parser.yy"
    { (yyval.expr) = make_literal((yyvsp[(1) - (1)].num)); }
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 383 "parser.yy"
    { (yyval.expr) = find_rate_or_variable((yyvsp[(1) - (1)].str)); }
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 384 "parser.yy"
    { (yyval.expr) = &Addition::make(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 385 "parser.yy"
    { (yyval.expr) = &Subtraction::make(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 386 "parser.yy"
    { (yyval.expr) = &Multiplication::make(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 387 "parser.yy"
    { (yyval.expr) = &Division::make(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 388 "parser.yy"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 391 "parser.yy"
    { (yyval.expr) = make_literal((yyvsp[(1) - (1)].num)); }
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 392 "parser.yy"
    { (yyval.expr) = find_rate((yyvsp[(1) - (1)].str)); }
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 394 "parser.yy"
    { (yyval.expr) = &Multiplication::make(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 396 "parser.yy"
    { (yyval.expr) = &Division::make(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 397 "parser.yy"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 404 "parser.yy"
    { (yyval.range) = make_range((yyvsp[(2) - (5)].expr), (yyvsp[(4) - (5)].expr)); }
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 407 "parser.yy"
    { (yyval.expr) = make_literal((yyvsp[(1) - (1)].nat)); }
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 408 "parser.yy"
    { (yyval.expr) = find_constant((yyvsp[(1) - (1)].str)); }
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 409 "parser.yy"
    { (yyval.expr) = &Addition::make(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 410 "parser.yy"
    { (yyval.expr) = &Subtraction::make(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 412 "parser.yy"
    { (yyval.expr) = &Multiplication::make(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 413 "parser.yy"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 416 "parser.yy"
    { (yyval.nat) = integer_value((yyvsp[(1) - (1)].num)); }
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 425 "parser.yy"
    { properties.push_back((yyvsp[(2) - (2)].formula)); StateFormula::ref((yyvsp[(2) - (2)].formula)); }
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 428 "parser.yy"
    { (yyval.formula) = new Conjunction(); }
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 429 "parser.yy"
    { (yyval.formula) = new Disjunction(); }
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 431 "parser.yy"
    { (yyval.formula) = make_probabilistic((yyvsp[(3) - (6)].num), true, true, *(yyvsp[(5) - (6)].path)); }
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 433 "parser.yy"
    { (yyval.formula) = make_probabilistic((yyvsp[(3) - (6)].num), false, true, *(yyvsp[(5) - (6)].path)); }
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 435 "parser.yy"
    { (yyval.formula) = make_probabilistic((yyvsp[(3) - (6)].num), false, false, *(yyvsp[(5) - (6)].path)); }
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 437 "parser.yy"
    { (yyval.formula) = make_probabilistic((yyvsp[(3) - (6)].num), true, false, *(yyvsp[(5) - (6)].path)); }
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 438 "parser.yy"
    { (yyval.formula) = new Implication(*(yyvsp[(1) - (3)].formula), *(yyvsp[(3) - (3)].formula)); }
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 439 "parser.yy"
    { (yyval.formula) = make_conjunction(*(yyvsp[(1) - (3)].formula), *(yyvsp[(3) - (3)].formula)); }
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 440 "parser.yy"
    { (yyval.formula) = make_disjunction(*(yyvsp[(1) - (3)].formula), *(yyvsp[(3) - (3)].formula)); }
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 441 "parser.yy"
    { (yyval.formula) = new Negation(*(yyvsp[(2) - (2)].formula)); }
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 442 "parser.yy"
    { (yyval.formula) = new LessThan(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 443 "parser.yy"
    { (yyval.formula) = new LessThanOrEqual(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 444 "parser.yy"
    { (yyval.formula) = new GreaterThanOrEqual(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 445 "parser.yy"
    { (yyval.formula) = new GreaterThan(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 446 "parser.yy"
    { (yyval.formula) = new Equality(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 447 "parser.yy"
    { (yyval.formula) = new Inequality(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 448 "parser.yy"
    { (yyval.formula) = (yyvsp[(2) - (3)].formula); }
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 452 "parser.yy"
    { (yyval.path) = make_until(*(yyvsp[(1) - (5)].formula), *(yyvsp[(5) - (5)].formula), NULL, (yyvsp[(4) - (5)].num)); }
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 454 "parser.yy"
    { (yyval.path) = make_until(*(yyvsp[(1) - (8)].formula), *(yyvsp[(8) - (8)].formula), (yyvsp[(4) - (8)].num), (yyvsp[(6) - (8)].num)); }
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 458 "parser.yy"
    { (yyval.expr) = make_literal((yyvsp[(1) - (1)].nat)); }
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 459 "parser.yy"
    { (yyval.expr) = value_or_variable((yyvsp[(1) - (1)].str)); }
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 460 "parser.yy"
    { (yyval.expr) = &Addition::make(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 461 "parser.yy"
    { (yyval.expr) = &Subtraction::make(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 462 "parser.yy"
    { (yyval.expr) = &Multiplication::make(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].expr)); }
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 463 "parser.yy"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;



/* Line 1455 of yacc.c  */
#line 2478 "parser.cc"
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
#line 467 "parser.yy"


namespace {

class ConstantExpressionEvaluator : public ExpressionVisitor {
 public:
  explicit ConstantExpressionEvaluator(
      const std::map<std::string, TypedValue>* constant_values);

  TypedValue value() const { return value_; }

 private:
  virtual void DoVisitLiteral(const Literal& expr);
  virtual void DoVisitVariable(const Variable& expr);
  virtual void DoVisitComputation(const Computation& expr);

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

void ConstantExpressionEvaluator::DoVisitVariable(const Variable& expr) {
  std::map<std::string, TypedValue>::const_iterator i =
      constant_values_->find(expr.name());
  CHECK(i != constant_values_->end());
  value_ = i->second;
}

void ConstantExpressionEvaluator::DoVisitComputation(const Computation& expr) {
  expr.operand1().Accept(this);
  TypedValue operand1 = value_;
  expr.operand2().Accept(this);
  switch (expr.op()) {
    case Computation::PLUS:
      value_ = operand1 + value_;
      break;
    case Computation::MINUS:
      value_ = operand1 - value_;
      break;
    case Computation::MULTIPLY:
      value_ = operand1 * value_;
      break;
    case Computation::DIVIDE:
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

}  // namespace

/* Clears all previously parsed declarations. */
void clear_declarations() {
  constants.clear();
  constant_values.clear();
  for (std::map<std::string, Variable*>::const_iterator i = variables.begin();
       i != variables.end(); ++i) {
    Expression::destructive_deref(i->second);
  }
  variables.clear();
}


/* Tests if the given variables is a member of the given list. */
static bool member(const std::vector<const Variable*>& vars,
                   const Variable* v) {
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


static int integer_value(const TypedValue* q) {
  int n;
  if (q->type() != TypedValue::INT) {
    yyerror("expecting integer");
    n = 0;
  } else {
    n = q->value<int>();
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
    Variable* v = new Variable(*ident);
    variables.insert(std::make_pair(*ident, v));
    Expression::ref(v);
    rates.insert(std::make_pair(*ident, v));
    constants.insert(std::make_pair(*ident, v));
    rate_values.insert(std::make_pair(*ident, 0));
    constant_values.insert(std::make_pair(*ident, 0));
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
    if (variables.find(*ident) != variables.end()) {
      yyerror("variable `" + *ident + "' used where expecting rate");
    } else {
      yyerror("undeclared rate `" + *ident + "'");
    }
    Variable* v = new Variable(*ident);
    variables.insert(std::make_pair(*ident, v));
    Expression::ref(v);
    rates.insert(std::make_pair(*ident, v));
    rate_values.insert(std::make_pair(*ident, 0));
    delete ident;
    return v;
  }
}


/* Returns a variable representing a rate constant or an integer variable. */
static const Variable* find_rate_or_variable(const std::string* ident) {
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


static const Literal* make_literal(int n) {
  return new Literal(n);
}


static const Literal* make_literal(const TypedValue* q) {
  const Literal* v = new Literal(*q);
  delete q;
  return v;
}


/* Returns a constant value or a variable for the given identifier. */
static const Expression* value_or_variable(const std::string* ident) {
  std::map<std::string, const Variable*>::const_iterator ci =
    constants.find(*ident);
  if (ci != constants.end()) {
    const Literal* value = new Literal(constant_values.find(*ident)->second);
    delete ident;
    return value;
  } else {
    Variable* v;
    std::map<std::string, Variable*>::const_iterator vi =
      variables.find(*ident);
    if (vi == variables.end()) {
      v = new Variable(*ident);
      variables.insert(std::make_pair(*ident, v));
      Expression::ref(v);
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
      v = new Variable(*ident);
      variables.insert(std::make_pair(*ident, v));
      Expression::ref(v);
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
static StateFormula* make_probabilistic(const TypedValue* p,
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
			       const TypedValue* t1, const TypedValue* t2) {
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
    v = new Variable(*ident);
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
  std::map<std::string, Variable*>::const_iterator vi =
    variables.find(*ident1);
  if (vi != variables.end()) {
    /* Variable substitution. */
    subst.insert(std::make_pair(*ident1, *ident2));
    delete ident2;
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
    std::map<std::string, TypedValue>::iterator override =
        const_overrides.find(*ident);
    if (value_expr == NULL && override == const_overrides.end()) {
      yyerror("uninitialized constant `" + *ident + "'");
      value_expr = make_literal(0);
    }
    Variable* v = new Variable(*ident);
    variables.insert(std::make_pair(*ident, v));
    Expression::ref(v);
    rates.insert(std::make_pair(*ident, v));
    constants.insert(std::make_pair(*ident, v));
    const int value =
        ((override != const_overrides.end()) ?
         override->second :
         EvaluateConstantExpression(*value_expr, constant_values)).value<int>();
    rate_values.insert(std::make_pair(*ident, value));
    constant_values.insert(std::make_pair(*ident, value));
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
    std::map<std::string, TypedValue>::iterator override =
        const_overrides.find(*ident);
    if (value_expr == NULL && override == const_overrides.end()) {
      yyerror("uninitialized rate `" + *ident + "'");
      value_expr = make_literal(0);
    }
    Variable* v = new Variable(*ident);
    variables.insert(std::make_pair(*ident, v));
    Expression::ref(v);
    rates.insert(std::make_pair(*ident, v));
    const TypedValue value = (override != const_overrides.end()) ?
        override->second : EvaluateConstantExpression(*value_expr, rate_values);
    rate_values.insert(std::make_pair(*ident, value));
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
	undeclared.erase(*ident);
      } else {
	yyerror("ignoring repeated declaration of variable `" + *ident + "'");
      }
    } else {
      v = new Variable(*ident);
    }
  }
  if (v != NULL) {
    int low =
        EvaluateConstantExpression(*range.l, constant_values).value<int>();
    int high =
        EvaluateConstantExpression(*range.h, constant_values).value<int>();
    int s = ((start != NULL)
             ? EvaluateConstantExpression(*start, constant_values).value<int>()
             : low);
    v->SetVariableProperties(low, high, s, next_variable_index, num_model_bits);
    ++next_variable_index;
    num_model_bits = v->high_bit() + 1;
    variable_lows.insert(std::make_pair(v, range.l));
    Expression::ref(range.l);
    variable_highs.insert(std::make_pair(v, range.h));
    Expression::ref(range.h);
    variable_starts.insert(std::make_pair(v, start));
    Expression::ref(start);
    variables.insert(std::make_pair(*ident, v));
    Expression::ref(v);
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
      std::map<std::string, const Variable*> c_subst;
      for (std::map<std::string, const Variable*>::const_iterator ci =
               constants.begin();
	   ci != constants.end(); ci++) {
	std::map<std::string, std::string>::const_iterator si =
            subst.find(ci->first);
	if (si != subst.end()) {
	  std::map<std::string, const Variable*>::const_iterator cj =
	    constants.find(si->second);
	  if (cj != constants.end()) {
	    c_subst.insert(std::make_pair(ci->first, cj->second));
	  } else {
	    yyerror("substituting constant with non-constant " + si->second);
	  }
	}
      }
      std::map<std::string, const Variable*> v_subst;
      for (std::vector<const Variable*>::const_iterator vi =
               src_module.variables().begin();
	   vi != src_module.variables().end(); vi++) {
	std::map<std::string, std::string>::const_iterator si =
            subst.find((*vi)->name());
	if (si == subst.end()) {
	  yyerror("missing substitution for module variable");
	} else {
	  const Expression* low =
              SubstituteIdentifiers(*variable_lows.find(*vi)->second, c_subst);
	  const Expression* high =
              SubstituteIdentifiers(*variable_highs.find(*vi)->second, c_subst);
	  const Expression* start;
	  std::map<const Variable*, const Expression*>::const_iterator i =
	    variable_starts.find(*vi);
	  if ((*i).second != NULL) {
	    start = SubstituteIdentifiers(*i->second, c_subst);
	  } else {
	    start = NULL;
	  }
	  Range r = { low, high };
	  const Variable* v =
              declare_variable(new std::string(si->second), r, start, true);
	  if (v != NULL) {
	    v_subst.insert(std::make_pair((*vi)->name(), v));
	  }
	}
      }
      for (std::map<std::string, std::string>::const_iterator si =
               subst.begin();
	   si != subst.end(); si++) {
	const Variable* v1 = find_variable(new std::string(si->first));
	if (!member(src_module.variables(), v1)) {
	  const Variable* v2 = find_variable(new std::string(si->second));
          v_subst.insert(std::make_pair(v1->name(), v2));
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
  next_variable_index = 0;
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


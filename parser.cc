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
#line 25 "parser.yy" /* yacc.c:339  */

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
/* Constant overrides. */
extern std::map<std::string, Rational> const_overrides;

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
/* Returns a variable representing a rate constant or an integer variable. */
static const Variable* find_rate_or_variable(const std::string* ident);
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

#line 216 "parser.cc" /* yacc.c:339  */

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
# define YYERROR_VERBOSE 0
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
    LTE = 281,
    GTE = 282,
    EQ = 283,
    NEQ = 284
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
#define LTE 281
#define GTE 282
#define EQ 283
#define NEQ 284

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 192 "parser.yy" /* yacc.c:355  */

  size_t synch;
  StateFormula* formula;
  const PathFormula* path;
  const Distribution* dist;
  const Expression* expr;
  Range range;
  int nat;
  const std::string* str;
  const Rational* num;

#line 326 "parser.cc" /* yacc.c:355  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 341 "parser.cc" /* yacc.c:358  */

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
#define YYLAST   386

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  51
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  40
/* YYNRULES -- Number of rules.  */
#define YYNRULES  124
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  284

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   284

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    28,     2,     2,     2,     2,    26,     2,
      45,    46,    37,    35,    44,    36,     2,    38,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    41,    39,
      29,    40,    32,     2,     2,     2,     2,     2,     2,     2,
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
      25,    30,    31,    33,    34
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   216,   216,   216,   220,   221,   228,   228,   232,   232,
     238,   239,   242,   243,   244,   245,   247,   248,   249,   250,
     252,   253,   261,   262,   265,   265,   267,   271,   272,   275,
     276,   279,   282,   283,   286,   287,   291,   292,   296,   295,
     299,   300,   303,   304,   305,   312,   313,   316,   319,   320,
     323,   324,   325,   328,   332,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   358,   359,   360,
     362,   364,   371,   372,   373,   374,   375,   376,   379,   380,
     381,   382,   383,   384,   385,   388,   389,   390,   392,   394,
     401,   404,   405,   406,   407,   408,   410,   413,   420,   421,
     425,   426,   427,   429,   431,   433,   435,   436,   437,   438,
     439,   440,   441,   442,   443,   444,   445,   448,   450,   455,
     456,   457,   458,   459,   460
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "STOCHASTIC", "CTMC", "CONST_TOKEN",
  "INT", "DOUBLE", "RATE", "GLOBAL", "INIT", "TRUE_TOKEN", "FALSE_TOKEN",
  "EXP", "REWARDS", "ENDREWARDS", "MODULE", "ENDMODULE", "PNAME", "NAME",
  "LABEL_NAME", "NUMBER", "ARROW", "DOTDOT", "ILLEGAL_TOKEN", "IMPLY",
  "'&'", "'|'", "'!'", "'<'", "LTE", "GTE", "'>'", "EQ", "NEQ", "'+'",
  "'-'", "'*'", "'/'", "';'", "'='", "':'", "'['", "']'", "','", "'('",
  "')'", "'W'", "'L'", "'U'", "'P'", "$accept", "file", "$@1",
  "model_or_properties", "model", "$@2", "model_type", "declarations",
  "declaration", "modules", "module_decl", "$@3", "substitutions",
  "subst_list", "subst", "variables", "variable_decl", "commands",
  "command", "$@4", "synchronization", "update", "rewards", "rewards_decl",
  "rewards_label", "reward_rules", "state_reward", "transition_reward",
  "formula", "distribution", "expr", "rate_expr", "const_rate_expr",
  "range", "const_expr", "integer", "properties", "csl_formula",
  "path_formula", "csl_expr", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,    38,   124,    33,    60,
     281,   282,    62,   283,   284,    43,    45,    42,    47,    59,
      61,    58,    91,    93,    44,    40,    41,    87,    76,    85,
      80
};
# endif

#define YYPACT_NINF -260

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-260)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -260,    27,   178,  -260,  -260,  -260,  -260,  -260,  -260,   112,
    -260,  -260,  -260,  -260,  -260,   112,   112,   303,  -260,   315,
     207,   291,  -260,   143,   156,    11,    15,    26,    49,   112,
     112,   112,    47,    47,    47,    47,    47,    47,    47,    47,
      47,   130,    53,    61,  -260,    66,  -260,  -260,    55,    59,
      87,    97,   199,  -260,  -260,    47,   308,   308,   308,   308,
     308,    58,    58,  -260,   308,   126,   129,   218,   319,   114,
     145,  -260,   170,   112,   112,   112,   112,   233,   321,   323,
    -260,    48,  -260,   106,   153,   157,   179,  -260,   116,   158,
     164,   197,   239,  -260,    48,  -260,   106,  -260,    48,   277,
    -260,  -260,  -260,   106,   309,    48,     5,   269,  -260,  -260,
    -260,    23,  -260,  -260,  -260,  -260,   282,   312,   237,    48,
      48,    48,  -260,    17,   106,   106,  -260,   198,    48,  -260,
     244,   278,   135,   294,   299,  -260,  -260,  -260,   288,   288,
    -260,  -260,  -260,  -260,    48,   287,   311,   325,  -260,    29,
    -260,  -260,  -260,  -260,   155,   348,   155,  -260,  -260,   189,
     219,  -260,   112,   324,   255,  -260,   329,   328,   326,  -260,
     153,  -260,    79,  -260,   155,  -260,   330,   152,   174,   155,
     155,   107,   113,   113,   113,   113,   113,   113,   113,   113,
     113,   315,   351,  -260,   355,   358,   311,     6,   333,  -260,
     202,   155,  -260,  -260,  -260,  -260,  -260,  -260,   107,   267,
     113,   317,   317,   317,   317,   317,   340,   340,  -260,   317,
     335,  -260,  -260,  -260,    48,  -260,  -260,    30,   191,   225,
     107,   107,   107,   107,  -260,   241,   112,   292,   334,   336,
     337,   338,   339,   301,   107,  -260,   327,   327,  -260,  -260,
     315,  -260,   107,   106,   106,   106,  -260,   272,   229,   175,
     208,   257,    -7,  -260,  -260,   106,   106,   106,   344,    -7,
     -13,   185,   243,   247,   113,   -22,    -7,  -260,  -260,  -260,
    -260,   317,  -260,  -260
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,    98,     1,     8,     9,     3,     4,     6,     5,
      10,   100,   101,   120,    97,     0,     0,     0,   119,    99,
       0,    22,   109,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    11,    45,   116,   124,     0,     0,
       0,     0,   106,   107,   108,     0,   110,   111,   112,   113,
     115,   121,   122,   123,   114,     0,     0,     0,     0,     0,
       0,    23,     7,     0,     0,     0,     0,     0,     0,     0,
      12,     0,    16,     0,     0,    24,    48,    46,     0,     0,
       0,     0,     0,    14,     0,    18,     0,    92,     0,     0,
      91,    86,    85,     0,     0,     0,     0,     0,    32,    49,
      50,     0,   102,   103,   104,   105,     0,     0,     0,     0,
       0,     0,    13,     0,     0,     0,    17,     0,     0,    20,
       0,    36,     0,     0,     0,    15,    19,    96,    93,    94,
      95,    89,    87,    88,     0,     0,    27,     0,    33,     0,
      55,    56,    47,    73,     0,     0,     0,    51,    52,     0,
       0,    72,     0,     0,     0,    21,     0,     0,    28,    29,
       0,    25,     0,    37,     0,    59,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   117,     0,    90,     0,     0,     0,     0,     0,    40,
       0,     0,    66,    77,    57,    58,    79,    78,     0,     0,
       0,    60,    61,    62,    63,    65,    74,    75,    76,    64,
       0,    31,    26,    30,     0,    34,    41,     0,     0,     0,
       0,     0,     0,     0,    53,     0,     0,     0,     0,     0,
       0,     0,     0,    67,     0,    84,    80,    81,    82,    83,
     118,    35,     0,     0,     0,     0,    38,     0,     0,     0,
       0,     0,     0,    54,    68,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    39,    69,    70,
      71,    42,    44,    43
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -260,  -260,  -260,  -260,  -260,  -260,  -260,  -260,  -260,  -260,
    -260,  -260,  -260,  -260,   190,  -260,  -260,  -260,  -260,  -260,
    -260,  -259,  -260,  -260,  -260,  -260,  -260,  -260,  -151,  -260,
     -99,  -171,   -94,   215,   -86,   -80,  -260,    -9,   281,    80
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     6,     7,    10,     8,    21,    44,    45,
      71,   108,   167,   168,   169,   131,   148,   149,   173,   262,
     174,   270,    72,    87,   110,   132,   157,   158,   159,   242,
     160,   209,   104,   106,    99,    18,     9,    88,    89,    20
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint16 yytable[] =
{
      19,   100,   117,   175,   276,   177,    22,    23,   116,   123,
     275,   268,   118,   276,   100,   128,   224,   283,   100,   127,
      52,    53,    54,   200,   282,   100,   277,     3,   204,   205,
     142,   143,    48,   138,   139,   140,    49,   229,   269,   100,
     100,   100,   145,   238,   129,   225,   171,    50,   100,   206,
     228,   207,   161,   133,   124,   125,   243,   178,   164,   246,
     247,   248,   249,   141,   100,   134,    13,    97,    14,    14,
      51,   172,    68,   257,   161,   208,   161,   239,   240,   241,
      69,   258,    70,   211,   212,   213,   214,   215,   216,   217,
     218,   219,    55,    98,   161,    39,    24,    73,   198,   161,
     161,    74,   161,   161,   161,   161,   161,   161,   161,   161,
     161,   235,    56,    57,    58,    59,    60,    61,    62,    63,
      64,   161,   199,    11,    12,   101,   206,   102,   207,    75,
     161,    13,   153,    14,    14,    77,    65,    66,   237,    76,
      15,    29,    30,    31,   100,    78,   150,   151,    79,    67,
     152,   103,   208,   191,   153,    84,    14,    16,   210,   259,
     260,   261,    17,   154,    85,   111,   150,   151,    29,    30,
      31,   271,   272,   273,   153,   281,    14,   155,   179,   180,
     156,     4,     5,   154,    86,    32,    33,    34,    35,    46,
      36,    37,    38,    39,   161,   105,    40,   107,   202,   109,
     156,   112,    47,   182,   183,   184,   185,   113,   186,   187,
     188,   189,   124,   125,   190,   179,   180,   179,   180,   265,
     203,   144,   124,   125,   227,    30,    31,   250,   179,   180,
     181,   278,   244,   119,   120,   121,    32,    33,    34,    35,
     114,    36,    37,    38,    39,   124,   125,    40,   182,   183,
     184,   185,   266,   186,   187,   188,   189,    80,    81,   190,
     230,   231,   232,   233,   230,   231,   232,   233,    37,    38,
      39,   245,   119,   120,   121,   264,   187,   188,   189,    47,
     124,   125,   115,   137,   124,   125,   146,   203,   130,   279,
     119,   120,   121,   280,   124,   125,    41,   147,   193,    42,
      43,   267,   230,   231,   232,   233,   234,   230,   231,   232,
     233,   263,   119,   120,   121,   162,   122,   119,   120,   121,
     163,   135,   119,   120,   121,   121,   165,   119,   120,   121,
     166,   251,    25,    26,    27,    28,   230,   231,   232,   233,
      29,    30,    31,    37,    38,    39,   124,   125,   126,   124,
     125,   136,   187,   188,   189,    90,    91,    92,    82,    83,
      93,    94,    95,    96,   232,   233,   170,   176,   192,   194,
     196,   195,   220,   201,   221,   222,   226,   189,   236,   252,
     256,   253,   254,   255,   274,   197,   223
};

static const yytype_uint16 yycheck[] =
{
       9,    81,    96,   154,    26,   156,    15,    16,    94,   103,
     269,    18,    98,    26,    94,    10,    10,   276,    98,   105,
      29,    30,    31,   174,    46,   105,    39,     0,   179,   180,
     124,   125,    21,   119,   120,   121,    21,   208,    45,   119,
     120,   121,   128,    13,    39,    39,    17,    21,   128,    19,
     201,    21,   132,    30,    37,    38,   227,   156,   144,   230,
     231,   232,   233,    46,   144,    42,    19,    19,    21,    21,
      21,    42,    19,   244,   154,    45,   156,    47,    48,    49,
      19,   252,    16,   182,   183,   184,   185,   186,   187,   188,
     189,   190,    45,    45,   174,    37,    16,    42,    19,   179,
     180,    42,   182,   183,   184,   185,   186,   187,   188,   189,
     190,   210,    32,    33,    34,    35,    36,    37,    38,    39,
      40,   201,    43,    11,    12,    19,    19,    21,    21,    42,
     210,    19,    19,    21,    21,    55,     6,     7,   224,    42,
      28,    25,    26,    27,   224,    19,    11,    12,    19,    19,
      15,    45,    45,   162,    19,    41,    21,    45,    45,   253,
     254,   255,    50,    28,    19,    49,    11,    12,    25,    26,
      27,   265,   266,   267,    19,   274,    21,    42,    26,    27,
      45,     3,     4,    28,    14,    29,    30,    31,    32,    46,
      34,    35,    36,    37,   274,    42,    40,    40,    46,    20,
      45,    43,    46,    29,    30,    31,    32,    43,    34,    35,
      36,    37,    37,    38,    40,    26,    27,    26,    27,    44,
      46,    23,    37,    38,    22,    26,    27,   236,    26,    27,
      41,    46,    41,    35,    36,    37,    29,    30,    31,    32,
      43,    34,    35,    36,    37,    37,    38,    40,    29,    30,
      31,    32,    44,    34,    35,    36,    37,    39,    40,    40,
      35,    36,    37,    38,    35,    36,    37,    38,    35,    36,
      37,    46,    35,    36,    37,    46,    35,    36,    37,    46,
      37,    38,    43,    46,    37,    38,    42,    46,    19,    46,
      35,    36,    37,    46,    37,    38,     5,    19,    43,     8,
       9,    44,    35,    36,    37,    38,    39,    35,    36,    37,
      38,    39,    35,    36,    37,    21,    39,    35,    36,    37,
      21,    39,    35,    36,    37,    37,    39,    35,    36,    37,
      19,    39,    29,    30,    31,    32,    35,    36,    37,    38,
      25,    26,    27,    35,    36,    37,    37,    38,    39,    37,
      38,    39,    35,    36,    37,    74,    75,    76,    39,    40,
      39,    40,    39,    40,    37,    38,    41,    19,    44,    40,
      44,    43,    21,    43,    19,    17,    43,    37,    43,    45,
      41,    45,    45,    45,    40,   170,   196
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    52,    53,     0,     3,     4,    54,    55,    57,    87,
      56,    11,    12,    19,    21,    28,    45,    50,    86,    88,
      90,    58,    88,    88,    90,    29,    30,    31,    32,    25,
      26,    27,    29,    30,    31,    32,    34,    35,    36,    37,
      40,     5,     8,     9,    59,    60,    46,    46,    21,    21,
      21,    21,    88,    88,    88,    45,    90,    90,    90,    90,
      90,    90,    90,    90,    90,     6,     7,    19,    19,    19,
      16,    61,    73,    42,    42,    42,    42,    90,    19,    19,
      39,    40,    39,    40,    41,    19,    14,    74,    88,    89,
      89,    89,    89,    39,    40,    39,    40,    19,    45,    85,
      86,    19,    21,    45,    83,    42,    84,    40,    62,    20,
      75,    49,    43,    43,    43,    43,    85,    83,    85,    35,
      36,    37,    39,    83,    37,    38,    39,    85,    10,    39,
      19,    66,    76,    30,    42,    39,    39,    46,    85,    85,
      85,    46,    83,    83,    23,    85,    42,    19,    67,    68,
      11,    12,    15,    19,    28,    42,    45,    77,    78,    79,
      81,    86,    21,    21,    85,    39,    19,    63,    64,    65,
      41,    17,    42,    69,    71,    79,    19,    79,    81,    26,
      27,    41,    29,    30,    31,    32,    34,    35,    36,    37,
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

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
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
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



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

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
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
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
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
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

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

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
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

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
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
      yychar = yylex ();
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


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 216 "parser.yy" /* yacc.c:1646  */
    { success = true; line_number = 1; }
#line 1629 "parser.cc" /* yacc.c:1646  */
    break;

  case 3:
#line 217 "parser.yy" /* yacc.c:1646  */
    { check_undeclared(); if (!success) YYERROR; }
#line 1635 "parser.cc" /* yacc.c:1646  */
    break;

  case 6:
#line 228 "parser.yy" /* yacc.c:1646  */
    { prepare_model(); }
#line 1641 "parser.cc" /* yacc.c:1646  */
    break;

  case 7:
#line 229 "parser.yy" /* yacc.c:1646  */
    { compile_model(); }
#line 1647 "parser.cc" /* yacc.c:1646  */
    break;

  case 12:
#line 242 "parser.yy" /* yacc.c:1646  */
    { declare_constant((yyvsp[-1].str), NULL); }
#line 1653 "parser.cc" /* yacc.c:1646  */
    break;

  case 13:
#line 243 "parser.yy" /* yacc.c:1646  */
    { declare_constant((yyvsp[-3].str), (yyvsp[-1].expr)); }
#line 1659 "parser.cc" /* yacc.c:1646  */
    break;

  case 14:
#line 244 "parser.yy" /* yacc.c:1646  */
    { declare_constant((yyvsp[-1].str), NULL); }
#line 1665 "parser.cc" /* yacc.c:1646  */
    break;

  case 15:
#line 246 "parser.yy" /* yacc.c:1646  */
    { declare_constant((yyvsp[-3].str), (yyvsp[-1].expr)); }
#line 1671 "parser.cc" /* yacc.c:1646  */
    break;

  case 16:
#line 247 "parser.yy" /* yacc.c:1646  */
    { declare_rate((yyvsp[-1].str), NULL); }
#line 1677 "parser.cc" /* yacc.c:1646  */
    break;

  case 17:
#line 248 "parser.yy" /* yacc.c:1646  */
    { declare_rate((yyvsp[-3].str), (yyvsp[-1].expr)); }
#line 1683 "parser.cc" /* yacc.c:1646  */
    break;

  case 18:
#line 249 "parser.yy" /* yacc.c:1646  */
    { declare_rate((yyvsp[-1].str), NULL); }
#line 1689 "parser.cc" /* yacc.c:1646  */
    break;

  case 19:
#line 251 "parser.yy" /* yacc.c:1646  */
    { declare_rate((yyvsp[-3].str), (yyvsp[-1].expr)); }
#line 1695 "parser.cc" /* yacc.c:1646  */
    break;

  case 20:
#line 252 "parser.yy" /* yacc.c:1646  */
    { declare_variable((yyvsp[-3].str), (yyvsp[-1].range), NULL); }
#line 1701 "parser.cc" /* yacc.c:1646  */
    break;

  case 21:
#line 254 "parser.yy" /* yacc.c:1646  */
    { declare_variable((yyvsp[-5].str), (yyvsp[-3].range), (yyvsp[-1].expr)); }
#line 1707 "parser.cc" /* yacc.c:1646  */
    break;

  case 24:
#line 265 "parser.yy" /* yacc.c:1646  */
    { prepare_module((yyvsp[0].str)); }
#line 1713 "parser.cc" /* yacc.c:1646  */
    break;

  case 25:
#line 266 "parser.yy" /* yacc.c:1646  */
    { add_module(); }
#line 1719 "parser.cc" /* yacc.c:1646  */
    break;

  case 26:
#line 268 "parser.yy" /* yacc.c:1646  */
    { add_module((yyvsp[-6].str), (yyvsp[-4].str)); }
#line 1725 "parser.cc" /* yacc.c:1646  */
    break;

  case 31:
#line 279 "parser.yy" /* yacc.c:1646  */
    { add_substitution((yyvsp[-2].str), (yyvsp[0].str)); }
#line 1731 "parser.cc" /* yacc.c:1646  */
    break;

  case 34:
#line 286 "parser.yy" /* yacc.c:1646  */
    { declare_variable((yyvsp[-3].str), (yyvsp[-1].range), NULL); }
#line 1737 "parser.cc" /* yacc.c:1646  */
    break;

  case 35:
#line 288 "parser.yy" /* yacc.c:1646  */
    { declare_variable((yyvsp[-5].str), (yyvsp[-3].range), (yyvsp[-1].expr)); }
#line 1743 "parser.cc" /* yacc.c:1646  */
    break;

  case 38:
#line 296 "parser.yy" /* yacc.c:1646  */
    { prepare_command((yyvsp[-4].synch), *(yyvsp[-3].formula), *(yyvsp[-1].dist)); }
#line 1749 "parser.cc" /* yacc.c:1646  */
    break;

  case 39:
#line 296 "parser.yy" /* yacc.c:1646  */
    { add_command(); }
#line 1755 "parser.cc" /* yacc.c:1646  */
    break;

  case 40:
#line 299 "parser.yy" /* yacc.c:1646  */
    { (yyval.synch) = 0; }
#line 1761 "parser.cc" /* yacc.c:1646  */
    break;

  case 41:
#line 300 "parser.yy" /* yacc.c:1646  */
    { (yyval.synch) = synchronization_value((yyvsp[-1].str)); }
#line 1767 "parser.cc" /* yacc.c:1646  */
    break;

  case 42:
#line 303 "parser.yy" /* yacc.c:1646  */
    { add_update((yyvsp[-2].str), *(yyvsp[0].expr)); }
#line 1773 "parser.cc" /* yacc.c:1646  */
    break;

  case 53:
#line 329 "parser.yy" /* yacc.c:1646  */
    { delete (yyvsp[-3].formula); delete (yyvsp[-1].expr); }
#line 1779 "parser.cc" /* yacc.c:1646  */
    break;

  case 54:
#line 333 "parser.yy" /* yacc.c:1646  */
    { delete (yyvsp[-5].str); delete (yyvsp[-3].formula); delete (yyvsp[-1].expr); }
#line 1785 "parser.cc" /* yacc.c:1646  */
    break;

  case 55:
#line 340 "parser.yy" /* yacc.c:1646  */
    { (yyval.formula) = new Conjunction(); }
#line 1791 "parser.cc" /* yacc.c:1646  */
    break;

  case 56:
#line 341 "parser.yy" /* yacc.c:1646  */
    { (yyval.formula) = new Disjunction(); }
#line 1797 "parser.cc" /* yacc.c:1646  */
    break;

  case 57:
#line 342 "parser.yy" /* yacc.c:1646  */
    { (yyval.formula) = make_conjunction(*(yyvsp[-2].formula), *(yyvsp[0].formula)); }
#line 1803 "parser.cc" /* yacc.c:1646  */
    break;

  case 58:
#line 343 "parser.yy" /* yacc.c:1646  */
    { (yyval.formula) = make_disjunction(*(yyvsp[-2].formula), *(yyvsp[0].formula)); }
#line 1809 "parser.cc" /* yacc.c:1646  */
    break;

  case 59:
#line 344 "parser.yy" /* yacc.c:1646  */
    { (yyval.formula) = new Negation(*(yyvsp[0].formula)); }
#line 1815 "parser.cc" /* yacc.c:1646  */
    break;

  case 60:
#line 345 "parser.yy" /* yacc.c:1646  */
    { (yyval.formula) = new LessThan(*(yyvsp[-2].expr), *(yyvsp[0].expr)); }
#line 1821 "parser.cc" /* yacc.c:1646  */
    break;

  case 61:
#line 346 "parser.yy" /* yacc.c:1646  */
    { (yyval.formula) = new LessThanOrEqual(*(yyvsp[-2].expr), *(yyvsp[0].expr)); }
#line 1827 "parser.cc" /* yacc.c:1646  */
    break;

  case 62:
#line 347 "parser.yy" /* yacc.c:1646  */
    { (yyval.formula) = new GreaterThanOrEqual(*(yyvsp[-2].expr), *(yyvsp[0].expr)); }
#line 1833 "parser.cc" /* yacc.c:1646  */
    break;

  case 63:
#line 348 "parser.yy" /* yacc.c:1646  */
    { (yyval.formula) = new GreaterThan(*(yyvsp[-2].expr), *(yyvsp[0].expr)); }
#line 1839 "parser.cc" /* yacc.c:1646  */
    break;

  case 64:
#line 349 "parser.yy" /* yacc.c:1646  */
    { (yyval.formula) = new Equality(*(yyvsp[-2].expr), *(yyvsp[0].expr)); }
#line 1845 "parser.cc" /* yacc.c:1646  */
    break;

  case 65:
#line 350 "parser.yy" /* yacc.c:1646  */
    { (yyval.formula) = new Inequality(*(yyvsp[-2].expr), *(yyvsp[0].expr)); }
#line 1851 "parser.cc" /* yacc.c:1646  */
    break;

  case 66:
#line 351 "parser.yy" /* yacc.c:1646  */
    { (yyval.formula) = (yyvsp[-1].formula); }
#line 1857 "parser.cc" /* yacc.c:1646  */
    break;

  case 67:
#line 358 "parser.yy" /* yacc.c:1646  */
    { (yyval.dist) = &Exponential::make(*(yyvsp[0].expr)); }
#line 1863 "parser.cc" /* yacc.c:1646  */
    break;

  case 68:
#line 359 "parser.yy" /* yacc.c:1646  */
    { (yyval.dist) = &Exponential::make(*(yyvsp[-1].expr)); }
#line 1869 "parser.cc" /* yacc.c:1646  */
    break;

  case 69:
#line 361 "parser.yy" /* yacc.c:1646  */
    { (yyval.dist) = &Weibull::make(*(yyvsp[-3].expr), *(yyvsp[-1].expr)); }
#line 1875 "parser.cc" /* yacc.c:1646  */
    break;

  case 70:
#line 363 "parser.yy" /* yacc.c:1646  */
    { (yyval.dist) = &Lognormal::make(*(yyvsp[-3].expr), *(yyvsp[-1].expr)); }
#line 1881 "parser.cc" /* yacc.c:1646  */
    break;

  case 71:
#line 365 "parser.yy" /* yacc.c:1646  */
    { (yyval.dist) = &Uniform::make(*(yyvsp[-3].expr), *(yyvsp[-1].expr)); }
#line 1887 "parser.cc" /* yacc.c:1646  */
    break;

  case 72:
#line 371 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = make_value((yyvsp[0].nat)); }
#line 1893 "parser.cc" /* yacc.c:1646  */
    break;

  case 73:
#line 372 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = find_variable((yyvsp[0].str)); }
#line 1899 "parser.cc" /* yacc.c:1646  */
    break;

  case 74:
#line 373 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = &Addition::make(*(yyvsp[-2].expr), *(yyvsp[0].expr)); }
#line 1905 "parser.cc" /* yacc.c:1646  */
    break;

  case 75:
#line 374 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = &Subtraction::make(*(yyvsp[-2].expr), *(yyvsp[0].expr)); }
#line 1911 "parser.cc" /* yacc.c:1646  */
    break;

  case 76:
#line 375 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = &Multiplication::make(*(yyvsp[-2].expr), *(yyvsp[0].expr)); }
#line 1917 "parser.cc" /* yacc.c:1646  */
    break;

  case 77:
#line 376 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[-1].expr); }
#line 1923 "parser.cc" /* yacc.c:1646  */
    break;

  case 78:
#line 379 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = make_value((yyvsp[0].num)); }
#line 1929 "parser.cc" /* yacc.c:1646  */
    break;

  case 79:
#line 380 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = find_rate_or_variable((yyvsp[0].str)); }
#line 1935 "parser.cc" /* yacc.c:1646  */
    break;

  case 80:
#line 381 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = &Addition::make(*(yyvsp[-2].expr), *(yyvsp[0].expr)); }
#line 1941 "parser.cc" /* yacc.c:1646  */
    break;

  case 81:
#line 382 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = &Subtraction::make(*(yyvsp[-2].expr), *(yyvsp[0].expr)); }
#line 1947 "parser.cc" /* yacc.c:1646  */
    break;

  case 82:
#line 383 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = &Multiplication::make(*(yyvsp[-2].expr), *(yyvsp[0].expr)); }
#line 1953 "parser.cc" /* yacc.c:1646  */
    break;

  case 83:
#line 384 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = &Division::make(*(yyvsp[-2].expr), *(yyvsp[0].expr)); }
#line 1959 "parser.cc" /* yacc.c:1646  */
    break;

  case 84:
#line 385 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[-1].expr); }
#line 1965 "parser.cc" /* yacc.c:1646  */
    break;

  case 85:
#line 388 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = make_value((yyvsp[0].num)); }
#line 1971 "parser.cc" /* yacc.c:1646  */
    break;

  case 86:
#line 389 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = find_rate((yyvsp[0].str)); }
#line 1977 "parser.cc" /* yacc.c:1646  */
    break;

  case 87:
#line 391 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = &Multiplication::make(*(yyvsp[-2].expr), *(yyvsp[0].expr)); }
#line 1983 "parser.cc" /* yacc.c:1646  */
    break;

  case 88:
#line 393 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = &Division::make(*(yyvsp[-2].expr), *(yyvsp[0].expr)); }
#line 1989 "parser.cc" /* yacc.c:1646  */
    break;

  case 89:
#line 394 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[-1].expr); }
#line 1995 "parser.cc" /* yacc.c:1646  */
    break;

  case 90:
#line 401 "parser.yy" /* yacc.c:1646  */
    { (yyval.range) = make_range((yyvsp[-3].expr), (yyvsp[-1].expr)); }
#line 2001 "parser.cc" /* yacc.c:1646  */
    break;

  case 91:
#line 404 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = make_value((yyvsp[0].nat)); }
#line 2007 "parser.cc" /* yacc.c:1646  */
    break;

  case 92:
#line 405 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = find_constant((yyvsp[0].str)); }
#line 2013 "parser.cc" /* yacc.c:1646  */
    break;

  case 93:
#line 406 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = &Addition::make(*(yyvsp[-2].expr), *(yyvsp[0].expr)); }
#line 2019 "parser.cc" /* yacc.c:1646  */
    break;

  case 94:
#line 407 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = &Subtraction::make(*(yyvsp[-2].expr), *(yyvsp[0].expr)); }
#line 2025 "parser.cc" /* yacc.c:1646  */
    break;

  case 95:
#line 409 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = &Multiplication::make(*(yyvsp[-2].expr), *(yyvsp[0].expr)); }
#line 2031 "parser.cc" /* yacc.c:1646  */
    break;

  case 96:
#line 410 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[-1].expr); }
#line 2037 "parser.cc" /* yacc.c:1646  */
    break;

  case 97:
#line 413 "parser.yy" /* yacc.c:1646  */
    { (yyval.nat) = integer_value((yyvsp[0].num)); }
#line 2043 "parser.cc" /* yacc.c:1646  */
    break;

  case 99:
#line 422 "parser.yy" /* yacc.c:1646  */
    { properties.push_back((yyvsp[0].formula)); StateFormula::ref((yyvsp[0].formula)); }
#line 2049 "parser.cc" /* yacc.c:1646  */
    break;

  case 100:
#line 425 "parser.yy" /* yacc.c:1646  */
    { (yyval.formula) = new Conjunction(); }
#line 2055 "parser.cc" /* yacc.c:1646  */
    break;

  case 101:
#line 426 "parser.yy" /* yacc.c:1646  */
    { (yyval.formula) = new Disjunction(); }
#line 2061 "parser.cc" /* yacc.c:1646  */
    break;

  case 102:
#line 428 "parser.yy" /* yacc.c:1646  */
    { (yyval.formula) = make_probabilistic((yyvsp[-3].num), true, true, *(yyvsp[-1].path)); }
#line 2067 "parser.cc" /* yacc.c:1646  */
    break;

  case 103:
#line 430 "parser.yy" /* yacc.c:1646  */
    { (yyval.formula) = make_probabilistic((yyvsp[-3].num), false, true, *(yyvsp[-1].path)); }
#line 2073 "parser.cc" /* yacc.c:1646  */
    break;

  case 104:
#line 432 "parser.yy" /* yacc.c:1646  */
    { (yyval.formula) = make_probabilistic((yyvsp[-3].num), false, false, *(yyvsp[-1].path)); }
#line 2079 "parser.cc" /* yacc.c:1646  */
    break;

  case 105:
#line 434 "parser.yy" /* yacc.c:1646  */
    { (yyval.formula) = make_probabilistic((yyvsp[-3].num), true, false, *(yyvsp[-1].path)); }
#line 2085 "parser.cc" /* yacc.c:1646  */
    break;

  case 106:
#line 435 "parser.yy" /* yacc.c:1646  */
    { (yyval.formula) = new Implication(*(yyvsp[-2].formula), *(yyvsp[0].formula)); }
#line 2091 "parser.cc" /* yacc.c:1646  */
    break;

  case 107:
#line 436 "parser.yy" /* yacc.c:1646  */
    { (yyval.formula) = make_conjunction(*(yyvsp[-2].formula), *(yyvsp[0].formula)); }
#line 2097 "parser.cc" /* yacc.c:1646  */
    break;

  case 108:
#line 437 "parser.yy" /* yacc.c:1646  */
    { (yyval.formula) = make_disjunction(*(yyvsp[-2].formula), *(yyvsp[0].formula)); }
#line 2103 "parser.cc" /* yacc.c:1646  */
    break;

  case 109:
#line 438 "parser.yy" /* yacc.c:1646  */
    { (yyval.formula) = new Negation(*(yyvsp[0].formula)); }
#line 2109 "parser.cc" /* yacc.c:1646  */
    break;

  case 110:
#line 439 "parser.yy" /* yacc.c:1646  */
    { (yyval.formula) = new LessThan(*(yyvsp[-2].expr), *(yyvsp[0].expr)); }
#line 2115 "parser.cc" /* yacc.c:1646  */
    break;

  case 111:
#line 440 "parser.yy" /* yacc.c:1646  */
    { (yyval.formula) = new LessThanOrEqual(*(yyvsp[-2].expr), *(yyvsp[0].expr)); }
#line 2121 "parser.cc" /* yacc.c:1646  */
    break;

  case 112:
#line 441 "parser.yy" /* yacc.c:1646  */
    { (yyval.formula) = new GreaterThanOrEqual(*(yyvsp[-2].expr), *(yyvsp[0].expr)); }
#line 2127 "parser.cc" /* yacc.c:1646  */
    break;

  case 113:
#line 442 "parser.yy" /* yacc.c:1646  */
    { (yyval.formula) = new GreaterThan(*(yyvsp[-2].expr), *(yyvsp[0].expr)); }
#line 2133 "parser.cc" /* yacc.c:1646  */
    break;

  case 114:
#line 443 "parser.yy" /* yacc.c:1646  */
    { (yyval.formula) = new Equality(*(yyvsp[-2].expr), *(yyvsp[0].expr)); }
#line 2139 "parser.cc" /* yacc.c:1646  */
    break;

  case 115:
#line 444 "parser.yy" /* yacc.c:1646  */
    { (yyval.formula) = new Inequality(*(yyvsp[-2].expr), *(yyvsp[0].expr)); }
#line 2145 "parser.cc" /* yacc.c:1646  */
    break;

  case 116:
#line 445 "parser.yy" /* yacc.c:1646  */
    { (yyval.formula) = (yyvsp[-1].formula); }
#line 2151 "parser.cc" /* yacc.c:1646  */
    break;

  case 117:
#line 449 "parser.yy" /* yacc.c:1646  */
    { (yyval.path) = make_until(*(yyvsp[-4].formula), *(yyvsp[0].formula), NULL, (yyvsp[-1].num)); }
#line 2157 "parser.cc" /* yacc.c:1646  */
    break;

  case 118:
#line 451 "parser.yy" /* yacc.c:1646  */
    { (yyval.path) = make_until(*(yyvsp[-7].formula), *(yyvsp[0].formula), (yyvsp[-4].num), (yyvsp[-2].num)); }
#line 2163 "parser.cc" /* yacc.c:1646  */
    break;

  case 119:
#line 455 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = make_value((yyvsp[0].nat)); }
#line 2169 "parser.cc" /* yacc.c:1646  */
    break;

  case 120:
#line 456 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = value_or_variable((yyvsp[0].str)); }
#line 2175 "parser.cc" /* yacc.c:1646  */
    break;

  case 121:
#line 457 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = &Addition::make(*(yyvsp[-2].expr), *(yyvsp[0].expr)); }
#line 2181 "parser.cc" /* yacc.c:1646  */
    break;

  case 122:
#line 458 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = &Subtraction::make(*(yyvsp[-2].expr), *(yyvsp[0].expr)); }
#line 2187 "parser.cc" /* yacc.c:1646  */
    break;

  case 123:
#line 459 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = &Multiplication::make(*(yyvsp[-2].expr), *(yyvsp[0].expr)); }
#line 2193 "parser.cc" /* yacc.c:1646  */
    break;

  case 124:
#line 460 "parser.yy" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[-1].expr); }
#line 2199 "parser.cc" /* yacc.c:1646  */
    break;


#line 2203 "parser.cc" /* yacc.c:1646  */
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


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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
  /* Do not reclaim the symbols of the rule whose action triggered
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
  return yyresult;
}
#line 464 "parser.yy" /* yacc.c:1906  */


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
    Expression::ref(v);
    rates.insert(std::make_pair(*ident, v));
    constants.insert(std::make_pair(*ident, v));
    rate_values.insert(std::make_pair(v, 0));
    constant_values.insert(std::make_pair(v, 0));
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
    Variable* v = new Variable();
    variables.insert(std::make_pair(*ident, v));
    Expression::ref(v);
    rates.insert(std::make_pair(*ident, v));
    rate_values.insert(std::make_pair(v, 0));
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
      v = new Variable();
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
    std::map<std::string, Rational>::iterator override =
        const_overrides.find(*ident);
    if (value_expr == NULL && override == const_overrides.end()) {
      yyerror("uninitialized constant `" + *ident + "'");
      value_expr = make_value(0);
    }
    Variable* v = new Variable();
    variables.insert(std::make_pair(*ident, v));
    Expression::ref(v);
    rates.insert(std::make_pair(*ident, v));
    constants.insert(std::make_pair(*ident, v));
    const int value =
        ((override != const_overrides.end()) ?
         override->second : value_expr->value(constant_values)).numerator();
    rate_values.insert(std::make_pair(v, value));
    constant_values.insert(std::make_pair(v, value));
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
    std::map<std::string, Rational>::iterator override =
        const_overrides.find(*ident);
    if (value_expr == NULL && override == const_overrides.end()) {
      yyerror("uninitialized rate `" + *ident + "'");
      value_expr = make_value(0);
    }
    Variable* v = new Variable();
    variables.insert(std::make_pair(*ident, v));
    Expression::ref(v);
    rates.insert(std::make_pair(*ident, v));
    const Rational value = (override != const_overrides.end()) ?
        override->second : value_expr->value(rate_values);
    rate_values.insert(std::make_pair(v, value));
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

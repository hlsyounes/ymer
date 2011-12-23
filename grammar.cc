
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
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 1



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 22 "grammar.yy"

#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "glog/logging.h"

#include "src/expression.h"
#include "src/model.h"
#include "src/parser-state.h"
#include "src/process-algebra.h"

#include "grammar.h"

// Lexical analyzer function.
extern int yylex(YYSTYPE* lvalp, YYLTYPE* llocp, void* scanner);

// Error reporting function.
static void yyerror(
    YYLTYPE* llocp, void* scanner, ParserState* state, const std::string& msg);

// Sets the type of state->model.
static void SetModelType(
    const YYLTYPE& location, ModelType type, ParserState* state);
// Adds a constant to state->model.
static void AddConstant(const YYLTYPE& location,
                        const std::string* name,
                        Type type,
                        const ParsedExpression* init,
                        ParserState* state);
// Adds an int variable to state->model.
static void AddIntVariable(const YYLTYPE& location,
                           const std::string* name,
                           const ParsedExpression* min,
                           const ParsedExpression* max,
                           const ParsedExpression* init,
                           ParserState* state);
// Adds a bool variable to state->model.
static void AddBoolVariable(const YYLTYPE& location,
                            const std::string* name,
                            const ParsedExpression* init,
                            ParserState* state);
// Adds a formula to state->model.
static void AddFormula(const YYLTYPE& location,
                       const std::string* name,
                       const ParsedExpression* expr,
                       ParserState* state);
// Adds a label to state->model.
static void AddLabel(const YYLTYPE& location,
                     const std::string* name,
                     const ParsedExpression* expr,
                     ParserState* state);
// Sets the init expression of state->model.
static void SetInit(
    const YYLTYPE& location, const ParsedExpression* init, ParserState* state);
// Starts a reward structure with the given label for state->model.
static void StartRewards(
    const YYLTYPE& location, const std::string* label, ParserState* state);
// Ends the current reward structure for state->model.
static void EndRewards(ParserState* state);
// Adds a state reward to state->model.
static void AddStateReward(const ParsedExpression* guard,
                           const ParsedExpression* reward,
                           ParserState* state);
// Adds a transition reward to state->model.
static void AddTransitionReward(const std::string* action,
                                const ParsedExpression* guard,
                                const ParsedExpression* reward,
                                ParserState* state);
// Starts a new module for state->model.
static void StartModule(
    const YYLTYPE& location, const std::string* name, ParserState* state);
// Ends the current module for state->model.
static void EndModule(ParserState* state);
// Adds variables and commands from the given module to the current module of
// state->model.
static void AddFromModule(
    const YYLTYPE& location,
    const std::string* from_module,
    const std::map<std::string, std::string>* substitutions,
    ParserState* state);
// Adds a command to state->model.
static void AddCommand(const YYLTYPE& location,
                       const std::string* action,
                       const ParsedExpression* guard,
                       std::vector<ParsedOutcome>* outcomes,
                       ParserState* state);
// Adds an indentifier substitution to the given set of substitutions, and
// returns the resulting substitutions.
static std::map<std::string, std::string>* AddSubstitution(
    const YYLTYPE& location,
    const std::string* from_name, const std::string* to_name,
    std::map<std::string, std::string>* substitutions,
    ParserState* state);
// Adds an outcome to the given outcomes, and returns the resulting outcomes.
static std::vector<ParsedOutcome>* AddOutcome(
    ParsedOutcome* outcome, std::vector<ParsedOutcome>* outcomes);
// Returns an outcome for the given probability expression and updates.
static ParsedOutcome* MakeOutcome(const ParsedExpression* probability,
                                  std::vector<ParsedUpdate>* updates);
// Adds an update to the given updates, and returns the resulting updates.
static std::vector<ParsedUpdate>* AddUpdate(ParsedUpdate* update,
                                            std::vector<ParsedUpdate>* updates);
// Returns an update for the given variable and with the given expression.
static ParsedUpdate* MakeUpdate(const std::string* variable,
                                const ParsedExpression* expr);
// Returns the function with the given name.
static Function MakeFunction(
    const YYLTYPE& location, const std::string* name, ParserState* state);
// Returns the unary operation with the given operator and operand.
static const ParsedExpression* MakeUnaryOperation(
    UnaryOperator op, const ParsedExpression* operand);
// Returns the binary operation with the given operator and operands.
static const ParsedExpression* MakeBinaryOperation(
    BinaryOperator op,
    const ParsedExpression* operand1,
    const ParsedExpression* operand2);
// Returns the conditional with the given condition, if_expr, and else_expr.
static const ParsedExpression* MakeConditional(
    const ParsedExpression* condition,
    const ParsedExpression* if_expr,
    const ParsedExpression* else_expr);
// Sets the system process algebra expression of state->model.
static void SetSystem(
    const YYLTYPE& location, const ProcessAlgebra* system, ParserState* state);
// Returns an action hiding process algebra operation.
static const ProcessAlgebra* MakeActionHiding(
    std::set<std::string>* actions, const ProcessAlgebra* operand);
// Returns an action renaming process algebra operation.
static const ProcessAlgebra* MakeActionRenaming(
    std::map<std::string, std::string>* substitutions,
    const ProcessAlgebra* operand);
// Returns a restricted parallel composition process algebra operation.
static const ProcessAlgebra* MakeRestrictedParallelComposition(
    std::set<std::string>* actions,
    const ProcessAlgebra* operand1,
    const ProcessAlgebra* operand2);
// Returns an alphabetized parallel composition process algebra operation.
static const ProcessAlgebra* MakeAlphabetizedParallelComposition(
    const ProcessAlgebra* operand1, const ProcessAlgebra* operand2);
// Returns an asynchronous parallel composition process algebra operation.
static const ProcessAlgebra* MakeAsynchronousParallelComposition(
    const ProcessAlgebra* operand1, const ProcessAlgebra* operand2);
// Adds an action to the given actions, and returns the resulting actions.
static std::set<std::string>* AddAction(const YYLTYPE& location,
                                        const std::string* action,
                                        std::set<std::string>* actions,
                                        ParserState* state);


/* Line 189 of yacc.c  */
#line 228 "grammar.cc"

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
     INT_LITERAL = 288,
     DOUBLE_LITERAL = 289,
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
     IMP = 312,
     NEQ = 313,
     GEQ = 314,
     LEQ = 315,
     UMINUS = 316
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
#define INT_LITERAL 288
#define DOUBLE_LITERAL 289
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
#define IMP 312
#define NEQ 313
#define GEQ 314
#define LEQ 315
#define UMINUS 316




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 216 "grammar.yy"

  const std::string* str;
  Type type;
  std::map<std::string, std::string>* substitutions;
  std::vector<ParsedOutcome>* outcomes;
  ParsedOutcome* outcome;
  std::vector<ParsedUpdate>* updates;
  ParsedUpdate* update;
  const ParsedExpression* expr;
  int int_literal;
  double double_literal;
  Function function;
  ParsedArgumentList* arguments;
  const ProcessAlgebra* process_algebra;
  std::set<std::string>* actions;



/* Line 214 of yacc.c  */
#line 405 "grammar.cc"
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


/* Line 264 of yacc.c  */
#line 430 "grammar.cc"

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
#define YYFINAL  57
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   490

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  82
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  40
/* YYNRULES -- Number of rules.  */
#define YYNRULES  108
/* YYNRULES -- Number of states.  */
#define YYNSTATES  223

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   316

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    62,     2,     2,     2,     2,    61,     2,
      79,    80,    71,    69,    78,    70,     2,    72,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    58,    75,
      65,    63,    66,    57,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    76,     2,    77,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    73,    60,    81,     2,     2,     2,     2,
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
      55,    56,    59,    64,    67,    68,    74
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,    10,    12,    14,    16,    18,
      20,    22,    24,    26,    28,    30,    32,    34,    36,    38,
      40,    45,    48,    50,    53,    55,    57,    60,    61,    64,
      67,    68,    71,    81,    87,    88,    91,    97,   103,   107,
     112,   113,   115,   116,   119,   122,   127,   135,   140,   142,
     145,   151,   155,   161,   162,   165,   173,   174,   176,   178,
     180,   182,   186,   190,   192,   194,   196,   200,   207,   209,
     211,   213,   215,   217,   222,   229,   233,   236,   240,   244,
     248,   252,   256,   260,   264,   268,   272,   276,   279,   283,
     287,   291,   297,   299,   301,   303,   305,   309,   313,   315,
     319,   325,   330,   338,   342,   346,   348,   352,   356
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      83,     0,    -1,    84,    -1,    85,    -1,    84,    85,    -1,
      86,    -1,    87,    -1,    90,    -1,    94,    -1,    95,    -1,
      96,    -1,    97,    -1,   102,    -1,   118,    -1,     3,    -1,
       4,    -1,     5,    -1,     6,    -1,     7,    -1,     8,    -1,
      88,    32,    89,    75,    -1,     9,    10,    -1,     9,    -1,
       9,    11,    -1,    13,    -1,    14,    -1,     9,    12,    -1,
      -1,    63,   115,    -1,    15,    92,    -1,    -1,    91,    92,
      -1,    32,    58,    76,   115,    16,   115,    77,    93,    75,
      -1,    32,    58,    12,    93,    75,    -1,    -1,    19,   115,
      -1,    17,    32,    63,   115,    75,    -1,    18,    35,    63,
     115,    75,    -1,    19,   115,    20,    -1,    21,    98,    99,
      22,    -1,    -1,    35,    -1,    -1,    99,   100,    -1,    99,
     101,    -1,   115,    58,   115,    75,    -1,    76,   108,    77,
     115,    58,   115,    75,    -1,    23,   103,   104,    24,    -1,
      32,    -1,    91,   106,    -1,    63,    32,    76,   105,    77,
      -1,    32,    63,    32,    -1,   105,    78,    32,    63,    32,
      -1,    -1,   106,   107,    -1,    76,   108,    77,   115,    25,
     109,    75,    -1,    -1,    32,    -1,   112,    -1,   110,    -1,
     111,    -1,   110,    69,   111,    -1,   115,    58,   112,    -1,
      27,    -1,   113,    -1,   114,    -1,   113,    61,   114,    -1,
      79,    32,    26,    63,   115,    80,    -1,    33,    -1,    34,
      -1,    27,    -1,    28,    -1,    32,    -1,   116,    79,   117,
      80,    -1,    31,    79,   116,    78,   117,    80,    -1,    79,
     115,    80,    -1,    70,   115,    -1,   115,    71,   115,    -1,
     115,    72,   115,    -1,   115,    69,   115,    -1,   115,    70,
     115,    -1,   115,    65,   115,    -1,   115,    68,   115,    -1,
     115,    67,   115,    -1,   115,    66,   115,    -1,   115,    63,
     115,    -1,   115,    64,   115,    -1,    62,   115,    -1,   115,
      61,   115,    -1,   115,    60,   115,    -1,   115,    59,   115,
      -1,   115,    57,   115,    58,   115,    -1,    32,    -1,    29,
      -1,    30,    -1,   115,    -1,   117,    78,   115,    -1,    36,
     119,    37,    -1,    32,    -1,    79,   119,    80,    -1,   119,
      72,    73,   120,    81,    -1,   119,    73,   121,    81,    -1,
     119,    60,    76,   120,    77,    60,   119,    -1,   119,    38,
     119,    -1,   119,    39,   119,    -1,    32,    -1,   120,    78,
      32,    -1,    32,    40,    32,    -1,   121,    78,    32,    40,
      32,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   261,   261,   264,   265,   268,   269,   270,   271,   272,
     273,   274,   275,   276,   279,   281,   283,   285,   287,   289,
     293,   297,   299,   301,   303,   305,   307,   312,   313,   317,
     320,   321,   324,   326,   331,   332,   336,   340,   344,   348,
     353,   354,   358,   359,   360,   363,   367,   371,   375,   379,
     380,   384,   386,   390,   391,   394,   399,   400,   403,   405,
     408,   410,   414,   418,   420,   423,   425,   429,   433,   435,
     437,   439,   441,   443,   445,   447,   449,   451,   453,   455,
     457,   459,   461,   463,   465,   467,   469,   471,   473,   475,
     477,   479,   483,   485,   487,   491,   493,   497,   501,   503,
     505,   507,   509,   511,   513,   517,   519,   523,   526
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
  "FUNC", "IDENTIFIER", "INT_LITERAL", "DOUBLE_LITERAL", "LABEL_NAME",
  "SYSTEM", "ENDSYSTEM", "DOUBLE_BAR", "TRIPLE_BAR", "BACK_ARROW", "A",
  "C", "E", "F", "G", "I", "PMAX", "PMIN", "P", "RMAX", "RMIN", "R", "S",
  "U", "W", "X", "'?'", "':'", "IMP", "'|'", "'&'", "'!'", "'='", "NEQ",
  "'<'", "'>'", "GEQ", "LEQ", "'+'", "'-'", "'*'", "'/'", "'{'", "UMINUS",
  "';'", "'['", "']'", "','", "'('", "')'", "'}'", "$accept", "model",
  "model_components", "model_component", "model_type", "constant",
  "constant_type", "constant_init", "global", "variables", "variable",
  "variable_init", "formula", "label", "init", "rewards", "rewards_label",
  "reward_rules", "state_reward", "transition_reward", "module",
  "module_name", "module_def", "substitutions", "commands", "command",
  "action", "outcomes", "update_distribution", "probability_and_updates",
  "true_or_updates", "updates", "update", "expr", "function_name",
  "arguments", "system", "process_algebra", "action_list",
  "action_substitutions", 0
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
     305,   306,   307,   308,   309,   310,   311,    63,    58,   312,
     124,    38,    33,    61,   313,    60,    62,   314,   315,    43,
      45,    42,    47,   123,   316,    59,    91,    93,    44,    40,
      41,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    82,    83,    84,    84,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    86,    86,    86,    86,    86,    86,
      87,    88,    88,    88,    88,    88,    88,    89,    89,    90,
      91,    91,    92,    92,    93,    93,    94,    95,    96,    97,
      98,    98,    99,    99,    99,   100,   101,   102,   103,   104,
     104,   105,   105,   106,   106,   107,   108,   108,   109,   109,
     110,   110,   111,   112,   112,   113,   113,   114,   115,   115,
     115,   115,   115,   115,   115,   115,   115,   115,   115,   115,
     115,   115,   115,   115,   115,   115,   115,   115,   115,   115,
     115,   115,   116,   116,   116,   117,   117,   118,   119,   119,
     119,   119,   119,   119,   119,   120,   120,   121,   121
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       4,     2,     1,     2,     1,     1,     2,     0,     2,     2,
       0,     2,     9,     5,     0,     2,     5,     5,     3,     4,
       0,     1,     0,     2,     2,     4,     7,     4,     1,     2,
       5,     3,     5,     0,     2,     7,     0,     1,     1,     1,
       1,     3,     3,     1,     1,     1,     3,     6,     1,     1,
       1,     1,     1,     4,     6,     3,     2,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     2,     3,     3,
       3,     5,     1,     1,     1,     1,     3,     3,     1,     3,
       5,     4,     7,     3,     3,     1,     3,     3,     5
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    14,    15,    16,    17,    18,    19,    22,    24,    25,
       0,     0,     0,     0,    40,     0,     0,     0,     2,     3,
       5,     6,     0,     7,     8,     9,    10,    11,    12,    13,
      21,    23,    26,     0,    29,     0,     0,    70,    71,    93,
      94,     0,    72,    68,    69,     0,     0,     0,     0,     0,
      41,    42,    48,    30,    98,     0,     0,     1,     4,    27,
       0,     0,     0,     0,    87,    76,     0,    38,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    53,     0,     0,    97,     0,
       0,     0,     0,     0,     0,     0,    34,     0,     0,     0,
      92,     0,    75,     0,    90,    89,    88,    85,    86,    81,
      84,    83,    82,    79,    80,    77,    78,    95,     0,    39,
      56,    43,    44,     0,     0,    31,    49,    47,    99,   103,
     104,     0,     0,     0,     0,    28,    20,     0,     0,     0,
      36,    37,     0,     0,     0,    73,    57,     0,     0,     0,
      56,    54,   105,     0,     0,     0,     0,   101,    35,    33,
       0,     0,    91,    96,     0,     0,     0,     0,     0,     0,
       0,   100,   107,     0,     0,    74,     0,    45,     0,    50,
       0,     0,     0,   106,     0,    34,     0,    51,     0,     0,
     102,   108,     0,     0,     0,     0,    32,    46,    52,    70,
       0,     0,    59,    60,    58,    64,    65,     0,    72,    55,
       0,     0,     0,     0,    61,     0,    66,    63,    62,     0,
       0,     0,    67
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    17,    18,    19,    20,    21,    22,    95,    23,    85,
      34,   138,    24,    25,    26,    27,    51,    83,   121,   122,
      28,    53,    86,   167,   126,   151,   147,   201,   202,   203,
     204,   205,   206,    66,    49,   118,    29,    56,   153,   134
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -62
static const yytype_int16 yypact[] =
{
     230,   -62,   -62,   -62,   -62,   -62,   -62,    -6,   -62,   -62,
       6,    15,    44,   109,    48,    58,   -29,    93,   230,   -62,
     -62,   -62,    65,   -62,   -62,   -62,   -62,   -62,   -62,   -62,
     -62,   -62,   -62,    68,   -62,    50,    71,   -62,   -62,   -62,
     -62,    83,    87,   -62,   -62,   109,   109,   109,    89,    88,
     -62,   -62,   -62,   106,   -62,   -29,   126,   -62,   -62,   107,
      -5,   109,   109,   -17,   410,   -62,   -43,   -62,   109,   109,
     109,   109,   109,   109,   109,   109,   109,   109,   109,   109,
     109,   109,   109,    12,   140,     6,   150,   145,   -62,   -29,
     -29,    99,   103,   146,   109,   102,   161,   109,   241,   258,
     -62,   122,   -62,   311,   388,   400,   410,   418,   418,   -61,
     -61,   -61,   -61,   -41,   -41,   -62,   -62,   375,     9,   -62,
     149,   -62,   -62,   327,   111,   -62,   125,   -62,   -62,    13,
      13,   153,   153,   163,    -3,   375,   -62,   109,   127,    35,
     -62,   -62,   109,   109,   109,   -62,   -62,   138,   109,   172,
     149,   -62,   -62,    55,    -1,   184,   187,   -62,   375,   -62,
     109,    47,   375,   375,   109,   275,   157,    67,   144,   166,
     190,   -62,   -62,   183,   222,   -62,   343,   -62,   195,   -62,
     196,   109,   -29,   -62,   197,   161,   109,   -62,   167,    51,
     -20,   -62,   156,   292,   208,   162,   -62,   -62,   -62,   171,
     180,   177,   185,   -62,   -62,   194,   -62,   359,    -7,   -62,
     109,   178,   -25,   193,   -62,   226,   -62,   -62,   -62,   109,
     234,   204,   -62
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -62,   -62,   -62,   244,   -62,   -62,   -62,   -62,   -62,   -62,
     192,    95,   -62,   -62,   -62,   -62,   -62,   -62,   -62,   -62,
     -62,   -62,   -62,   -62,   -62,   -62,   128,   -62,   -62,    85,
      84,   -62,    86,   -13,   240,   189,   -62,   -54,   182,   -62
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -93
static const yytype_int16 yytable[] =
{
      48,    87,   217,    54,    30,    31,    32,    96,    78,    79,
      80,    81,    39,    40,    68,   100,    69,    70,    71,   213,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      80,    81,    64,    65,   119,   129,   130,   102,    33,    37,
      38,    39,    40,    41,    42,    43,    44,    35,    98,    99,
      55,   160,    92,    93,   215,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     123,    97,   -92,    91,    45,   156,   195,   170,   157,    36,
     171,   135,    46,    50,   139,    92,    93,   144,   120,   145,
      52,    47,    68,    57,    69,    70,    71,    59,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    68,    67,
      69,    70,    71,    61,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,   158,   144,    60,   175,   190,   117,
     162,   163,   169,   170,    62,   165,    37,    38,    39,    40,
      41,    42,    43,    44,   179,   180,    68,   174,    69,    70,
      71,   176,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    63,    88,    89,    90,   -92,    82,   189,    84,
      94,    45,   124,   193,   127,   131,   132,   136,   133,    46,
     137,   146,   207,    89,    90,   152,    91,   149,    47,   199,
      38,    39,    40,    41,    42,    43,    44,   207,    92,    93,
     142,   150,   159,   155,   166,    91,   221,    37,    38,    39,
      40,    41,   208,    43,    44,   164,   172,    92,    93,   173,
     178,   181,   183,   184,    45,   128,   182,   187,   188,   191,
     194,   196,    46,     1,     2,     3,     4,     5,     6,     7,
     198,   200,    45,     8,     9,    10,   -63,    11,    12,    13,
      46,    14,   209,    15,   210,   211,   219,   215,   220,    47,
     213,    68,    58,    69,    70,    71,    16,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,   125,   168,    68,
     192,    69,    70,    71,   222,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,   214,   218,   216,    68,   185,
      69,    70,    71,   101,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,   154,    68,   140,    69,    70,    71,
       0,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,   161,    68,   141,    69,    70,    71,     0,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,     0,    68,
     177,    69,    70,    71,     0,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,     0,     0,   197,    68,   143,
      69,    70,    71,     0,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    68,   148,    69,    70,    71,     0,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      68,   186,    69,    70,    71,     0,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    68,   212,    69,    70,
      71,     0,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    68,     0,    69,    70,    71,     0,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    70,    71,
       0,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    71,     0,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    74,    75,    76,    77,    78,    79,    80,
      81
};

static const yytype_int16 yycheck[] =
{
      13,    55,    27,    32,    10,    11,    12,    12,    69,    70,
      71,    72,    29,    30,    57,    32,    59,    60,    61,    26,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      71,    72,    45,    46,    22,    89,    90,    80,    32,    27,
      28,    29,    30,    31,    32,    33,    34,    32,    61,    62,
      79,    16,    72,    73,    79,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    76,    79,    60,    62,    78,    25,    78,    81,    35,
      81,    94,    70,    35,    97,    72,    73,    78,    76,    80,
      32,    79,    57,     0,    59,    60,    61,    32,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    57,    20,
      59,    60,    61,    63,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,   137,    78,    58,    80,   182,   142,
     143,   144,    77,    78,    63,   148,    27,    28,    29,    30,
      31,    32,    33,    34,    77,    78,    57,   160,    59,    60,
      61,   164,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    79,    37,    38,    39,    79,    79,   181,    63,
      63,    62,    32,   186,    24,    76,    73,    75,    32,    70,
      19,    32,   195,    38,    39,    32,    60,    76,    79,    27,
      28,    29,    30,    31,    32,    33,    34,   210,    72,    73,
      78,    76,    75,    40,    32,    60,   219,    27,    28,    29,
      30,    31,    32,    33,    34,    77,    32,    72,    73,    32,
      63,    77,    32,    40,    62,    80,    60,    32,    32,    32,
      63,    75,    70,     3,     4,     5,     6,     7,     8,     9,
      32,    79,    62,    13,    14,    15,    75,    17,    18,    19,
      70,    21,    75,    23,    69,    61,    63,    79,    32,    79,
      26,    57,    18,    59,    60,    61,    36,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    85,   150,    57,
     185,    59,    60,    61,    80,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,   210,   212,   211,    57,    77,
      59,    60,    61,    63,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,   132,    57,    75,    59,    60,    61,
      -1,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,   142,    57,    75,    59,    60,    61,    -1,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    -1,    57,
      75,    59,    60,    61,    -1,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    -1,    -1,    75,    57,    58,
      59,    60,    61,    -1,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    57,    58,    59,    60,    61,    -1,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      57,    58,    59,    60,    61,    -1,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    57,    58,    59,    60,
      61,    -1,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    57,    -1,    59,    60,    61,    -1,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    60,    61,
      -1,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    61,    -1,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    65,    66,    67,    68,    69,    70,    71,
      72
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,     9,    13,    14,
      15,    17,    18,    19,    21,    23,    36,    83,    84,    85,
      86,    87,    88,    90,    94,    95,    96,    97,   102,   118,
      10,    11,    12,    32,    92,    32,    35,    27,    28,    29,
      30,    31,    32,    33,    34,    62,    70,    79,   115,   116,
      35,    98,    32,   103,    32,    79,   119,     0,    85,    32,
      58,    63,    63,    79,   115,   115,   115,    20,    57,    59,
      60,    61,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    79,    99,    63,    91,   104,   119,    37,    38,
      39,    60,    72,    73,    63,    89,    12,    76,   115,   115,
      32,   116,    80,   115,   115,   115,   115,   115,   115,   115,
     115,   115,   115,   115,   115,   115,   115,   115,   117,    22,
      76,   100,   101,   115,    32,    92,   106,    24,    80,   119,
     119,    76,    73,    32,   121,   115,    75,    19,    93,   115,
      75,    75,    78,    58,    78,    80,    32,   108,    58,    76,
      76,   107,    32,   120,   120,    40,    78,    81,   115,    75,
      16,   117,   115,   115,    77,   115,    32,   105,   108,    77,
      78,    81,    32,    32,   115,    80,   115,    75,    63,    77,
      78,    77,    60,    32,    40,    77,    58,    32,    32,   115,
     119,    32,    93,   115,    63,    25,    75,    75,    32,    27,
      79,   109,   110,   111,   112,   113,   114,   115,    32,    75,
      69,    61,    58,    26,   111,    79,   114,    27,   112,    63,
      32,   115,    80
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
      yyerror (&yylloc, scanner, state, YY_("syntax error: cannot back up")); \
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
# define YYLEX yylex (&yylval, &yylloc, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, &yylloc, scanner)
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
		  Type, Value, Location, scanner, state); \
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void* scanner, ParserState* state)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, scanner, state)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    void* scanner;
    ParserState* state;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
  YYUSE (scanner);
  YYUSE (state);
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void* scanner, ParserState* state)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp, scanner, state)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    void* scanner;
    ParserState* state;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, scanner, state);
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
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, void* scanner, ParserState* state)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule, scanner, state)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
    void* scanner;
    ParserState* state;
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
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       , scanner, state);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule, scanner, state); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, void* scanner, ParserState* state)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp, scanner, state)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
    void* scanner;
    ParserState* state;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (scanner);
  YYUSE (state);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {
      case 32: /* "IDENTIFIER" */

/* Line 1000 of yacc.c  */
#line 248 "grammar.yy"
	{ delete (yyvaluep->str); };

/* Line 1000 of yacc.c  */
#line 1611 "grammar.cc"
	break;
      case 35: /* "LABEL_NAME" */

/* Line 1000 of yacc.c  */
#line 248 "grammar.yy"
	{ delete (yyvaluep->str); };

/* Line 1000 of yacc.c  */
#line 1620 "grammar.cc"
	break;
      case 89: /* "constant_init" */

/* Line 1000 of yacc.c  */
#line 254 "grammar.yy"
	{ delete (yyvaluep->expr); };

/* Line 1000 of yacc.c  */
#line 1629 "grammar.cc"
	break;
      case 93: /* "variable_init" */

/* Line 1000 of yacc.c  */
#line 254 "grammar.yy"
	{ delete (yyvaluep->expr); };

/* Line 1000 of yacc.c  */
#line 1638 "grammar.cc"
	break;
      case 105: /* "substitutions" */

/* Line 1000 of yacc.c  */
#line 249 "grammar.yy"
	{ delete (yyvaluep->substitutions); };

/* Line 1000 of yacc.c  */
#line 1647 "grammar.cc"
	break;
      case 108: /* "action" */

/* Line 1000 of yacc.c  */
#line 248 "grammar.yy"
	{ delete (yyvaluep->str); };

/* Line 1000 of yacc.c  */
#line 1656 "grammar.cc"
	break;
      case 109: /* "outcomes" */

/* Line 1000 of yacc.c  */
#line 250 "grammar.yy"
	{ delete (yyvaluep->outcomes); };

/* Line 1000 of yacc.c  */
#line 1665 "grammar.cc"
	break;
      case 110: /* "update_distribution" */

/* Line 1000 of yacc.c  */
#line 250 "grammar.yy"
	{ delete (yyvaluep->outcomes); };

/* Line 1000 of yacc.c  */
#line 1674 "grammar.cc"
	break;
      case 111: /* "probability_and_updates" */

/* Line 1000 of yacc.c  */
#line 251 "grammar.yy"
	{ delete (yyvaluep->outcome); };

/* Line 1000 of yacc.c  */
#line 1683 "grammar.cc"
	break;
      case 112: /* "true_or_updates" */

/* Line 1000 of yacc.c  */
#line 252 "grammar.yy"
	{ delete (yyvaluep->updates); };

/* Line 1000 of yacc.c  */
#line 1692 "grammar.cc"
	break;
      case 113: /* "updates" */

/* Line 1000 of yacc.c  */
#line 252 "grammar.yy"
	{ delete (yyvaluep->updates); };

/* Line 1000 of yacc.c  */
#line 1701 "grammar.cc"
	break;
      case 114: /* "update" */

/* Line 1000 of yacc.c  */
#line 253 "grammar.yy"
	{ delete (yyvaluep->update); };

/* Line 1000 of yacc.c  */
#line 1710 "grammar.cc"
	break;
      case 115: /* "expr" */

/* Line 1000 of yacc.c  */
#line 254 "grammar.yy"
	{ delete (yyvaluep->expr); };

/* Line 1000 of yacc.c  */
#line 1719 "grammar.cc"
	break;
      case 117: /* "arguments" */

/* Line 1000 of yacc.c  */
#line 255 "grammar.yy"
	{ delete (yyvaluep->arguments); };

/* Line 1000 of yacc.c  */
#line 1728 "grammar.cc"
	break;
      case 119: /* "process_algebra" */

/* Line 1000 of yacc.c  */
#line 256 "grammar.yy"
	{ delete (yyvaluep->process_algebra); };

/* Line 1000 of yacc.c  */
#line 1737 "grammar.cc"
	break;
      case 120: /* "action_list" */

/* Line 1000 of yacc.c  */
#line 257 "grammar.yy"
	{ delete (yyvaluep->actions); };

/* Line 1000 of yacc.c  */
#line 1746 "grammar.cc"
	break;
      case 121: /* "action_substitutions" */

/* Line 1000 of yacc.c  */
#line 249 "grammar.yy"
	{ delete (yyvaluep->substitutions); };

/* Line 1000 of yacc.c  */
#line 1755 "grammar.cc"
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
int yyparse (void* scanner, ParserState* state);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */





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
yyparse (void* scanner, ParserState* state)
#else
int
yyparse (scanner, state)
    void* scanner;
    ParserState* state;
#endif
#endif
{
/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;

    /* Number of syntax errors so far.  */
    int yynerrs;

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
    YYLTYPE yyerror_range[2];

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

#if YYLTYPE_IS_TRIVIAL
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

/* Line 1455 of yacc.c  */
#line 261 "grammar.yy"
    { if (!state->success) YYERROR; }
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 280 "grammar.yy"
    { SetModelType(yylloc, ModelType::DTMC, state); }
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 282 "grammar.yy"
    { SetModelType(yylloc, ModelType::CTMC, state); }
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 284 "grammar.yy"
    { SetModelType(yylloc, ModelType::MDP, state); }
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 286 "grammar.yy"
    { SetModelType(yylloc, ModelType::DTMC, state); }
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 288 "grammar.yy"
    { SetModelType(yylloc, ModelType::CTMC, state); }
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 290 "grammar.yy"
    { SetModelType(yylloc, ModelType::MDP, state); }
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 294 "grammar.yy"
    { AddConstant(yylloc, (yyvsp[(2) - (4)].str), (yyvsp[(1) - (4)].type), (yyvsp[(3) - (4)].expr), state); }
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 298 "grammar.yy"
    { (yyval.type) = Type::INT; }
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 300 "grammar.yy"
    { (yyval.type) = Type::INT; }
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 302 "grammar.yy"
    { (yyval.type) = Type::DOUBLE; }
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 304 "grammar.yy"
    { (yyval.type) = Type::DOUBLE; }
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 306 "grammar.yy"
    { (yyval.type) = Type::DOUBLE; }
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 308 "grammar.yy"
    { (yyval.type) = Type::BOOL; }
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 312 "grammar.yy"
    { (yyval.expr) = nullptr; }
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 314 "grammar.yy"
    { (yyval.expr) = (yyvsp[(2) - (2)].expr); }
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 325 "grammar.yy"
    { AddIntVariable(yylloc, (yyvsp[(1) - (9)].str), (yyvsp[(4) - (9)].expr), (yyvsp[(6) - (9)].expr), (yyvsp[(8) - (9)].expr), state); }
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 327 "grammar.yy"
    { AddBoolVariable(yylloc, (yyvsp[(1) - (5)].str), (yyvsp[(4) - (5)].expr), state); }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 331 "grammar.yy"
    { (yyval.expr) = nullptr; }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 333 "grammar.yy"
    { (yyval.expr) = (yyvsp[(2) - (2)].expr); }
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 337 "grammar.yy"
    { AddFormula(yylloc, (yyvsp[(2) - (5)].str), (yyvsp[(4) - (5)].expr), state); }
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 341 "grammar.yy"
    { AddLabel(yylloc, (yyvsp[(2) - (5)].str), (yyvsp[(4) - (5)].expr), state); }
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 345 "grammar.yy"
    { SetInit(yylloc, (yyvsp[(2) - (3)].expr), state); }
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 349 "grammar.yy"
    { EndRewards(state); }
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 353 "grammar.yy"
    { StartRewards(yylloc, nullptr, state); }
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 355 "grammar.yy"
    { StartRewards(yylloc, (yyvsp[(1) - (1)].str), state); }
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 364 "grammar.yy"
    { AddStateReward((yyvsp[(1) - (4)].expr), (yyvsp[(3) - (4)].expr), state); }
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 368 "grammar.yy"
    { AddTransitionReward((yyvsp[(2) - (7)].str), (yyvsp[(4) - (7)].expr), (yyvsp[(6) - (7)].expr), state); }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 372 "grammar.yy"
    { EndModule(state); }
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 376 "grammar.yy"
    { StartModule(yylloc, (yyvsp[(1) - (1)].str), state); }
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 381 "grammar.yy"
    { AddFromModule(yylloc, (yyvsp[(2) - (5)].str), (yyvsp[(4) - (5)].substitutions), state); }
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 385 "grammar.yy"
    { (yyval.substitutions) = AddSubstitution(yylloc, (yyvsp[(1) - (3)].str), (yyvsp[(3) - (3)].str), nullptr, state); }
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 387 "grammar.yy"
    { (yyval.substitutions) = AddSubstitution(yylloc, (yyvsp[(3) - (5)].str), (yyvsp[(5) - (5)].str), (yyvsp[(1) - (5)].substitutions), state); }
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 395 "grammar.yy"
    { AddCommand(yylloc, (yyvsp[(2) - (7)].str), (yyvsp[(4) - (7)].expr), (yyvsp[(6) - (7)].outcomes), state); }
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 399 "grammar.yy"
    { (yyval.str) = nullptr; }
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 404 "grammar.yy"
    { (yyval.outcomes) = AddOutcome(MakeOutcome(nullptr, (yyvsp[(1) - (1)].updates)), nullptr); }
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 409 "grammar.yy"
    { (yyval.outcomes) = AddOutcome((yyvsp[(1) - (1)].outcome), nullptr); }
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 411 "grammar.yy"
    { (yyval.outcomes) = AddOutcome((yyvsp[(3) - (3)].outcome), (yyvsp[(1) - (3)].outcomes)); }
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 415 "grammar.yy"
    { (yyval.outcome) = MakeOutcome((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].updates)); }
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 419 "grammar.yy"
    { (yyval.updates) = new std::vector<ParsedUpdate>(); }
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 424 "grammar.yy"
    { (yyval.updates) = AddUpdate((yyvsp[(1) - (1)].update), nullptr); }
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 426 "grammar.yy"
    { (yyval.updates) = AddUpdate((yyvsp[(3) - (3)].update), (yyvsp[(1) - (3)].updates)); }
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 430 "grammar.yy"
    { (yyval.update) = MakeUpdate((yyvsp[(2) - (6)].str), (yyvsp[(5) - (6)].expr)); }
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 434 "grammar.yy"
    { (yyval.expr) = new ParsedLiteral((yyvsp[(1) - (1)].int_literal)); }
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 436 "grammar.yy"
    { (yyval.expr) = new ParsedLiteral((yyvsp[(1) - (1)].double_literal)); }
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 438 "grammar.yy"
    { (yyval.expr) = new ParsedLiteral(true); }
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 440 "grammar.yy"
    { (yyval.expr) = new ParsedLiteral(false); }
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 442 "grammar.yy"
    { (yyval.expr) = new ParsedIdentifier(*(yyvsp[(1) - (1)].str)); delete (yyvsp[(1) - (1)].str); }
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 444 "grammar.yy"
    { (yyval.expr) = new ParsedFunctionCall((yyvsp[(1) - (4)].function), std::move(*(yyvsp[(3) - (4)].arguments))); delete (yyvsp[(3) - (4)].arguments); }
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 446 "grammar.yy"
    { (yyval.expr) = new ParsedFunctionCall((yyvsp[(3) - (6)].function), std::move(*(yyvsp[(5) - (6)].arguments))); delete (yyvsp[(5) - (6)].arguments); }
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 448 "grammar.yy"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 450 "grammar.yy"
    { (yyval.expr) = MakeUnaryOperation(UnaryOperator::NEGATE, (yyvsp[(2) - (2)].expr)); }
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 452 "grammar.yy"
    { (yyval.expr) = MakeBinaryOperation(BinaryOperator::MULTIPLY, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 454 "grammar.yy"
    { (yyval.expr) = MakeBinaryOperation(BinaryOperator::DIVIDE, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 456 "grammar.yy"
    { (yyval.expr) = MakeBinaryOperation(BinaryOperator::PLUS, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 458 "grammar.yy"
    { (yyval.expr) = MakeBinaryOperation(BinaryOperator::MINUS, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 460 "grammar.yy"
    { (yyval.expr) = MakeBinaryOperation(BinaryOperator::LESS, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 462 "grammar.yy"
    { (yyval.expr) = MakeBinaryOperation(BinaryOperator::LESS_EQUAL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 464 "grammar.yy"
    { (yyval.expr) = MakeBinaryOperation(BinaryOperator::GREATER_EQUAL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 466 "grammar.yy"
    { (yyval.expr) = MakeBinaryOperation(BinaryOperator::GREATER, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 468 "grammar.yy"
    { (yyval.expr) = MakeBinaryOperation(BinaryOperator::EQUAL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 470 "grammar.yy"
    { (yyval.expr) = MakeBinaryOperation(BinaryOperator::NOT_EQUAL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 472 "grammar.yy"
    { (yyval.expr) = MakeUnaryOperation(UnaryOperator::NOT, (yyvsp[(2) - (2)].expr)); }
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 474 "grammar.yy"
    { (yyval.expr) = MakeBinaryOperation(BinaryOperator::AND, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 476 "grammar.yy"
    { (yyval.expr) = MakeBinaryOperation(BinaryOperator::OR, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 478 "grammar.yy"
    { (yyval.expr) = MakeBinaryOperation(BinaryOperator::IMPLY, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 480 "grammar.yy"
    { (yyval.expr) = MakeConditional((yyvsp[(1) - (5)].expr), (yyvsp[(3) - (5)].expr), (yyvsp[(5) - (5)].expr)); }
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 484 "grammar.yy"
    { (yyval.function) = MakeFunction(yylloc, (yyvsp[(1) - (1)].str), state); }
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 486 "grammar.yy"
    { (yyval.function) = Function::MAX; }
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 488 "grammar.yy"
    { (yyval.function) = Function::MIN; }
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 492 "grammar.yy"
    { (yyval.arguments) = new ParsedArgumentList(std::unique_ptr<const ParsedExpression>((yyvsp[(1) - (1)].expr))); }
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 494 "grammar.yy"
    { (yyval.arguments) = (yyvsp[(1) - (3)].arguments); (yyval.arguments)->push_back(std::unique_ptr<const ParsedExpression>((yyvsp[(3) - (3)].expr))); }
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 498 "grammar.yy"
    { SetSystem(yylloc, (yyvsp[(2) - (3)].process_algebra), state); }
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 502 "grammar.yy"
    { (yyval.process_algebra) = new ModuleIdentifier(*(yyvsp[(1) - (1)].str)); delete (yyvsp[(1) - (1)].str); }
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 504 "grammar.yy"
    { (yyval.process_algebra) = (yyvsp[(2) - (3)].process_algebra); }
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 506 "grammar.yy"
    { (yyval.process_algebra) = MakeActionHiding((yyvsp[(4) - (5)].actions), (yyvsp[(1) - (5)].process_algebra)); }
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 508 "grammar.yy"
    { (yyval.process_algebra) = MakeActionRenaming((yyvsp[(3) - (4)].substitutions), (yyvsp[(1) - (4)].process_algebra)); }
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 510 "grammar.yy"
    { (yyval.process_algebra) = MakeRestrictedParallelComposition((yyvsp[(4) - (7)].actions), (yyvsp[(1) - (7)].process_algebra), (yyvsp[(7) - (7)].process_algebra)); }
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 512 "grammar.yy"
    { (yyval.process_algebra) = MakeAlphabetizedParallelComposition((yyvsp[(1) - (3)].process_algebra), (yyvsp[(3) - (3)].process_algebra)); }
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 514 "grammar.yy"
    { (yyval.process_algebra) = MakeAsynchronousParallelComposition((yyvsp[(1) - (3)].process_algebra), (yyvsp[(3) - (3)].process_algebra)); }
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 518 "grammar.yy"
    { (yyval.actions) = AddAction(yylloc, (yyvsp[(1) - (1)].str), nullptr, state); }
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 520 "grammar.yy"
    { (yyval.actions) = AddAction(yylloc, (yyvsp[(3) - (3)].str), (yyvsp[(1) - (3)].actions), state); }
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 524 "grammar.yy"
    { (yyval.substitutions) = AddSubstitution(yylloc, (yyvsp[(1) - (3)].str), (yyvsp[(3) - (3)].str), nullptr,
                                                state); }
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 527 "grammar.yy"
    { (yyval.substitutions) = AddSubstitution(yylloc, (yyvsp[(3) - (5)].str), (yyvsp[(5) - (5)].str), (yyvsp[(1) - (5)].substitutions), state); }
    break;



/* Line 1455 of yacc.c  */
#line 2676 "grammar.cc"
      default: break;
    }
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
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (&yylloc, scanner, state, YY_("syntax error"));
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
	    yyerror (&yylloc, scanner, state, yymsg);
	  }
	else
	  {
	    yyerror (&yylloc, scanner, state, YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }

  yyerror_range[0] = yylloc;

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

  yyerror_range[0] = yylsp[1-yylen];
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

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp, scanner, state);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
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
  yyerror (&yylloc, scanner, state, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc, scanner, state);
  /* Do not reclaim the symbols of the rule which action triggered
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
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 530 "grammar.yy"


namespace {

Optional<std::string> MakeOptionalAction(const std::string* action) {
  if (action) {
    std::unique_ptr<const std::string> owned_action(action);
    return Optional<std::string>(*action);
  } else {
    return Optional<std::string>();
  }
}

}  // namespace

static void yyerror(
    const YYLTYPE& location, const std::string& msg, ParserState* state) {
  CHECK(state);

  if (state->message) {
    std::ostringstream out;
    if (state->filename && *state->filename != "-") {
      out << *state->filename << ':';
    }
    out << location.first_line << ':' << msg;
    if (!state->message->empty()) {
      *state->message += "\n";
    }
    *state->message += out.str();
  }
  state->success = false;
}

static void yyerror(
    YYLTYPE* llocp, void* scanner, ParserState* state, const std::string& msg) {
  yyerror(*llocp, msg, state);
}

static void SetModelType(
    const YYLTYPE& location, ModelType type, ParserState* state) {
  CHECK(state);
  CHECK(state->model);

  if (!state->model->SetType(type)) {
    yyerror(location, "multiple model types", state);
  }
}

static void AddConstant(const YYLTYPE& location,
                        const std::string* name,
                        Type type,
                        const ParsedExpression* init,
                        ParserState* state) {
  CHECK(name);
  CHECK(state);
  CHECK(state->model);

  if (!state->model->AddConstant(
          *name, type, std::unique_ptr<const ParsedExpression>(init))) {
    yyerror(location, "duplicate identifier", state);
  }
  delete name;
}

static void AddIntVariable(const YYLTYPE& location,
                           const std::string* name,
                           const ParsedExpression* min,
                           const ParsedExpression* max,
                           const ParsedExpression* init,
                           ParserState* state) {
  CHECK(name);
  CHECK(state);
  CHECK(state->model);

  if (!state->model->AddIntVariable(
          *name,
          Range(std::unique_ptr<const ParsedExpression>(min),
                std::unique_ptr<const ParsedExpression>(max)),
          std::unique_ptr<const ParsedExpression>(init))) {
    yyerror(location, "duplicate identifier", state);
  }
  delete name;
}

static void AddBoolVariable(const YYLTYPE& location,
                            const std::string* name,
                            const ParsedExpression* init,
                            ParserState* state) {
  CHECK(name);
  CHECK(state);
  CHECK(state->model);

  if (!state->model->AddBoolVariable(
          *name, std::unique_ptr<const ParsedExpression>(init))) {
    yyerror(location, "duplicate identifier", state);
  }
  delete name;
}

static void AddFormula(const YYLTYPE& location,
                       const std::string* name,
                       const ParsedExpression* expr,
                       ParserState* state) {
  CHECK(name);
  CHECK(state);
  CHECK(state->model);

  if (!state->model->AddFormula(
          *name, std::unique_ptr<const ParsedExpression>(expr))) {
    yyerror(location, "duplicate identifier", state);
  }
  delete name;
}

static void AddLabel(const YYLTYPE& location,
                     const std::string* name,
                     const ParsedExpression* expr,
                     ParserState* state) {
  CHECK(name);
  CHECK(state);
  CHECK(state->model);

  if (!state->model->AddLabel(
          *name, std::unique_ptr<const ParsedExpression>(expr))) {
    yyerror(location, "duplicate label", state);
  }
  delete name;
}

static void SetInit(
    const YYLTYPE& location, const ParsedExpression* init, ParserState* state) {
  CHECK(state);
  CHECK(state->model);

  if (!state->model->SetInit(std::unique_ptr<const ParsedExpression>(init))) {
    yyerror(location, "multiple init blocks", state);
  }
}

static void StartRewards(
    const YYLTYPE& location, const std::string* label, ParserState* state) {
  CHECK(state);
  CHECK(state->model);

  if (label) {
    if (!state->model->StartLabeledRewards(*label)) {
      yyerror(location, "duplicate rewards label", state);
    }
    delete label;
  } else {
    state->model->StartUnlabeledRewards();
  }
}

static void EndRewards(ParserState* state) {
  CHECK(state);
  CHECK(state->model);

  state->model->EndRewards();
}

static void AddStateReward(const ParsedExpression* guard,
                           const ParsedExpression* reward,
                           ParserState* state) {
  CHECK(state);
  CHECK(state->model);

  state->model->AddStateReward(StateReward(
      std::unique_ptr<const ParsedExpression>(guard),
      std::unique_ptr<const ParsedExpression>(reward)));
}

static void AddTransitionReward(const std::string* action,
                                const ParsedExpression* guard,
                                const ParsedExpression* reward,
                                ParserState* state) {
  CHECK(state);
  CHECK(state->model);

  state->model->AddTransitionReward(TransitionReward(
      MakeOptionalAction(action),
      std::unique_ptr<const ParsedExpression>(guard),
      std::unique_ptr<const ParsedExpression>(reward)));
}

static void StartModule(
    const YYLTYPE& location, const std::string* name, ParserState* state) {
  CHECK(state);
  CHECK(state->model);

  if (!state->model->StartModule(*name)) {
    yyerror(location, "duplicate module identifier", state);
  }
  delete name;
}

static void EndModule(ParserState* state) {
  CHECK(state);
  CHECK(state->model);

  state->model->EndModule();
}

static void AddFromModule(
    const YYLTYPE& location,
    const std::string* from_module,
    const std::map<std::string, std::string>* substitutions,
    ParserState* state) {
  CHECK(from_module);
  CHECK(substitutions);
  CHECK(state);
  CHECK(state->model);

  std::string error;
  if (!state->model->AddFromModule(*from_module, *substitutions, &error)) {
    yyerror(location, error, state);
  }
  delete from_module;
  delete substitutions;
}

static void AddCommand(const YYLTYPE& location,
                       const std::string* action,
                       const ParsedExpression* guard,
                       std::vector<ParsedOutcome>* outcomes,
                       ParserState* state) {
  CHECK(state);
  CHECK(state->model);

  if (!state->model->AddCommand(
          ParsedCommand(MakeOptionalAction(action),
                        std::unique_ptr<const ParsedExpression>(guard),
                        std::move(*outcomes)))) {
    yyerror(location, "duplicate identifier", state);
  }
  delete outcomes;
}

static std::map<std::string, std::string>* AddSubstitution(
    const YYLTYPE& location,
    const std::string* from_name,
    const std::string* to_name,
    std::map<std::string, std::string>* substitutions,
    ParserState* state) {
  CHECK(from_name);
  CHECK(to_name);

  if (substitutions) {
    if (!substitutions->insert({ *from_name, *to_name }).second) {
      yyerror(location, "duplicate substitution", state);
    }
  } else {
    substitutions = new std::map<std::string, std::string>(
      { { *from_name, *to_name } });
  }
  delete from_name;
  delete to_name;
  return substitutions;
}

namespace {

template <typename Movable>
static std::vector<Movable>* AddToVector(Movable* element,
                                         std::vector<Movable>* elements) {
  CHECK(element);

  if (elements == nullptr) {
    elements = new std::vector<Movable>();
  }
  elements->push_back(std::move(*element));
  delete element;
  return elements;
}

}  // namespace

static std::vector<ParsedOutcome>* AddOutcome(
    ParsedOutcome* outcome, std::vector<ParsedOutcome>* outcomes) {
  return AddToVector(outcome, outcomes);
}

static ParsedOutcome* MakeOutcome(const ParsedExpression* probability,
                                  std::vector<ParsedUpdate>* updates) {
  CHECK(updates);

  if (probability == nullptr) {
    probability = new ParsedLiteral(1.0);
  }
  ParsedOutcome* outcome =
      new ParsedOutcome(std::unique_ptr<const ParsedExpression>(probability),
                        std::move(*updates));
  delete updates;
  return outcome;
}

static std::vector<ParsedUpdate>* AddUpdate(
    ParsedUpdate* update, std::vector<ParsedUpdate>* updates) {
  return AddToVector(update, updates);
}

static ParsedUpdate* MakeUpdate(const std::string* variable,
                                const ParsedExpression* expr) {
  CHECK(variable);

  ParsedUpdate* update = new ParsedUpdate(
      *variable, std::unique_ptr<const ParsedExpression>(expr));
  delete variable;
  return update;
}

static Function MakeFunction(
    const YYLTYPE& location, const std::string* name, ParserState* state) {
  CHECK(name);

  Function function;
  if (*name == "floor") {
    function = Function::FLOOR;
  } else if (*name == "ceil") {
    function = Function::CEIL;
  } else if (*name == "pow") {
    function = Function::POW;
  } else if (*name == "mod") {
    function = Function::MOD;
  } else if (*name == "log") {
    function = Function::LOG;
  } else {
    function = Function::NO_FUNCTION;
    yyerror(location, "unknown function", state);
  }
  delete name;
  return function;
}

static const ParsedExpression* MakeUnaryOperation(
    UnaryOperator op, const ParsedExpression* operand) {
  return new ParsedUnaryOperation(
      op, std::unique_ptr<const ParsedExpression>(operand));
}

static const ParsedExpression* MakeBinaryOperation(
    BinaryOperator op,
    const ParsedExpression* operand1,
    const ParsedExpression* operand2) {
  return new ParsedBinaryOperation(
      op,
      std::unique_ptr<const ParsedExpression>(operand1),
      std::unique_ptr<const ParsedExpression>(operand2));
}

static const ParsedExpression* MakeConditional(
    const ParsedExpression* condition,
    const ParsedExpression* if_expr,
    const ParsedExpression* else_expr) {
  return new ParsedConditional(
      std::unique_ptr<const ParsedExpression>(condition),
      std::unique_ptr<const ParsedExpression>(if_expr),
      std::unique_ptr<const ParsedExpression>(else_expr));
}

static void SetSystem(
    const YYLTYPE& location, const ProcessAlgebra* system, ParserState* state) {
  CHECK(state);
  CHECK(state->model);

  if (!state->model->SetSystem(std::unique_ptr<const ProcessAlgebra>(system))) {
    yyerror(location, "multiple system blocks", state);
  }
}

static const ProcessAlgebra* MakeActionHiding(
    std::set<std::string>* actions, const ProcessAlgebra* operand) {
  CHECK(actions);

  const ProcessAlgebra* process_algebra = new ActionHiding(
      std::move(*actions), std::unique_ptr<const ProcessAlgebra>(operand));
  delete actions;
  return process_algebra;
}

static const ProcessAlgebra* MakeActionRenaming(
    std::map<std::string, std::string>* substitutions,
    const ProcessAlgebra* operand) {
  CHECK(substitutions);

  const ProcessAlgebra* process_algebra = new ActionRenaming(
      std::move(*substitutions),
      std::unique_ptr<const ProcessAlgebra>(operand));
  delete substitutions;
  return process_algebra;
}

static const ProcessAlgebra* MakeRestrictedParallelComposition(
    std::set<std::string>* actions,
    const ProcessAlgebra* operand1, const ProcessAlgebra* operand2) {
  CHECK(actions);

  const ProcessAlgebra* process_algebra = new RestrictedParallelComposition(
      std::move(*actions),
      std::unique_ptr<const ProcessAlgebra>(operand1),
      std::unique_ptr<const ProcessAlgebra>(operand2));
  delete actions;
  return process_algebra;
}

namespace {

const ProcessAlgebra* MakeParallelComposition(ParallelCompositionOperator op,
                                              const ProcessAlgebra* operand1,
                                              const ProcessAlgebra* operand2) {
  return new ParallelComposition(
      op,
      std::unique_ptr<const ProcessAlgebra>(operand1),
      std::unique_ptr<const ProcessAlgebra>(operand2));
}

}  // namespace

static const ProcessAlgebra* MakeAlphabetizedParallelComposition(
    const ProcessAlgebra* operand1, const ProcessAlgebra* operand2) {
  return MakeParallelComposition(ParallelCompositionOperator::ALPHABETIZED,
                                 operand1, operand2);
}

static const ProcessAlgebra* MakeAsynchronousParallelComposition(
    const ProcessAlgebra* operand1, const ProcessAlgebra* operand2) {
  return MakeParallelComposition(ParallelCompositionOperator::ASYNCHRONOUS,
                                 operand1, operand2);
}

static std::set<std::string>* AddAction(const YYLTYPE& location,
                                        const std::string* action,
                                        std::set<std::string>* actions,
                                        ParserState* state) {
  CHECK(action);

  if (actions) {
    if (!actions->insert(*action).second) {
      yyerror(location, "duplicate action", state);
    }
  } else {
    actions = new std::set<std::string>({ *action });
  }
  delete action;
  return actions;
}


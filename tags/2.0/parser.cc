/* A Bison parser, made from parser.yy
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

# define	STOCHASTIC	257
# define	CONST	258
# define	RATE	259
# define	GLOBAL	260
# define	INIT	261
# define	TRUE_TOKEN	262
# define	FALSE_TOKEN	263
# define	EXP	264
# define	MODULE	265
# define	ENDMODULE	266
# define	PNAME	267
# define	NAME	268
# define	NUMBER	269
# define	ARROW	270
# define	DOTDOT	271
# define	ILLEGAL_TOKEN	272
# define	IMPLY	273
# define	LTE	274
# define	GTE	275
# define	EQ	276
# define	NEQ	277

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
static const Variable* find_rate(const std::string* ident, bool constant);
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
static void declare_constant(const std::string* ident, int value);
/* Declares a rate constant. */
static void declare_rate(const std::string* ident, const Rational* value);
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

#line 185 "parser.yy"
#ifndef YYSTYPE
typedef union {
  size_t synch;
  StateFormula* formula;
  const PathFormula* path;
  const Distribution* dist;
  const Expression* expr;
  Range range;
  int nat;
  const std::string* str;
  const Rational* num;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 0
#endif



#define	YYFINAL		243
#define	YYFLAG		-32768
#define	YYNTBASE	45

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 277 ? yytranslate[x] : 78)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    22,     2,     2,     2,     2,    20,     2,
      39,    40,    31,    29,    38,    30,     2,    32,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    35,    34,
      23,    33,    26,     2,     2,     2,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    24,    25,    27,    28
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     1,     4,     6,     8,     9,    14,    16,    17,
      20,    26,    32,    38,    46,    47,    50,    51,    58,    67,
      68,    70,    72,    76,    80,    81,    84,    89,    96,    97,
     100,   101,   110,   113,   117,   121,   125,   129,   133,   137,
     140,   144,   148,   152,   156,   160,   164,   168,   170,   175,
     182,   189,   196,   198,   200,   204,   208,   212,   216,   218,
     220,   224,   228,   232,   234,   236,   240,   244,   248,   254,
     256,   258,   262,   266,   270,   274,   276,   277,   280,   282,
     284,   291,   298,   305,   312,   316,   320,   324,   327,   331,
     335,   339,   343,   347,   351,   355,   361,   370,   372,   374,
     378,   382,   386
};
static const short yyrhs[] =
{
      -1,    46,    47,     0,    48,     0,    74,     0,     0,    50,
      49,    51,    53,     0,     3,     0,     0,    51,    52,     0,
       4,    14,    33,    73,    34,     0,     5,    14,    33,    15,
      34,     0,     6,    14,    35,    71,    34,     0,     6,    14,
      35,    71,     7,    72,    34,     0,     0,    53,    54,     0,
       0,    11,    14,    55,    59,    61,    12,     0,    11,    14,
      33,    14,    36,    56,    37,    12,     0,     0,    57,     0,
      58,     0,    57,    38,    58,     0,    14,    33,    14,     0,
       0,    59,    60,     0,    14,    35,    71,    34,     0,    14,
      35,    71,     7,    72,    34,     0,     0,    61,    62,     0,
       0,    64,    66,    16,    67,    35,    63,    65,    34,     0,
      36,    37,     0,    36,    14,    37,     0,    13,    33,    68,
       0,    65,    20,    65,     0,    39,    65,    40,     0,    66,
      20,    66,     0,    66,    21,    66,     0,    22,    66,     0,
      68,    23,    68,     0,    68,    24,    68,     0,    68,    25,
      68,     0,    68,    26,    68,     0,    68,    33,    68,     0,
      68,    28,    68,     0,    39,    66,    40,     0,    69,     0,
      10,    39,    69,    40,     0,    41,    39,    70,    38,    70,
      40,     0,    42,    39,    70,    38,    70,    40,     0,    43,
      39,    70,    38,    70,    40,     0,    73,     0,    14,     0,
      68,    29,    68,     0,    68,    30,    68,     0,    68,    31,
      68,     0,    39,    68,    40,     0,    15,     0,    14,     0,
      69,    31,    69,     0,    69,    32,    69,     0,    39,    69,
      40,     0,    15,     0,    14,     0,    70,    31,    70,     0,
      70,    32,    70,     0,    39,    70,    40,     0,    36,    72,
      17,    72,    37,     0,    73,     0,    14,     0,    72,    29,
      72,     0,    72,    30,    72,     0,    72,    31,    72,     0,
      39,    72,    40,     0,    15,     0,     0,    74,    75,     0,
       8,     0,     9,     0,    44,    23,    15,    36,    76,    37,
       0,    44,    24,    15,    36,    76,    37,     0,    44,    25,
      15,    36,    76,    37,     0,    44,    26,    15,    36,    76,
      37,     0,    75,    19,    75,     0,    75,    20,    75,     0,
      75,    21,    75,     0,    22,    75,     0,    77,    23,    77,
       0,    77,    24,    77,     0,    77,    25,    77,     0,    77,
      26,    77,     0,    77,    33,    77,     0,    77,    28,    77,
       0,    39,    75,    40,     0,    75,    43,    24,    15,    75,
       0,    75,    43,    36,    15,    38,    15,    37,    75,     0,
      73,     0,    14,     0,    77,    29,    77,     0,    77,    30,
      77,     0,    77,    31,    77,     0,    39,    77,    40,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   209,   209,   213,   214,   221,   221,   225,   231,   232,
     235,   236,   237,   238,   246,   247,   250,   250,   252,   256,
     257,   260,   261,   264,   267,   268,   271,   272,   276,   277,
     280,   280,   284,   285,   288,   289,   290,   297,   298,   299,
     300,   301,   302,   303,   304,   305,   306,   313,   314,   315,
     317,   319,   326,   327,   328,   329,   330,   331,   334,   335,
     336,   337,   338,   341,   342,   343,   345,   347,   354,   357,
     358,   359,   360,   361,   363,   366,   373,   374,   378,   379,
     380,   382,   384,   386,   388,   389,   390,   391,   392,   393,
     394,   395,   396,   397,   398,   401,   403,   408,   409,   410,
     411,   412,   413
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "STOCHASTIC", "CONST", "RATE", "GLOBAL", 
  "INIT", "TRUE_TOKEN", "FALSE_TOKEN", "EXP", "MODULE", "ENDMODULE", 
  "PNAME", "NAME", "NUMBER", "ARROW", "DOTDOT", "ILLEGAL_TOKEN", "IMPLY", 
  "'&'", "'|'", "'!'", "'<'", "LTE", "GTE", "'>'", "EQ", "NEQ", "'+'", 
  "'-'", "'*'", "'/'", "'='", "';'", "':'", "'['", "']'", "','", "'('", 
  "')'", "'W'", "'L'", "'U'", "'P'", "file", "@1", "model_or_properties", 
  "model", "@2", "model_type", "declarations", "declaration", "modules", 
  "module_decl", "@3", "substitutions", "subst_list", "subst", 
  "variables", "variable_decl", "commands", "command", "@4", 
  "synchronization", "update", "formula", "distribution", "expr", 
  "rate_expr", "const_rate_expr", "range", "const_expr", "integer", 
  "properties", "csl_formula", "path_formula", "csl_expr", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    46,    45,    47,    47,    49,    48,    50,    51,    51,
      52,    52,    52,    52,    53,    53,    55,    54,    54,    56,
      56,    57,    57,    58,    59,    59,    60,    60,    61,    61,
      63,    62,    64,    64,    65,    65,    65,    66,    66,    66,
      66,    66,    66,    66,    66,    66,    66,    67,    67,    67,
      67,    67,    68,    68,    68,    68,    68,    68,    69,    69,
      69,    69,    69,    70,    70,    70,    70,    70,    71,    72,
      72,    72,    72,    72,    72,    73,    74,    74,    75,    75,
      75,    75,    75,    75,    75,    75,    75,    75,    75,    75,
      75,    75,    75,    75,    75,    76,    76,    77,    77,    77,
      77,    77,    77
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     0,     2,     1,     1,     0,     4,     1,     0,     2,
       5,     5,     5,     7,     0,     2,     0,     6,     8,     0,
       1,     1,     3,     3,     0,     2,     4,     6,     0,     2,
       0,     8,     2,     3,     3,     3,     3,     3,     3,     2,
       3,     3,     3,     3,     3,     3,     3,     1,     4,     6,
       6,     6,     1,     1,     3,     3,     3,     3,     1,     1,
       3,     3,     3,     1,     1,     3,     3,     3,     5,     1,
       1,     3,     3,     3,     3,     1,     0,     2,     1,     1,
       6,     6,     6,     6,     3,     3,     3,     2,     3,     3,
       3,     3,     3,     3,     3,     5,     8,     1,     1,     3,
       3,     3,     3
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       1,    76,     7,     2,     3,     5,     4,     8,    78,    79,
      98,    75,     0,     0,     0,    97,    77,     0,    14,    87,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     9,     6,    94,   102,     0,     0,     0,     0,    84,
      85,    86,     0,    88,    89,    90,    91,    93,    99,   100,
     101,    92,     0,     0,     0,     0,    15,     0,     0,     0,
       0,     0,     0,     0,     0,    16,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    24,     0,    80,    81,
      82,    83,    10,    11,    70,     0,     0,    69,     0,    12,
       0,    28,     0,     0,     0,     0,     0,     0,     0,     0,
      19,     0,    25,     0,     0,     0,    74,     0,    71,    72,
      73,    13,     0,     0,    20,    21,     0,    17,     0,    29,
       0,    95,     0,    68,     0,     0,     0,     0,     0,    32,
      53,     0,     0,     0,     0,    52,     0,    23,    18,    22,
       0,    26,    33,    39,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      46,    57,     0,    59,    58,     0,     0,     0,     0,     0,
      47,    37,    38,     0,    40,    41,    42,    43,    45,    54,
      55,    56,    44,    96,    27,     0,     0,     0,     0,     0,
      30,     0,     0,     0,     0,    62,    64,    63,     0,     0,
       0,     0,     0,    60,    61,    48,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    67,    65,    66,     0,     0,
       0,     0,     0,     0,    31,    49,    50,    51,    34,    36,
      35,     0,     0,     0
};

static const short yydefgoto[] =
{
     241,     1,     3,     4,     7,     5,    18,    41,    42,    66,
      86,   123,   124,   125,   101,   112,   113,   129,   212,   130,
     224,   143,   179,   144,   180,   209,    84,    96,    15,     6,
      76,    77,    17
};

static const short yypact[] =
{
  -32768,    10,-32768,-32768,-32768,-32768,   101,-32768,-32768,-32768,
  -32768,-32768,   101,   101,   263,-32768,   246,   185,   286,-32768,
     108,   140,    43,    54,    60,    61,   101,   101,   101,    38,
      38,    38,    38,    38,    38,    38,    38,    38,    53,    64,
      97,-32768,   115,-32768,-32768,    83,    94,    96,   105,    52,
  -32768,-32768,    38,   265,   265,   265,   265,   265,   104,   104,
  -32768,   265,   106,   111,   112,   136,-32768,   101,   101,   101,
     101,   160,   146,   152,   151,   118,    42,   135,   155,   167,
     169,   154,   173,    92,    -3,   209,-32768,   -10,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,    92,   168,-32768,    92,-32768,
     181,   214,   223,   225,   165,    92,    92,    92,    92,   242,
     228,   224,-32768,    29,   101,   206,-32768,   221,   238,   238,
  -32768,-32768,   241,   219,   237,-32768,   151,-32768,    99,-32768,
      18,   246,   266,-32768,   269,   281,   228,     4,   267,-32768,
  -32768,    18,    18,   264,   196,-32768,   270,-32768,-32768,-32768,
      92,-32768,-32768,-32768,   117,   153,    41,    18,    18,   103,
     103,   103,   103,   103,   103,   103,   103,   103,   101,   248,
  -32768,-32768,   261,-32768,-32768,   107,   271,   272,   273,   274,
     -22,-32768,-32768,   103,   268,   268,   268,   268,   268,   275,
     275,-32768,   268,   246,-32768,   107,   199,   110,   110,   110,
  -32768,   107,   107,   172,   201,-32768,-32768,-32768,   110,   222,
     230,   232,   -11,-32768,-32768,-32768,   203,   110,   110,   110,
     110,   110,   280,   -11,    34,-32768,-32768,-32768,   205,   215,
     217,   103,    -1,   -11,-32768,-32768,-32768,-32768,   268,-32768,
  -32768,   305,   308,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,   178,-32768,-32768,-32768,-32768,-32768,-32768,
    -159,    17,-32768,  -117,  -172,   -65,   189,   -90,   -71,-32768,
      -6,   233,    68
};


#define	YYLAST		315


static const short yytable[] =
{
      16,    81,   222,   196,    98,   104,    19,    20,   109,   201,
     202,   150,    97,     2,   102,   117,   118,   119,   120,   233,
      49,    50,    51,   204,    97,   155,   103,    97,   223,   213,
     214,    99,   140,    11,    97,    97,    97,    97,   151,   239,
     141,   127,   184,   185,   186,   187,   188,   189,   190,   191,
     192,   172,    10,    11,   233,   173,   174,   142,    45,   145,
     169,    26,    27,    28,   232,   128,   203,    62,   234,    46,
     145,   145,    27,    28,   240,    47,    48,    52,    63,    97,
     175,    21,   176,   177,   178,    87,   145,   145,   145,   145,
     145,   145,   145,   145,   145,   145,   145,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    94,    11,   131,     8,
       9,    64,   145,   138,   238,    10,    11,   140,    11,    67,
      71,   173,   174,    12,   206,   207,    65,    26,    27,    28,
      68,    95,    69,   210,   211,    36,   139,   157,   158,    72,
      13,    70,   183,   216,    73,    14,   175,    74,    43,   208,
      75,    85,   226,   227,   228,   229,   230,   170,   153,   154,
     145,    11,   193,    29,    30,    31,    32,    82,    33,    34,
      35,    36,    88,    37,   181,   182,   159,   160,   161,   162,
      44,   163,   164,   165,   166,   105,   167,    83,    92,    34,
      35,    36,    89,   171,   106,   107,   108,   106,   107,   108,
      44,   164,   165,   166,    90,   116,    91,    93,    29,    30,
      31,    32,   171,    33,    34,    35,    36,   110,    37,   159,
     160,   161,   162,   100,   163,   164,   165,   166,   111,   167,
     201,   202,   201,   202,   217,   218,   217,   218,   114,   205,
     115,   215,   122,   225,   132,   235,   217,   218,   217,   218,
     106,   107,   108,   217,   218,   236,   135,   237,   133,   126,
     219,   217,   218,   217,   218,    26,    27,    28,   220,   108,
     221,   106,   107,   108,   134,   136,   121,   106,   107,   108,
     156,   146,   194,   147,   157,   158,    22,    23,    24,    25,
      38,    39,    40,   148,    34,    35,    36,   164,   165,   166,
     195,    78,    79,    80,   152,   242,   166,   168,   243,   200,
     197,   198,   199,   231,   149,   137
};

static const short yycheck[] =
{
       6,    72,    13,   175,     7,    95,    12,    13,    98,    31,
      32,     7,    83,     3,    24,   105,   106,   107,   108,    20,
      26,    27,    28,   195,    95,   142,    36,    98,    39,   201,
     202,    34,    14,    15,   105,   106,   107,   108,    34,    40,
      22,    12,   159,   160,   161,   162,   163,   164,   165,   166,
     167,    10,    14,    15,    20,    14,    15,    39,    15,   130,
     150,    19,    20,    21,   223,    36,   183,    14,    34,    15,
     141,   142,    20,    21,   233,    15,    15,    39,    14,   150,
      39,    13,    41,    42,    43,    43,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    14,    15,   114,     8,
       9,    14,   183,    14,   231,    14,    15,    14,    15,    36,
      52,    14,    15,    22,    14,    15,    11,    19,    20,    21,
      36,    39,    36,   198,   199,    31,    37,    20,    21,    33,
      39,    36,    39,   208,    33,    44,    39,    35,    40,    39,
      14,    33,   217,   218,   219,   220,   221,    40,   141,   142,
     231,    15,   168,    23,    24,    25,    26,    15,    28,    29,
      30,    31,    37,    33,   157,   158,    23,    24,    25,    26,
      40,    28,    29,    30,    31,    17,    33,    36,    34,    29,
      30,    31,    37,    40,    29,    30,    31,    29,    30,    31,
      40,    29,    30,    31,    37,    40,    37,    34,    23,    24,
      25,    26,    40,    28,    29,    30,    31,    36,    33,    23,
      24,    25,    26,    14,    28,    29,    30,    31,    14,    33,
      31,    32,    31,    32,    31,    32,    31,    32,    15,    40,
      15,    40,    14,    40,    38,    40,    31,    32,    31,    32,
      29,    30,    31,    31,    32,    40,    37,    40,    37,    35,
      38,    31,    32,    31,    32,    19,    20,    21,    38,    31,
      38,    29,    30,    31,    33,    38,    34,    29,    30,    31,
      16,    15,    34,    14,    20,    21,    23,    24,    25,    26,
       4,     5,     6,    12,    29,    30,    31,    29,    30,    31,
      39,    68,    69,    70,    37,     0,    31,    37,     0,    35,
      39,    39,    39,    33,   136,   126
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison/bison.simple"

/* Skeleton output parser for bison,

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software
   Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser when
   the %semantic_parser declaration is not specified in the grammar.
   It was written by Richard Stallman by simplifying the hairy parser
   used when %semantic_parser is specified.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

#if ! defined (yyoverflow) || defined (YYERROR_VERBOSE)

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || defined (YYERROR_VERBOSE) */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
# if YYLSP_NEEDED
  YYLTYPE yyls;
# endif
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# if YYLSP_NEEDED
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAX)
# else
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)
# endif

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif


#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
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
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).

   When YYLLOC_DEFAULT is run, CURRENT is set the location of the
   first token.  By default, to implement support for ranges, extend
   its range to the last symbol.  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)       	\
   Current.last_line   = Rhs[N].last_line;	\
   Current.last_column = Rhs[N].last_column;
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#if YYPURE
# if YYLSP_NEEDED
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, &yylloc, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval, &yylloc)
#  endif
# else /* !YYLSP_NEEDED */
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval)
#  endif
# endif /* !YYLSP_NEEDED */
#else /* !YYPURE */
# define YYLEX			yylex ()
#endif /* !YYPURE */


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
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

#ifdef YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif
#endif

#line 315 "/usr/share/bison/bison.simple"


/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif

/* YY_DECL_VARIABLES -- depending whether we use a pure parser,
   variables are global, or local to YYPARSE.  */

#define YY_DECL_NON_LSP_VARIABLES			\
/* The lookahead symbol.  */				\
int yychar;						\
							\
/* The semantic value of the lookahead symbol. */	\
YYSTYPE yylval;						\
							\
/* Number of parse errors so far.  */			\
int yynerrs;

#if YYLSP_NEEDED
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES			\
						\
/* Location data for the lookahead symbol.  */	\
YYLTYPE yylloc;
#else
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES
#endif


/* If nonreentrant, generate the variables here. */

#if !YYPURE
YY_DECL_VARIABLES
#endif  /* !YYPURE */

int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  /* If reentrant, generate the variables here. */
#if YYPURE
  YY_DECL_VARIABLES
#endif  /* !YYPURE */

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack. */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;

#if YYLSP_NEEDED
  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
#endif

#if YYLSP_NEEDED
# define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
# define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  YYSIZE_T yystacksize = YYINITDEPTH;


  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
#if YYLSP_NEEDED
  YYLTYPE yyloc;
#endif

  /* When reducing, the number of symbols on the RHS of the reduced
     rule. */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
#if YYLSP_NEEDED
  yylsp = yyls;
#endif
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  */
# if YYLSP_NEEDED
	YYLTYPE *yyls1 = yyls;
	/* This used to be a conditional around just the two extra args,
	   but that might be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
# else
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);
# endif
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
# if YYLSP_NEEDED
	YYSTACK_RELOCATE (yyls);
# endif
# undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
#if YYLSP_NEEDED
      yylsp = yyls + yysize - 1;
#endif

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

#if YYDEBUG
     /* We have to keep this `#if YYDEBUG', since we use variables
	which are defined only if `YYDEBUG' is set.  */
      if (yydebug)
	{
	  YYFPRINTF (stderr, "Next token is %d (%s",
		     yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise
	     meaning of a token, for further debugging info.  */
# ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
# endif
	  YYFPRINTF (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
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

     Otherwise, the following line sets YYVAL to the semantic value of
     the lookahead token.  This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

#if YYLSP_NEEDED
  /* Similarly for the default location.  Let the user run additional
     commands if for instance locations are ranges.  */
  yyloc = yylsp[1-yylen];
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
#endif

#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] > 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif

  switch (yyn) {

case 1:
#line 209 "parser.yy"
{ success = true; line_number = 1; }
    break;
case 2:
#line 210 "parser.yy"
{ check_undeclared(); if (!success) YYERROR; }
    break;
case 5:
#line 221 "parser.yy"
{ prepare_model(); }
    break;
case 6:
#line 222 "parser.yy"
{ compile_model(); }
    break;
case 10:
#line 235 "parser.yy"
{ declare_constant(yyvsp[-3].str, yyvsp[-1].nat); }
    break;
case 11:
#line 236 "parser.yy"
{ declare_rate(yyvsp[-3].str, yyvsp[-1].num); }
    break;
case 12:
#line 237 "parser.yy"
{ declare_variable(yyvsp[-3].str, yyvsp[-1].range, NULL); }
    break;
case 13:
#line 239 "parser.yy"
{ declare_variable(yyvsp[-5].str, yyvsp[-3].range, yyvsp[-1].expr); }
    break;
case 16:
#line 250 "parser.yy"
{ prepare_module(yyvsp[0].str); }
    break;
case 17:
#line 251 "parser.yy"
{ add_module(); }
    break;
case 18:
#line 253 "parser.yy"
{ add_module(yyvsp[-6].str, yyvsp[-4].str); }
    break;
case 23:
#line 264 "parser.yy"
{ add_substitution(yyvsp[-2].str, yyvsp[0].str); }
    break;
case 26:
#line 271 "parser.yy"
{ declare_variable(yyvsp[-3].str, yyvsp[-1].range, NULL); }
    break;
case 27:
#line 273 "parser.yy"
{ declare_variable(yyvsp[-5].str, yyvsp[-3].range, yyvsp[-1].expr); }
    break;
case 30:
#line 281 "parser.yy"
{ prepare_command(yyvsp[-4].synch, *yyvsp[-3].formula, *yyvsp[-1].dist); }
    break;
case 31:
#line 281 "parser.yy"
{ add_command(); }
    break;
case 32:
#line 284 "parser.yy"
{ yyval.synch = 0; }
    break;
case 33:
#line 285 "parser.yy"
{ yyval.synch = synchronization_value(yyvsp[-1].str); }
    break;
case 34:
#line 288 "parser.yy"
{ add_update(yyvsp[-2].str, *yyvsp[0].expr); }
    break;
case 37:
#line 297 "parser.yy"
{ yyval.formula = make_conjunction(*yyvsp[-2].formula, *yyvsp[0].formula); }
    break;
case 38:
#line 298 "parser.yy"
{ yyval.formula = make_disjunction(*yyvsp[-2].formula, *yyvsp[0].formula); }
    break;
case 39:
#line 299 "parser.yy"
{ yyval.formula = new Negation(*yyvsp[0].formula); }
    break;
case 40:
#line 300 "parser.yy"
{ yyval.formula = new LessThan(*yyvsp[-2].expr, *yyvsp[0].expr); }
    break;
case 41:
#line 301 "parser.yy"
{ yyval.formula = new LessThanOrEqual(*yyvsp[-2].expr, *yyvsp[0].expr); }
    break;
case 42:
#line 302 "parser.yy"
{ yyval.formula = new GreaterThanOrEqual(*yyvsp[-2].expr, *yyvsp[0].expr); }
    break;
case 43:
#line 303 "parser.yy"
{ yyval.formula = new GreaterThan(*yyvsp[-2].expr, *yyvsp[0].expr); }
    break;
case 44:
#line 304 "parser.yy"
{ yyval.formula = new Equality(*yyvsp[-2].expr, *yyvsp[0].expr); }
    break;
case 45:
#line 305 "parser.yy"
{ yyval.formula = new Inequality(*yyvsp[-2].expr, *yyvsp[0].expr); }
    break;
case 46:
#line 306 "parser.yy"
{ yyval.formula = yyvsp[-1].formula; }
    break;
case 47:
#line 313 "parser.yy"
{ yyval.dist = &Exponential::make(*yyvsp[0].expr); }
    break;
case 48:
#line 314 "parser.yy"
{ yyval.dist = &Exponential::make(*yyvsp[-1].expr); }
    break;
case 49:
#line 316 "parser.yy"
{ yyval.dist = &Weibull::make(*yyvsp[-3].expr, *yyvsp[-1].expr); }
    break;
case 50:
#line 318 "parser.yy"
{ yyval.dist = &Lognormal::make(*yyvsp[-3].expr, *yyvsp[-1].expr); }
    break;
case 51:
#line 320 "parser.yy"
{ yyval.dist = &Uniform::make(*yyvsp[-3].expr, *yyvsp[-1].expr); }
    break;
case 52:
#line 326 "parser.yy"
{ yyval.expr = make_value(yyvsp[0].nat); }
    break;
case 53:
#line 327 "parser.yy"
{ yyval.expr = find_variable(yyvsp[0].str); }
    break;
case 54:
#line 328 "parser.yy"
{ yyval.expr = &Addition::make(*yyvsp[-2].expr, *yyvsp[0].expr); }
    break;
case 55:
#line 329 "parser.yy"
{ yyval.expr = &Subtraction::make(*yyvsp[-2].expr, *yyvsp[0].expr); }
    break;
case 56:
#line 330 "parser.yy"
{ yyval.expr = &Multiplication::make(*yyvsp[-2].expr, *yyvsp[0].expr); }
    break;
case 57:
#line 331 "parser.yy"
{ yyval.expr = yyvsp[-1].expr; }
    break;
case 58:
#line 334 "parser.yy"
{ yyval.expr = make_value(yyvsp[0].num); }
    break;
case 59:
#line 335 "parser.yy"
{ yyval.expr = find_rate(yyvsp[0].str, false); }
    break;
case 60:
#line 336 "parser.yy"
{ yyval.expr = &Multiplication::make(*yyvsp[-2].expr, *yyvsp[0].expr); }
    break;
case 61:
#line 337 "parser.yy"
{ yyval.expr = &Division::make(*yyvsp[-2].expr, *yyvsp[0].expr); }
    break;
case 62:
#line 338 "parser.yy"
{ yyval.expr = yyvsp[-1].expr; }
    break;
case 63:
#line 341 "parser.yy"
{ yyval.expr = make_value(yyvsp[0].num); }
    break;
case 64:
#line 342 "parser.yy"
{ yyval.expr = find_rate(yyvsp[0].str, true); }
    break;
case 65:
#line 344 "parser.yy"
{ yyval.expr = &Multiplication::make(*yyvsp[-2].expr, *yyvsp[0].expr); }
    break;
case 66:
#line 346 "parser.yy"
{ yyval.expr = &Division::make(*yyvsp[-2].expr, *yyvsp[0].expr); }
    break;
case 67:
#line 347 "parser.yy"
{ yyval.expr = yyvsp[-1].expr; }
    break;
case 68:
#line 354 "parser.yy"
{ yyval.range = make_range(yyvsp[-3].expr, yyvsp[-1].expr); }
    break;
case 69:
#line 357 "parser.yy"
{ yyval.expr = make_value(yyvsp[0].nat); }
    break;
case 70:
#line 358 "parser.yy"
{ yyval.expr = find_constant(yyvsp[0].str); }
    break;
case 71:
#line 359 "parser.yy"
{ yyval.expr = &Addition::make(*yyvsp[-2].expr, *yyvsp[0].expr); }
    break;
case 72:
#line 360 "parser.yy"
{ yyval.expr = &Subtraction::make(*yyvsp[-2].expr, *yyvsp[0].expr); }
    break;
case 73:
#line 362 "parser.yy"
{ yyval.expr = &Multiplication::make(*yyvsp[-2].expr, *yyvsp[0].expr); }
    break;
case 74:
#line 363 "parser.yy"
{ yyval.expr = yyvsp[-1].expr; }
    break;
case 75:
#line 366 "parser.yy"
{ yyval.nat = integer_value(yyvsp[0].num); }
    break;
case 77:
#line 375 "parser.yy"
{ properties.push_back(yyvsp[0].formula); StateFormula::ref(yyvsp[0].formula); }
    break;
case 78:
#line 378 "parser.yy"
{ yyval.formula = new Conjunction(); }
    break;
case 79:
#line 379 "parser.yy"
{ yyval.formula = new Disjunction(); }
    break;
case 80:
#line 381 "parser.yy"
{ yyval.formula = make_probabilistic(yyvsp[-3].num, true, true, *yyvsp[-1].path); }
    break;
case 81:
#line 383 "parser.yy"
{ yyval.formula = make_probabilistic(yyvsp[-3].num, false, true, *yyvsp[-1].path); }
    break;
case 82:
#line 385 "parser.yy"
{ yyval.formula = make_probabilistic(yyvsp[-3].num, false, false, *yyvsp[-1].path); }
    break;
case 83:
#line 387 "parser.yy"
{ yyval.formula = make_probabilistic(yyvsp[-3].num, true, false, *yyvsp[-1].path); }
    break;
case 84:
#line 388 "parser.yy"
{ yyval.formula = new Implication(*yyvsp[-2].formula, *yyvsp[0].formula); }
    break;
case 85:
#line 389 "parser.yy"
{ yyval.formula = make_conjunction(*yyvsp[-2].formula, *yyvsp[0].formula); }
    break;
case 86:
#line 390 "parser.yy"
{ yyval.formula = make_disjunction(*yyvsp[-2].formula, *yyvsp[0].formula); }
    break;
case 87:
#line 391 "parser.yy"
{ yyval.formula = new Negation(*yyvsp[0].formula); }
    break;
case 88:
#line 392 "parser.yy"
{ yyval.formula = new LessThan(*yyvsp[-2].expr, *yyvsp[0].expr); }
    break;
case 89:
#line 393 "parser.yy"
{ yyval.formula = new LessThanOrEqual(*yyvsp[-2].expr, *yyvsp[0].expr); }
    break;
case 90:
#line 394 "parser.yy"
{ yyval.formula = new GreaterThanOrEqual(*yyvsp[-2].expr, *yyvsp[0].expr); }
    break;
case 91:
#line 395 "parser.yy"
{ yyval.formula = new GreaterThan(*yyvsp[-2].expr, *yyvsp[0].expr); }
    break;
case 92:
#line 396 "parser.yy"
{ yyval.formula = new Equality(*yyvsp[-2].expr, *yyvsp[0].expr); }
    break;
case 93:
#line 397 "parser.yy"
{ yyval.formula = new Inequality(*yyvsp[-2].expr, *yyvsp[0].expr); }
    break;
case 94:
#line 398 "parser.yy"
{ yyval.formula = yyvsp[-1].formula; }
    break;
case 95:
#line 402 "parser.yy"
{ yyval.path = make_until(*yyvsp[-4].formula, *yyvsp[0].formula, NULL, yyvsp[-1].num); }
    break;
case 96:
#line 404 "parser.yy"
{ yyval.path = make_until(*yyvsp[-7].formula, *yyvsp[0].formula, yyvsp[-4].num, yyvsp[-2].num); }
    break;
case 97:
#line 408 "parser.yy"
{ yyval.expr = make_value(yyvsp[0].nat); }
    break;
case 98:
#line 409 "parser.yy"
{ yyval.expr = value_or_variable(yyvsp[0].str); }
    break;
case 99:
#line 410 "parser.yy"
{ yyval.expr = &Addition::make(*yyvsp[-2].expr, *yyvsp[0].expr); }
    break;
case 100:
#line 411 "parser.yy"
{ yyval.expr = &Subtraction::make(*yyvsp[-2].expr, *yyvsp[0].expr); }
    break;
case 101:
#line 412 "parser.yy"
{ yyval.expr = &Multiplication::make(*yyvsp[-2].expr, *yyvsp[0].expr); }
    break;
case 102:
#line 413 "parser.yy"
{ yyval.expr = yyvsp[-1].expr; }
    break;
}

#line 705 "/usr/share/bison/bison.simple"


  yyvsp -= yylen;
  yyssp -= yylen;
#if YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;
#if YYLSP_NEEDED
  *++yylsp = yyloc;
#endif

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[YYTRANSLATE (yychar)]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[YYTRANSLATE (yychar)]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* defined (YYERROR_VERBOSE) */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*--------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action |
`--------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;
      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;


/*-------------------------------------------------------------------.
| yyerrdefault -- current state does not do anything special for the |
| error token.                                                       |
`-------------------------------------------------------------------*/
yyerrdefault:
#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */

  /* If its default is to accept any token, ok.  Otherwise pop it.  */
  yyn = yydefact[yystate];
  if (yyn)
    goto yydefault;
#endif


/*---------------------------------------------------------------.
| yyerrpop -- pop the current state because it cannot handle the |
| error token                                                    |
`---------------------------------------------------------------*/
yyerrpop:
  if (yyssp == yyss)
    YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#if YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "Error: state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

/*--------------.
| yyerrhandle.  |
`--------------*/
yyerrhandle:
  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

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

/*---------------------------------------------.
| yyoverflowab -- parser overflow comes here.  |
`---------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}
#line 417 "parser.yy"


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
static const Variable* find_rate(const std::string* ident, bool constant) {
  std::map<std::string, const Variable*>::const_iterator ri =
    rates.find(*ident);
  if (ri != rates.end()) {
    delete ident;
    return (*ri).second;
  } else {
    if (constant) {
      yyerror("variable parameters only permitted for exponential"
	      " distribution");
    }
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
static void declare_constant(const std::string* ident, int value) {
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
    constant_values.insert(std::make_pair(v, value));
    rate_values.insert(std::make_pair(v, value));
  }
  delete ident;
}


/* Declares a rate constant. */
static void declare_rate(const std::string* ident, const Rational* value) {
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
    rate_values.insert(std::make_pair(v, *value));
  }
  delete ident;
  delete value;
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

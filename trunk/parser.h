/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
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
     EQV = 313,
     IMP = 314,
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
#define EQV 313
#define IMP 314
#define NEQ 315
#define GEQ 316
#define LEQ 317
#define UMINUS 318




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 2068 of yacc.c  */
#line 336 "parser.yy"

  size_t synch;
  StateFormula* formula;
  const PathFormula* path;
  const Distribution* dist;
  const Expression* expr;
  int nat;
  const std::string* str;
  const TypedValue* num;
  Function function;
  UniquePtrVector<const Expression>* arguments;



/* Line 2068 of yacc.c  */
#line 191 "parser.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

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

extern YYLTYPE yylloc;


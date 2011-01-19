
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
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

/* Line 1676 of yacc.c  */
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



/* Line 1676 of yacc.c  */
#line 113 "parser.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;



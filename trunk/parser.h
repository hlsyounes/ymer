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

/* Line 2068 of yacc.c  */
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
  PointerVector<const Expression>* arguments;



/* Line 2068 of yacc.c  */
#line 128 "parser.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;



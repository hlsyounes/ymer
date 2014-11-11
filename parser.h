/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

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

#ifndef YY_YY_PARSER_H_INCLUDED
# define YY_YY_PARSER_H_INCLUDED
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
#line 192 "parser.yy" /* yacc.c:1909  */

  size_t synch;
  StateFormula* formula;
  const PathFormula* path;
  const Distribution* dist;
  const Expression* expr;
  Range range;
  int nat;
  const std::string* str;
  const Rational* num;

#line 124 "parser.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PARSER_H_INCLUDED  */

/* -*-C++-*- */
/*
 * Tokenizer.
 *
 * Copyright (C) 2003, 2004 Carnegie Mellon University
 *
 * This file is part of Ymer.
 *
 * Ymer is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Ymer is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
 * License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ymer; if not, write to the Free Software Foundation,
 * Inc., #59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * $Id: tokenizer.ll,v 2.1 2004-01-25 12:44:37 lorens Exp $
 */
%{
#include <config.h>
#include "rational.h"
#include <string>

struct StateFormula;
struct PathFormula;
struct Distribution;
struct Expression;
/* An integer range (duplicated from parser.yy). */
struct Range { int l, h; };

#include "parser.h"


/* Current line number. */
size_t line_number;

/* Allocates a string containing the lowercase characters of the given
   C string, and returns the given token. */
static int make_string(const char* s, int token);
/* Makes a number of the given string, and return the NUMBER token. */
static int make_number(const char* s);

%}

%option never-interactive nounput noyywrap

IDENT	[A-Za-z]([A-Za-z0-9_])*

%%

stochastic		return STOCHASTIC;
const			return CONST;
rate			return RATE;
global			return GLOBAL;
module			return MODULE;
endmodule		return ENDMODULE;
init			return INIT;
true			return TRUE_TOKEN;
false			return FALSE_TOKEN;
Exp			return EXP;
[WLPUX]			return yytext[0];
{IDENT}\'		return make_string(yytext, PNAME);
{IDENT}			return make_string(yytext, NAME);
[0-9]*[./]?[0-9]+	return make_number(yytext);
->			return ARROW;
=>			return IMPLY;
\<=			return LTE;
>=			return GTE;
!=			return NEQ;
\.\.			return DOTDOT;
[,;:\[\]=+\-*/<>!&|()]	return yytext[0];
\/\/.*$			/* comment */
[ \t\r]+		/* whitespace */
\n			line_number++;
.			return ILLEGAL_TOKEN;

%%

/* Allocates a string containing the lowercase characters of the given
   C string, and returns the given token. */
static int make_string(const char* s, int token) {
  std::string* result = new std::string();
  for (const char* p = s; *p != '\0'; p++) {
    if (*p != '\'') {
      *result += *p;
    }
  }
  yylval.str = result;
  return token;
}


/* Makes a number of the given string, and return the NUMBER token. */
static int make_number(const char* s) {
  yylval.num = new Rational(s);
  return NUMBER;
}

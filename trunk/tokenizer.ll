/* -*-C++-*- */
/*
 * Tokenizer.
 *
 * Copyright (C) 2003 Carnegie Mellon University
 * Written by Håkan L. S. Younes.
 *
 * Permission is hereby granted to distribute this software for
 * non-commercial research purposes, provided that this copyright
 * notice is included with any such distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
 * SOFTWARE IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU
 * ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
 *
 * $Id: tokenizer.ll,v 1.1 2003-08-10 01:53:03 lorens Exp $
 */
%{
#include <config.h>
#include "rational.h"
#include <string>

struct StateFormula;
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
{IDENT}\'		return make_string(yytext, PNAME);
{IDENT}			return make_string(yytext, NAME);
[0-9]*[./]?[0-9]+	return make_number(yytext);
->			return ARROW;
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

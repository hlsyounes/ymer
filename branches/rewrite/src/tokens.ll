/* -*-C++-*-
 * Copyright (C) 2011 Google Inc
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
 * Lexical analyzer for the PRISM language.
 */

%{
#include <cstdlib>
#include <map>
#include <string>
#include <vector>

#include "src/expression.h"
#include "src/model.h"
#include "src/process-algebra.h"

#include "grammar.h"

#define YY_USER_INIT yylloc->first_line = yylloc->last_line = 1;

// Sets lval->str to a newly allocated string with content s and returns
// token.
static int MakeString(YYSTYPE* lval, const char* s, int token);
// Sets lval->int_literal to the int represented by s and returns
// INT_LITERAL.
static int MakeInt(YYSTYPE* lval, const char* s);
// Sets lval->double_literal to the double represented by s and returns
// DOUBLE_LITERAL.
static int MakeDouble(YYSTYPE* lval, const char* s);
%}

%option bison-bridge bison-locations never-interactive nounput noyywrap
%option reentrant

%%

A                          return A;
bool                       return BOOL_TOKEN;
const                      return CONST;
ctmc                       return CTMC_TOKEN;
C                          return C;
double                     return DOUBLE_TOKEN;
dtmc                       return DTMC_TOKEN;
endinit                    return ENDINIT;
endmodule                  return ENDMODULE;
endrewards                 return ENDREWARDS;
endsystem                  return ENDSYSTEM;
E                          return E;
false                      return FALSE;
formula                    return FORMULA;
func                       return FUNC;
F                          return F;
global                     return GLOBAL;
G                          return G;
init                       return INIT;
int                        return INT_TOKEN;
I                          return I;
label                      return LABEL;
max                        return MAX_TOKEN;
mdp                        return MDP_TOKEN;
min                        return MIN_TOKEN;
module                     return MODULE;
nondeterministic           return NONDETERMINISTIC;
Pmax                       return PMAX;
Pmin                       return PMIN;
P                          return P;
probabilistic              return PROBABILISTIC;
prob                       return PROB;
rate                       return RATE;
rewards                    return REWARDS;
Rmax                       return RMAX;
Rmin                       return RMIN;
R                          return R;
stochastic                 return STOCHASTIC;
system                     return SYSTEM;
S                          return S;
true                       return TRUE;
U                          return U;
W                          return W;
X                          return X;
[A-Za-z][A-Za-z0-9_]*      return MakeString(yylval, yytext, IDENTIFIER);
\"[A-Za-z][A-Za-z0-9_]*\"  return MakeString(yylval, yytext, LABEL_NAME);
[0-9]+\.[0-9]+             return MakeDouble(yylval, yytext);
[0-9]+                     return MakeInt(yylval, yytext);
->                         return ARROW;
\'                         return PRIME;
=>                         return IMP;
!=                         return NEQ;
\<=                        return LEQ;
>=                         return GEQ;
\.\.                       return DOTDOT;
\|\|\|                     return TRIPLE_BAR;
\|\|                       return DOUBLE_BAR;
\<-                        return BACK_ARROW;
[;,[\]?:|&!=<>+\-*/(){}]   return yytext[0];
\/\/.*$                    /* comment */
[ \t\r]+                   /* whitespace */
\n                         yylloc->last_line = ++yylloc->first_line;

%%

static int MakeString(YYSTYPE* lval, const char* s, int token) {
  lval->str = new std::string(s);
  return token;
}

static int MakeInt(YYSTYPE* lval, const char* s) {
  lval->int_literal = atoi(s);
  return INT_LITERAL;
}

static int MakeDouble(YYSTYPE* lval, const char* s) {
  lval->double_literal = atof(s);
  return DOUBLE_LITERAL;
}

/* -*-C++-*-
 * Copyright (C) 2003--2005 Carnegie Mellon University
 * Copyright (C) 2011--2014 Google Inc
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
#include <string>

#include "distribution.h"
#include "expression.h"
#include "model.h"
#include "parser-state.h"
#include "process-algebra.h"
#include "typed-value.h"
#include "unique-ptr-vector.h"

#include "grammar.hh"

#define YY_USER_INIT yylloc->first_line = yylloc->last_line = 1;

namespace {

// Sets lval->str to a newly allocated string with content s, and returns token.
int MakeString(YYSTYPE* lval, const char* s, int token) {
  lval->str = new std::string(s);
  return token;
}

// Sets lval->number to a newly allocated TypedValue with the int value given by
// s, and returns LITERAL.
int MakeIntLiteral(YYSTYPE* lval, const char* s) {
  lval->number = new TypedValue(atoi(s));
  return NUMBER;
}

// Sets lval->number to a newly allocated TypedValue with the double value given
// by s, and returns NUMBER.
int MakeDoubleLiteral(YYSTYPE* lval, const char* s) {
  lval->number = new TypedValue(atof(s));
  return NUMBER;
}

}  // namespace
%}

%option bison-bridge bison-locations never-interactive noinput nounput noyywrap
%option reentrant

%%

A                          return A;
bool                       return BOOL_TOKEN;
C                          return C;
clock                      return CLOCK;
const                      return CONST;
ctmc                       return CTMC_TOKEN;
double                     return DOUBLE_TOKEN;
dtmc                       return DTMC_TOKEN;
E                          return E;
endinit                    return ENDINIT;
endinvariant               return ENDINVARIANT;
endmodule                  return ENDMODULE;
endrewards                 return ENDREWARDS;
endsystem                  return ENDSYSTEM;
F                          return F;
false                      return FALSE;
filter                     return FILTER;
formula                    return FORMULA;
func                       return FUNC;
G                          return G;
global                     return GLOBAL;
gsmp                       return GSMP_TOKEN;
I                          return I;
init                       return INIT;
int                        return INT_TOKEN;
invariant                  return INVARIANT;
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
pta                        return PTA;
Rmax                       return RMAX;
Rmin                       return RMIN;
R                          return R;
rate                       return RATE;
rewards                    return REWARDS;
S                          return S;
stochastic                 return STOCHASTIC;
system                     return SYSTEM;
true                       return TRUE;
U                          return U;
W                          return W;
X                          return X;
@L                         return LOGNORMAL;
@U                         return UNIFORM;
@W                         return WEIBULL;
[A-Za-z][A-Za-z0-9_]*      return MakeString(yylval, yytext, IDENTIFIER);
\"[A-Za-z][A-Za-z0-9_]*\"  return MakeString(yylval, yytext, LABEL_NAME);
[0-9]+\.[0-9]+             return MakeDoubleLiteral(yylval, yytext);
[0-9]+                     return MakeIntLiteral(yylval, yytext);
->                         return ARROW;
\'                         return PRIME;
\<=>                       return IFF_TOKEN;
=>                         return IMPLY_TOKEN;
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

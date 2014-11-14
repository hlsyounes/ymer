/* -*-C++-*- */
/*
 * Parser.
 *
 * Copyright (C) 2003--2005 Carnegie Mellon University
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
 */
%{
#include <config.h>
#include "models.h"
#include "modules.h"
#include "distributions.h"
#include "formulas.h"
#include "expressions.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <stdexcept>


/* Workaround for bug in Bison 1.35 that disables stack growth. */
#define YYLTYPE_IS_TRIVIAL 1


/* An integer range. */
struct Range {
  const Expression<int>* l;
  const Expression<int>* h;
};


/* The lexer. */
extern int yylex();
/* Current line number. */
extern int line_number;
/* Name of current file. */
extern std::string current_file;

/* Last model parsed. */
const Model* global_model = 0;
/* Parsed properties. */
FormulaList properties;
/* Parsed path formula. */
const PathFormula* global_path_formula = 0;

/* Parser mode (0: standard; 1: model; 2: path formula) */
static int mode = 0;
/* Current model. */
static Model* model;
/* Current module. */
static Module* module;
/* Source module for substitutions. */
static Module* src_module;
/* Current constant and variable substitutions. */
static Substitutions subst;
/* Current synchronization substitutions. */
static SynchSubstitutions synch_subst;
/* Current command. */
static Command* command;
/* Declared integer constants. */
static std::map<std::string, const Constant<int>*> constants;
/* Declared rate constants. */
static std::map<std::string, const Constant<double>*> rates;
/* All state variables. */
static std::map<std::string, Variable<int>*> variables;
/* Declared modules. */
static std::map<std::string, Module*> modules;
/* Declared synchronizations. */
static std::map<std::string, int> synchronizations;
/* Yet undeclared state variables. */
static std::set<std::string> undeclared;
/* Whether to negate the current formula. */ 
static std::stack<bool> negate;
/* Whether the last parsing attempt succeeded. */
static bool success = true;

/* Outputs an error message. */
static void yyerror(const std::string& s); 
/* Outputs a warning message. */
static void yywarning(const std::string& s);
/* Checks if undeclared variables were used. */
static void check_undeclared();
/* Returns a pointer to a variable or constant with the given name, or 0. */
static const Expression<int>* find_named_expression(const std::string* ident,
						    bool constant);
/* Returns a pointer to a rate expression with the given name, or 0. */
static const Expression<double>* find_rate(const std::string* ident,
					   bool constant);
/* Returns a range with the given bounds, signaling an error if the
   range is empty. */
static Range make_range(const Expression<int>* l, const Expression<int>* h);
/* Returns a value expression. */
static const Value<int>* make_value(int n);
/* Returns a value expression. */
static const Value<double>* make_value(double x);
/* Returns a tautology. */ 
static StateFormula* make_tautology();
/* Returns a contradiction. */
static StateFormula* make_contradiction();
/* Returns a conjunction. */
static StateFormula* make_conjunction(StateFormula& f1,
				      const StateFormula& f2);
/* Returns a disjunction. */
static StateFormula* make_disjunction(StateFormula& f1,
				      const StateFormula& f2);
/* Returns an implication. */
static StateFormula* make_implication(const StateFormula& f1,
				      const StateFormula& f2);
/* Returns a probabilistic path quantification. */
static StateFormula* make_probabilistic(bool negate, double p, bool strict,
					const PathFormula& f);
/* Returns an until formula. */
static const Until* make_until(const StateFormula& f1, const StateFormula& f2,
			       double t1, double t2);
/* Adds an update to the current command. */
static void add_update(const std::string* ident, const Expression<int>& expr);
/* Returns the value of the given synchronization. */
static int synchronization_value(const std::string* ident);
/* Prepares a module substitution for parsing. */
static void prepare_substitutions(const std::string& ident);
/* Adds a substitution to the current substitution map. */
static void add_substitution(const std::string* ident1,
			     const std::string* ident2);
/* Declares an integer constant. */
static void declare_constant(const std::string* ident, int value);
/* Declares a rate constant. */
static void declare_rate(const std::string* ident, double value);
/* Declares a variable. */
static const Variable<int>* declare_variable(const std::string* ident,
					     const Range& range,
					     const Expression<int>* init);
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
/* Adds a parsed property. */
static void add_property(const StateFormula* f);
%}

%error-verbose

%token STOCHASTIC
%token CONST RATE GLOBAL INIT
%token TRUE_TOKEN FALSE_TOKEN
%token EXP
%token MODULE ENDMODULE
%token PNAME NAME INTEGER REAL
%token ARROW DOTDOT

%left IMPLY
%left '&' '|'
%left '!'
%left '<' LTE GTE '>' EQ NEQ
%left '+' '-'
%left '*' '/'

%union {
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
}

%type <synch> synchronization
%type <formula> formula csl_formula
%type <path> path_formula
%type <dist> distribution
%type <iexpr> expr const_expr
%type <dexpr> rate_expr
%type <range> range
%type <str> PNAME NAME
%type <nat> INTEGER
%type <real> real REAL

%%

file : { success = true; line_number = 1; negate.push(false); }
         model_or_properties { check_undeclared(); if (!success) YYERROR; }
     ;

model_or_properties : model
                    | properties
                    | sampling_formula
                    ;


/* ====================================================================== */
/* Model files. */

model : model_type { prepare_model(); } declarations modules
          { global_model = model; }
      ;

model_type : STOCHASTIC
           ;

/* ====================================================================== */
/* Declarations. */

declarations : /* empty */
             | declarations declaration
             ;

declaration : CONST NAME '=' INTEGER ';' { declare_constant($2, $4); }
            | RATE NAME '=' real ';' { declare_rate($2, $4); }
            | GLOBAL NAME ':' range ';' { declare_variable($2, $4, 0); }
            | GLOBAL NAME ':' range INIT const_expr ';'
                { declare_variable($2, $4, $6); }
            ;


/* ====================================================================== */
/* Modules. */

modules : /* empty */
        | modules module_decl
        ;

module_decl : MODULE NAME { prepare_module($2); } variables commands ENDMODULE
                { add_module(); }
            | MODULE NAME '=' NAME { prepare_substitutions(*$4); }
                '[' substitutions ']' ENDMODULE { add_module($2, $4); }
            ;

substitutions : /* empty */
              | subst_list
              ;

subst_list : subst
           | subst_list ',' subst
           ;

subst : NAME '=' NAME { add_substitution($1, $3); }
      ;

variables : /* empty */
          | variables variable_decl
          ;

variable_decl : NAME ':' range ';' { declare_variable($1, $3, 0); }
              | NAME ':' range INIT const_expr ';'
                  { declare_variable($1, $3, $5); }
              ;

commands : /* empty */
         | commands command
         ;

command : synchronization formula ARROW distribution ':'
            { prepare_command($1, *$2, *$4); } update ';' { add_command(); }
        ;

synchronization : '[' ']' { $$ = 0; }
                | '[' NAME ']' { $$ = synchronization_value($2); }
                ;

update : PNAME '=' expr { add_update($1, *$3); }
       | update '&' update
       | '(' update ')'
       ;


/* ====================================================================== */
/* Formulas. */

formula : formula '&' formula { $$ = make_conjunction(*$1, *$3); }
        | formula '|' formula { $$ = make_disjunction(*$1, *$3); }
        | '!' { negate.top() = !negate.top(); } formula
            { $$ = $3; negate.top() = !negate.top(); }
        | expr '<' expr { $$ = new LessThan<int>(*$1, *$3, negate.top()); }
        | expr LTE expr { $$ = new GreaterThan<int>(*$1, *$3, !negate.top()); }
        | expr GTE expr { $$ = new LessThan<int>(*$1, *$3, !negate.top()); }
        | expr '>' expr { $$ = new GreaterThan<int>(*$1, *$3, negate.top()); }
        | expr '=' expr %prec EQ
            { $$ = new Equality<int>(*$1, *$3, negate.top()); }
        | expr NEQ expr { $$ = new Equality<int>(*$1, *$3, !negate.top()); }
        | '(' formula ')' { $$ = $2; }
        ;


/* ====================================================================== */
/* Distributions. */

distribution : rate_expr { $$ = &Exponential::make(*$1); }
             | EXP '(' rate_expr ')' { $$ = &Exponential::make(*$3); }
             | 'W' '(' rate_expr ',' rate_expr ')'
                 { $$ = &Weibull::make(*$3, *$5); }
             | 'L' '(' rate_expr ',' rate_expr ')'
                 { $$ = &Lognormal::make(*$3, *$5); }
             | 'U' '(' rate_expr ',' rate_expr ')'
                 { $$ = &Uniform::make(*$3, *$5); }
             ;

/* ====================================================================== */
/* Expressions. */

expr : INTEGER { $$ = make_value($1); }
     | NAME { $$ = find_named_expression($1, false); }
     | expr '+' expr { $$ = &Addition<int>::make(*$1, *$3); }
     | expr '-' expr { $$ = &Subtraction<int>::make(*$1, *$3); }
     | expr '*' expr { $$ = &Multiplication<int>::make(*$1, *$3); }
     | '(' expr ')' { $$ = $2; }
     ;

rate_expr : real { $$ = make_value($1); }
          | NAME { $$ = find_rate($1, false); }
          | rate_expr '*' rate_expr
              { $$ = &Multiplication<double>::make(*$1, *$3); }
          | rate_expr '/' rate_expr { $$ = &Division::make(*$1, *$3); }
          | '(' rate_expr ')' { $$ = $2; }
          ;


/* ====================================================================== */
/* Ranges and integers. */

range : '[' const_expr DOTDOT const_expr ']' { $$ = make_range($2, $4); }
      ;

const_expr : INTEGER { $$ = make_value($1); }
           | NAME { $$ = find_named_expression($1, true); }
           | const_expr '+' const_expr { $$ = &Addition<int>::make(*$1, *$3); }
           | const_expr '-' const_expr
               { $$ = &Subtraction<int>::make(*$1, *$3); }
           | const_expr '*' const_expr
               { $$ = &Multiplication<int>::make(*$1, *$3); }
           | '(' const_expr ')' { $$ = $2; }
	   ;


/* ====================================================================== */
/* Properties. */

properties : /* empty */
           | properties csl_formula
               { add_property($2); }
           ;

sampling_formula : '[' { if (mode != 2) yyerror("unexpected path formula"); }
                     path_formula ']' { global_path_formula = $3; }
                 ;

csl_formula : TRUE_TOKEN { $$ = make_tautology(); }
            | FALSE_TOKEN { $$ = make_contradiction(); }
            | 'P' '<' real '[' { negate.push(false); } path_formula ']'
                {
		  negate.pop();
		  $$ = make_probabilistic(!negate.top(), $3, false, *$6);
		}
            | 'P' LTE real '[' { negate.push(false); } path_formula ']'
                {
		  negate.pop();
		  $$ = make_probabilistic(!negate.top(), $3, true, *$6);
		}
            | 'P' GTE real '[' { negate.push(false); } path_formula ']'
                {
		  negate.pop();
		  $$ = make_probabilistic(negate.top(), $3, false, *$6);
		}
            | 'P' '>' real '[' { negate.push(false); } path_formula ']'
                {
		  negate.pop();
		  $$ = make_probabilistic(negate.top(), $3, true, *$6);
		}
            | csl_formula IMPLY csl_formula
                { $$ = make_implication(*$1, *$3); }
            | csl_formula '&' csl_formula { $$ = make_conjunction(*$1, *$3); }
            | csl_formula '|' csl_formula { $$ = make_disjunction(*$1, *$3); }
            | '!' { negate.top() = !negate.top(); } csl_formula
                { $$ = $3; negate.top() = !negate.top(); }
            | expr '<' expr { $$ = new LessThan<int>(*$1, *$3, negate.top()); }
            | expr LTE expr
                { $$ = new GreaterThan<int>(*$1, *$3, !negate.top()); }
            | expr GTE expr
                { $$ = new LessThan<int>(*$1, *$3, !negate.top()); }
            | expr '>' expr
                { $$ = new GreaterThan<int>(*$1, *$3, negate.top()); }
            | expr '=' expr %prec EQ
                { $$ = new Equality<int>(*$1, *$3, negate.top()); }
            | expr NEQ expr
                { $$ = new Equality<int>(*$1, *$3, !negate.top()); }
            | '(' csl_formula ')' { $$ = $2; }
            ;

path_formula : csl_formula 'U' LTE real csl_formula
                 { $$ = make_until(*$1, *$5, 0, $4); }
             | csl_formula 'U' GTE real csl_formula
                 { $$ = make_until(*$1, *$5, $4, HUGE_VAL); }
             | csl_formula 'U' '[' real ',' real ']' csl_formula
                 { $$ = make_until(*$1, *$8, $4, $6); }
             | csl_formula 'U' csl_formula
                 { $$ = new Until(*$1, *$3, 0.0, HUGE_VAL); }
//             | 'X' csl_formula
             ;

real : INTEGER { $$ = $1; } | REAL
     ;

%%

/* Clears all previously parsed declarations. */
void clear_declarations() {
  constants.clear();
  rates.clear();
  variables.clear();
}


/* Tests if the given variables is a member of the given list. */
template<typename T>
static bool member(const VariableList<T>& vars, const Variable<T>* v) {
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
  negate.pop();
  if (!undeclared.empty()) {
    yyerror("undeclared variables used in expressions");
    for (std::set<std::string>::const_iterator si = undeclared.begin();
	 si != undeclared.end(); si++) {
      std::cerr << "  " << *si << std::endl;
    }
  }
  undeclared.clear();
  modules.clear();
  synchronizations.clear();
}


/* Returns a pointer to a variable or constant with the given name, or 0. */
static const Expression<int>* find_named_expression(const std::string* ident,
						    bool constant) {
  std::map<std::string, const Constant<int>*>::const_iterator ci =
    constants.find(*ident);
  if (ci != constants.end()) {
    delete ident;
    return (*ci).second;
  } else {
    if (constant) {
      if (variables.find(*ident) != variables.end()) {
	yyerror("variable `" + *ident + "' used where expecting constant");
      } else if (rates.find(*ident) != rates.end()) {
	yyerror("rate `" + *ident + "' used where expecting constant");
      } else {
	yyerror("undeclared constant `" + *ident + "'");
      }
      Constant<int>* c = new Constant<int>(*ident, 0);
      constants.insert(std::make_pair(*ident, c));
      delete ident;
      return c;
    } else {
      Variable<int>* v;
      std::map<std::string, Variable<int>*>::const_iterator vi =
	variables.find(*ident);
      if (vi == variables.end()) {
	v = new Variable<int>(*ident, *new Value<int>(0), *new Value<int>(1),
			      *new Value<int>(0));
	variables.insert(std::make_pair(*ident, v));
	undeclared.insert(*ident);
      } else {
	v = (*vi).second;
      }
      delete ident;
      return v;
    }
  }
}


/* Returns a pointer to the rate constant with the given name, or 0. */
static const Expression<double>* find_rate(const std::string* ident,
					   bool constant) {
  std::map<std::string, const Constant<double>*>::const_iterator ri =
    rates.find(*ident);
  if (ri != rates.end()) {
    delete ident;
    return (*ri).second;
  } else {
    return &TypeCast<int, double>::make(*find_named_expression(ident,
							       constant));
  }
}


/* Returns a range with the given bounds. */
static Range make_range(const Expression<int>* l, const Expression<int>* h) {
  Range r = { l, h };
  return r;
}


/* Returns a value expression. */
static const Value<int>* make_value(int n) {
  return new Value<int>(n);
}


/* Returns a value expression. */
static const Value<double>* make_value(double x) {
  return new Value<double>(x);
}


/* Returns a tautology. */ 
static StateFormula* make_tautology() {
  if (negate.top()) {
    return new Disjunction();
  } else {
    return new Conjunction();
  }
}


/* Returns a contradiction. */
static StateFormula* make_contradiction() {
  if (negate.top()) {
    return new Conjunction();
  } else {
    return new Disjunction();
  }
}


/* Returns a conjunction. */
static StateFormula* make_conjunction(StateFormula& f1,
				      const StateFormula& f2) {
  if (negate.top()) {
    Disjunction* disj = dynamic_cast<Disjunction*>(&f1);
    if (disj == 0) {
      disj = new Disjunction();
      disj->add_disjunct(f1);
    }
    disj->add_disjunct(f2);
    return disj;
  } else {
    Conjunction* conj = dynamic_cast<Conjunction*>(&f1);
    if (conj == 0) {
      conj = new Conjunction();
      conj->add_conjunct(f1);
    }
    conj->add_conjunct(f2);
    return conj;
  }
}


/* Returns a disjunction. */
static StateFormula* make_disjunction(StateFormula& f1,
				      const StateFormula& f2) {
  if (negate.top()) {
    Conjunction* conj = dynamic_cast<Conjunction*>(&f1);
    if (conj == 0) {
      conj = new Conjunction();
      conj->add_conjunct(f1);
    }
    conj->add_conjunct(f2);
    return conj;
  } else {
    Disjunction* disj = dynamic_cast<Disjunction*>(&f1);
    if (disj == 0) {
      disj = new Disjunction();
      disj->add_disjunct(f1);
    }
    disj->add_disjunct(f2);
    return disj;
  }
}


/* Negates the given state formula. */
static StateFormula* negation(const StateFormula& f) {
  const Conjunction* cp = dynamic_cast<const Conjunction*>(&f);
  if (cp != 0) {
    Disjunction* disj = new Disjunction();
    for (FormulaList::const_iterator fi = cp->conjuncts().begin();
	 fi != cp->conjuncts().end(); fi++) {
      disj->add_disjunct(*negation(**fi));
    }
    return disj;
  }
  const Disjunction* dp = dynamic_cast<const Disjunction*>(&f);
  if (dp != 0) {
    Conjunction* conj = new Conjunction();
    for (FormulaList::const_iterator fi = dp->disjuncts().begin();
	 fi != dp->disjuncts().end(); fi++) {
      conj->add_conjunct(*negation(**fi));
    }
    return conj;
  }
  const Probabilistic* pp = dynamic_cast<const Probabilistic*>(&f);
  if (pp != 0) {
    return new Probabilistic(!pp->negated(), pp->threshold(), pp->strict(),
			     pp->formula());
  }
  const LessThan<int>* lp = dynamic_cast<const LessThan<int>*>(&f);
  if (lp != 0) {
    return new LessThan<int>(lp->expr1(), lp->expr2(), !lp->negated());
  }
  const GreaterThan<int>* gp = dynamic_cast<const GreaterThan<int>*>(&f);
  if (gp != 0) {
    return new GreaterThan<int>(gp->expr1(), gp->expr2(), !gp->negated());
  }
  const Equality<int>* ep = dynamic_cast<const Equality<int>*>(&f);
  if (ep != 0) {
    return new Equality<int>(ep->expr1(), ep->expr2(), !ep->negated());
  }
  throw std::logic_error("unexpected formula in `negation'");
}


/* Returns an implication. */
static StateFormula* make_implication(const StateFormula& f1,
				      const StateFormula& f2) {
  StateFormula* result;
  if (negate.top()) {
    result = make_conjunction(*negation(f1), f2);
  } else {
    result = make_disjunction(*negation(f1), f2);
  }
  RCObject::ref(&f1);
  RCObject::deref(&f1);
  return result;
}


/* Returns a probabilistic path quantification. */
static StateFormula* make_probabilistic(bool negate, double p, bool strict,
					const PathFormula& f) {
  if (p < 0.0 || p > 1.0) {
    yyerror("probability bound outside the interval [0,1]");
  }
  return new Probabilistic(negate, p, strict, f);
}


/* Returns an until formula. */
static const Until* make_until(const StateFormula& f1, const StateFormula& f2,
			       double t1, double t2) {
  if (t1 < 0) {
    yyerror("negative time bound");
  } else if (t2 < t1) {
    yyerror("empty time interval");
  }
  return new Until(f1, f2, t1, t2);
}


/* Adds an update to the current command. */
static void add_update(const std::string* ident, const Expression<int>& expr) {
  const Variable<int>* v;
  std::map<std::string, Variable<int>*>::const_iterator vi =
    variables.find(*ident);
  if (vi == variables.end()) {
    yyerror("updating undeclared variable `" + *ident + "'");
    v = new Variable<int>(*ident, *new Value<int>(0), *new Value<int>(1),
			  *new Value<int>(0));
  } else {
    v = (*vi).second;
  }
  if (member(model->int_variables(), v)) {
    if (command->synch() != 0) {
      yywarning("updating global variable in synchronized command");
    }
  } else if (!member(module->int_variables(), v)) {
    yyerror("updating variable belonging to other module");
  }
  command->add_update(*new Update<int>(*v, expr));
  delete ident;
}


/* Returns the value of the given synchronization. */
static int synchronization_value(const std::string* ident) {
  int s;
  std::map<std::string, int>::const_iterator si =
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


/* Prepares a module substitution for parsing. */
static void prepare_substitutions(const std::string& ident) {
  std::map<std::string, Module*>::const_iterator mi = modules.find(ident);
  if (mi != modules.end()) {
    src_module = (*mi).second;
  } else {
    yyerror("ignoring renaming of undeclared module `" + ident + "'");
    src_module = 0;
  }
}


/* Adds a substitution to the current substitution map. */
static void add_substitution(const std::string* ident1,
			     const std::string* ident2) {
  std::map<std::string, Variable<int>*>::const_iterator vi =
    variables.find(*ident1);
  if (vi != variables.end()) {
    /* Variable substitution. */
    const Variable<int>& v1 = *(*vi).second;
    const Variable<int>* v2;
    std::map<std::string, Variable<int>*>::const_iterator vj =
      variables.find(*ident2);
    bool module_v = (src_module != 0
		     && (find(src_module->int_variables().begin(),
			      src_module->int_variables().end(), &v1)
			 != src_module->int_variables().end()));
    /* If v1 is a module variable for src_module, then v2 must be new. */
    if (vj != variables.end()) {
      if (module_v && undeclared.find(*ident2) == undeclared.end()) {
	yyerror("module variable " + v1.name()
		+ " must be substituted with fresh variable");
      }
      v2 = (*vj).second;
    } else {
      if (constants.find(*ident2) != constants.end()
	  || rates.find(*ident2) != rates.end()
	  || synchronizations.find(*ident2) != synchronizations.end()) {
	yyerror("illegal substitution `" + *ident1 + "=" + *ident2 + "'");
      } else if (!module_v) {
	yyerror("fresh variable " + *ident2
		+ " must be substituting module variable");
      }
      Variable<int>* vp2 = new Variable<int>(*ident2,
					     v1.low(), v1.high(), v1.init());
      variables.insert(std::make_pair(*ident2, vp2));
      v2 = vp2;
    }
    subst.insert(v1, *v2);
    delete ident2;
  } else {
    std::map<std::string, const Constant<int>*>::const_iterator ci =
      constants.find(*ident1);
    if (ci != constants.end()) {
      /* Constant substitution. */
      const Constant<int>& c1 = *(*ci).second;
      const Constant<int>* c2;
      std::map<std::string, const Constant<int>*>::const_iterator cj =
	constants.find(*ident2);
      if (cj != constants.end()) {
	c2 = (*cj).second;
      } else {
	yyerror("undeclared constant `" + *ident2 + "'");
	c2 = new Constant<int>(*ident2, c1.value());
	constants.insert(std::make_pair(*ident2, c2));
      }
      subst.insert(c1, *c2);
      delete ident2;
    } else {
      std::map<std::string, const Constant<double>*>::const_iterator ri =
	rates.find(*ident1);
      if (ri != rates.end()) {
	/* Rate substitution. */
	const Constant<double>& r1 = *(*ri).second;
	const Constant<double>* r2;
	std::map<std::string, const Constant<double>*>::const_iterator rj =
	  rates.find(*ident2);
	if (rj != rates.end()) {
	  r2 = (*rj).second;
	} else {
	  yyerror("undeclared rate `" + *ident2 + "'");
	  r2 = new Constant<double>(*ident2, r1.value());
	  rates.insert(std::make_pair(*ident2, r2));
	}
	subst.insert(r1, *r2);
	delete ident2;
      } else {
	std::map<std::string, int>::const_iterator si =
	  synchronizations.find(*ident1);
	if (si != synchronizations.end()) {
	  /* Synchronization substitution. */
	  int s = synchronization_value(ident2);
	  synch_subst.insert(std::make_pair((*si).second, s));
	} else {
	  yyerror("illegal substitution `" + *ident1 + "=" + *ident2 + "'");
	  delete ident2;
	}
      }
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
  } else {
    const Constant<int>* c = new Constant<int>(*ident, value);
    constants.insert(std::make_pair(*ident, c));
    model->add_constant(*c);
  }
  delete ident;
}


/* Declares a rate constant. */
static void declare_rate(const std::string* ident, double value) {
  if (rates.find(*ident) != rates.end()) {
    yyerror("ignoring repeated declaration of rate `" + *ident + "'");
  } else if (constants.find(*ident) != constants.end()) {
    yyerror("ignoring declaration of rate `" + *ident
	    + "' previously declared as constant");
  } else if (variables.find(*ident) != variables.end()) {
    yyerror("ignoring declaration of rate `" + *ident
	    + "' previously declared as variable");
  } else {
    const Constant<double>* r = new Constant<double>(*ident, value);
    rates.insert(std::make_pair(*ident, r));
    model->add_constant(*r);
  }
  delete ident;
}


/* Declares a variable. */
static const Variable<int>* declare_variable(const std::string* ident,
					     const Range& range,
					     const Expression<int>* init) {
  Variable<int>* v = 0;
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
    std::map<std::string, Variable<int>*>::const_iterator vi =
      variables.find(*ident);
    if (vi != variables.end()) {
      if (undeclared.find(*ident) != undeclared.end()) {
	v = (*vi).second;
	v->set_low(*range.l);
	v->set_high(*range.h);
	if (init != 0) {
	  v->set_init(*init);
	} else {
	  v->set_init(*range.l);
	}
	undeclared.erase(*ident);
      } else {
	yyerror("ignoring repeated declaration of variable `" + *ident + "'");
      }
    } else {
      v = new Variable<int>(*ident, *range.l, *range.h,
			    ((init != 0) ? *init : *range.l));
      variables.insert(std::make_pair(*ident, v));
    }
  }
  if (v != 0) {
    if (module != 0) {
      module->add_variable(*v);
    } else {
      model->add_variable(*v);
    }
  } else {
    RCObject::ref(range.l);
    RCObject::deref(range.l);
    RCObject::ref(range.h);
    RCObject::deref(range.h);
    if (init != 0) {
      RCObject::ref(init);
      RCObject::deref(init);
    }
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
    if (src_module == 0) {
      yyerror("ignoring renaming of undeclared module `" + *ident2 + "'");
    } else {
      for (VariableList<int>::const_iterator vi =
	     src_module->int_variables().begin();
	   vi != src_module->int_variables().end(); vi++) {
	const Variable<int>* vj = subst.find(**vi);
	if (vj == 0) {
	  yyerror("missing substitution for module variable `"
		  + (*vi)->name() + "'");
	} else {
	  std::map<std::string, Variable<int>*>::const_iterator vk =
	    variables.find(vj->name());
	  if (vk == variables.end()) {
	    throw std::logic_error("missing variable `" + vj->name() + "'");
	  }
	  Variable<int>& sv = *(*vk).second;
	  sv.set_low((*vi)->low().substitution(subst));
	  sv.set_high((*vi)->high().substitution(subst));
	  sv.set_init((*vi)->init().substitution(subst));
	}
      }
      Module* mod = &src_module->substitution(*ident1, subst, synch_subst);
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
  module = 0;
}


/* Prepares a module for parsing. */
static void prepare_module(const std::string* ident) {
  std::map<std::string, Module*>::const_iterator mi = modules.find(*ident);
  if (mi != modules.end()) {
    yyerror("ignoring repeated declaration of module `" + *ident + "'");
  } else {
    module = new Module(*ident);
    modules.insert(std::make_pair(*ident, module));
  }
  delete ident;
}


/* Prepares a model for parsing. */
static void prepare_model() {
  if (mode == 2) {
    yyerror("expecting path formula");
  }
  if (model != 0) {
    delete model;
  }
  for (FormulaList::const_iterator fi = properties.begin();
       fi != properties.end(); fi++) {
    StateFormula::deref(*fi);
  }
  properties.clear();
  clear_declarations();
  model = new Model();
}


/* Adds a parsed property. */
static void add_property(const StateFormula* f) {
  if (mode != 0) {
    yyerror("expecting model or path formula");
  }
  properties.push_back(f);
  StateFormula::ref(f);
}


/* Parses a model. */
int parse_model() {
  mode = 1;
  int result = yyparse();
  mode = 0;
  return result;
}


/* Parses a path formula. */
int parse_path_formula() {
  mode = 2;
  RCObject::deref(global_path_formula);
  int result = yyparse();
  RCObject::ref(global_path_formula);
  mode = 0;
  return result;
}

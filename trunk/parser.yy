/* -*-C++-*- */
/*
 * Parser.
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
 */

%{
#include <config.h>
#include "models.h"
#include "distributions.h"
#include "formulas.h"
#include "glog/logging.h"
#include <algorithm>
#include <iostream>
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
/* Constant overrides. */
extern std::map<std::string, TypedValue> const_overrides;

/* Last model parsed. */
const Model* global_model = NULL;
/* Parsed properties. */
std::vector<const StateFormula*> properties;

/* Current model. */
static Model* model;
/* Current module. */
static Module* module;
/* Current variable substitutions. */
static std::map<std::string, std::string> subst;
/* Current synchronization substitutions. */
static std::map<size_t, size_t> synch_subst;
/* Current command. */
static Command* command;
/* Declared integer constants. */
static std::map<std::string, const Variable*> constants;
/* Constant values. */
static std::map<std::string, TypedValue> constant_values;
/* Declared rate constants. */
static std::map<std::string, const Variable*> rates;
/* Rate values. */
static std::map<std::string, TypedValue> rate_values;
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
/* Returns the integer value of the given typed value, signaling an error if
   the type is not INT. */
static int integer_value(const TypedValue* q);
/* Returns a variable representing an integer constant. */
static const Variable* find_constant(const std::string* ident);
/* Returns a variable representing a rate constant. */
static const Variable* find_rate(const std::string* ident);
/* Returns a variable representing a rate constant or an integer variable. */
static const Variable* find_rate_or_variable(const std::string* ident);
/* Returns a range with the given bounds, signaling an error if the
   range is empty. */
static Range make_range(const Expression* l, const Expression* h);
/* Returns a literal expression. */
static const Literal* make_literal(int n);
/* Returns a literal expression. */
static const Literal* make_literal(const TypedValue* q);
/* Returns a constant value or a variable for the given identifier. */
static const Expression* value_or_variable(const std::string* ident);
/* Returns a variable for the given identifier. */
static const Variable* find_variable(const std::string* ident);
/* Returns a conjunction. */
static Conjunction* make_conjunction(StateFormula* f1,
				     const StateFormula* f2);
/* Returns a disjunction. */
static Disjunction* make_disjunction(StateFormula* f1,
				     const StateFormula* f2);
/* Returns a probabilistic path quantification. */
static StateFormula* make_probabilistic(const TypedValue* p,
					bool strict, bool negate,
					const PathFormula* f);
/* Returns an until formula. */
static const Until* make_until(const StateFormula* f1, const StateFormula* f2,
			       const TypedValue* t1, const TypedValue* t2);
/* Adds an update to the current command. */
static void add_update(const std::string* ident, const Expression& expr);
/* Returns the value of the given synchronization. */
static size_t synchronization_value(const std::string* ident);
/* Adds a substitution to the current substitution map. */
static void add_substitution(const std::string* ident1,
			     const std::string* ident2);
/* Declares an integer constant. */
static void declare_constant(const std::string* ident,
                             const Expression* value_expr);
/* Declares a rate constant. */
static void declare_rate(const std::string* ident,
                         const Expression* value_expr);
/* Declares a variable. */
static const Variable* declare_variable(const std::string* ident,
					const Range& range,
					const Expression* start,
					bool delayed_addition = false);
/* Adds a command to the current module. */
static void add_command();
/* Prepares a command for parsing. */
static void prepare_command(int synch, const StateFormula* guard,
			    const Distribution* delay);
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
%}

%token STOCHASTIC CTMC
%token CONST_TOKEN INT_TOKEN DOUBLE RATE GLOBAL INIT
%token TRUE_TOKEN FALSE_TOKEN
%token EXP
%token REWARDS ENDREWARDS
%token MODULE ENDMODULE
%token PNAME NAME LABEL_NAME NUMBER
%token ARROW DOTDOT
%token ILLEGAL_TOKEN

%left IMPLY
%left '&' '|'
%left '!'
%left '<' LTE GTE '>' EQ NEQ
%left '+' '-'
%left '*' '/'

%union {
  size_t synch;
  StateFormula* formula;
  const PathFormula* path;
  const Distribution* dist;
  const Expression* expr;
  Range range;
  int nat;
  const std::string* str;
  const TypedValue* num;
}

%type <synch> synchronization
%type <formula> formula csl_formula
%type <path> path_formula
%type <dist> distribution
%type <expr> expr rate_expr const_rate_expr const_expr csl_expr
%type <range> range
%type <nat> integer
%type <str> PNAME NAME
%type <num> NUMBER

%%

file : { success = true; line_number = 1; } model_or_properties
         { check_undeclared(); if (!success) YYERROR; }
     ;

model_or_properties : model
                    | properties
                    ;


/* ====================================================================== */
/* Model files. */

model : model_type { prepare_model(); } declarations modules rewards
          { compile_model(); }
      ;

model_type : STOCHASTIC | CTMC
           ;

/* ====================================================================== */
/* Declarations. */

declarations : /* empty */
             | declarations declaration
             ;

declaration : CONST_TOKEN NAME ';' { declare_constant($2, NULL); }
            | CONST_TOKEN NAME '=' const_expr ';' { declare_constant($2, $4); }
            | CONST_TOKEN INT_TOKEN NAME ';' { declare_constant($3, NULL); }
            | CONST_TOKEN INT_TOKEN NAME '=' const_expr ';'
                { declare_constant($3, $5); }
            | RATE NAME ';' { declare_rate($2, NULL); }
            | RATE NAME '=' const_rate_expr ';' { declare_rate($2, $4); }
            | CONST_TOKEN DOUBLE NAME ';' { declare_rate($3, NULL); }
            | CONST_TOKEN DOUBLE NAME '=' const_rate_expr ';'
                { declare_rate($3, $5); }
            | GLOBAL NAME ':' range ';' { declare_variable($2, $4, NULL); }
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
            | MODULE NAME '=' NAME '[' substitutions ']' ENDMODULE
                { add_module($2, $4); }
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

variable_decl : NAME ':' range ';' { declare_variable($1, $3, NULL); }
              | NAME ':' range INIT const_expr ';'
                  { declare_variable($1, $3, $5); }
              ;

commands : /* empty */
         | commands command
         ;

command : synchronization formula ARROW distribution ':'
            { prepare_command($1, $2, $4); } update ';' { add_command(); }
        ;

synchronization : '[' ']' { $$ = 0; }
                | '[' NAME ']' { $$ = synchronization_value($2); }
                ;

update : PNAME '=' expr { add_update($1, *$3); }
       | update '&' update
       | '(' update ')'
       ;


/* ====================================================================== */
/* Rewards. */

rewards : /* empty */
        | rewards rewards_decl
        ;

rewards_decl : REWARDS rewards_label reward_rules ENDREWARDS
             ;

rewards_label : /* empty */
              | LABEL_NAME
              ;

reward_rules : /* empty */
             | reward_rules state_reward
             | reward_rules transition_reward
             ;

state_reward : formula ':' rate_expr ';'
                 { delete $1; delete $3; }
             ;

transition_reward : '[' NAME ']' formula ':' rate_expr ';'
                      { delete $2; delete $4; delete $6; }
                  ;


/* ====================================================================== */
/* Formulas. */

formula : TRUE_TOKEN { $$ = new Conjunction(); }
        | FALSE_TOKEN { $$ = new Disjunction(); }
        | formula '&' formula { $$ = make_conjunction($1, $3); }
        | formula '|' formula { $$ = make_disjunction($1, $3); }
        | '!' formula { $$ = new Negation($2); }
        | expr '<' expr { $$ = new LessThan(*$1, *$3); }
        | expr LTE expr { $$ = new LessThanOrEqual(*$1, *$3); }
        | expr GTE expr { $$ = new GreaterThanOrEqual(*$1, *$3); }
        | expr '>' expr { $$ = new GreaterThan(*$1, *$3); }
        | expr '=' expr %prec EQ { $$ = new Equality(*$1, *$3); }
        | expr NEQ expr { $$ = new Inequality(*$1, *$3); }
        | '(' formula ')' { $$ = $2; }
        ;


/* ====================================================================== */
/* Distributions. */

distribution : rate_expr { $$ = Exponential::make(*$1); }
             | EXP '(' rate_expr ')' { $$ = Exponential::make(*$3); }
             | 'W' '(' const_rate_expr ',' const_rate_expr ')'
                 { $$ = Weibull::make(*$3, *$5); }
             | 'L' '(' const_rate_expr ',' const_rate_expr ')'
                 { $$ = Lognormal::make(*$3, *$5); }
             | 'U' '(' const_rate_expr ',' const_rate_expr ')'
                 { $$ = Uniform::make(*$3, *$5); }
             ;

/* ====================================================================== */
/* Expressions. */

expr : integer { $$ = make_literal($1); }
     | NAME { $$ = find_variable($1); }
     | expr '+' expr { $$ = Computation::make(Computation::PLUS, *$1, *$3); }
     | expr '-' expr { $$ = Computation::make(Computation::MINUS, *$1, *$3); }
     | expr '*' expr
         { $$ = Computation::make(Computation::MULTIPLY, *$1, *$3); }
     | '(' expr ')' { $$ = $2; }
     ;

rate_expr : NUMBER { $$ = make_literal($1); }
          | NAME { $$ = find_rate_or_variable($1); }
          | rate_expr '+' rate_expr
              { $$ = Computation::make(Computation::PLUS, *$1, *$3); }
          | rate_expr '-' rate_expr
              { $$ = Computation::make(Computation::MINUS, *$1, *$3); }
          | rate_expr '*' rate_expr
              { $$ = Computation::make(Computation::MULTIPLY, *$1, *$3); }
          | rate_expr '/' rate_expr
              { $$ = Computation::make(Computation::DIVIDE, *$1, *$3); }
          | '(' rate_expr ')' { $$ = $2; }
          ;

const_rate_expr : NUMBER { $$ = make_literal($1); }
                | NAME { $$ = find_rate($1); }
                | const_rate_expr '*' const_rate_expr
                    { $$ = Computation::make(Computation::MULTIPLY, *$1, *$3); }
                | const_rate_expr '/' const_rate_expr
                    { $$ = Computation::make(Computation::DIVIDE, *$1, *$3); }
                | '(' const_rate_expr ')' { $$ = $2; }
                ;


/* ====================================================================== */
/* Ranges and integers. */

range : '[' const_expr DOTDOT const_expr ']' { $$ = make_range($2, $4); }
      ;

const_expr : integer { $$ = make_literal($1); }
           | NAME { $$ = find_constant($1); }
           | const_expr '+' const_expr
               { $$ = Computation::make(Computation::PLUS, *$1, *$3); }
           | const_expr '-' const_expr
               { $$ = Computation::make(Computation::MINUS, *$1, *$3); }
           | const_expr '*' const_expr
               { $$ = Computation::make(Computation::MULTIPLY, *$1, *$3); }
           | '(' const_expr ')' { $$ = $2; }
	   ;

integer : NUMBER { $$ = integer_value($1); }
        ;


/* ====================================================================== */
/* Properties. */

properties : /* empty */
           | properties csl_formula
               { properties.push_back($2); }
           ;

csl_formula : TRUE_TOKEN { $$ = new Conjunction(); }
            | FALSE_TOKEN { $$ = new Disjunction(); }
            | 'P' '<' NUMBER '[' path_formula ']'
                { $$ = make_probabilistic($3, true, true, $5); }
            | 'P' LTE NUMBER '[' path_formula ']'
                { $$ = make_probabilistic($3, false, true, $5); }
            | 'P' GTE NUMBER '[' path_formula ']'
                { $$ = make_probabilistic($3, false, false, $5); }
            | 'P' '>' NUMBER '[' path_formula ']'
                { $$ = make_probabilistic($3, true, false, $5); }
            | csl_formula IMPLY csl_formula { $$ = new Implication($1, $3); }
            | csl_formula '&' csl_formula { $$ = make_conjunction($1, $3); }
            | csl_formula '|' csl_formula { $$ = make_disjunction($1, $3); }
            | '!' csl_formula { $$ = new Negation($2); }
            | csl_expr '<' csl_expr { $$ = new LessThan(*$1, *$3); }
            | csl_expr LTE csl_expr { $$ = new LessThanOrEqual(*$1, *$3); }
            | csl_expr GTE csl_expr { $$ = new GreaterThanOrEqual(*$1, *$3); }
            | csl_expr '>' csl_expr { $$ = new GreaterThan(*$1, *$3); }
            | csl_expr '=' csl_expr %prec EQ { $$ = new Equality(*$1, *$3); }
            | csl_expr NEQ csl_expr { $$ = new Inequality(*$1, *$3); }
            | '(' csl_formula ')' { $$ = $2; }
            ;

path_formula : csl_formula 'U' LTE NUMBER csl_formula
                 { $$ = make_until($1, $5, NULL, $4); }
             | csl_formula 'U' '[' NUMBER ',' NUMBER ']' csl_formula
                 { $$ = make_until($1, $8, $4, $6); }
//             | 'X' csl_formula
             ;

csl_expr : integer { $$ = make_literal($1); }
         | NAME { $$ = value_or_variable($1); }
         | csl_expr '+' csl_expr
             { $$ = Computation::make(Computation::PLUS, *$1, *$3); }
         | csl_expr '-' csl_expr
             { $$ = Computation::make(Computation::MINUS, *$1, *$3); }
         | csl_expr '*' csl_expr
             { $$ = Computation::make(Computation::MULTIPLY, *$1, *$3); }
         | '(' csl_expr ')' { $$ = $2; }
         ;


%%

namespace {

class ConstantExpressionEvaluator : public ExpressionVisitor {
 public:
  explicit ConstantExpressionEvaluator(
      const std::map<std::string, TypedValue>* constant_values);

  TypedValue value() const { return value_; }

 private:
  virtual void DoVisitLiteral(const Literal& expr);
  virtual void DoVisitVariable(const Variable& expr);
  virtual void DoVisitComputation(const Computation& expr);

  const std::map<std::string, TypedValue>* constant_values_;
  TypedValue value_;
};

ConstantExpressionEvaluator::ConstantExpressionEvaluator(
    const std::map<std::string, TypedValue>* constant_values)
    : constant_values_(constant_values), value_(0) {
}

void ConstantExpressionEvaluator::DoVisitLiteral(const Literal& expr) {
  value_ = expr.value();
}

void ConstantExpressionEvaluator::DoVisitVariable(const Variable& expr) {
  std::map<std::string, TypedValue>::const_iterator i =
      constant_values_->find(expr.name());
  CHECK(i != constant_values_->end());
  value_ = i->second;
}

void ConstantExpressionEvaluator::DoVisitComputation(const Computation& expr) {
  expr.operand1().Accept(this);
  TypedValue operand1 = value_;
  expr.operand2().Accept(this);
  switch (expr.op()) {
    case Computation::PLUS:
      value_ = operand1 + value_;
      break;
    case Computation::MINUS:
      value_ = operand1 - value_;
      break;
    case Computation::MULTIPLY:
      value_ = operand1 * value_;
      break;
    case Computation::DIVIDE:
      value_ = operand1 / value_;
      break;
  }
}

TypedValue EvaluateConstantExpression(
    const Expression& expr,
    const std::map<std::string, TypedValue>& constant_values) {
  ConstantExpressionEvaluator evaluator(&constant_values);
  expr.Accept(&evaluator);
  return evaluator.value();
}

bool InVariableSet(
    const Model& model, const std::set<int> variables, const std::string name) {
  for (std::set<int>::const_iterator i = variables.begin();
       i != variables.end(); ++i) {
    if (name == model.variables()[*i].name()) {
      return true;
    }
  }
  return false;
}

}  // namespace

/* Clears all previously parsed declarations. */
void clear_declarations() {
  constants.clear();
  constant_values.clear();
  for (std::map<std::string, Variable*>::const_iterator i = variables.begin();
       i != variables.end(); ++i) {
    Expression::destructive_deref(i->second);
  }
  variables.clear();
}


/* Tests if the given variables is a member of the given list. */
static bool member(const std::vector<const Variable*>& vars,
                   const Variable* v) {
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


static int integer_value(const TypedValue* q) {
  int n;
  if (q->type() != Type::INT) {
    yyerror("expecting integer");
    n = 0;
  } else {
    n = q->value<int>();
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
    Variable* v = new Variable(*ident);
    variables.insert(std::make_pair(*ident, v));
    Expression::ref(v);
    rates.insert(std::make_pair(*ident, v));
    constants.insert(std::make_pair(*ident, v));
    rate_values.insert(std::make_pair(*ident, 0));
    constant_values.insert(std::make_pair(*ident, 0));
    delete ident;
    return v;
  }
}


/* Returns a variable representing a rate constant. */
static const Variable* find_rate(const std::string* ident) {
  std::map<std::string, const Variable*>::const_iterator ri =
    rates.find(*ident);
  if (ri != rates.end()) {
    delete ident;
    return (*ri).second;
  } else {
    if (variables.find(*ident) != variables.end()) {
      yyerror("variable `" + *ident + "' used where expecting rate");
    } else {
      yyerror("undeclared rate `" + *ident + "'");
    }
    Variable* v = new Variable(*ident);
    variables.insert(std::make_pair(*ident, v));
    Expression::ref(v);
    rates.insert(std::make_pair(*ident, v));
    rate_values.insert(std::make_pair(*ident, 0));
    delete ident;
    return v;
  }
}


/* Returns a variable representing a rate constant or an integer variable. */
static const Variable* find_rate_or_variable(const std::string* ident) {
  std::map<std::string, const Variable*>::const_iterator ri =
    rates.find(*ident);
  if (ri != rates.end()) {
    delete ident;
    return (*ri).second;
  } else {
    return find_variable(ident);
  }
}


/* Returns a range with the given bounds. */
static Range make_range(const Expression* l, const Expression* h) {
  Range r = { l, h };
  return r;
}


static const Literal* make_literal(int n) {
  return new Literal(n);
}


static const Literal* make_literal(const TypedValue* q) {
  const Literal* v = new Literal(*q);
  delete q;
  return v;
}


/* Returns a constant value or a variable for the given identifier. */
static const Expression* value_or_variable(const std::string* ident) {
  std::map<std::string, const Variable*>::const_iterator ci =
    constants.find(*ident);
  if (ci != constants.end()) {
    const Literal* value = new Literal(constant_values.find(*ident)->second);
    delete ident;
    return value;
  } else {
    Variable* v;
    std::map<std::string, Variable*>::const_iterator vi =
      variables.find(*ident);
    if (vi == variables.end()) {
      v = new Variable(*ident);
      variables.insert(std::make_pair(*ident, v));
      Expression::ref(v);
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
      v = new Variable(*ident);
      variables.insert(std::make_pair(*ident, v));
      Expression::ref(v);
      undeclared.insert(*ident);
    } else {
      v = (*vi).second;
    }
    delete ident;
    return v;
  }
}


/* Returns a conjunction. */
static Conjunction* make_conjunction(StateFormula* f1,
				     const StateFormula* f2) {
  Conjunction* conj = dynamic_cast<Conjunction*>(f1);
  if (conj == NULL) {
    conj = new Conjunction();
    conj->add_conjunct(f1);
  }
  conj->add_conjunct(f2);
  return conj;
}


/* Returns a disjunction. */
static Disjunction* make_disjunction(StateFormula* f1,
				     const StateFormula* f2) {
  Disjunction* disj = dynamic_cast<Disjunction*>(f1);
  if (disj == NULL) {
    disj = new Disjunction();
    disj->add_disjunct(f1);
  }
  disj->add_disjunct(f2);
  return disj;
}


/* Returns a probabilistic path quantification. */
static StateFormula* make_probabilistic(const TypedValue* p,
					bool strict, bool negate,
					const PathFormula* f) {
  if (*p < 0 || *p > 1) {
    yyerror("probability bound outside the interval [0,1]");
  }
  bool s = (strict && !negate) || (!strict && negate);
  StateFormula* pr = new Probabilistic(*p, s, f);
  if (negate) {
    pr = new Negation(pr);
  }
  delete p;
  return pr;
}


/* Returns an until formula. */
static const Until* make_until(const StateFormula* f1, const StateFormula* f2,
			       const TypedValue* t1, const TypedValue* t2) {
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
    v = new Variable(*ident);
  } else {
    v = (*vi).second;
  }
  const int module_index = model->modules().size();
  if (InVariableSet(*model, model->global_variables(), v->name())) {
    if (command->synch() != 0) {
      yywarning("updating global variable in synchronized command");
    }
  } else if (!InVariableSet(*model, model->module_variables(module_index),
                            v->name())) {
    yyerror("updating variable belonging to other module");
  }
  command->add_update(new Update(*v, expr));
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
  std::map<std::string, Variable*>::const_iterator vi =
    variables.find(*ident1);
  if (vi != variables.end()) {
    /* Variable substitution. */
    subst.insert(std::make_pair(*ident1, *ident2));
    delete ident2;
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
static void declare_constant(const std::string* ident,
                             const Expression* value_expr) {
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
    std::map<std::string, TypedValue>::iterator override =
        const_overrides.find(*ident);
    if (value_expr == NULL && override == const_overrides.end()) {
      yyerror("uninitialized constant `" + *ident + "'");
      value_expr = make_literal(0);
    }
    Variable* v = new Variable(*ident);
    variables.insert(std::make_pair(*ident, v));
    Expression::ref(v);
    rates.insert(std::make_pair(*ident, v));
    constants.insert(std::make_pair(*ident, v));
    const int value =
        ((override != const_overrides.end()) ?
         override->second :
         EvaluateConstantExpression(*value_expr, constant_values)).value<int>();
    rate_values.insert(std::make_pair(*ident, value));
    constant_values.insert(std::make_pair(*ident, value));
  }
  delete ident;
  delete value_expr;
}


/* Declares a rate constant. */
static void declare_rate(const std::string* ident,
                         const Expression* value_expr) {
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
    std::map<std::string, TypedValue>::iterator override =
        const_overrides.find(*ident);
    if (value_expr == NULL && override == const_overrides.end()) {
      yyerror("uninitialized rate `" + *ident + "'");
      value_expr = make_literal(0);
    }
    Variable* v = new Variable(*ident);
    variables.insert(std::make_pair(*ident, v));
    Expression::ref(v);
    rates.insert(std::make_pair(*ident, v));
    const TypedValue value = (override != const_overrides.end()) ?
        override->second : EvaluateConstantExpression(*value_expr, rate_values);
    rate_values.insert(std::make_pair(*ident, value));
  }
  delete ident;
  delete value_expr;
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
      v = (*vi).second;
      if (undeclared.find(*ident) != undeclared.end()) {
	undeclared.erase(*ident);
      } else {
	yyerror("ignoring repeated declaration of variable `" + *ident + "'");
      }
    } else {
      v = new Variable(*ident);
      variables.insert(std::make_pair(*ident, v));
      Expression::ref(v);
    }
  }
  if (v != NULL) {
    int low =
        EvaluateConstantExpression(*range.l, constant_values).value<int>();
    int high =
        EvaluateConstantExpression(*range.h, constant_values).value<int>();
    int s = ((start != NULL)
             ? EvaluateConstantExpression(*start, constant_values).value<int>()
             : low);
    v->SetVariableProperties(model->variables().size());
    variable_lows.insert(std::make_pair(v, range.l));
    Expression::ref(range.l);
    variable_highs.insert(std::make_pair(v, range.h));
    Expression::ref(range.h);
    variable_starts.insert(std::make_pair(v, start));
    Expression::ref(start);
    model->AddIntVariable(*ident, low, high, s);
    if (!delayed_addition) {
      if (module != NULL) {
	module->add_variable(*v);
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
  module->add_command(command);
}


/* Prepares a command for parsing. */
static void prepare_command(int synch, const StateFormula* guard,
			    const Distribution* delay) {
  command = new Command(synch, guard, delay);
}

namespace {

const Variable* SubstituteVariable(
    const Variable* variable,
    const std::map<std::string, const Variable*>& substitutions) {
  std::map<std::string, const Variable*>::const_iterator i =
      substitutions.find(variable->name());
  return (i == substitutions.end()) ? variable : i->second;
}

class ExpressionIdentifierSubstituter : public ExpressionVisitor {
 public:
  explicit ExpressionIdentifierSubstituter(
      const std::map<std::string, const Variable*>* substitutions);

  ~ExpressionIdentifierSubstituter();

  const Expression* release_expr();

 private:
  virtual void DoVisitLiteral(const Literal& expr);
  virtual void DoVisitVariable(const Variable& expr);
  virtual void DoVisitComputation(const Computation& expr);

  const std::map<std::string, const Variable*>* substitutions_;
  const Expression* expr_;
};

const Expression* SubstituteIdentifiers(
    const Expression& expr,
    const std::map<std::string, const Variable*>& substitutions) {
  ExpressionIdentifierSubstituter substituter(&substitutions);
  expr.Accept(&substituter);
  return substituter.release_expr();
}

class StateFormulaIdentifierSubstituter : public StateFormulaVisitor {
 public:
  explicit StateFormulaIdentifierSubstituter(
      const std::map<std::string, const Variable*>* substitutions);

  ~StateFormulaIdentifierSubstituter();

  const StateFormula* release_formula();

 private:
  virtual void DoVisitConjunction(const Conjunction& formula);
  virtual void DoVisitDisjunction(const Disjunction& formula);
  virtual void DoVisitNegation(const Negation& formula);
  virtual void DoVisitImplication(const Implication& formula);
  virtual void DoVisitProbabilistic(const Probabilistic& formula);
  virtual void DoVisitComparison(const Comparison& formula);

  const std::map<std::string, const Variable*>* substitutions_;
  const StateFormula* formula_;
};

const StateFormula* SubstituteIdentifiers(
    const StateFormula& formula,
    const std::map<std::string, const Variable*>& substitutions) {
  StateFormulaIdentifierSubstituter substituter(&substitutions);
  formula.Accept(&substituter);
  return substituter.release_formula();
}

class PathFormulaIdentifierSubstituter : public PathFormulaVisitor {
 public:
  explicit PathFormulaIdentifierSubstituter(
      const std::map<std::string, const Variable*>* substitutions);

  ~PathFormulaIdentifierSubstituter();

  const PathFormula* release_formula();

 private:
  virtual void DoVisitUntil(const Until& formula);

  const std::map<std::string, const Variable*>* substitutions_;
  const PathFormula* formula_;
};

const PathFormula* SubstituteIdentifiers(
    const PathFormula& formula,
    const std::map<std::string, const Variable*>& substitutions) {
  PathFormulaIdentifierSubstituter substituter(&substitutions);
  formula.Accept(&substituter);
  return substituter.release_formula();
}

class DistributionIdentifierSubstituter : public DistributionVisitor {
 public:
  explicit DistributionIdentifierSubstituter(
      const std::map<std::string, const Variable*>* substitutions);

  ~DistributionIdentifierSubstituter();

  const Distribution* release_distribution();

 private:
  virtual void DoVisitExponential(const Exponential& distribution);
  virtual void DoVisitWeibull(const Weibull& distribution);
  virtual void DoVisitLognormal(const Lognormal& distribution);
  virtual void DoVisitUniform(const Uniform& distribution);

  const std::map<std::string, const Variable*>* substitutions_;
  const Distribution* distribution_;
};

const Distribution* SubstituteIdentifiers(
    const Distribution& distribution,
    const std::map<std::string, const Variable*>& substitutions) {
  DistributionIdentifierSubstituter substituter(&substitutions);
  distribution.Accept(&substituter);
  return substituter.release_distribution();
}

const Update* SubstituteIdentifiers(
    const Update& update,
    const std::map<std::string, const Variable*>& substitutions) {
  return new Update(*SubstituteVariable(&update.variable(), substitutions),
                    *SubstituteIdentifiers(update.expr(), substitutions));
}

const Command* SubstituteIdentifiers(
    const Command& command,
    const std::map<std::string, const Variable*>& substitutions,
    const std::map<size_t, size_t>& synchs) {
  size_t s;
  std::map<size_t, size_t>::const_iterator si = synchs.find(command.synch());
  if (si == synchs.end()) {
    s = command.synch();
  } else {
    s = (*si).second;
  }
  Command* subst_comm = new Command(
      s,
      SubstituteIdentifiers(command.guard(), substitutions),
      SubstituteIdentifiers(command.delay(), substitutions));
  for (const Update* update : command.updates()) {
    subst_comm->add_update(SubstituteIdentifiers(*update, substitutions));
  }
  return subst_comm;
}

Module* SubstituteIdentifiers(
    const Module& module,
    const std::map<std::string, const Variable*>& substitutions,
    const std::map<size_t, size_t>& syncs) {
  Module* subst_mod = new Module();
  for (std::vector<const Variable*>::const_iterator vi =
           module.variables().begin();
       vi != module.variables().end(); ++vi) {
    subst_mod->add_variable(*SubstituteVariable(*vi, substitutions));
  }
  for (const Command* command : module.commands()) {
    subst_mod->add_command(
        SubstituteIdentifiers(*command, substitutions, syncs));
  }
  return subst_mod;
}

ExpressionIdentifierSubstituter::ExpressionIdentifierSubstituter(
    const std::map<std::string, const Variable*>* substitutions)
    : substitutions_(substitutions), expr_(NULL) {
}

ExpressionIdentifierSubstituter::~ExpressionIdentifierSubstituter() {
  Expression::ref(expr_);
  Expression::destructive_deref(expr_);
}

const Expression* ExpressionIdentifierSubstituter::release_expr() {
  const Expression* expr = expr_;
  expr_ = NULL;
  return expr;
}

void ExpressionIdentifierSubstituter::DoVisitLiteral(const Literal& expr) {
  expr_ = new Literal(expr.value());
}

void ExpressionIdentifierSubstituter::DoVisitVariable(const Variable& expr) {
  std::map<std::string, const Variable*>::const_iterator i =
      substitutions_->find(expr.name());
  if (i == substitutions_->end()) {
    // TODO(hlsyounes): Make copy once Variable* does not represent identity.
    expr_ = &expr;
  } else {
    expr_ = i->second;
  }
}

void ExpressionIdentifierSubstituter::DoVisitComputation(
    const Computation& expr) {
  expr.operand1().Accept(this);
  const Expression* operand1 = release_expr();
  expr.operand2().Accept(this);
  expr_ = Computation::make(expr.op(), *operand1, *release_expr());
}

StateFormulaIdentifierSubstituter::StateFormulaIdentifierSubstituter(
    const std::map<std::string, const Variable*>* substitutions)
    : substitutions_(substitutions), formula_(NULL) {
}

StateFormulaIdentifierSubstituter::~StateFormulaIdentifierSubstituter() {
  delete formula_;
}

const StateFormula* StateFormulaIdentifierSubstituter::release_formula() {
  const StateFormula* formula = formula_;
  formula_ = NULL;
  return formula;
}

void StateFormulaIdentifierSubstituter::DoVisitConjunction(
    const Conjunction& formula) {
  Conjunction* conjunction = new Conjunction();
  for (const StateFormula* conjunct : formula.conjuncts()) {
    conjunct->Accept(this);
    conjunction->add_conjunct(release_formula());
  }
  formula_ = conjunction;
}

void StateFormulaIdentifierSubstituter::DoVisitDisjunction(
    const Disjunction& formula) {
  Disjunction* disjunction = new Disjunction();
  for (const StateFormula* disjunct : formula.disjuncts()) {
    disjunct->Accept(this);
    disjunction->add_disjunct(release_formula());
  }
  formula_ = disjunction;
}

void StateFormulaIdentifierSubstituter::DoVisitNegation(
    const Negation& formula) {
  formula.negand().Accept(this);
  formula_ = new Negation(release_formula());
}

void StateFormulaIdentifierSubstituter::DoVisitImplication(
    const Implication& formula) {
  formula.antecedent().Accept(this);
  const StateFormula* antecedent = release_formula();
  formula.consequent().Accept(this);
  formula_ = new Implication(antecedent, release_formula());
}

void StateFormulaIdentifierSubstituter::DoVisitProbabilistic(
    const Probabilistic& formula) {
  formula_ = new Probabilistic(
      formula.threshold(), formula.strict(),
      SubstituteIdentifiers(formula.formula(), *substitutions_));
}

void StateFormulaIdentifierSubstituter::DoVisitComparison(
    const Comparison& formula) {
  switch (formula.op()) {
    case Comparison::LESS:
      formula_ = new LessThan(
          *SubstituteIdentifiers(formula.expr1(), *substitutions_),
          *SubstituteIdentifiers(formula.expr2(), *substitutions_));
      break;
    case Comparison::LESS_EQUAL:
      formula_ = new LessThanOrEqual(
          *SubstituteIdentifiers(formula.expr1(), *substitutions_),
          *SubstituteIdentifiers(formula.expr2(), *substitutions_));
      break;
    case Comparison::GREATER_EQUAL:
      formula_ = new GreaterThanOrEqual(
          *SubstituteIdentifiers(formula.expr1(), *substitutions_),
          *SubstituteIdentifiers(formula.expr2(), *substitutions_));
      break;
    case Comparison::GREATER:
      formula_ = new GreaterThan(
          *SubstituteIdentifiers(formula.expr1(), *substitutions_),
          *SubstituteIdentifiers(formula.expr2(), *substitutions_));
      break;
    case Comparison::EQUAL:
      formula_ = new Equality(
          *SubstituteIdentifiers(formula.expr1(), *substitutions_),
          *SubstituteIdentifiers(formula.expr2(), *substitutions_));
      break;
    case Comparison::NOT_EQUAL:
      formula_ = new Inequality(
          *SubstituteIdentifiers(formula.expr1(), *substitutions_),
          *SubstituteIdentifiers(formula.expr2(), *substitutions_));
      break;
  }
}

PathFormulaIdentifierSubstituter::PathFormulaIdentifierSubstituter(
    const std::map<std::string, const Variable*>* substitutions)
    : substitutions_(substitutions), formula_(NULL) {
}

PathFormulaIdentifierSubstituter::~PathFormulaIdentifierSubstituter() {
  delete formula_;
}

const PathFormula* PathFormulaIdentifierSubstituter::release_formula() {
  const PathFormula* formula = formula_;
  formula_ = NULL;
  return formula;
}

void PathFormulaIdentifierSubstituter::DoVisitUntil(const Until& formula) {
  formula_ = new Until(SubstituteIdentifiers(formula.pre(), *substitutions_),
                       SubstituteIdentifiers(formula.post(), *substitutions_),
                       formula.min_time(), formula.max_time());
}

DistributionIdentifierSubstituter::DistributionIdentifierSubstituter(
    const std::map<std::string, const Variable*>* substitutions)
    : substitutions_(substitutions), distribution_(NULL) {
}

DistributionIdentifierSubstituter::~DistributionIdentifierSubstituter() {
  delete distribution_;
}

const Distribution* DistributionIdentifierSubstituter::release_distribution() {
  const Distribution* distribution = distribution_;
  distribution_ = NULL;
  return distribution;
}

void DistributionIdentifierSubstituter::DoVisitExponential(
    const Exponential& distribution) {
  distribution_ = Exponential::make(
      *SubstituteIdentifiers(distribution.rate(), *substitutions_));
}

void DistributionIdentifierSubstituter::DoVisitWeibull(
    const Weibull& distribution) {
  distribution_ = Weibull::make(
      *SubstituteIdentifiers(distribution.scale(), *substitutions_),
      *SubstituteIdentifiers(distribution.shape(), *substitutions_));
}

void DistributionIdentifierSubstituter::DoVisitLognormal(
    const Lognormal& distribution) {
  distribution_ = Lognormal::make(
      *SubstituteIdentifiers(distribution.scale(), *substitutions_),
      *SubstituteIdentifiers(distribution.shape(), *substitutions_));
}

void DistributionIdentifierSubstituter::DoVisitUniform(
    const Uniform& distribution) {
  distribution_ = Uniform::make(
      *SubstituteIdentifiers(distribution.low(), *substitutions_),
      *SubstituteIdentifiers(distribution.high(), *substitutions_));
}

}  // namespace

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
      std::map<std::string, const Variable*> c_subst;
      for (std::map<std::string, const Variable*>::const_iterator ci =
               constants.begin();
	   ci != constants.end(); ci++) {
	std::map<std::string, std::string>::const_iterator si =
            subst.find(ci->first);
	if (si != subst.end()) {
	  std::map<std::string, const Variable*>::const_iterator cj =
	    constants.find(si->second);
	  if (cj != constants.end()) {
	    c_subst.insert(std::make_pair(ci->first, cj->second));
	  } else {
	    yyerror("substituting constant with non-constant " + si->second);
	  }
	}
      }
      std::map<std::string, const Variable*> v_subst;
      model->OpenModuleScope();
      for (std::vector<const Variable*>::const_iterator vi =
               src_module.variables().begin();
	   vi != src_module.variables().end(); vi++) {
	std::map<std::string, std::string>::const_iterator si =
            subst.find((*vi)->name());
	if (si == subst.end()) {
	  yyerror("missing substitution for module variable");
	} else {
	  const Expression* low =
              SubstituteIdentifiers(*variable_lows.find(*vi)->second, c_subst);
	  const Expression* high =
              SubstituteIdentifiers(*variable_highs.find(*vi)->second, c_subst);
	  const Expression* start;
	  std::map<const Variable*, const Expression*>::const_iterator i =
	    variable_starts.find(*vi);
	  if ((*i).second != NULL) {
	    start = SubstituteIdentifiers(*i->second, c_subst);
	  } else {
	    start = NULL;
	  }
	  Range r = { low, high };
	  const Variable* v =
              declare_variable(new std::string(si->second), r, start, true);
	  if (v != NULL) {
	    v_subst.insert(std::make_pair((*vi)->name(), v));
	  }
	}
      }
      for (std::map<std::string, std::string>::const_iterator si =
               subst.begin();
	   si != subst.end(); si++) {
	const Variable* v1 = find_variable(new std::string(si->first));
	if (!member(src_module.variables(), v1)) {
	  const Variable* v2 = find_variable(new std::string(si->second));
          v_subst.insert(std::make_pair(v1->name(), v2));
	}
      }
      Module* mod = SubstituteIdentifiers(src_module, v_subst, synch_subst);
      modules.insert(std::make_pair(*ident1, mod));
      model->add_module(*mod);
      model->CloseModuleScope();
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
  model->CloseModuleScope();
}


/* Prepares a module for parsing. */
static void prepare_module(const std::string* ident) {
  std::map<std::string, Module*>::const_iterator mi = modules.find(*ident);
  if (mi != modules.end()) {
    yyerror("ignoring repeated declaration of module `" + *ident + "'");
  } else {
    module = new Module();
    modules.insert(std::make_pair(*ident, module));
    model->OpenModuleScope();
  }
  delete ident;
}


/* Prepares a model for parsing. */
static void prepare_model() {
  clear_declarations();
  for (const StateFormula* property : properties) {
    delete property;
  }
  properties.clear();
  if (model != NULL) {
    delete model;
  }
  model = new Model();
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

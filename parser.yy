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

#include "parser.h"

// Lexical analyzer function.
extern int yylex(YYSTYPE* lvalp, YYLTYPE* llocp, void* scanner);
/* Name of current file. */
extern std::string current_file;
/* Constant overrides. */
extern std::map<std::string, TypedValue> const_overrides;

/* Last model parsed. */
const Model* global_model = nullptr;
/* Parsed properties. */
std::vector<const StateFormula*> properties;

/* Current model. */
static Model* model;
/* Current module. */
static Module* module;
/* Current identifier substitutions. */
static std::map<std::string, std::string> subst;
/* Current synchronization substitutions. */
static std::map<size_t, size_t> synch_subst;
/* Current command. */
static Command* command;
/* Declared integer constants. */
static std::set<std::string> constants;
/* Constant values. */
static std::map<std::string, TypedValue> constant_values;
/* Declared rate constants. */
static std::set<std::string> rates;
/* Rate values. */
static std::map<std::string, TypedValue> rate_values;
/* All state variables. */
static std::set<std::string> variables;
/* Variables lows. */
static std::map<std::string, std::unique_ptr<const Expression>> variable_lows;
/* Variables highs. */
static std::map<std::string, std::unique_ptr<const Expression>> variable_highs;
/* Variables starts. */
static std::map<std::string, std::unique_ptr<const Expression>> variable_starts;
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

/* Checks if undeclared variables were used. */
static void check_undeclared();
/* Returns the integer value of the given typed value, signaling an error if
   the type is not INT. */
static int integer_value(const TypedValue* q);
/* Returns an identifier representing an integer constant. */
static const Identifier* find_constant(const std::string* ident);
/* Returns an identifier representing a rate constant. */
static const Identifier* find_rate(const std::string* ident);
/* Returns an identifier representing a rate constant or an integer variable. */
static const Identifier* find_rate_or_variable(const std::string* ident);
/* Returns a literal expression. */
static const Literal* make_literal(int n);
/* Returns a literal expression. */
static const Literal* make_literal(const TypedValue* q);
/* Returns a constant value or a variable for the given identifier. */
static const Expression* value_or_variable(const std::string* ident);
/* Returns a variable for the given identifier. */
static const Identifier* find_variable(const std::string* ident);
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
static void add_update(const std::string* ident, const Expression* expr);
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
static bool declare_variable(const std::string* ident,
                             const Expression* low,
                             const Expression* high,
                             const Expression* start,
                             bool delayed_addition = false);
/* Adds a command to the current module. */
static void add_command();
/* Prepares a command for parsing. */
static void prepare_command(int synch,
                            std::unique_ptr<const Expression>&& guard,
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

namespace {

// Wrapper for lexical analyzer function called by generated code.
int yylex(void* scanner) {
  return yylex(&yylval, &yylloc, scanner);
}

// Error reporting function.
void yyerror(const std::string& msg) {
  std::cerr << PACKAGE ":" << current_file << ':' << yylloc.first_line << ':'
            << msg << std::endl;
  success = false;
}

// Wrapper for error reporting function called by generated code.
void yyerror(void* scanner, const std::string& msg) {
  yyerror(msg);
}

void yywarning(const std::string& msg) {
  std::cerr << PACKAGE ":" << current_file << ':' << yylloc.first_line << ':'
            << msg << std::endl;
}

template <typename T>
std::unique_ptr<T> MakeUnique(T* ptr) {
  return std::unique_ptr<T>(ptr);
}

Function MakeFunction(const std::string& name) {
  if (name == "min") {
    return Function::MIN;
  } else if (name == "max") {
    return Function::MAX;
  } else if (name == "floor") {
    return Function::FLOOR;
  } else if (name == "ceil") {
    return Function::CEIL;
  } else if (name == "pow") {
    return Function::POW;
  } else if (name == "log") {
    return Function::LOG;
  } else if (name == "mod") {
    return Function::MOD;
  } else {
    yyerror("unknown function");
    return Function::UNKNOWN;
  }
}

const UnaryOperation* NewNegate(std::unique_ptr<const Expression>&& operand) {
  return new UnaryOperation(UnaryOperator::NEGATE, std::move(operand));
}

const UnaryOperation* NewNot(std::unique_ptr<const Expression>&& operand) {
  return new UnaryOperation(UnaryOperator::NOT, std::move(operand));
}

const BinaryOperation* NewPlus(std::unique_ptr<const Expression>&& operand1,
                               std::unique_ptr<const Expression>&& operand2) {
  return new BinaryOperation(BinaryOperator::PLUS,
                             std::move(operand1), std::move(operand2));
}

const BinaryOperation* NewMinus(std::unique_ptr<const Expression>&& operand1,
                                std::unique_ptr<const Expression>&& operand2) {
  return new BinaryOperation(BinaryOperator::MINUS,
                             std::move(operand1), std::move(operand2));
}

const BinaryOperation* NewMultiply(
    std::unique_ptr<const Expression>&& operand1,
    std::unique_ptr<const Expression>&& operand2) {
  return new BinaryOperation(BinaryOperator::MULTIPLY,
                             std::move(operand1), std::move(operand2));
}

const BinaryOperation* NewDivide(std::unique_ptr<const Expression>&& operand1,
                                 std::unique_ptr<const Expression>&& operand2) {
  return new BinaryOperation(BinaryOperator::DIVIDE,
                             std::move(operand1), std::move(operand2));
}

const BinaryOperation* NewAnd(std::unique_ptr<const Expression>&& operand1,
                              std::unique_ptr<const Expression>&& operand2) {
  return new BinaryOperation(BinaryOperator::AND,
                             std::move(operand1), std::move(operand2));
}

const BinaryOperation* NewOr(std::unique_ptr<const Expression>&& operand1,
                             std::unique_ptr<const Expression>&& operand2) {
  return new BinaryOperation(BinaryOperator::OR,
                             std::move(operand1), std::move(operand2));
}

const BinaryOperation* NewImply(std::unique_ptr<const Expression>&& operand1,
                                std::unique_ptr<const Expression>&& operand2) {
  return new BinaryOperation(BinaryOperator::IMPLY,
                             std::move(operand1), std::move(operand2));
}

const BinaryOperation* NewIff(std::unique_ptr<const Expression>&& operand1,
                              std::unique_ptr<const Expression>&& operand2) {
  return new BinaryOperation(BinaryOperator::IFF,
                             std::move(operand1), std::move(operand2));
}

const BinaryOperation* NewLess(std::unique_ptr<const Expression>&& operand1,
                               std::unique_ptr<const Expression>&& operand2) {
  return new BinaryOperation(BinaryOperator::LESS,
                             std::move(operand1), std::move(operand2));
}

const BinaryOperation* NewLessEqual(
    std::unique_ptr<const Expression>&& operand1,
    std::unique_ptr<const Expression>&& operand2) {
  return new BinaryOperation(BinaryOperator::LESS_EQUAL,
                             std::move(operand1), std::move(operand2));
}

const BinaryOperation* NewGreaterEqual(
    std::unique_ptr<const Expression>&& operand1,
    std::unique_ptr<const Expression>&& operand2) {
  return new BinaryOperation(BinaryOperator::GREATER_EQUAL,
                             std::move(operand1), std::move(operand2));
}

const BinaryOperation* NewGreater(
    std::unique_ptr<const Expression>&& operand1,
    std::unique_ptr<const Expression>&& operand2) {
  return new BinaryOperation(BinaryOperator::GREATER,
                             std::move(operand1), std::move(operand2));
}

const BinaryOperation* NewEqual(std::unique_ptr<const Expression>&& operand1,
                                std::unique_ptr<const Expression>&& operand2) {
  return new BinaryOperation(BinaryOperator::EQUAL,
                             std::move(operand1), std::move(operand2));
}

const BinaryOperation* NewNotEqual(
    std::unique_ptr<const Expression>&& operand1,
    std::unique_ptr<const Expression>&& operand2) {
  return new BinaryOperation(BinaryOperator::NOT_EQUAL,
                             std::move(operand1), std::move(operand2));
}

LessThan* NewLessThan(std::unique_ptr<const Expression>&& expr1,
                      std::unique_ptr<const Expression>&& expr2) {
  return new LessThan(std::move(expr1), std::move(expr2));
}

LessThanOrEqual* NewLessThanOrEqual(std::unique_ptr<const Expression>&& expr1,
                                    std::unique_ptr<const Expression>&& expr2) {
  return new LessThanOrEqual(std::move(expr1), std::move(expr2));
}

GreaterThanOrEqual* NewGreaterThanOrEqual(
    std::unique_ptr<const Expression>&& expr1,
    std::unique_ptr<const Expression>&& expr2) {
  return new GreaterThanOrEqual(std::move(expr1), std::move(expr2));
}

GreaterThan* NewGreaterThan(std::unique_ptr<const Expression>&& expr1,
                            std::unique_ptr<const Expression>&& expr2) {
  return new GreaterThan(std::move(expr1), std::move(expr2));
}

Equality* NewEquality(std::unique_ptr<const Expression>&& expr1,
                      std::unique_ptr<const Expression>&& expr2) {
  return new Equality(std::move(expr1), std::move(expr2));
}

Inequality* NewInequality(std::unique_ptr<const Expression>&& expr1,
                          std::unique_ptr<const Expression>&& expr2) {
  return new Inequality(std::move(expr1), std::move(expr2));
}

const Exponential* NewExponential(std::unique_ptr<const Expression>&& rate) {
  return Exponential::make(std::move(rate));
}

const Distribution* NewWeibull(std::unique_ptr<const Expression>&& scale,
                               std::unique_ptr<const Expression>&& shape) {
  return Weibull::make(std::move(scale), std::move(shape));
}

const Lognormal* NewLognormal(std::unique_ptr<const Expression>&& scale,
                              std::unique_ptr<const Expression>&& shape) {
  return Lognormal::make(std::move(scale), std::move(shape));
}

const Uniform* NewUniform(std::unique_ptr<const Expression>&& low,
                          std::unique_ptr<const Expression>&& high) {
  return Uniform::make(std::move(low), std::move(high));
}

}  // namespace
%}

%defines
%locations
%lex-param {void* scanner}
%parse-param {void* scanner}
%error-verbose

%token DTMC_TOKEN CTMC_TOKEN MDP_TOKEN PROBABILISTIC STOCHASTIC NONDETERMINISTIC
%token CONST INT_TOKEN DOUBLE_TOKEN BOOL_TOKEN RATE PROB
%token GLOBAL DOTDOT
%token FORMULA LABEL
%token INIT ENDINIT
%token REWARDS ENDREWARDS
%token MODULE ENDMODULE
%token ARROW PRIME
%token TRUE FALSE
%token MAX_TOKEN MIN_TOKEN FUNC
%token IDENTIFIER NUMBER LABEL_NAME
%token SYSTEM ENDSYSTEM
%token DOUBLE_BAR TRIPLE_BAR BACK_ARROW

%token A C
%token E F G I
%token PMAX PMIN P RMAX
%token RMIN R S U W X EXP L

%left DOUBLE_BAR TRIPLE_BAR
%right '?' ':'
%left IFF_TOKEN
%left IMPLY_TOKEN
%left '|'
%left '&'
%left '=' NEQ
%left '<' LEQ GEQ '>'
%left '+' '-'
%left '*' '/'
%left '{'
%right UMINUS '!'

%union {
  size_t synch;
  StateFormula* formula;
  const PathFormula* path;
  const Distribution* dist;
  const Expression* expr;
  int nat;
  const std::string* str;
  const TypedValue* number;
  Function function;
  UniquePtrVector<const Expression>* arguments;
}

%type <synch> synchronization
%type <formula> csl_formula
%type <path> path_formula
%type <dist> distribution
%type <expr> expr rate_expr const_rate_expr const_expr csl_expr
%type <nat> integer
%type <str> IDENTIFIER
%type <number> NUMBER
%type <function> function
%type <arguments> arguments

%destructor { delete $$; } <expr>
%destructor { delete $$; } <number>
%destructor { delete $$; } <arguments>

%%

file : { success = true; } model_or_properties
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

model_type : STOCHASTIC | CTMC_TOKEN
           ;

/* ====================================================================== */
/* Declarations. */

declarations : /* empty */
             | declarations declaration
             ;

declaration : CONST IDENTIFIER ';'
                { declare_constant($2, nullptr); }
            | CONST IDENTIFIER '=' const_expr ';'
                { declare_constant($2, $4); }
            | CONST INT_TOKEN IDENTIFIER ';'
                { declare_constant($3, nullptr); }
            | CONST INT_TOKEN IDENTIFIER '=' const_expr ';'
                { declare_constant($3, $5); }
            | RATE IDENTIFIER ';'
                { declare_rate($2, nullptr); }
            | RATE IDENTIFIER '=' const_rate_expr ';'
                { declare_rate($2, $4); }
            | CONST DOUBLE_TOKEN IDENTIFIER ';'
                { declare_rate($3, nullptr); }
            | CONST DOUBLE_TOKEN IDENTIFIER '=' const_rate_expr ';'
                { declare_rate($3, $5); }
            | GLOBAL IDENTIFIER ':' '[' const_expr DOTDOT const_expr ']' ';'
                { declare_variable($2, $5, $7, nullptr); }
            | GLOBAL IDENTIFIER ':' '[' const_expr DOTDOT const_expr ']'
                INIT const_expr ';'
                { declare_variable($2, $5, $7, $10); }
            ;


/* ====================================================================== */
/* Modules. */

modules : /* empty */
        | modules module_decl
        ;

module_decl : MODULE IDENTIFIER { prepare_module($2); } variables commands
              ENDMODULE
                { add_module(); }
            | MODULE IDENTIFIER '=' IDENTIFIER '[' substitutions ']' ENDMODULE
                { add_module($2, $4); }
            ;

substitutions : /* empty */
              | subst_list
              ;

subst_list : subst
           | subst_list ',' subst
           ;

subst : IDENTIFIER '=' IDENTIFIER { add_substitution($1, $3); }
      ;

variables : /* empty */
          | variables variable_decl
          ;

variable_decl : IDENTIFIER ':' '[' const_expr DOTDOT const_expr ']' ';'
                  { declare_variable($1, $4, $6, nullptr); }
              | IDENTIFIER ':' '[' const_expr DOTDOT const_expr ']'
                  INIT const_expr ';'
                  { declare_variable($1, $4, $6, $9); }
              ;

commands : /* empty */
         | commands command
         ;

command : synchronization expr ARROW distribution ':'
            { prepare_command($1, MakeUnique($2), $4); } update ';'
            { add_command(); }
        ;

synchronization : '[' ']' { $$ = 0; }
                | '[' IDENTIFIER ']' { $$ = synchronization_value($2); }
                ;

update : IDENTIFIER PRIME '=' expr { add_update($1, $4); }
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

state_reward : expr ':' rate_expr ';'
                 { delete $1; delete $3; }
             ;

transition_reward : '[' IDENTIFIER ']' expr ':' rate_expr ';'
                      { delete $2; delete $4; delete $6; }
                  ;


/* ====================================================================== */
/* Distributions. */

distribution : rate_expr
                 { $$ = NewExponential(MakeUnique($1)); }
             | EXP '(' rate_expr ')'
                 { $$ = NewExponential(MakeUnique($3)); }
             | W '(' const_rate_expr ',' const_rate_expr ')'
                 { $$ = NewWeibull(MakeUnique($3), MakeUnique($5)); }
             | L '(' const_rate_expr ',' const_rate_expr ')'
                 { $$ = NewLognormal(MakeUnique($3), MakeUnique($5)); }
             | U '(' const_rate_expr ',' const_rate_expr ')'
                 { $$ = NewUniform(MakeUnique($3), MakeUnique($5)); }
             ;

/* ====================================================================== */
/* Expressions. */

expr : NUMBER
         { $$ = new Literal(*$1); delete $1; }
     | TRUE
         { $$ = new Literal(true); }
     | FALSE
         { $$ = new Literal(true); }
     | IDENTIFIER
         { $$ = find_variable($1); }
     | function '(' arguments ')'
         { $$ = new FunctionCall($1, std::move(*$3)); delete $3; }
     | FUNC '(' function ',' arguments ')'
         { $$ = new FunctionCall($3, std::move(*$5)); delete $5; }
     | '-' expr %prec UMINUS
         { $$ = NewNegate(MakeUnique($2)); }
     | '!' expr
         { $$ = NewNot(MakeUnique($2)); }
     | expr '+' expr
         { $$ = NewPlus(MakeUnique($1), MakeUnique($3)); }
     | expr '-' expr
         { $$ = NewMinus(MakeUnique($1), MakeUnique($3)); }
     | expr '*' expr
         { $$ = NewMultiply(MakeUnique($1), MakeUnique($3)); }
     | expr '/' expr
         { $$ = NewDivide(MakeUnique($1), MakeUnique($3)); }
     | expr '&' expr
         { $$ = NewAnd(MakeUnique($1), MakeUnique($3)); }
     | expr '|' expr
         { $$ = NewOr(MakeUnique($1), MakeUnique($3)); }
     | expr IMPLY_TOKEN expr
         { $$ = NewImply(MakeUnique($1), MakeUnique($3)); }
     | expr IFF_TOKEN expr
         { $$ = NewIff(MakeUnique($1), MakeUnique($3)); }
     | expr '<' expr
         { $$ = NewLess(MakeUnique($1), MakeUnique($3)); }
     | expr LEQ expr
         { $$ = NewLessEqual(MakeUnique($1), MakeUnique($3)); }
     | expr GEQ expr
         { $$ = NewGreaterEqual(MakeUnique($1), MakeUnique($3)); }
     | expr '>' expr
         { $$ = NewGreater(MakeUnique($1), MakeUnique($3)); }
     | expr '=' expr
         { $$ = NewEqual(MakeUnique($1), MakeUnique($3)); }
     | expr NEQ expr
         { $$ = NewNotEqual(MakeUnique($1), MakeUnique($3)); }
     | '(' expr ')'
         { $$ = $2; }
     ;

rate_expr : NUMBER
              { $$ = make_literal($1); }
          | IDENTIFIER
              { $$ = find_rate_or_variable($1); }
          | '-' rate_expr %prec UMINUS
              { $$ = NewNegate(MakeUnique($2)); }
          | rate_expr '+' rate_expr
              { $$ = NewPlus(MakeUnique($1), MakeUnique($3)); }
          | rate_expr '-' rate_expr
              { $$ = NewMinus(MakeUnique($1), MakeUnique($3)); }
          | rate_expr '*' rate_expr
              { $$ = NewMultiply(MakeUnique($1), MakeUnique($3)); }
          | rate_expr '/' rate_expr
              { $$ = NewDivide(MakeUnique($1), MakeUnique($3)); }
          | '(' rate_expr ')'
              { $$ = $2; }
          ;

const_rate_expr : NUMBER
                    { $$ = make_literal($1); }
                | IDENTIFIER
                    { $$ = find_rate($1); }
                | const_rate_expr '*' const_rate_expr
                    { $$ = NewMultiply(MakeUnique($1), MakeUnique($3)); }
                | const_rate_expr '/' const_rate_expr
                    { $$ = NewDivide(MakeUnique($1), MakeUnique($3)); }
                | '(' const_rate_expr ')'
                    { $$ = $2; }
                ;

function : IDENTIFIER
               { $$ = MakeFunction(*$1); delete $1; }
         ;

arguments : expr
              { $$ = new UniquePtrVector<const Expression>(MakeUnique($1)); }
          | arguments ',' expr
              { $$ = $1; $$->push_back(MakeUnique($3)); }
          ;


/* ====================================================================== */
/* Integers. */

const_expr : integer
               { $$ = make_literal($1); }
           | IDENTIFIER
               { $$ = find_constant($1); }
           | '-' const_expr %prec UMINUS
               { $$ = NewNegate(MakeUnique($2)); }
           | const_expr '+' const_expr
               { $$ = NewPlus(MakeUnique($1), MakeUnique($3)); }
           | const_expr '-' const_expr
               { $$ = NewMinus(MakeUnique($1), MakeUnique($3)); }
           | const_expr '*' const_expr
               { $$ = NewMultiply(MakeUnique($1), MakeUnique($3)); }
           | '(' const_expr ')'
               { $$ = $2; }
	   ;

integer : NUMBER { $$ = integer_value($1); }
        ;


/* ====================================================================== */
/* Properties. */

properties : /* empty */
           | properties csl_formula
               { properties.push_back($2); }
           ;

csl_formula : TRUE { $$ = new Conjunction(); }
            | FALSE { $$ = new Disjunction(); }
            | P '<' NUMBER '[' path_formula ']'
                { $$ = make_probabilistic($3, true, true, $5); }
            | P LEQ NUMBER '[' path_formula ']'
                { $$ = make_probabilistic($3, false, true, $5); }
            | P GEQ NUMBER '[' path_formula ']'
                { $$ = make_probabilistic($3, false, false, $5); }
            | P '>' NUMBER '[' path_formula ']'
                { $$ = make_probabilistic($3, true, false, $5); }
            | csl_formula IMPLY_TOKEN csl_formula
                { $$ = new Implication($1, $3); }
            | csl_formula '&' csl_formula { $$ = make_conjunction($1, $3); }
            | csl_formula '|' csl_formula { $$ = make_disjunction($1, $3); }
            | '!' csl_formula { $$ = new Negation($2); }
            | csl_expr '<' csl_expr
                { $$ = NewLessThan(MakeUnique($1), MakeUnique($3)); }
            | csl_expr LEQ csl_expr
                { $$ = NewLessThanOrEqual(MakeUnique($1), MakeUnique($3)); }
            | csl_expr GEQ csl_expr
                { $$ = NewGreaterThanOrEqual(MakeUnique($1), MakeUnique($3)); }
            | csl_expr '>' csl_expr
                { $$ = NewGreaterThan(MakeUnique($1), MakeUnique($3)); }
            | csl_expr '=' csl_expr
                { $$ = NewEquality(MakeUnique($1), MakeUnique($3)); }
            | csl_expr NEQ csl_expr
                { $$ = NewInequality(MakeUnique($1), MakeUnique($3)); }
            | '(' csl_formula ')'
                { $$ = $2; }
            ;

path_formula : csl_formula U LEQ NUMBER csl_formula
                 { $$ = make_until($1, $5, nullptr, $4); }
             | csl_formula U '[' NUMBER ',' NUMBER ']' csl_formula
                 { $$ = make_until($1, $8, $4, $6); }
//             | X csl_formula
             ;

csl_expr : integer
             { $$ = make_literal($1); }
         | IDENTIFIER
             { $$ = value_or_variable($1); }
         | '-' csl_expr %prec UMINUS
             { $$ = NewNegate(MakeUnique($2)); }
         | csl_expr '+' csl_expr
             { $$ = NewPlus(MakeUnique($1), MakeUnique($3)); }
         | csl_expr '-' csl_expr
             { $$ = NewMinus(MakeUnique($1), MakeUnique($3)); }
         | csl_expr '*' csl_expr
             { $$ = NewMultiply(MakeUnique($1), MakeUnique($3)); }
         | '(' csl_expr ')'
             { $$ = $2; }
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
  virtual void DoVisitIdentifier(const Identifier& expr);
  virtual void DoVisitFunctionCall(const FunctionCall& expr);
  virtual void DoVisitUnaryOperation(const UnaryOperation& expr);
  virtual void DoVisitBinaryOperation(const BinaryOperation& expr);
  virtual void DoVisitConditional(const Conditional& expr);
  virtual void DoVisitProbabilityThresholdOperation(
      const ProbabilityThresholdOperation& expr);

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

void ConstantExpressionEvaluator::DoVisitIdentifier(const Identifier& expr) {
  auto i = constant_values_->find(expr.name());
  CHECK(i != constant_values_->end());
  value_ = i->second;
}

void ConstantExpressionEvaluator::DoVisitFunctionCall(
    const FunctionCall& expr) {
  std::vector<TypedValue> arguments;
  for (const Expression& argument : expr.arguments()) {
    argument.Accept(this);
    arguments.push_back(value_);
  }
  switch (expr.function()) {
    case Function::UNKNOWN:
      LOG(FATAL) << "bad function call";
    case Function::MIN:
      CHECK(!arguments.empty());
      value_ = arguments[0];
      for (size_t i = 1; i < arguments.size(); ++i) {
        value_ = std::min(value_, arguments[i]);
      }
      break;
    case Function::MAX:
      CHECK(!arguments.empty());
      value_ = arguments[0];
      for (size_t i = 1; i < arguments.size(); ++i) {
        value_ = std::max(value_, arguments[i]);
      }
      break;
    case Function::FLOOR:
      CHECK(arguments.size() == 1);
      value_ = floor(arguments[0]);
      break;
    case Function::CEIL:
      CHECK(arguments.size() == 2);
      value_ = ceil(arguments[0]);
      break;
    case Function::POW:
      CHECK(arguments.size() == 2);
      value_ = pow(arguments[0], arguments[1]);
      break;
    case Function::LOG:
      CHECK(arguments.size() == 2);
      value_ = log(arguments[0]) / log(arguments[1]);
      break;
    case Function::MOD:
      CHECK(arguments.size() == 2);
      value_ = arguments[0] % arguments[1];
      break;
  }
}

void ConstantExpressionEvaluator::DoVisitUnaryOperation(
    const UnaryOperation& expr) {
  expr.operand().Accept(this);
  switch (expr.op()) {
    case UnaryOperator::NEGATE:
      value_ = -value_;
      break;
    case UnaryOperator::NOT:
      value_ = !value_;
      break;
  }
}

void ConstantExpressionEvaluator::DoVisitBinaryOperation(
    const BinaryOperation& expr) {
  expr.operand1().Accept(this);
  TypedValue operand1 = value_;
  expr.operand2().Accept(this);
  switch (expr.op()) {
    case BinaryOperator::PLUS:
      value_ = operand1 + value_;
      break;
    case BinaryOperator::MINUS:
      value_ = operand1 - value_;
      break;
    case BinaryOperator::MULTIPLY:
      value_ = operand1 * value_;
      break;
    case BinaryOperator::DIVIDE:
      value_ = operand1 / value_;
      break;
    case BinaryOperator::AND:
      value_ = operand1.value<bool>() && value_.value<bool>();
      break;
    case BinaryOperator::OR:
      value_ = operand1.value<bool>() || value_.value<bool>();
      break;
    case BinaryOperator::IMPLY:
      value_ = !operand1.value<bool>() || value_.value<bool>();
      break;
    case BinaryOperator::IFF:
      value_ = operand1.value<bool>() == value_.value<bool>();
      break;
    case BinaryOperator::LESS:
      value_ = operand1 < value_;
      break;
    case BinaryOperator::LESS_EQUAL:
      value_ = operand1 <= value_;
      break;
    case BinaryOperator::GREATER_EQUAL:
      value_ = operand1 >= value_;
      break;
    case BinaryOperator::GREATER:
      value_ = operand1 > value_;
      break;
    case BinaryOperator::EQUAL:
      value_ = operand1 == value_;
      break;
    case BinaryOperator::NOT_EQUAL:
      value_ = operand1 != value_;
      break;
  }
}

void ConstantExpressionEvaluator::DoVisitConditional(const Conditional& expr) {
  expr.condition().Accept(this);
  if (value_.value<bool>()) {
    expr.if_branch().Accept(this);
  } else {
    expr.else_branch().Accept(this);
  }
}

void ConstantExpressionEvaluator::DoVisitProbabilityThresholdOperation(
    const ProbabilityThresholdOperation& expr) {
  LOG(FATAL) << "not a constant expression";
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

void clear_declarations() {
  constants.clear();
  constant_values.clear();
  variables.clear();
}

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
  variable_lows.clear();
  variable_highs.clear();
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

static const Identifier* find_constant(const std::string* ident) {
  if (constants.find(*ident) == constants.end()) {
    if (variables.find(*ident) != variables.end()) {
      yyerror("variable `" + *ident + "' used where expecting constant");
    } else if (rates.find(*ident) != rates.end()) {
      yyerror("rate `" + *ident + "' used where expecting constant");
    } else {
      yyerror("undeclared constant `" + *ident + "'");
    }
    variables.insert(*ident);
    rates.insert(*ident);
    constants.insert(*ident);
    rate_values.insert({*ident, 0});
    constant_values.insert({*ident, 0});
  }
  const Identifier* identifier = new Identifier(*ident);
  delete ident;
  return identifier;
}

static const Identifier* find_rate(const std::string* ident) {
  if (rates.find(*ident) == rates.end()) {
    if (variables.find(*ident) != variables.end()) {
      yyerror("variable `" + *ident + "' used where expecting rate");
    } else {
      yyerror("undeclared rate `" + *ident + "'");
    }
    variables.insert(*ident);
    rates.insert(*ident);
    rate_values.insert({*ident, 0});
  }
  const Identifier* identifier = new Identifier(*ident);
  delete ident;
  return identifier;
}

static const Identifier* find_rate_or_variable(const std::string* ident) {
  if (rates.find(*ident) != rates.end()) {
    const Identifier* identifier = new Identifier(*ident);
    delete ident;
    return identifier;
  } else {
    return find_variable(ident);
  }
}

static const Literal* make_literal(int n) {
  return new Literal(n);
}

static const Literal* make_literal(const TypedValue* q) {
  const Literal* v = new Literal(*q);
  delete q;
  return v;
}

static const Expression* value_or_variable(const std::string* ident) {
  if (constants.find(*ident) != constants.end()) {
    const Literal* value = new Literal(constant_values.find(*ident)->second);
    delete ident;
    return value;
  } else {
    if (variables.find(*ident) == variables.end()) {
      variables.insert(*ident);
      undeclared.insert(*ident);
    }
    const Identifier* identifier = new Identifier(*ident);
    delete ident;
    return identifier;
  }
}
static const Identifier* find_variable(const std::string* ident) {
  if (constants.find(*ident) == constants.end()) {
    if (variables.find(*ident) == variables.end()) {
      variables.insert(*ident);
      undeclared.insert(*ident);
    }
  }
  const Identifier* identifier = new Identifier(*ident);
  delete ident;
  return identifier;
}

static Conjunction* make_conjunction(StateFormula* f1,
				     const StateFormula* f2) {
  Conjunction* conj = dynamic_cast<Conjunction*>(f1);
  if (conj == nullptr) {
    conj = new Conjunction();
    conj->add_conjunct(f1);
  }
  conj->add_conjunct(f2);
  return conj;
}

static Disjunction* make_disjunction(StateFormula* f1,
				     const StateFormula* f2) {
  Disjunction* disj = dynamic_cast<Disjunction*>(f1);
  if (disj == nullptr) {
    disj = new Disjunction();
    disj->add_disjunct(f1);
  }
  disj->add_disjunct(f2);
  return disj;
}

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

static const Until* make_until(const StateFormula* f1, const StateFormula* f2,
			       const TypedValue* t1, const TypedValue* t2) {
  const Until* until;
  if (t1 == nullptr) {
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

static void add_update(const std::string* ident, const Expression* expr) {
  if (variables.find(*ident) == variables.end()) {
    yyerror("updating undeclared variable `" + *ident + "'");
  }
  const int module_index = model->modules().size();
  if (InVariableSet(*model, model->global_variables(), *ident)) {
    if (command->synch() != 0) {
      yywarning("updating global variable in synchronized command");
    }
  } else if (!InVariableSet(*model, model->module_variables(module_index),
                            *ident)) {
    yyerror("updating variable belonging to other module");
  }
  command->add_update(new Update(*ident,
                                 std::unique_ptr<const Expression>(expr)));
  delete ident;
}

static size_t synchronization_value(const std::string* ident) {
  size_t s;
  std::map<std::string, size_t>::const_iterator si =
    synchronizations.find(*ident);
  if (si == synchronizations.end()) {
    s = synchronizations.size() + 1;
    synchronizations.insert(std::make_pair(*ident, s));
  } else {
    s = si->second;
  }
  delete ident;
  return s;
}

static void add_substitution(const std::string* ident1,
			     const std::string* ident2) {
  if (variables.find(*ident1) != variables.end()) {
    /* Variable substitution. */
    subst.insert(std::make_pair(*ident1, *ident2));
    delete ident2;
  } else {
    std::map<std::string, size_t>::const_iterator si =
      synchronizations.find(*ident1);
    if (si != synchronizations.end()) {
      /* Synchronization substitution. */
      size_t s = synchronization_value(ident2);
      synch_subst.insert({si->second, s});
    } else {
      yyerror("illegal substitution `" + *ident1 + "=" + *ident2 + "'");
      delete ident2;
    }
  }
  delete ident1;
}

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
    if (value_expr == nullptr && override == const_overrides.end()) {
      yyerror("uninitialized constant `" + *ident + "'");
      value_expr = make_literal(0);
    }
    variables.insert(*ident);
    rates.insert(*ident);
    constants.insert(*ident);
    const int value =
        ((override != const_overrides.end()) ?
         override->second :
         EvaluateConstantExpression(*value_expr, constant_values)).value<int>();
    rate_values.insert({*ident, value});
    constant_values.insert({*ident, value});
  }
  delete ident;
  delete value_expr;
}

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
    if (value_expr == nullptr && override == const_overrides.end()) {
      yyerror("uninitialized rate `" + *ident + "'");
      value_expr = make_literal(0);
    }
    variables.insert(*ident);
    rates.insert(*ident);
    const TypedValue value = (override != const_overrides.end()) ?
        override->second : EvaluateConstantExpression(*value_expr, rate_values);
    rate_values.insert({*ident, value});
  }
  delete ident;
  delete value_expr;
}

static bool declare_variable(const std::string* ident,
                             const Expression* low,
                             const Expression* high,
                             const Expression* start,
                             bool delayed_addition) {
  bool declared = false;
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
    declared = true;
    if (variables.find(*ident) != variables.end()) {
      if (undeclared.find(*ident) != undeclared.end()) {
	undeclared.erase(*ident);
      } else {
	yyerror("ignoring repeated declaration of variable `" + *ident + "'");
      }
    } else {
      variables.insert(*ident);
    }
  }
  if (declared) {
    int l =
        EvaluateConstantExpression(*low, constant_values).value<int>();
    int h =
        EvaluateConstantExpression(*high, constant_values).value<int>();
    int s = ((start != nullptr)
             ? EvaluateConstantExpression(*start, constant_values).value<int>()
             : l);
    variable_lows.insert(
        std::make_pair(*ident, std::unique_ptr<const Expression>(low)));
    variable_highs.insert(
        std::make_pair(*ident, std::unique_ptr<const Expression>(high)));
    variable_starts.insert(
        std::make_pair(*ident, std::unique_ptr<const Expression>(start)));
    model->AddIntVariable(*ident, l, h, s);
    if (!delayed_addition) {
      if (module != nullptr) {
	module->add_variable(*ident);
      }
    }
  } else {
    delete low;
    delete high;
    delete start;
  }
  delete ident;
  return declared;
}


/* Adds a command to the current module. */
static void add_command() {
  module->add_command(command);
}


/* Prepares a command for parsing. */
static void prepare_command(int synch,
                            std::unique_ptr<const Expression>&& guard,
			    const Distribution* delay) {
  command = new Command(synch, std::move(guard), delay);
}

namespace {

std::string SubstituteName(
    const std::string& name,
    const std::map<std::string, std::string>& substitutions) {
  auto i = substitutions.find(name);
  return (i == substitutions.end()) ? name : i->second;
}

class ExpressionIdentifierSubstituter : public ExpressionVisitor {
 public:
  explicit ExpressionIdentifierSubstituter(
      const std::map<std::string, std::string>& substitutions);

  std::unique_ptr<const Expression> release_expr() { return std::move(expr_); }

 private:
  virtual void DoVisitLiteral(const Literal& expr);
  virtual void DoVisitIdentifier(const Identifier& expr);
  virtual void DoVisitFunctionCall(const FunctionCall& expr);
  virtual void DoVisitUnaryOperation(const UnaryOperation& expr);
  virtual void DoVisitBinaryOperation(const BinaryOperation& expr);
  virtual void DoVisitConditional(const Conditional& expr);
  virtual void DoVisitProbabilityThresholdOperation(
      const ProbabilityThresholdOperation& expr);

  std::map<std::string, std::string> substitutions_;
  std::unique_ptr<const Expression> expr_;
};

std::unique_ptr<const Expression> SubstituteIdentifiers(
    const Expression& expr,
    const std::map<std::string, std::string>& substitutions) {
  ExpressionIdentifierSubstituter substituter(substitutions);
  expr.Accept(&substituter);
  return substituter.release_expr();
}

class StateFormulaIdentifierSubstituter : public StateFormulaVisitor {
 public:
  explicit StateFormulaIdentifierSubstituter(
      const std::map<std::string, std::string>& substitutions);

  ~StateFormulaIdentifierSubstituter();

  const StateFormula* release_formula();

 private:
  virtual void DoVisitConjunction(const Conjunction& formula);
  virtual void DoVisitDisjunction(const Disjunction& formula);
  virtual void DoVisitNegation(const Negation& formula);
  virtual void DoVisitImplication(const Implication& formula);
  virtual void DoVisitProbabilistic(const Probabilistic& formula);
  virtual void DoVisitComparison(const Comparison& formula);

  std::map<std::string, std::string> substitutions_;
  const StateFormula* formula_;
};

const StateFormula* SubstituteIdentifiers(
    const StateFormula& formula,
    const std::map<std::string, std::string>& substitutions) {
  StateFormulaIdentifierSubstituter substituter(substitutions);
  formula.Accept(&substituter);
  return substituter.release_formula();
}

class PathFormulaIdentifierSubstituter : public PathFormulaVisitor {
 public:
  explicit PathFormulaIdentifierSubstituter(
      const std::map<std::string, std::string>& substitutions);

  ~PathFormulaIdentifierSubstituter();

  const PathFormula* release_formula();

 private:
  virtual void DoVisitUntil(const Until& formula);

  std::map<std::string, std::string> substitutions_;
  const PathFormula* formula_;
};

const PathFormula* SubstituteIdentifiers(
    const PathFormula& formula,
    const std::map<std::string, std::string>& substitutions) {
  PathFormulaIdentifierSubstituter substituter(substitutions);
  formula.Accept(&substituter);
  return substituter.release_formula();
}

class DistributionIdentifierSubstituter : public DistributionVisitor {
 public:
  explicit DistributionIdentifierSubstituter(
      const std::map<std::string, std::string>& substitutions);

  ~DistributionIdentifierSubstituter();

  const Distribution* release_distribution();

 private:
  virtual void DoVisitExponential(const Exponential& distribution);
  virtual void DoVisitWeibull(const Weibull& distribution);
  virtual void DoVisitLognormal(const Lognormal& distribution);
  virtual void DoVisitUniform(const Uniform& distribution);

  std::map<std::string, std::string> substitutions_;
  const Distribution* distribution_;
};

const Distribution* SubstituteIdentifiers(
    const Distribution& distribution,
    const std::map<std::string, std::string>& substitutions) {
  DistributionIdentifierSubstituter substituter(substitutions);
  distribution.Accept(&substituter);
  return substituter.release_distribution();
}

const Update* SubstituteIdentifiers(
    const Update& update,
    const std::map<std::string, std::string>& substitutions) {
  return new Update(SubstituteName(update.variable(), substitutions),
                    SubstituteIdentifiers(update.expr(), substitutions));
}

const Command* SubstituteIdentifiers(
    const Command& command,
    const std::map<std::string, std::string>& substitutions,
    const std::map<size_t, size_t>& synchs) {
  size_t s;
  std::map<size_t, size_t>::const_iterator si = synchs.find(command.synch());
  if (si == synchs.end()) {
    s = command.synch();
  } else {
    s = si->second;
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
    const std::map<std::string, std::string>& substitutions,
    const std::map<size_t, size_t>& syncs) {
  Module* subst_mod = new Module();
  for (const std::string& variable : module.variables()) {
    subst_mod->add_variable(SubstituteName(variable, substitutions));
  }
  for (const Command* command : module.commands()) {
    subst_mod->add_command(
        SubstituteIdentifiers(*command, substitutions, syncs));
  }
  return subst_mod;
}

ExpressionIdentifierSubstituter::ExpressionIdentifierSubstituter(
    const std::map<std::string, std::string>& substitutions)
    : substitutions_(substitutions) {
}

void ExpressionIdentifierSubstituter::DoVisitLiteral(const Literal& expr) {
  expr_.reset(new Literal(expr.value()));
}

void ExpressionIdentifierSubstituter::DoVisitIdentifier(
    const Identifier& expr) {
  auto i = substitutions_.find(expr.name());
  if (i == substitutions_.end()) {
    expr_.reset(new Identifier(expr.name()));
  } else {
    expr_.reset(new Identifier(i->second));
  }
}

void ExpressionIdentifierSubstituter::DoVisitFunctionCall(
    const FunctionCall& expr) {
  UniquePtrVector<const Expression> arguments;
  for (const Expression& argument : expr.arguments()) {
    argument.Accept(this);
    arguments.push_back(release_expr());
  }
  expr_ = FunctionCall::New(expr.function(), std::move(arguments));
}

void ExpressionIdentifierSubstituter::DoVisitUnaryOperation(
    const UnaryOperation& expr) {
  expr.operand().Accept(this);
  expr_ = UnaryOperation::New(expr.op(), release_expr());
}

void ExpressionIdentifierSubstituter::DoVisitBinaryOperation(
    const BinaryOperation& expr) {
  expr.operand1().Accept(this);
  std::unique_ptr<const Expression> operand1 = release_expr();
  expr.operand2().Accept(this);
  expr_ = BinaryOperation::New(expr.op(), std::move(operand1), release_expr());
}

void ExpressionIdentifierSubstituter::DoVisitConditional(
    const Conditional& expr) {
  expr.condition().Accept(this);
  std::unique_ptr<const Expression> condition = release_expr();
  expr.if_branch().Accept(this);
  std::unique_ptr<const Expression> if_branch = release_expr();
  expr.else_branch().Accept(this);
  expr_ = Conditional::New(std::move(condition),
                           std::move(if_branch), release_expr());
}

void ExpressionIdentifierSubstituter::DoVisitProbabilityThresholdOperation(
    const ProbabilityThresholdOperation& expr) {
  LOG(FATAL) << "not an expression";
}

StateFormulaIdentifierSubstituter::StateFormulaIdentifierSubstituter(
    const std::map<std::string, std::string>& substitutions)
    : substitutions_(substitutions), formula_(nullptr) {
}

StateFormulaIdentifierSubstituter::~StateFormulaIdentifierSubstituter() {
  delete formula_;
}

const StateFormula* StateFormulaIdentifierSubstituter::release_formula() {
  const StateFormula* formula = formula_;
  formula_ = nullptr;
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
      SubstituteIdentifiers(formula.formula(), substitutions_));
}

void StateFormulaIdentifierSubstituter::DoVisitComparison(
    const Comparison& formula) {
  switch (formula.op()) {
    case Comparison::LESS:
      formula_ = new LessThan(
          SubstituteIdentifiers(formula.expr1(), substitutions_),
          SubstituteIdentifiers(formula.expr2(), substitutions_));
      break;
    case Comparison::LESS_EQUAL:
      formula_ = new LessThanOrEqual(
          SubstituteIdentifiers(formula.expr1(), substitutions_),
          SubstituteIdentifiers(formula.expr2(), substitutions_));
      break;
    case Comparison::GREATER_EQUAL:
      formula_ = new GreaterThanOrEqual(
          SubstituteIdentifiers(formula.expr1(), substitutions_),
          SubstituteIdentifiers(formula.expr2(), substitutions_));
      break;
    case Comparison::GREATER:
      formula_ = new GreaterThan(
          SubstituteIdentifiers(formula.expr1(), substitutions_),
          SubstituteIdentifiers(formula.expr2(), substitutions_));
      break;
    case Comparison::EQUAL:
      formula_ = new Equality(
          SubstituteIdentifiers(formula.expr1(), substitutions_),
          SubstituteIdentifiers(formula.expr2(), substitutions_));
      break;
    case Comparison::NOT_EQUAL:
      formula_ = new Inequality(
          SubstituteIdentifiers(formula.expr1(), substitutions_),
          SubstituteIdentifiers(formula.expr2(), substitutions_));
      break;
  }
}

PathFormulaIdentifierSubstituter::PathFormulaIdentifierSubstituter(
    const std::map<std::string, std::string>& substitutions)
    : substitutions_(substitutions), formula_(nullptr) {
}

PathFormulaIdentifierSubstituter::~PathFormulaIdentifierSubstituter() {
  delete formula_;
}

const PathFormula* PathFormulaIdentifierSubstituter::release_formula() {
  const PathFormula* formula = formula_;
  formula_ = nullptr;
  return formula;
}

void PathFormulaIdentifierSubstituter::DoVisitUntil(const Until& formula) {
  formula_ = new Until(SubstituteIdentifiers(formula.pre(), substitutions_),
                       SubstituteIdentifiers(formula.post(), substitutions_),
                       formula.min_time(), formula.max_time());
}

DistributionIdentifierSubstituter::DistributionIdentifierSubstituter(
    const std::map<std::string, std::string>& substitutions)
    : substitutions_(substitutions), distribution_(nullptr) {
}

DistributionIdentifierSubstituter::~DistributionIdentifierSubstituter() {
  delete distribution_;
}

const Distribution* DistributionIdentifierSubstituter::release_distribution() {
  const Distribution* distribution = distribution_;
  distribution_ = nullptr;
  return distribution;
}

void DistributionIdentifierSubstituter::DoVisitExponential(
    const Exponential& distribution) {
  distribution_ = Exponential::make(
      SubstituteIdentifiers(distribution.rate(), substitutions_));
}

void DistributionIdentifierSubstituter::DoVisitWeibull(
    const Weibull& distribution) {
  distribution_ = Weibull::make(
      SubstituteIdentifiers(distribution.scale(), substitutions_),
      SubstituteIdentifiers(distribution.shape(), substitutions_));
}

void DistributionIdentifierSubstituter::DoVisitLognormal(
    const Lognormal& distribution) {
  distribution_ = Lognormal::make(
      SubstituteIdentifiers(distribution.scale(), substitutions_),
      SubstituteIdentifiers(distribution.shape(), substitutions_));
}

void DistributionIdentifierSubstituter::DoVisitUniform(
    const Uniform& distribution) {
  distribution_ = Uniform::make(
      SubstituteIdentifiers(distribution.low(), substitutions_),
      SubstituteIdentifiers(distribution.high(), substitutions_));
}

}  // namespace

static void add_module(const std::string* ident1, const std::string* ident2) {
  std::map<std::string, Module*>::const_iterator mi = modules.find(*ident1);
  if (mi != modules.end()) {
    yyerror("ignoring repeated declaration of module `" + *ident1 + "'");
  } else {
    mi = modules.find(*ident2);
    if (mi == modules.end()) {
      yyerror("ignoring renaming of undeclared module `" + *ident2 + "'");
    } else {
      const Module& src_module = *mi->second;
      std::map<std::string, std::string> c_subst;
      for (const std::string& constant : constants) {
	auto si = subst.find(constant);
	if (si != subst.end()) {
	  auto cj = constants.find(si->second);
	  if (cj == constants.end()) {
	    yyerror("substituting constant with non-constant " + si->second);
	  } else {
	    c_subst.insert(*si);
	  }
	}
      }
      std::map<std::string, std::string> v_subst;
      model->OpenModuleScope();
      for (const std::string& variable : src_module.variables()) {
	auto si = subst.find(variable);
	if (si == subst.end()) {
	  yyerror("missing substitution for module variable");
	} else {
          std::unique_ptr<const Expression> low =
              SubstituteIdentifiers(*variable_lows.find(variable)->second,
                                    c_subst);
          std::unique_ptr<const Expression> high =
              SubstituteIdentifiers(*variable_highs.find(variable)->second,
                                    c_subst);
          std::unique_ptr<const Expression> start;
	  auto i = variable_starts.find(variable);
	  if (i->second != nullptr) {
	    start = SubstituteIdentifiers(*i->second, c_subst);
	  }
	  if (declare_variable(new std::string(si->second), low.release(),
                               high.release(), start.release(), true)) {
	    v_subst.insert(*si);
	  }
	}
      }
      for (auto si = subst.begin(); si != subst.end(); si++) {
        std::unique_ptr<const Identifier> v1(
            find_variable(new std::string(si->first)));
	if (find(src_module.variables().begin(), src_module.variables().end(),
                 si->first) != src_module.variables().end()) {
          std::unique_ptr<const Identifier> v2(
              find_variable(new std::string(si->second)));
          v_subst.insert(*si);
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
  module = nullptr;
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
  if (model != nullptr) {
    delete model;
  }
  model = new Model();
}


/* Compiles the current model. */
static void compile_model() {
  for (std::map<std::string, Module*>::const_iterator mi = modules.begin();
       mi != modules.end(); mi++) {
    mi->second->compile(constant_values, rate_values);
  }
  model->compile();
  global_model = model;
}

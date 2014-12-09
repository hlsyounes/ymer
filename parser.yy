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

#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <string>

#include "models.h"
#include "src/distribution.h"
#include "src/strutil.h"

#include "glog/logging.h"

#include "parser.hh"

// Lexical analyzer function.
extern int yylex(YYSTYPE* lvalp, YYLTYPE* llocp, void* scanner);
/* Name of current file. */
extern std::string current_file;

/* Last model parsed. */
const Model* global_model = nullptr;
/* Parsed properties. */
UniquePtrVector<const Expression> properties;

/* Current model. */
static Model* model;
/* Whether the last parsing attempt succeeded. */
static bool success = true;

/* Returns a variable for the given identifier. */
static const Identifier* find_variable(const std::string* ident);
/* Prepares a model for parsing. */
static void prepare_model(ModelType model_type);
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

template <typename T>
std::unique_ptr<T> WrapUnique(T* ptr) {
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

const UnaryOperation* NewNegate(const Expression* operand) {
  return new UnaryOperation(UnaryOperator::NEGATE, WrapUnique(operand));
}

const UnaryOperation* NewNot(const Expression* operand) {
  return new UnaryOperation(UnaryOperator::NOT, WrapUnique(operand));
}

const BinaryOperation* NewPlus(const Expression* operand1,
                               const Expression* operand2) {
  return new BinaryOperation(BinaryOperator::PLUS, WrapUnique(operand1),
                             WrapUnique(operand2));
}

const BinaryOperation* NewMinus(const Expression* operand1,
                                const Expression* operand2) {
  return new BinaryOperation(BinaryOperator::MINUS, WrapUnique(operand1),
                             WrapUnique(operand2));
}

const BinaryOperation* NewMultiply(const Expression* operand1,
                                   const Expression* operand2) {
  return new BinaryOperation(BinaryOperator::MULTIPLY, WrapUnique(operand1),
                             WrapUnique(operand2));
}

const BinaryOperation* NewDivide(const Expression* operand1,
                                 const Expression* operand2) {
  return new BinaryOperation(BinaryOperator::DIVIDE, WrapUnique(operand1),
                             WrapUnique(operand2));
}

const BinaryOperation* NewAnd(const Expression* operand1,
                              const Expression* operand2) {
  return new BinaryOperation(BinaryOperator::AND, WrapUnique(operand1),
                             WrapUnique(operand2));
}

const BinaryOperation* NewOr(const Expression* operand1,
                             const Expression* operand2) {
  return new BinaryOperation(BinaryOperator::OR, WrapUnique(operand1),
                             WrapUnique(operand2));
}

const BinaryOperation* NewImply(const Expression* operand1,
                                const Expression* operand2) {
  return new BinaryOperation(BinaryOperator::IMPLY, WrapUnique(operand1),
                             WrapUnique(operand2));
}

const BinaryOperation* NewIff(const Expression* operand1,
                              const Expression* operand2) {
  return new BinaryOperation(BinaryOperator::IFF, WrapUnique(operand1),
                             WrapUnique(operand2));
}

const BinaryOperation* NewLess(const Expression* operand1,
                               const Expression* operand2) {
  return new BinaryOperation(BinaryOperator::LESS, WrapUnique(operand1),
                             WrapUnique(operand2));
}

const BinaryOperation* NewLessEqual(const Expression* operand1,
                                    const Expression* operand2) {
  return new BinaryOperation(BinaryOperator::LESS_EQUAL, WrapUnique(operand1),
                             WrapUnique(operand2));
}

const BinaryOperation* NewGreaterEqual(const Expression* operand1,
                                       const Expression* operand2) {
  return new BinaryOperation(BinaryOperator::GREATER_EQUAL,
                             WrapUnique(operand1), WrapUnique(operand2));
}

const BinaryOperation* NewGreater(const Expression* operand1,
                                  const Expression* operand2) {
  return new BinaryOperation(BinaryOperator::GREATER, WrapUnique(operand1),
                             WrapUnique(operand2));
}

const BinaryOperation* NewEqual(const Expression* operand1,
                                const Expression* operand2) {
  return new BinaryOperation(BinaryOperator::EQUAL, WrapUnique(operand1),
                             WrapUnique(operand2));
}

const BinaryOperation* NewNotEqual(const Expression* operand1,
                                   const Expression* operand2) {
  return new BinaryOperation(BinaryOperator::NOT_EQUAL,
                             WrapUnique(operand1), WrapUnique(operand2));
}

const Conditional* NewConditional(const Expression* condition,
                                  const Expression* if_branch,
                                  const Expression* else_branch) {
  return new Conditional(WrapUnique(condition), WrapUnique(if_branch),
                         WrapUnique(else_branch));
}

double Double(const TypedValue* typed_value) {
  CHECK(typed_value->type() != Type::BOOL);
  double value = typed_value->value<double>();
  delete typed_value;
  return value;
}

const ProbabilityThresholdOperation* NewProbabilityLess(
    double threshold, const PathProperty* path_property) {
  return new ProbabilityThresholdOperation(
      ProbabilityThresholdOperator::LESS, threshold, WrapUnique(path_property));
}

const ProbabilityThresholdOperation* NewProbabilityLessEqual(
    double threshold, const PathProperty* path_property) {
  return new ProbabilityThresholdOperation(
      ProbabilityThresholdOperator::LESS_EQUAL, threshold,
      WrapUnique(path_property));
}

const ProbabilityThresholdOperation* NewProbabilityGreaterEqual(
    double threshold, const PathProperty* path_property) {
  return new ProbabilityThresholdOperation(
      ProbabilityThresholdOperator::GREATER_EQUAL, threshold,
      WrapUnique(path_property));
}

const ProbabilityThresholdOperation* NewProbabilityGreater(
    double threshold, const PathProperty* path_property) {
  return new ProbabilityThresholdOperation(
      ProbabilityThresholdOperator::GREATER, threshold,
      WrapUnique(path_property));
}

const UntilProperty* NewUntil(double min_time, double max_time,
                              const Expression* pre_expr,
                              const Expression* post_expr) {
  return new UntilProperty(min_time, max_time, WrapUnique(pre_expr),
                           WrapUnique(post_expr));
}

const Memoryless* NewMemoryless(const Expression* rate) {
  return new Memoryless(WrapUnique(rate));
}

const Distribution* NewWeibull(const Expression* scale,
                               const Expression* shape) {
  return new Weibull(WrapUnique(scale), WrapUnique(shape));
}

const Lognormal* NewLognormal(const Expression* scale,
                              const Expression* shape) {
  return new Lognormal(WrapUnique(scale), WrapUnique(shape));
}

const Uniform* NewUniform(const Expression* low,
                          const Expression* high) {
  return new Uniform(WrapUnique(low), WrapUnique(high));
}

void AddConstant(const std::string* name, Type type, const Expression* init) {
  std::vector<std::string> errors;
  if (!model->AddConstant(*WrapUnique(name), type, WrapUnique(init), &errors)) {
    for (const auto& error : errors) {
      yyerror(error);
    }
  }
}

void AddIntVariable(const std::string* name, const Expression* low,
                    const Expression* high, const Expression* init) {
  std::vector<std::string> errors;
  if (!model->AddIntVariable(*WrapUnique(name), WrapUnique(low),
                             WrapUnique(high), WrapUnique(init), &errors)) {
    for (const auto& error : errors) {
      yyerror(error);
    }
  }
}

void AddBoolVariable(const std::string* name, const Expression* init) {
  std::vector<std::string> errors;
  if (!model->AddBoolVariable(*WrapUnique(name), WrapUnique(init), &errors)) {
    for (const auto& error : errors) {
      yyerror(error);
    }
  }
}

void StartModule(const std::string* name) {
  auto name_ptr = WrapUnique(name);
  if (!model->StartModule(*name_ptr)) {
    yyerror(StrCat("duplicate module ", *name_ptr));
  }
}

void AddFromModule(const std::string* from_name,
                   const std::map<std::string, std::string>* substitutions) {
  std::vector<std::string> errors;
  if (!model->AddFromModule(*WrapUnique(from_name), *WrapUnique(substitutions),
                            &errors)) {
    for (const auto& error : errors) {
      yyerror(error);
    }
  }
}

void EndModule() {
  model->EndModule();
}

std::map<std::string, std::string>* AddSubstitution(
    const std::string* from_name, const std::string* to_name,
    std::map<std::string, std::string>* substitutions) {
  if (substitutions == nullptr) {
    substitutions = new std::map<std::string, std::string>();
  }
  auto from_name_ptr = WrapUnique(from_name);
  if (!substitutions->emplace(*from_name_ptr, *WrapUnique(to_name)).second) {
    yyerror(StrCat("duplicate substitution for ", *from_name_ptr));
  }
  return substitutions;
}

Update* NewUpdate(const std::string* variable, const Expression* expr) {
  return new Update(*WrapUnique(variable), WrapUnique(expr));
}

std::vector<Update>* AddUpdate(Update* update, std::vector<Update>* updates) {
  if (updates == nullptr) {
    updates = new std::vector<Update>();
  }
  updates->push_back(std::move(*WrapUnique(update)));
  return updates;
}

Outcome* NewOutcome(const Distribution* delay, std::vector<Update>* updates) {
  return new Outcome(
      (delay == nullptr) ? Memoryless::New(Literal::New(1)) : WrapUnique(delay),
      std::move(*WrapUnique(updates)));
}

std::vector<Outcome>* AddOutcome(Outcome* outcome,
                                 std::vector<Outcome>* outcomes) {
  if (outcomes == nullptr) {
    outcomes = new std::vector<Outcome>();
  }
  outcomes->push_back(std::move(*WrapUnique(outcome)));
  return outcomes;
}

void AddCommand(const std::string* action, const Expression* guard,
                std::vector<Outcome>* outcomes) {
        std::vector<std::string> errors;
        if (!model->AddCommand(Command(*WrapUnique(action), WrapUnique(guard),
                                       std::move(*WrapUnique(outcomes))),
                               &errors)) {
    for (const auto& error : errors) {
      yyerror(error);
    }
  }
}

void StartRewards(const std::string* label) { delete label; }

void AddStateReward(const Expression* guard, const Expression* reward) {
  delete guard;
  delete reward;
}

void AddTransitionReward(const std::string* action, const Expression* guard,
                         const Expression* reward) {
  delete action;
  delete guard;
  delete reward;
}

void EndRewards() {}

}  // namespace
%}

%defines
%locations
%lex-param {void* scanner}
%parse-param {void* scanner}
%error-verbose

%token DTMC_TOKEN CTMC_TOKEN MDP_TOKEN GSMP_TOKEN
%token PROBABILISTIC STOCHASTIC NONDETERMINISTIC
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
%token RMIN R S U W X L

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
  Type type;
  ModelType model_type;
  const PathProperty* path;
  std::map<std::string, std::string>* substitutions;
  std::vector<Outcome>* outcomes;
  Outcome* outcome;
  const Distribution* dist;
  std::vector<Update>* updates;
  Update* update;
  const Expression* expr;
  const std::string* str;
  const TypedValue* number;
  Function function;
  UniquePtrVector<const Expression>* arguments;
}

%type <type> constant_type
%type <model_type> model_type
%type <substitutions> substitutions
%type <expr> constant_init variable_init expr property
%type <path> path_property
%type <outcomes> outcomes update_distribution
%type <outcome> distribution_and_updates
%type <dist> distribution
%type <updates> true_or_updates updates
%type <update> update
%type <str> IDENTIFIER LABEL_NAME action
%type <number> NUMBER
%type <function> function
%type <arguments> arguments

%destructor { delete $$; } <substitutions>
%destructor { delete $$; } <path>
%destructor { delete $$; } <outcomes>
%destructor { delete $$; } <outcome>
%destructor { delete $$; } <dist>
%destructor { delete $$; } <updates>
%destructor { delete $$; } <expr>
%destructor { delete $$; } <str>
%destructor { delete $$; } <number>
%destructor { delete $$; } <arguments>

%%

file : { success = true; } model_or_properties
         { if (!success) YYERROR; }
     ;

model_or_properties : model
                    | properties
                    ;


/* ====================================================================== */
/* Model files. */

model : model_type { prepare_model($1); } model_components
          { compile_model(); }
      ;

model_components : model_component
                 | model_components model_component
                 ;

model_component : constant
                | global
                | module
                | rewards
                ;

model_type : NONDETERMINISTIC
               { $$ = ModelType::MDP; }
           | MDP_TOKEN
               { $$ = ModelType::MDP; }
           | PROBABILISTIC
               { $$ = ModelType::DTMC; }
           | DTMC_TOKEN
               { $$ = ModelType::DTMC; }
           | STOCHASTIC
               { $$ = ModelType::CTMC; }
           | CTMC_TOKEN
               { $$ = ModelType::CTMC; }
           | GSMP_TOKEN
               { $$ = ModelType::GSMP; }
           ;

constant : constant_type IDENTIFIER constant_init ';'
             { AddConstant($2, $1, $3); }
         ;

constant_type : CONST INT_TOKEN
                  { $$ = Type::INT; }
              | CONST
                  { $$ = Type::INT; }
              | CONST DOUBLE_TOKEN
                  { $$ = Type::DOUBLE; }
              | RATE
                  { $$ = Type::DOUBLE; }
              | PROB
                  { $$ = Type::DOUBLE; }
              | CONST BOOL_TOKEN
                  { $$ = Type::BOOL; }
              ;

constant_init : /* empty */
                  { $$ = nullptr; }
              | '=' expr
                  { $$ = $2; }
              ;

global : GLOBAL variable
       ;

variable : IDENTIFIER ':' '[' expr DOTDOT expr ']' variable_init ';'
             { AddIntVariable($1, $4, $6, $8); }
         | IDENTIFIER ':' BOOL_TOKEN variable_init ';'
             { AddBoolVariable($1, $4); }
         ;

variable_init : /* empty */
                  { $$ = nullptr; }
              | INIT expr
                  { $$ = $2; }
              ;

module : MODULE module_name module_def ENDMODULE
           { EndModule(); }
       ;

module_name : IDENTIFIER
                { StartModule($1); }
            ;

module_def : variables commands
           | '=' IDENTIFIER '[' substitutions ']'
               { AddFromModule($2, $4); }
           ;

substitutions : IDENTIFIER '=' IDENTIFIER
                  { $$ = AddSubstitution($1, $3, nullptr); }
              | substitutions ',' IDENTIFIER '=' IDENTIFIER
                  { $$ = AddSubstitution($3, $5, $1); }
              ;

variables : /* empty */
          | variables variable
          ;

commands : /* empty */
         | commands command
         ;

command : '[' action ']' expr ARROW outcomes ';'
            { AddCommand($2, $4, $6); }
        ;

action : /* empty */
           { $$ = new std::string(); }
       | IDENTIFIER
       ;

outcomes : true_or_updates
             { $$ = AddOutcome(NewOutcome(nullptr, $1), nullptr); }
         | update_distribution
         ;

update_distribution : distribution_and_updates
                        { $$ = AddOutcome($1, nullptr); }
                    | update_distribution '+' distribution_and_updates
                        { $$ = AddOutcome($3, $1); }
                    ;

distribution_and_updates : distribution ':' true_or_updates
                             { $$ = NewOutcome($1, $3); }
                         ;

distribution : expr
                 { $$ = NewMemoryless($1); }
             | W '(' expr ',' expr ')'
                 { $$ = NewWeibull($3, $5); }
             | L '(' expr ',' expr ')'
                 { $$ = NewLognormal($3, $5); }
             | U '(' expr ',' expr ')'
                 { $$ = NewUniform($3, $5); }
             ;

true_or_updates : TRUE
                    { $$ = new std::vector<Update>(); }
                | updates
                ;

updates : update
            { $$ = AddUpdate($1, nullptr); }
        | updates '&' update
            { $$ = AddUpdate($3, $1); }
        ;

update : '(' IDENTIFIER PRIME '=' expr ')'
           { $$ = NewUpdate($2, $5); }
       ;

rewards : REWARDS rewards_label reward_rules ENDREWARDS
            { EndRewards(); }
        ;

rewards_label : /* empty */
                  { StartRewards(nullptr); }
              | LABEL_NAME
                  { StartRewards($1); }
              ;

reward_rules : /* empty */
             | reward_rules state_reward
             | reward_rules transition_reward
             ;

state_reward : expr ':' expr ';'
                 { AddStateReward($1, $3); }
             ;

transition_reward : '[' action ']' expr ':' expr ';'
                      { AddTransitionReward($2, $4, $6); }
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
         { $$ = NewNegate($2); }
     | '!' expr
         { $$ = NewNot($2); }
     | expr '+' expr
         { $$ = NewPlus($1, $3); }
     | expr '-' expr
         { $$ = NewMinus($1, $3); }
     | expr '*' expr
         { $$ = NewMultiply($1, $3); }
     | expr '/' expr
         { $$ = NewDivide($1, $3); }
     | expr '&' expr
         { $$ = NewAnd($1, $3); }
     | expr '|' expr
         { $$ = NewOr($1, $3); }
     | expr IMPLY_TOKEN expr
         { $$ = NewImply($1, $3); }
     | expr IFF_TOKEN expr
         { $$ = NewIff($1, $3); }
     | expr '<' expr
         { $$ = NewLess($1, $3); }
     | expr LEQ expr
         { $$ = NewLessEqual($1, $3); }
     | expr GEQ expr
         { $$ = NewGreaterEqual($1, $3); }
     | expr '>' expr
         { $$ = NewGreater($1, $3); }
     | expr '=' expr
         { $$ = NewEqual($1, $3); }
     | expr NEQ expr
         { $$ = NewNotEqual($1, $3); }
     | expr '?' expr ':' expr
         { $$ = NewConditional($1, $3, $5); }
     | '(' expr ')'
         { $$ = $2; }
     ;

function : IDENTIFIER
             { $$ = MakeFunction(*$1); delete $1; }
         | MIN_TOKEN
             { $$ = Function::MIN; }
         | MAX_TOKEN
             { $$ = Function::MAX; }
         ;

arguments : expr
              { $$ = new UniquePtrVector<const Expression>(WrapUnique($1)); }
          | arguments ',' expr
              { $$ = $1; $$->push_back(WrapUnique($3)); }
          ;


/* ====================================================================== */
/* Properties. */

properties : property_list
           | property_list ';'
           ;

property_list : property
                  { properties.push_back(WrapUnique($1)); }
              | property_list ';' property
                  { properties.push_back(WrapUnique($3)); }
              ;

property : NUMBER
             { $$ = new Literal(*$1); delete $1; }
         | TRUE
             { $$ = new Literal(true); }
         | FALSE
             { $$ = new Literal(false); }
         | IDENTIFIER
             { $$ = find_variable($1); }
         | function '(' arguments ')'
             { $$ = new FunctionCall($1, std::move(*$3)); delete $3; }
         | FUNC '(' function ',' arguments ')'
             { $$ = new FunctionCall($3, std::move(*$5)); delete $5; }
         | P '<' NUMBER '[' path_property ']'
             { $$ = NewProbabilityLess(Double($3), $5); }
         | P LEQ NUMBER '[' path_property ']'
             { $$ = NewProbabilityLessEqual(Double($3), $5); }
         | P GEQ NUMBER '[' path_property ']'
             { $$ = NewProbabilityGreaterEqual(Double($3), $5); }
         | P '>' NUMBER '[' path_property ']'
             { $$ = NewProbabilityGreater(Double($3), $5); }
         | '-' property %prec UMINUS
             { $$ = NewNegate($2); }
         | '!' property
             { $$ = NewNot($2); }
         | property '+' property
             { $$ = NewPlus($1, $3); }
         | property '-' property
             { $$ = NewMinus($1, $3); }
         | property '*' property
             { $$ = NewMultiply($1, $3); }
         | property '/' property
             { $$ = NewDivide($1, $3); }
         | property '&' property
             { $$ = NewAnd($1, $3); }
         | property '|' property
             { $$ = NewOr($1, $3); }
         | property IMPLY_TOKEN property
             { $$ = NewImply($1, $3); }
         | property IFF_TOKEN property
             { $$ = NewIff($1, $3); }
         | property '<' property
             { $$ = NewLess($1, $3); }
         | property LEQ property
             { $$ = NewLessEqual($1, $3); }
         | property GEQ property
             { $$ = NewGreaterEqual($1, $3); }
         | property '>' property
             { $$ = NewGreater($1, $3); }
         | property '=' property
             { $$ = NewEqual($1, $3); }
         | property NEQ property
             { $$ = NewNotEqual($1, $3); }
         | property '?' property ':' property
             { $$ = NewConditional($1, $3, $5); }
         | '(' property ')'
             { $$ = $2; }
         ;

path_property : property U LEQ NUMBER property
                  { $$ = NewUntil(0, Double($4), $1, $5); }
              | property U '[' NUMBER ',' NUMBER ']' property
                  { $$ = NewUntil(Double($4), Double($6), $1, $8); }
              ;

%%

static const Identifier* find_variable(const std::string* ident) {
  const Identifier* identifier = new Identifier(*ident);
  delete ident;
  return identifier;
}

/* Prepares a model for parsing. */
static void prepare_model(ModelType model_type) {
  properties.clear();
  if (model != nullptr) {
    delete model;
  }
  model = new Model();
  model->SetType(model_type);
}


/* Compiles the current model. */
static void compile_model() {
  model->compile();
  global_model = model;
}

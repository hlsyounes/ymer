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
/* Constant overrides. */
extern std::map<std::string, TypedValue> const_overrides;

/* Last model parsed. */
const Model* global_model = nullptr;
/* Parsed properties. */
UniquePtrVector<const Expression> properties;

/* Current model. */
static Model* model;
/* Current identifier substitutions. */
static std::map<std::string, std::string> subst;
/* Current command. */
static Command* command;
/* Whether the last parsing attempt succeeded. */
static bool success = true;

/* Returns a variable for the given identifier. */
static const Identifier* find_variable(const std::string* ident);
/* Adds an update to the current command. */
static void add_update(const std::string* ident, const Expression* expr);
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
static void declare_variable(const std::string* ident, const Expression* low,
                             const Expression* high, const Expression* start);
/* Adds a command to the current module. */
static void add_command();
/* Prepares a command for parsing. */
static void prepare_command(std::unique_ptr<const std::string>&& action,
                            std::unique_ptr<const Expression>&& guard,
			    const Distribution* delay);
/* Adds a module to the current model defined by renaming. */
static void add_module(const std::string* ident1, const std::string* ident2);
/* Adds a module to the current model. */
static void add_module();
/* Prepares a module for parsing. */
static void prepare_module(const std::string* ident);
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

void yywarning(const std::string& msg) {
  std::cerr << PACKAGE ":" << current_file << ':' << yylloc.first_line << ':'
            << msg << std::endl;
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

const Exponential* NewExponential(const Expression* rate) {
  return new Exponential(WrapUnique(rate));
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

void AddConstant(std::unique_ptr<const std::string>&& name, Type type,
                 std::unique_ptr<const Expression>&& init) {
  std::vector<std::string> errors;
  if (!model->AddConstant(*name, type, std::move(init), &errors)) {
    for (const auto& error : errors) {
      yyerror(error);
    }
  }
}

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
  ModelType model_type;
  const PathProperty* path;
  const Distribution* dist;
  const Expression* expr;
  const std::string* str;
  const TypedValue* number;
  Function function;
  UniquePtrVector<const Expression>* arguments;
}

%type <model_type> model_type
%type <expr> property
%type <path> path_property
%type <dist> distribution
%type <expr> expr
%type <str> IDENTIFIER synchronization
%type <number> NUMBER
%type <function> function
%type <arguments> arguments

%destructor { delete $$; } <expr>
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

model : model_type { prepare_model($1); } declarations modules rewards
          { compile_model(); }
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

/* ====================================================================== */
/* Declarations. */

declarations : /* empty */
             | declarations declaration
             ;

declaration : CONST IDENTIFIER ';'
                { declare_constant($2, nullptr); }
            | CONST IDENTIFIER '=' expr ';'
                { declare_constant($2, $4); }
            | CONST INT_TOKEN IDENTIFIER ';'
                { declare_constant($3, nullptr); }
            | CONST INT_TOKEN IDENTIFIER '=' expr ';'
                { declare_constant($3, $5); }
            | RATE IDENTIFIER ';'
                { declare_rate($2, nullptr); }
            | RATE IDENTIFIER '=' expr ';'
                { declare_rate($2, $4); }
            | CONST DOUBLE_TOKEN IDENTIFIER ';'
                { declare_rate($3, nullptr); }
            | CONST DOUBLE_TOKEN IDENTIFIER '=' expr ';'
                { declare_rate($3, $5); }
            | GLOBAL IDENTIFIER ':' '[' expr DOTDOT expr ']' ';'
                { declare_variable($2, $5, $7, nullptr); }
            | GLOBAL IDENTIFIER ':' '[' expr DOTDOT expr ']' INIT expr ';'
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

variable_decl : IDENTIFIER ':' '[' expr DOTDOT expr ']' ';'
                  { declare_variable($1, $4, $6, nullptr); }
              | IDENTIFIER ':' '[' expr DOTDOT expr ']' INIT expr ';'
                  { declare_variable($1, $4, $6, $9); }
              ;

commands : /* empty */
         | commands command
         ;

command : synchronization expr ARROW distribution ':'
            { prepare_command(WrapUnique($1), WrapUnique($2), $4); } update ';'
            { add_command(); }
        ;

synchronization : '[' ']' { $$ = new std::string(); }
                | '[' IDENTIFIER ']' { $$ = $2; }
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

state_reward : expr ':' expr ';'
                 { delete $1; delete $3; }
             ;

transition_reward : '[' IDENTIFIER ']' expr ':' expr ';'
                      { delete $2; delete $4; delete $6; }
                  ;


/* ====================================================================== */
/* Distributions. */

distribution : expr
                 { $$ = NewExponential($1); }
             | W '(' expr ',' expr ')'
                 { $$ = NewWeibull($3, $5); }
             | L '(' expr ',' expr ')'
                 { $$ = NewLognormal($3, $5); }
             | U '(' expr ',' expr ')'
                 { $$ = NewUniform($3, $5); }
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

static void add_update(const std::string* ident, const Expression* expr) {
  command->add_update(Update(*ident, std::unique_ptr<const Expression>(expr)));
  delete ident;
}

static void add_substitution(const std::string* ident1,
			     const std::string* ident2) {
  subst.insert(std::make_pair(*ident1, *ident2));
  delete ident1;
  delete ident2;
}

static void declare_constant(const std::string* ident,
                             const Expression* value_expr) {
  AddConstant(WrapUnique(ident), Type::INT, WrapUnique(value_expr));
}

static void declare_rate(const std::string* ident,
                         const Expression* value_expr) {
  AddConstant(WrapUnique(ident), Type::DOUBLE, WrapUnique(value_expr));
}

static void declare_variable(const std::string* ident, const Expression* low,
                             const Expression* high, const Expression* start) {
  std::vector<std::string> errors;
  if (!model->AddIntVariable(*ident, WrapUnique(low), WrapUnique(high),
                             WrapUnique(start), &errors)) {
    for (const auto& error : errors) {
      yyerror(error);
    }
  }
  delete ident;
}

/* Prepares a command for parsing. */
static void prepare_command(std::unique_ptr<const std::string>&& action,
                            std::unique_ptr<const Expression>&& guard,
			    const Distribution* delay) {
  command = new Command(*action, std::move(guard), WrapUnique(delay), {});
}

static void add_command() {
  std::vector<std::string> errors;
  if (!model->AddCommand(std::move(*command), &errors)) {
    for (const auto& error : errors) {
      yyerror(error);
    }
  }
}

static void add_module(const std::string* ident1, const std::string* ident2) {
  if (!model->StartModule(*ident1)) {
    yyerror(StrCat("duplicate module ", *ident1));
  } else {
    std::vector<std::string> errors;
    if (!model->AddFromModule(*ident2, subst, &errors)) {
      for (const auto& error : errors) {
        yyerror(error);
      }
    }
    model->EndModule();
  }
  subst.clear();
  delete ident1;
  delete ident2;
}


/* Adds a module to the current model. */
static void add_module() {
  model->EndModule();
}


/* Prepares a module for parsing. */
static void prepare_module(const std::string* ident) {
  if (!model->StartModule(*ident)) {
    yyerror(StrCat("duplicate module ", *ident));
  }
  delete ident;
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

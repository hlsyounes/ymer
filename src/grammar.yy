// -*-C++-*-
// Copyright (C) 2011 Google Inc
//
// This file is part of Ymer.
//
// Ymer is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Ymer is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Ymer; if not, write to the Free Software Foundation,
// Inc., #59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// Grammar for the PRISM language.

%{
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "glog/logging.h"

#include "src/expression.h"
#include "src/model.h"
#include "src/parser-state.h"
#include "src/process-algebra.h"

#include "grammar.h"

// Lexical analyzer function.
extern int yylex(YYSTYPE* lvalp, YYLTYPE* llocp, void* scanner);

// Error reporting function.
static void yyerror(
    YYLTYPE* llocp, void* scanner, ParserState* state, const std::string& msg);

// Sets the type of state->model.
static void SetModelType(
    const YYLTYPE& location, ModelType type, ParserState* state);
// Adds a constant to state->model.
static void AddConstant(const YYLTYPE& location,
                        const std::string* name,
                        Type type,
                        const Expression* init,
                        ParserState* state);
// Adds an int variable to state->model.
static void AddIntVariable(const YYLTYPE& location,
                           const std::string* name,
                           const Expression* min,
                           const Expression* max,
                           const Expression* init,
                           ParserState* state);
// Adds a bool variable to state->model.
static void AddBoolVariable(const YYLTYPE& location,
                            const std::string* name,
                            const Expression* init,
                            ParserState* state);
// Adds a formula to state->model.
static void AddFormula(const YYLTYPE& location,
                       const std::string* name,
                       const Expression* expr,
                       ParserState* state);
// Adds a label to state->model.
static void AddLabel(const YYLTYPE& location,
                     const std::string* name,
                     const Expression* expr,
                     ParserState* state);
// Sets the init expression of state->model.
static void SetInit(
    const YYLTYPE& location, const Expression* init, ParserState* state);
// Starts a reward structure with the given label for state->model.
static void StartRewards(
    const YYLTYPE& location, const std::string* label, ParserState* state);
// Ends the current reward structure for state->model.
static void EndRewards(ParserState* state);
// Adds a state reward to state->model.
static void AddStateReward(
    const Expression* guard, const Expression* reward, ParserState* state);
// Adds a transition reward to state->model.
static void AddTransitionReward(const std::string* action,
                                const Expression* guard,
                                const Expression* reward,
                                ParserState* state);
// Starts a new module for state->model.
static void StartModule(
    const YYLTYPE& location, const std::string* name, ParserState* state);
// Ends the current module for state->model.
static void EndModule(ParserState* state);
// Adds variables and commands from the given module to the current module of
// state->model.
static void AddFromModule(
    const YYLTYPE& location,
    const std::string* from_module,
    const std::map<std::string, std::string>* substitutions,
    ParserState* state);
// Adds a command to state->model.
static void AddCommand(const YYLTYPE& location,
                       const std::string* action,
                       const Expression* guard,
                       std::vector<Outcome>* outcomes,
                       ParserState* state);
// Adds an indentifier substitution to the given set of substitutions, and
// returns the resulting substitutions.
static std::map<std::string, std::string>* AddSubstitution(
    const YYLTYPE& location,
    const std::string* from_name, const std::string* to_name,
    std::map<std::string, std::string>* substitutions,
    ParserState* state);
// Adds an outcome to the given outcomes, and returns the resulting outcomes.
static std::vector<Outcome>* AddOutcome(Outcome* outcome,
                                        std::vector<Outcome>* outcomes);
// Returns an outcome for the given probability expression and updates.
static Outcome* MakeOutcome(const Expression* probability,
                            std::vector<Update>* updates);
// Adds an update to the given updates, and returns the resulting updates.
static std::vector<Update>* AddUpdate(Update* update,
                                      std::vector<Update>* updates);
// Returns an update for the given variable and with the given expression.
static Update* MakeUpdate(const std::string* variable, const Expression* expr);
// Returns the function with the given name.
static Function MakeFunction(
    const YYLTYPE& location, const std::string* name, ParserState* state);
// Returns the unary operation with the given operator and operand.
static const Expression* MakeUnaryOperation(UnaryOperator op,
                                            const Expression* operand);
// Returns the binary operation with the given operator and operands.
static const Expression* MakeBinaryOperation(BinaryOperator op,
                                             const Expression* operand1,
                                             const Expression* operand2);
// Returns the conditional with the given condition, if_expr, and else_expr.
static const Expression* MakeConditional(const Expression* condition,
                                         const Expression* if_expr,
                                         const Expression* else_expr);
// Sets the system process algebra expression of state->model.
static void SetSystem(
    const YYLTYPE& location, const ProcessAlgebra* system, ParserState* state);
// Returns an action hiding process algebra operation.
static const ProcessAlgebra* MakeActionHiding(
    std::set<std::string>* actions, const ProcessAlgebra* operand);
// Returns an action renaming process algebra operation.
static const ProcessAlgebra* MakeActionRenaming(
    std::map<std::string, std::string>* substitutions,
    const ProcessAlgebra* operand);
// Returns a restricted parallel composition process algebra operation.
static const ProcessAlgebra* MakeRestrictedParallelComposition(
    std::set<std::string>* actions,
    const ProcessAlgebra* operand1,
    const ProcessAlgebra* operand2);
// Returns an alphabetized parallel composition process algebra operation.
static const ProcessAlgebra* MakeAlphabetizedParallelComposition(
    const ProcessAlgebra* operand1, const ProcessAlgebra* operand2);
// Returns an asynchronous parallel composition process algebra operation.
static const ProcessAlgebra* MakeAsynchronousParallelComposition(
    const ProcessAlgebra* operand1, const ProcessAlgebra* operand2);
// Adds an action to the given actions, and returns the resulting actions.
static std::set<std::string>* AddAction(const YYLTYPE& location,
                                        const std::string* action,
                                        std::set<std::string>* actions,
                                        ParserState* state);
%}

%defines
%define api.pure
%locations
%lex-param {void* scanner}
%parse-param {void* scanner}
%parse-param {ParserState* state}
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
%token IDENTIFIER INT_LITERAL DOUBLE_LITERAL LABEL_NAME
%token SYSTEM ENDSYSTEM
%token DOUBLE_BAR TRIPLE_BAR BACK_ARROW

%token A C
%token E F G I
%token PMAX PMIN P RMAX
%token RMIN R S U W X

%left DOUBLE_BAR TRIPLE_BAR
%right '?' ':'
%left IMP
%left '|'
%left '&'
%left '!'
%left '=' NEQ
%left '<' LEQ GEQ '>'
%left '+' '-'
%left '*' '/'
%left '{'
%right UMINUS

%union {
  std::string* str;
  Type type;
  std::map<std::string, std::string>* substitutions;
  std::vector<Outcome>* outcomes;
  Outcome* outcome;
  std::vector<Update>* updates;
  Update* update;
  const Expression* expr;
  int int_literal;
  double double_literal;
  Function function;
  ArgumentList* arguments;
  const ProcessAlgebra* process_algebra;
  std::set<std::string>* actions;
}

%type <str> IDENTIFIER LABEL_NAME action
%type <type> constant_type
%type <substitutions> substitutions action_substitutions
%type <outcomes> outcomes update_distribution
%type <outcome> probability_and_updates
%type <updates> true_or_updates updates
%type <update> update
%type <expr> constant_init variable_init expr
%type <int_literal> INT_LITERAL
%type <double_literal> DOUBLE_LITERAL
%type <function> function_name
%type <arguments> arguments
%type <process_algebra> process_algebra
%type <actions> action_list

%destructor { delete $$; } <str>
%destructor { delete $$; } <substitutions>
%destructor { delete $$; } <outcomes>
%destructor { delete $$; } <outcome>
%destructor { delete $$; } <updates>
%destructor { delete $$; } <update>
%destructor { delete $$; } <expr>
%destructor { delete $$; } <arguments>
%destructor { delete $$; } <process_algebra>
%destructor { delete $$; } <actions>

%%

model : model_components { if (!state->success) YYERROR; }
      ;

model_components : model_component
                 | model_components model_component
                 ;

model_component : model_type
                | constant
                | global
                | formula
                | label
                | init
                | rewards
                | module
                | system
                ;

model_type : DTMC_TOKEN
               { SetModelType(yylloc, ModelType::DTMC, state); }
           | CTMC_TOKEN
               { SetModelType(yylloc, ModelType::CTMC, state); }
           | MDP_TOKEN
               { SetModelType(yylloc, ModelType::MDP, state); }
           | PROBABILISTIC
               { SetModelType(yylloc, ModelType::DTMC, state); }
           | STOCHASTIC
               { SetModelType(yylloc, ModelType::CTMC, state); }
           | NONDETERMINISTIC
               { SetModelType(yylloc, ModelType::MDP, state); }
           ;

constant : constant_type IDENTIFIER constant_init ';'
             { AddConstant(yylloc, $2, $1, $3, state); }
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

variables : /* empty */
          | variables variable
          ;

variable : IDENTIFIER ':' '[' expr DOTDOT expr ']' variable_init ';'
             { AddIntVariable(yylloc, $1, $4, $6, $8, state); }
         | IDENTIFIER ':' BOOL_TOKEN variable_init ';'
             { AddBoolVariable(yylloc, $1, $4, state); }
         ;

variable_init : /* empty */
                  { $$ = nullptr; }
              | INIT expr
                  { $$ = $2; }
              ;

formula : FORMULA IDENTIFIER '=' expr ';'
            { AddFormula(yylloc, $2, $4, state); }
        ;

label : LABEL LABEL_NAME '=' expr ';'
          { AddLabel(yylloc, $2, $4, state); }
      ;

init : INIT expr ENDINIT
         { SetInit(yylloc, $2, state); }
     ;

rewards : REWARDS rewards_label reward_rules ENDREWARDS
            { EndRewards(state); }
        ;

rewards_label : /* empty */
                  { StartRewards(yylloc, nullptr, state); }
              | LABEL_NAME
                  { StartRewards(yylloc, $1, state); }
              ;

reward_rules : /* empty */
             | reward_rules state_reward
             | reward_rules transition_reward
             ;

state_reward : expr ':' expr ';'
                 { AddStateReward($1, $3, state); }
             ;

transition_reward : '[' action ']' expr ':' expr ';'
                      { AddTransitionReward($2, $4, $6, state); }
                  ;

module : MODULE module_name module_def ENDMODULE
           { EndModule(state); }
       ;

module_name : IDENTIFIER
                { StartModule(yylloc, $1, state); }
            ;

module_def : variables commands
           | '=' IDENTIFIER '[' substitutions ']'
               { AddFromModule(yylloc, $2, $4, state); }
           ;

substitutions : IDENTIFIER '=' IDENTIFIER
                  { $$ = AddSubstitution(yylloc, $1, $3, nullptr, state); }
              | substitutions ',' IDENTIFIER '=' IDENTIFIER
                  { $$ = AddSubstitution(yylloc, $3, $5, $1, state); }
              ;

commands : /* empty */
         | commands command
         ;

command : '[' action ']' expr ARROW outcomes ';'
            { AddCommand(yylloc, $2, $4, $6, state); }
        ;

action : /* empty */
           { $$ = nullptr; }
       | IDENTIFIER
       ;

outcomes : true_or_updates
             { $$ = AddOutcome(MakeOutcome(nullptr, $1), nullptr); }
         | update_distribution
         ;

update_distribution : probability_and_updates
                        { $$ = AddOutcome($1, nullptr); }
                    | update_distribution '+' probability_and_updates
                        { $$ = AddOutcome($3, $1); }
                    ;

probability_and_updates : expr ':' true_or_updates
                            { $$ = MakeOutcome($1, $3); }
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
           { $$ = MakeUpdate($2, $5); }
       ;

expr : INT_LITERAL
         { $$ = new IntLiteral($1); }
     | DOUBLE_LITERAL
         { $$ = new DoubleLiteral($1); }
     | TRUE
         { $$ = new BoolLiteral(true); }
     | FALSE
         { $$ = new BoolLiteral(false); }
     | IDENTIFIER
         { $$ = new Identifier(*$1); delete $1; }
     | function_name '(' arguments ')'
         { $$ = new FunctionCall($1, std::move(*$3)); delete $3; }
     | FUNC '(' function_name ',' arguments ')'
         { $$ = new FunctionCall($3, std::move(*$5)); delete $5; }
     | '(' expr ')'
         { $$ = $2; }
     | '-' expr %prec UMINUS
         { $$ = MakeUnaryOperation(UnaryOperator::NEGATE, $2); }
     | expr '*' expr
         { $$ = MakeBinaryOperation(BinaryOperator::MULTIPLY, $1, $3); }
     | expr '/' expr
         { $$ = MakeBinaryOperation(BinaryOperator::DIVIDE, $1, $3); }
     | expr '+' expr
         { $$ = MakeBinaryOperation(BinaryOperator::PLUS, $1, $3); }
     | expr '-' expr
         { $$ = MakeBinaryOperation(BinaryOperator::MINUS, $1, $3); }
     | expr '<' expr
         { $$ = MakeBinaryOperation(BinaryOperator::LESS, $1, $3); }
     | expr LEQ expr
         { $$ = MakeBinaryOperation(BinaryOperator::LESS_EQUAL, $1, $3); }
     | expr GEQ expr
         { $$ = MakeBinaryOperation(BinaryOperator::GREATER_EQUAL, $1, $3); }
     | expr '>' expr
         { $$ = MakeBinaryOperation(BinaryOperator::GREATER, $1, $3); }
     | expr '=' expr
         { $$ = MakeBinaryOperation(BinaryOperator::EQUAL, $1, $3); }
     | expr NEQ expr
         { $$ = MakeBinaryOperation(BinaryOperator::NOT_EQUAL, $1, $3); }
     | '!' expr
         { $$ = MakeUnaryOperation(UnaryOperator::NOT, $2); }
     | expr '&' expr
         { $$ = MakeBinaryOperation(BinaryOperator::AND, $1, $3); }
     | expr '|' expr
         { $$ = MakeBinaryOperation(BinaryOperator::OR, $1, $3); }
     | expr IMP expr
         { $$ = MakeBinaryOperation(BinaryOperator::IMPLY, $1, $3); }
     | expr '?' expr ':' expr
         { $$ = MakeConditional($1, $3, $5); }
     ;

function_name : IDENTIFIER
                  { $$ = MakeFunction(yylloc, $1, state); }
              | MAX_TOKEN
                  { $$ = Function::MAX; }
              | MIN_TOKEN
                  { $$ = Function::MIN; }
              ;

arguments : expr
              { $$ = new ArgumentList(std::unique_ptr<const Expression>($1)); }
          | arguments ',' expr
              { $$ = $1; $$->push_back(std::unique_ptr<const Expression>($3)); }
          ;

system : SYSTEM process_algebra ENDSYSTEM
           { SetSystem(yylloc, $2, state); }
       ;

process_algebra : IDENTIFIER
                    { $$ = new ModuleIdentifier(*$1); delete $1; }
                | '(' process_algebra ')'
                    { $$ = $2; }
                | process_algebra '/' '{' action_list '}'
                    { $$ = MakeActionHiding($4, $1); }
                | process_algebra '{' action_substitutions '}'
                    { $$ = MakeActionRenaming($3, $1); }
                | process_algebra '|' '[' action_list ']' '|' process_algebra
                    { $$ = MakeRestrictedParallelComposition($4, $1, $7); }
                | process_algebra DOUBLE_BAR process_algebra
                    { $$ = MakeAlphabetizedParallelComposition($1, $3); }
                | process_algebra TRIPLE_BAR process_algebra
                    { $$ = MakeAsynchronousParallelComposition($1, $3); }
                ;

action_list : IDENTIFIER
                { $$ = AddAction(yylloc, $1, nullptr, state); }
            | action_list ',' IDENTIFIER
                { $$ = AddAction(yylloc, $3, $1, state); }
            ;

action_substitutions : IDENTIFIER BACK_ARROW IDENTIFIER
                         { $$ = AddSubstitution(yylloc, $1, $3, nullptr,
                                                state); }
                     | action_substitutions ',' IDENTIFIER BACK_ARROW IDENTIFIER
                         { $$ = AddSubstitution(yylloc, $3, $5, $1, state); }
                     ;

%%

static void yyerror(
    const YYLTYPE& location, const std::string& msg, ParserState* state) {
  CHECK(state);

  if (state->message) {
    std::ostringstream os;
    if (state->filename && *state->filename != "-") {
      os << *state->filename << ':';
    }
    os << location.first_line << ':' << msg;
    if (!state->message->empty()) {
      *state->message += "\n";
    }
    *state->message += os.str();
  }
  state->success = false;
}

static void yyerror(
    YYLTYPE* llocp, void* scanner, ParserState* state, const std::string& msg) {
  yyerror(*llocp, msg, state);
}

static void SetModelType(
    const YYLTYPE& location, ModelType type, ParserState* state) {
  CHECK(state);
  CHECK(state->model);

  if (!state->model->SetType(type)) {
    yyerror(location, "multiple model types", state);
  }
}

static void AddConstant(const YYLTYPE& location,
                        const std::string* name,
                        Type type,
                        const Expression* init,
                        ParserState* state) {
  CHECK(name);
  CHECK(state);
  CHECK(state->model);

  if (!state->model->AddConstant(
          *name, type, std::unique_ptr<const Expression>(init))) {
    yyerror(location, "duplicate identifier", state);
  }
  delete name;
}

static void AddIntVariable(const YYLTYPE& location,
                           const std::string* name,
                           const Expression* min,
                           const Expression* max,
                           const Expression* init,
                           ParserState* state) {
  CHECK(name);
  CHECK(state);
  CHECK(state->model);

  if (!state->model->AddIntVariable(
          *name,
          Range(std::unique_ptr<const Expression>(min),
                std::unique_ptr<const Expression>(max)),
          std::unique_ptr<const Expression>(init))) {
    yyerror(location, "duplicate identifier", state);
  }
  delete name;
}

static void AddBoolVariable(const YYLTYPE& location,
                            const std::string* name,
                            const Expression* init,
                            ParserState* state) {
  CHECK(name);
  CHECK(state);
  CHECK(state->model);

  if (!state->model->AddBoolVariable(
          *name, std::unique_ptr<const Expression>(init))) {
    yyerror(location, "duplicate identifier", state);
  }
  delete name;
}

static void AddFormula(const YYLTYPE& location,
                       const std::string* name,
                       const Expression* expr,
                       ParserState* state) {
  CHECK(name);
  CHECK(state);
  CHECK(state->model);

  if (!state->model->AddFormula(
          *name, std::unique_ptr<const Expression>(expr))) {
    yyerror(location, "duplicate identifier", state);
  }
  delete name;
}

static void AddLabel(const YYLTYPE& location,
                     const std::string* name,
                     const Expression* expr,
                     ParserState* state) {
  CHECK(name);
  CHECK(state);
  CHECK(state->model);

  if (!state->model->AddLabel(
          *name, std::unique_ptr<const Expression>(expr))) {
    yyerror(location, "duplicate label", state);
  }
  delete name;
}

static void SetInit(
    const YYLTYPE& location, const Expression* init, ParserState* state) {
  CHECK(state);
  CHECK(state->model);

  if (!state->model->SetInit(std::unique_ptr<const Expression>(init))) {
    yyerror(location, "multiple init blocks", state);
  }
}

static void StartRewards(
    const YYLTYPE& location, const std::string* label, ParserState* state) {
  CHECK(state);
  CHECK(state->model);

  if (label) {
    if (!state->model->StartLabeledRewards(*label)) {
      yyerror(location, "duplicate rewards label", state);
    }
    delete label;
  } else {
    state->model->StartUnlabeledRewards();
  }
}

static void EndRewards(ParserState* state) {
  CHECK(state);
  CHECK(state->model);

  state->model->EndRewards();
}

static void AddStateReward(
    const Expression* guard, const Expression* reward, ParserState* state) {
  CHECK(state);
  CHECK(state->model);

  state->model->AddStateReward(StateReward(
      std::unique_ptr<const Expression>(guard),
      std::unique_ptr<const Expression>(reward)));
}

static void AddTransitionReward(const std::string* action,
                                const Expression* guard,
                                const Expression* reward,
                                ParserState* state) {
  CHECK(state);
  CHECK(state->model);

  state->model->AddTransitionReward(TransitionReward(
      std::unique_ptr<const std::string>(action),
      std::unique_ptr<const Expression>(guard),
      std::unique_ptr<const Expression>(reward)));
}

static void StartModule(
    const YYLTYPE& location, const std::string* name, ParserState* state) {
  CHECK(state);
  CHECK(state->model);

  if (!state->model->StartModule(*name)) {
    yyerror(location, "duplicate module identifier", state);
  }
  delete name;
}

static void EndModule(ParserState* state) {
  CHECK(state);
  CHECK(state->model);

  state->model->EndModule();
}

static void AddFromModule(
    const YYLTYPE& location,
    const std::string* from_module,
    const std::map<std::string, std::string>* substitutions,
    ParserState* state) {
  CHECK(from_module);
  CHECK(substitutions);
  CHECK(state);
  CHECK(state->model);

  std::string error;
  if (!state->model->AddFromModule(*from_module, *substitutions, &error)) {
    yyerror(location, error, state);
  }
  delete from_module;
  delete substitutions;
}

static void AddCommand(const YYLTYPE& location,
                       const std::string* action,
                       const Expression* guard,
                       std::vector<Outcome>* outcomes,
                       ParserState* state) {
  CHECK(state);
  CHECK(state->model);

  if (!state->model->AddCommand(
          Command(std::unique_ptr<const std::string>(action),
                  std::unique_ptr<const Expression>(guard),
                  std::move(*outcomes)))) {
    yyerror(location, "duplicate identifier", state);
  }
  delete outcomes;
}

static std::map<std::string, std::string>* AddSubstitution(
    const YYLTYPE& location,
    const std::string* from_name,
    const std::string* to_name,
    std::map<std::string, std::string>* substitutions,
    ParserState* state) {
  CHECK(from_name);
  CHECK(to_name);

  if (substitutions) {
    if (!substitutions->insert({ *from_name, *to_name }).second) {
      yyerror(location, "duplicate substitution", state);
    }
  } else {
    substitutions = new std::map<std::string, std::string>(
      { { *from_name, *to_name } });
  }
  delete from_name;
  delete to_name;
  return substitutions;
}

namespace {

template <typename Movable>
static std::vector<Movable>* AddToVector(Movable* element,
                                         std::vector<Movable>* elements) {
  CHECK(element);

  if (elements == nullptr) {
    elements = new std::vector<Movable>();
  }
  elements->push_back(std::move(*element));
  delete element;
  return elements;
}

}  // namespace

static std::vector<Outcome>* AddOutcome(Outcome* outcome,
                                        std::vector<Outcome>* outcomes) {
  return AddToVector(outcome, outcomes);
}

static Outcome* MakeOutcome(const Expression* probability,
                            std::vector<Update>* updates) {
  CHECK(updates);

  if (probability == nullptr) {
    probability = new DoubleLiteral(1.0);
  }
  Outcome* outcome = new Outcome(std::unique_ptr<const Expression>(probability),
                                 std::move(*updates));
  delete updates;
  return outcome;
}

static std::vector<Update>* AddUpdate(Update* update,
                                      std::vector<Update>* updates) {
  return AddToVector(update, updates);
}

static Update* MakeUpdate(const std::string* variable, const Expression* expr) {
  CHECK(variable);

  Update* update = new Update(*variable,
                              std::unique_ptr<const Expression>(expr));
  delete variable;
  return update;
}

static Function MakeFunction(
    const YYLTYPE& location, const std::string* name, ParserState* state) {
  CHECK(name);

  Function function;
  if (*name == "floor") {
    function = Function::FLOOR;
  } else if (*name == "ceil") {
    function = Function::CEIL;
  } else if (*name == "pow") {
    function = Function::POW;
  } else if (*name == "mod") {
    function = Function::MOD;
  } else if (*name == "log") {
    function = Function::LOG;
  } else {
    function = Function::NO_FUNCTION;
    yyerror(location, "unknown function", state);
  }
  delete name;
  return function;
}

static const Expression* MakeUnaryOperation(UnaryOperator op,
                                            const Expression* operand) {
  return new UnaryOperation(op, std::unique_ptr<const Expression>(operand));
}

static const Expression* MakeBinaryOperation(BinaryOperator op,
                                             const Expression* operand1,
                                             const Expression* operand2) {
  return new BinaryOperation(op,
                             std::unique_ptr<const Expression>(operand1),
                             std::unique_ptr<const Expression>(operand2));
}

static const Expression* MakeConditional(const Expression* condition,
                                         const Expression* if_expr,
                                         const Expression* else_expr) {
  return new Conditional(std::unique_ptr<const Expression>(condition),
                         std::unique_ptr<const Expression>(if_expr),
                         std::unique_ptr<const Expression>(else_expr));
}

static void SetSystem(
    const YYLTYPE& location, const ProcessAlgebra* system, ParserState* state) {
  CHECK(state);
  CHECK(state->model);

  if (!state->model->SetSystem(std::unique_ptr<const ProcessAlgebra>(system))) {
    yyerror(location, "multiple system blocks", state);
  }
}

static const ProcessAlgebra* MakeActionHiding(
    std::set<std::string>* actions, const ProcessAlgebra* operand) {
  CHECK(actions);

  const ProcessAlgebra* process_algebra = new ActionHiding(
      std::move(*actions), std::unique_ptr<const ProcessAlgebra>(operand));
  delete actions;
  return process_algebra;
}

static const ProcessAlgebra* MakeActionRenaming(
    std::map<std::string, std::string>* substitutions,
    const ProcessAlgebra* operand) {
  CHECK(substitutions);

  const ProcessAlgebra* process_algebra = new ActionRenaming(
      std::move(*substitutions),
      std::unique_ptr<const ProcessAlgebra>(operand));
  delete substitutions;
  return process_algebra;
}

static const ProcessAlgebra* MakeRestrictedParallelComposition(
    std::set<std::string>* actions,
    const ProcessAlgebra* operand1, const ProcessAlgebra* operand2) {
  CHECK(actions);

  const ProcessAlgebra* process_algebra = new RestrictedParallelComposition(
      std::move(*actions),
      std::unique_ptr<const ProcessAlgebra>(operand1),
      std::unique_ptr<const ProcessAlgebra>(operand2));
  delete actions;
  return process_algebra;
}

namespace {

const ProcessAlgebra* MakeParallelComposition(ParallelCompositionOperator op,
                                              const ProcessAlgebra* operand1,
                                              const ProcessAlgebra* operand2) {
  return new ParallelComposition(
      op,
      std::unique_ptr<const ProcessAlgebra>(operand1),
      std::unique_ptr<const ProcessAlgebra>(operand2));
}

}  // namespace

static const ProcessAlgebra* MakeAlphabetizedParallelComposition(
    const ProcessAlgebra* operand1, const ProcessAlgebra* operand2) {
  return MakeParallelComposition(ParallelCompositionOperator::ALPHABETIZED,
                                 operand1, operand2);
}

static const ProcessAlgebra* MakeAsynchronousParallelComposition(
    const ProcessAlgebra* operand1, const ProcessAlgebra* operand2) {
  return MakeParallelComposition(ParallelCompositionOperator::ASYNCHRONOUS,
                                 operand1, operand2);
}

static std::set<std::string>* AddAction(const YYLTYPE& location,
                                        const std::string* action,
                                        std::set<std::string>* actions,
                                        ParserState* state) {
  CHECK(action);

  if (actions) {
    if (!actions->insert(*action).second) {
      yyerror(location, "duplicate action", state);
    }
  } else {
    actions = new std::set<std::string>({ *action });
  }
  delete action;
  return actions;
}

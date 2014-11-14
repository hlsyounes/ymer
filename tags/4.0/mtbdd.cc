#include "mtbdd.h"

#include <cstdio>  // Needed by cudd.h for declaration of FILE.
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>

#include "cudd.h"
#include "expressions.h"
#include "formulas.h"
#include "models.h"
#include "modules.h"


/* Set of state variables. */
struct VariableSet : public std::set<const Variable<int>*> {
};


/* Mapping from variables to bit ranges. */
struct VariableBitMap
  : public std::map<const Variable<int>*, std::pair<int, int> > {
};


/* DD manager. */
static DdManager* dd_man = 0;
/* Bit ranges for state variables. */
static VariableBitMap bit_range;


/* Allocates bits for the given state variable. */
static int allocate_bits(const Variable<int>& variable, int low_bit) {
  int h = variable.high().value(Values()) - variable.low().value(Values());
  int nbits = 0;
  while (h > 0) {
    h >>= 1;
    nbits++;
  }
  bit_range.insert(std::make_pair(&variable,
				  std::make_pair(low_bit,
						 low_bit + nbits - 1)));
  return nbits;
}


/* Returns an MTBDD representation for the given addition. */
template<typename T>
static DdNode* addition_mtbdd(const Addition<T>& addition) {
  DdNode* dd1 = expression_mtbdd(addition.operand1());
  DdNode* dd2 = expression_mtbdd(addition.operand2());
  DdNode* dde = Cudd_addApply(dd_man, Cudd_addPlus, dd1, dd2);
  Cudd_Ref(dde);
  dd_deref(dd1);
  dd_deref(dd2);
  return dde;
}


/* Returns an MTBDD representation for the given subtraction. */
template<typename T>
static DdNode* subtraction_mtbdd(const Subtraction<T>& subtraction) {
  DdNode* dd1 = expression_mtbdd(subtraction.operand1());
  DdNode* dd2 = expression_mtbdd(subtraction.operand2());
  DdNode* dde = Cudd_addApply(dd_man, Cudd_addMinus, dd1, dd2);
  Cudd_Ref(dde);
  dd_deref(dd1);
  dd_deref(dd2);
  return dde;
}


/* Returns an MTBDD representation for the given multiplication. */
template<typename T>
static DdNode*
multiplication_mtbdd(const Multiplication<T>& multiplication) {
  DdNode* dd1 = expression_mtbdd(multiplication.operand1());
  DdNode* dd2 = expression_mtbdd(multiplication.operand2());
  DdNode* dde = Cudd_addApply(dd_man, Cudd_addTimes, dd1, dd2);
  Cudd_Ref(dde);
  dd_deref(dd1);
  dd_deref(dd2);
  return dde;
}


/* Returns an MTBDD representation for the given constant. */
template<typename T>
static DdNode* constant_mtbdd(const Constant<T>& constant) {
  DdNode* dde = Cudd_addConst(dd_man, constant.value());
  Cudd_Ref(dde);
  return dde;
}


/* Returns an MTBDD representation for the given state variable. */
static DdNode* variable_mtbdd(const Variable<int>& variable, bool primed) {
  VariableBitMap::const_iterator vbi = bit_range.find(&variable);
  if (vbi == bit_range.end()) {
    throw std::logic_error("no bit range for variable `"
			   + variable.name() + "'");
  }
  int low_bit = (*vbi).second.first;
  int high_bit = (*vbi).second.second;
  DdNode* ddv = Cudd_ReadZero(dd_man);
  Cudd_Ref(ddv);
  for (int i = high_bit; i >= low_bit; i--) {
    DdNode* ddc = Cudd_addConst(dd_man, 1 << (high_bit - i));
    Cudd_Ref(ddc);
    DdNode* ddi = Cudd_addIthVar(dd_man, 2*i + (primed ? 1 : 0));
    Cudd_Ref(ddi);
    DdNode* ddm = Cudd_addApply(dd_man, Cudd_addTimes, ddi, ddc);
    Cudd_Ref(ddm);
    dd_deref(ddi);
    dd_deref(ddc);
    DdNode* dds = Cudd_addApply(dd_man, Cudd_addPlus, ddv, ddm);
    Cudd_Ref(dds);
    dd_deref(ddv);
    dd_deref(ddm);
    ddv = dds;
  }
  int low = variable.low().value(Values());
  if (low != 0) {
    DdNode* ddl = Cudd_addConst(dd_man, low);
    Cudd_Ref(ddl);
    DdNode* dds = Cudd_addApply(dd_man, Cudd_addPlus, ddv, ddl);
    Cudd_Ref(dds);
    dd_deref(ddv);
    dd_deref(ddl);
    return dds;
  } else {
    return ddv;
  }
}


/* Returns an MTBDD representation for the given value. */
template<typename T>
static DdNode* value_mtbdd(const Value<T>& value) {
  DdNode* dde = Cudd_addConst(dd_man, value.value());
  Cudd_Ref(dde);
  return dde;
}


/* Returns a BDD representation for the equality of the given state
   variable's current and next-state versions. */
static DdNode* variable_identity_bdd(const Variable<int>& variable) {
  VariableBitMap::const_iterator vbi = bit_range.find(&variable);
  if (vbi == bit_range.end()) {
    throw std::logic_error("no bit range for variable `"
			   + variable.name() + "'");
  }
  int low_bit = (*vbi).second.first;
  int high_bit = (*vbi).second.second;
  DdNode* ddi;
  int m = high_bit - low_bit + 1;
  if (m > 0) {
    DdNode** xvars = new DdNode*[m];
    DdNode** yvars = new DdNode*[m];
    for (int b = high_bit; b >= low_bit; b--) {
      xvars[b - low_bit] = Cudd_bddIthVar(dd_man, 2*b);
      yvars[b - low_bit] = Cudd_bddIthVar(dd_man, 2*b + 1);
    }
    ddi = Cudd_Xeqy(dd_man, m, xvars, yvars);
    Cudd_Ref(ddi);
    delete xvars;
    delete yvars;
  } else {
    ddi = Cudd_ReadOne(dd_man);
    Cudd_Ref(ddi);
  }
  return ddi;
}


/* Returns a BDD representation for the given conjunction. */
static DdNode* conjunction_bdd(const Conjunction& conjunction) {
  DdNode* ddf = Cudd_ReadOne(dd_man);
  Cudd_Ref(ddf);
  for (FormulaList::const_iterator fi = conjunction.conjuncts().begin();
       fi != conjunction.conjuncts().end(); fi++) {
    DdNode* ddi = formula_bdd(**fi);
    DdNode* dda = Cudd_bddAnd(dd_man, ddf, ddi);
    Cudd_Ref(dda);
    dd_deref(ddf);
    dd_deref(ddi);
    ddf = dda;
  }
  return ddf;
}


/* Returns a BDD representation for the given disjunction. */
static DdNode* disjunction_bdd(const Disjunction& disjunction) {
  DdNode* ddf = Cudd_ReadLogicZero(dd_man);
  Cudd_Ref(ddf);
  for (FormulaList::const_iterator fi = disjunction.disjuncts().begin();
       fi != disjunction.disjuncts().end(); fi++) {
    DdNode* ddi = formula_bdd(**fi);
    DdNode* ddo = Cudd_bddOr(dd_man, ddf, ddi);
    Cudd_Ref(ddo);
    dd_deref(ddf);
    dd_deref(ddi);
    ddf = ddo;
  }
  return ddf;
}


/* Returns a BDD representation for the given less-than formula. */
static DdNode* less_than_bdd(const LessThan<int>& lt) {
  /*
   * expr1 < expr2  <==> expr2 - expr1 > 0
   */
  DdNode* dd1 = expression_mtbdd(lt.expr1());
  DdNode* dd2 = expression_mtbdd(lt.expr2());
  DdNode* dde = Cudd_addApply(dd_man, Cudd_addMinus, dd2, dd1);
  Cudd_Ref(dde);
  dd_deref(dd1);
  dd_deref(dd2);
  DdNode* ddf = Cudd_addBddStrictThreshold(dd_man, dde, 0.0);
  Cudd_Ref(ddf);
  dd_deref(dde);
  if (lt.negated()) {
    DdNode* ddn = Cudd_Not(ddf);
    Cudd_Ref(ddn);
    dd_deref(ddf);
    return ddn;
  } else {
    return ddf;
  }
}


/* Returns a BDD representation for the given greater-than formula. */
static DdNode* greater_than_bdd(const GreaterThan<int>& gt) {
  /*
   * expr1 > expr2  <==> expr1 - expr2 > 0
   */
  DdNode* dd1 = expression_mtbdd(gt.expr1());
  DdNode* dd2 = expression_mtbdd(gt.expr2());
  DdNode* dde = Cudd_addApply(dd_man, Cudd_addMinus, dd1, dd2);
  Cudd_Ref(dde);
  dd_deref(dd1);
  dd_deref(dd2);
  DdNode* ddf = Cudd_addBddStrictThreshold(dd_man, dde, 0.0);
  Cudd_Ref(ddf);
  dd_deref(dde);
  if (gt.negated()) {
    DdNode* ddn = Cudd_Not(ddf);
    Cudd_Ref(ddn);
    dd_deref(ddf);
    return ddn;
  } else {
    return ddf;
  }
}


/* Returns a BDD representation for the given equality. */
static DdNode* equality_bdd(const Equality<int>& eq) {
  /*
   * expr1 == expr2  <==> expr1 - expr2 in [0,0]
   */
  DdNode* dd1 = expression_mtbdd(eq.expr1());
  DdNode* dd2 = expression_mtbdd(eq.expr2());
  DdNode* dde = Cudd_addApply(dd_man, Cudd_addMinus, dd1, dd2);
  Cudd_Ref(dde);
  dd_deref(dd1);
  dd_deref(dd2);
  DdNode* ddf = Cudd_addBddInterval(dd_man, dde, 0.0, 0.0);
  Cudd_Ref(ddf);
  dd_deref(dde);
  if (eq.negated()) {
    DdNode* ddn = Cudd_Not(ddf);
    Cudd_Ref(ddn);
    dd_deref(ddf);
    return ddn;
  } else {
    return ddf;
  }
}


/* Returns a BDD representation for the given update. */
static DdNode* update_bdd(const Update<int>& update) {
  /*
   * variable' == expr  <==>  variable' - expr in [0,0]
   */
  DdNode* ddv = variable_mtbdd(update.variable(), true);
  DdNode* dde = expression_mtbdd(update.expr());
  DdNode* ddm = Cudd_addApply(dd_man, Cudd_addMinus, ddv, dde);
  Cudd_Ref(ddm);
  dd_deref(ddv);
  dd_deref(dde);
  DdNode* ddu = Cudd_addBddInterval(dd_man, ddm, 0.0, 0.0);
  Cudd_Ref(ddu);
  dd_deref(ddm);
  return ddu;
}


/* Returns a BDD representation of the transition matrix for the given
   command. */
static DdNode* command_bdd(const Command& command, const Model& model) {
  /* Touched state variables. */
  VariableSet touched;
  /*
   * Conjunction of BDDs for all updates.
   */
  DdNode* ddu = Cudd_ReadOne(dd_man);
  Cudd_Ref(ddu);
  for (UpdateList<int>::const_iterator ui = command.int_updates().begin();
       ui != command.int_updates().end(); ui++) {
    const Update<int>& update = **ui;
    DdNode* ddi = update_bdd(update);
    DdNode* dda = Cudd_bddAnd(dd_man, ddi, ddu);
    Cudd_Ref(dda);
    dd_deref(ddi);
    dd_deref(ddu);
    ddu = dda;
    touched.insert(&update.variable());
  }
  /*
   * Conjunction with BDD for guard.
   */
  DdNode* ddg = formula_bdd(command.guard());
  DdNode* ddc = Cudd_bddAnd(dd_man, ddg, ddu);
  Cudd_Ref(ddc);
  dd_deref(ddg);
  dd_deref(ddu);
  /*
   * Conjunction with identity BDD for untouched variables.
   */
  for (ModuleList::const_reverse_iterator mi = model.modules().rbegin();
       mi != model.modules().rend(); mi++) {
    const Module& module = **mi;
    for (VariableList<int>::const_reverse_iterator vi =
	   module.int_variables().rbegin();
	 vi != module.int_variables().rend(); vi++) {
      if (touched.find(*vi) == touched.end()) {
	DdNode* ddi = variable_identity_bdd(**vi);
	DdNode* dda = Cudd_bddAnd(dd_man, ddc, ddi);
	Cudd_Ref(dda);
	dd_deref(ddc);
	dd_deref(ddi);
	ddc = dda;
      }
    }
  }
  for (VariableList<int>::const_reverse_iterator vi =
	 model.int_variables().rbegin();
       vi != model.int_variables().rend(); vi++) {
    if (touched.find(*vi) == touched.end()) {
      DdNode* ddi = variable_identity_bdd(**vi);
      DdNode* dda = Cudd_bddAnd(dd_man, ddc, ddi);
      Cudd_Ref(dda);
      dd_deref(ddc);
      dd_deref(ddi);
      ddc = dda;
    }
  }
  return ddc;
}


/* Initializes DD manager */
void mtbdd_init(const Model& model) {
  if (dd_man == 0) {
    int nbits = 0;
    for (VariableList<int>::const_iterator vi = model.int_variables().begin();
	 vi != model.int_variables().end(); vi++) {
      nbits += allocate_bits(**vi, nbits);
    }
    for (ModuleList::const_iterator mi = model.modules().begin();
	 mi != model.modules().end(); mi++) {
      const Module& module = **mi;
      for (VariableList<int>::const_iterator vi =
	     module.int_variables().begin();
	   vi != module.int_variables().end(); vi++) {
	nbits += allocate_bits(**vi, nbits);
      }
    }
    dd_man = Cudd_Init(2*nbits, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    Cudd_SetEpsilon(dd_man, 1e-15);
  }
}


/* Finalizes DD manager. */
void mtbdd_finalize() {
  if (dd_man != 0) {
    bit_range.clear();
    int unrel = Cudd_CheckZeroRef(dd_man);
    if (unrel != 0) {
      std::cerr << unrel << " unreleased DDs" << std::endl;
    }
    Cudd_Quit(dd_man);
    dd_man = 0;
  }
}


/* Dereferences the given DD. */
void dd_deref(DdNode* dd) {
  Cudd_RecursiveDeref(dd_man, dd);
}


/* Returns the DD for one. */
DdNode* dd_one() {
  return Cudd_ReadOne(dd_man);
}


/* Returns the BDD for zero. */
DdNode* bdd_zero() {
  return Cudd_ReadLogicZero(dd_man);
}


/* Returns the negation of the given DD. */
DdNode* dd_not(DdNode* dd) {
  DdNode* ddn = Cudd_Not(dd);
  Cudd_Ref(ddn);
  return ddn;
}


/* Returns the conjunction of the given BDDs. */
DdNode* bdd_and(DdNode* dd1, DdNode* dd2) {
  DdNode* dda = Cudd_bddAnd(dd_man, dd1, dd2);
  Cudd_Ref(dda);
  return dda;
}


/* Returns an MTBDD representation for the given expression. */
DdNode* expression_mtbdd(const Expression<int>& expr) {
  const Addition<int>* ap = dynamic_cast<const Addition<int>*>(&expr);
  if (ap != 0) {
    return addition_mtbdd(*ap);
  }
  const Subtraction<int>* sp = dynamic_cast<const Subtraction<int>*>(&expr);
  if (sp != 0) {
    return subtraction_mtbdd(*sp);
  }
  const Multiplication<int>* mp =
    dynamic_cast<const Multiplication<int>*>(&expr);
  if (mp != 0) {
    return multiplication_mtbdd(*mp);
  }
  const Constant<int>* cp = dynamic_cast<const Constant<int>*>(&expr);
  if (cp != 0) {
    return constant_mtbdd(*cp);
  }
  const Variable<int>* xp = dynamic_cast<const Variable<int>*>(&expr);
  if (xp != 0) {
    return variable_mtbdd(*xp, false);
  }
  const Value<int>* vp = dynamic_cast<const Value<int>*>(&expr);
  if (vp != 0) {
    return value_mtbdd(*vp);
  }
  throw std::logic_error("unexpected int expression in `expression_mtbdd'");
}


/* Returns an MTBDD representation for the given expression. */
DdNode* expression_mtbdd(const Expression<double>& expr) {
  const Addition<double>* ap = dynamic_cast<const Addition<double>*>(&expr);
  if (ap != 0) {
    return addition_mtbdd(*ap);
  }
  const Subtraction<double>* sp =
    dynamic_cast<const Subtraction<double>*>(&expr);
  if (sp != 0) {
    return subtraction_mtbdd(*sp);
  }
  const Multiplication<double>* mp =
    dynamic_cast<const Multiplication<double>*>(&expr);
  if (mp != 0) {
    return multiplication_mtbdd(*mp);
  }
  const Constant<double>* cp = dynamic_cast<const Constant<double>*>(&expr);
  if (cp != 0) {
    return constant_mtbdd(*cp);
  }
  const Value<double>* vp = dynamic_cast<const Value<double>*>(&expr);
  if (vp != 0) {
    return value_mtbdd(*vp);
  }
  const TypeCast<int, double>* tp =
    dynamic_cast<const TypeCast<int, double>*>(&expr);
  if (tp != 0) {
    return expression_mtbdd(tp->expr());
  }
  throw std::logic_error("unexpected double expression in `expression_mtbdd'");
}


/* Returns a BDD representation for the given formula. */
DdNode* formula_bdd(const StateFormula& formula) {
  const Conjunction* cp = dynamic_cast<const Conjunction*>(&formula);
  if (cp != 0) {
    return conjunction_bdd(*cp);
  }
  const Disjunction* dp = dynamic_cast<const Disjunction*>(&formula);
  if (dp != 0) {
    return disjunction_bdd(*dp);
  }
  const LessThan<int>* lp = dynamic_cast<const LessThan<int>*>(&formula);
  if (lp != 0) {
    return less_than_bdd(*lp);
  }
  const GreaterThan<int>* gp = dynamic_cast<const GreaterThan<int>*>(&formula);
  if (gp != 0) {
    return greater_than_bdd(*gp);
  }
  const Equality<int>* ep = dynamic_cast<const Equality<int>*>(&formula);
  if (ep != 0) {
    return equality_bdd(*ep);
  }
  throw std::logic_error("unexpected formula in `formula_bdd'");
}


/* Returns a BDD representation of the transition matrix for the given
   model. */
DdNode* transition_bdd(const Model& model) {
  DdNode* ddT = Cudd_ReadLogicZero(dd_man);
  Cudd_Ref(ddT);
  for (CommandList::const_reverse_iterator ci = model.commands().rbegin();
       ci != model.commands().rend(); ci++) {
    DdNode* ddc = command_bdd(**ci, model);
    DdNode* ddt = Cudd_bddOr(dd_man, ddT, ddc);
    Cudd_Ref(ddt);
    dd_deref(ddT);
    dd_deref(ddc);
    ddT = ddt;
  }
  return ddT;
}


/* Returns an MTBDD representation of the transition matrix for the
   given model. */
DdNode* transition_mtbdd(const Model& model) {
  DdNode* ddT = Cudd_ReadZero(dd_man);
  Cudd_Ref(ddT);
  for (CommandList::const_reverse_iterator ci = model.commands().rbegin();
       ci != model.commands().rend(); ci++) {
    const Command& c = **ci;
    DdNode* ddc = command_bdd(c, model);
    const Exponential* exp_delay =
      dynamic_cast<const Exponential*>(&c.delay());
    if (exp_delay == 0) {
      throw std::runtime_error("non-exponential distributions not supported"
			       " by numerical engine");
    }
    DdNode* ddr = expression_mtbdd(exp_delay->rate());
    Cudd_Ref(ddr);
    DdNode* ddC = Cudd_BddToAdd(dd_man, ddc);
    Cudd_Ref(ddC);
    dd_deref(ddc);
    ddc = Cudd_addApply(dd_man, Cudd_addTimes, ddC, ddr);
    Cudd_Ref(ddc);
    dd_deref(ddC);
    dd_deref(ddr);
    DdNode* ddt = Cudd_addApply(dd_man, Cudd_addPlus, ddT, ddc);
    Cudd_Ref(ddt);
    dd_deref(ddT);
    dd_deref(ddc);
    ddT = ddt;
  }
  return ddT;
}


/* Returns a BDD representation of the preimage for the given
   states. */
DdNode* preimage_bdd(const Model& model, DdNode* dds) {
  DdNode* ddT = transition_bdd(model);
  int m = Cudd_ReadSize(dd_man)/2;
  DdNode** col_variables = new DdNode*[m];
  int* row_to_col = new int[2*m];
  for (int i = 0; i < m; i++) {
    col_variables[i] = Cudd_bddIthVar(dd_man, 2*i + 1);
    row_to_col[2*i] = 2*i + 1;
    row_to_col[2*i + 1] = 2*i + 1;
  }
  DdNode* col_cube = Cudd_bddComputeCube(dd_man, col_variables, 0, m);
  Cudd_Ref(col_cube);
  delete col_variables;
  DdNode* ddV = dds;
  Cudd_Ref(ddV);
  bool done = false;
  while (!done) {
    DdNode* ddVp = Cudd_bddPermute(dd_man, ddV, row_to_col);
    Cudd_Ref(ddVp);
    DdNode* ddv = Cudd_bddAndAbstract(dd_man, ddT, ddVp, col_cube);
    Cudd_Ref(ddv);
    dd_deref(ddVp);
    DdNode* ddo = Cudd_bddOr(dd_man, ddv, ddV);
    Cudd_Ref(ddo);
    dd_deref(ddv);
    if (ddo == ddV) {
      done = true;
      dd_deref(ddo);
    } else {
      dd_deref(ddV);
      ddV = ddo;
    }
  }
  delete row_to_col;
  dd_deref(col_cube);
  dd_deref(ddT);
  return ddV;
}


/* Returns a BDD representation for the given state. */
DdNode* state_bdd(const Values& values) {
  DdNode* dds = Cudd_ReadOne(dd_man);
  Cudd_Ref(dds);
  for (std::map<const Variable<int>*, int>::const_iterator vi =
	 values.int_values().begin();
       vi != values.int_values().end(); vi++) {
    DdNode* ddv = variable_mtbdd(*(*vi).first, false);
    int c = (*vi).second;
    DdNode* ddx = Cudd_addBddInterval(dd_man, ddv, c, c);
    Cudd_Ref(ddx);
    dd_deref(ddv);
    DdNode* dda = Cudd_bddAnd(dd_man, dds, ddx);
    Cudd_Ref(dda);
    dd_deref(dds);
    dd_deref(ddx);
    dds = dda;
  }
  return dds;
}

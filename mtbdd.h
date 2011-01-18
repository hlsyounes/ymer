#ifndef MTBDD_H
#define MTBDD_H

struct DdNode;
template <typename T> struct Expression;
struct Model;
struct StateFormula;
struct Values;

/* Initializes DD manager */
void mtbdd_init(const Model& model);
/* Finalizes DD manager. */
void mtbdd_finalize();
/* Dereferences the given DD. */
void dd_deref(DdNode* dd);
/* Returns the DD for one. */
DdNode* dd_one();
/* Returns the BDD for zero. */
DdNode* bdd_zero();
/* Returns the negation of the given DD. */
DdNode* dd_not(DdNode* dd);
/* Returns the conjunction of the given BDDs. */
DdNode* bdd_and(DdNode* dd1, DdNode* dd2);
/* Returns an MTBDD representation for the given expression. */
DdNode* expression_mtbdd(const Expression<int>& expr);
DdNode* expression_mtbdd(const Expression<double>& expr);
/* Returns a BDD representation for the given formula. */
DdNode* formula_bdd(const StateFormula& formula);
/* Returns a BDD representation of the transition matrix for the given
   model. */
DdNode* transition_bdd(const Model& model);
/* Returns an MTBDD representation of the transition matrix for the
   given model. */
DdNode* transition_mtbdd(const Model& model);
/* Returns a BDD representation of the preimage for the given
   states. */
DdNode* preimage_bdd(const Model& model, DdNode* dds);
/* Returns a BDD representation for the given state. */
DdNode* state_bdd(const Values& values);


#endif /* MTBDD_H */

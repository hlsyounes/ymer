/* -*-C++-*- */
/*
 * Formulas.
 *
 * Copyright (C) 2003 Carnegie Mellon University
 * Written by Håkan L. S. Younes.
 *
 * Permission is hereby granted to distribute this software for
 * non-commercial research purposes, provided that this copyright
 * notice is included with any such distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
 * SOFTWARE IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU
 * ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
 *
 * $Id: formulas.h,v 1.1 2003-08-10 01:52:37 lorens Exp $
 */
#ifndef FORMULAS_H
#define FORMULAS_H

#include <config.h>
#include <deque>

struct DdManager;
struct DdNode;
struct Expression;
struct SubstitutionMap;
struct ValueMap;


/* ====================================================================== */
/* StateFormula */

/*
 * A state formula.
 */
struct StateFormula {
  /* Register use of the given state formula. */
  static void register_use(const StateFormula* f) {
    if (f != NULL) {
      f->ref_count_++;
    }
  }

  /* Unregister use of the given state formula. */
  static void unregister_use(const StateFormula* f) {
    if (f != NULL) {
      f->ref_count_--;
      if (f->ref_count_ == 0) {
	delete f;
      }
    }
  }

  /* Deletes this state formula. */
  virtual ~StateFormula() {}

  /* Tests if this state formula contains probabilistic elements. */
  virtual bool probabilistic() const = 0;

  /* Returns this state formula subject to the given substitutions. */
  virtual const StateFormula& substitution(const ValueMap& values) const = 0;

  /* Returns this state formula subject to the given substitutions. */
  virtual const StateFormula&
  substitution(const SubstitutionMap& subst) const = 0;

  /* Returns a BDD representation for this state formula. */
  virtual DdNode* bdd(DdManager* dd_man) const = 0;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const = 0;

protected:
  /* Constructs a state formula. */
  StateFormula() : ref_count_(0) {}

private:
  /* Reference counter. */
  mutable size_t ref_count_;
};


/* ====================================================================== */
/* FormulaList */

/*
 * List of formulas.
 */
struct FormulaList : public std::deque<const StateFormula*> {
};


/* ====================================================================== */
/* Conjunction */

struct Conjunction : public StateFormula {
  /* Deletes this conjunction. */
  virtual ~Conjunction();

  /* Adds a conjunct to this conjunction. */
  void add_conjunct(const StateFormula& conjunct);

  /* Returns the conjuncts for this conjunction. */
  const FormulaList& conjuncts() const { return conjuncts_; }

  /* Tests if this state formula contains probabilistic elements. */
  virtual bool probabilistic() const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const StateFormula& substitution(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const Conjunction& substitution(const SubstitutionMap& subst) const;

  /* Returns a BDD representation for this state formula. */
  virtual DdNode* bdd(DdManager* dd_man) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* The conjuncts. */
  FormulaList conjuncts_;
};


/* ====================================================================== */
/* Disjunction */

struct Disjunction : public StateFormula {
  /* Deletes this disjunction. */
  virtual ~Disjunction();

  /* Adds a disjunct to this disjunction. */
  void add_disjunct(const StateFormula& disjunct);

  /* Returns the disjuncts for this disjunction. */
  const FormulaList& disjuncts() const { return disjuncts_; }

  /* Tests if this state formula contains probabilistic elements. */
  virtual bool probabilistic() const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const StateFormula& substitution(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const Disjunction& substitution(const SubstitutionMap& subst) const;

  /* Returns a BDD representation for this state formula. */
  virtual DdNode* bdd(DdManager* dd_man) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* The disjuncts. */
  FormulaList disjuncts_;
};


/* ====================================================================== */
/* Negation */

/*
 * A negated state formula.
 */
struct Negation : public StateFormula {
  /* Constructs a negation. */
  Negation(const StateFormula& negand);

  /* Deletes this negation. */
  virtual ~Negation();

  /* Returns the negated state formula. */
  const StateFormula& negand() const { return *negand_; }

  /* Tests if this state formula contains probabilistic elements. */
  virtual bool probabilistic() const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const StateFormula& substitution(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const Negation& substitution(const SubstitutionMap& subst) const;

  /* Returns a BDD representation for this state formula. */
  virtual DdNode* bdd(DdManager* dd_man) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* The negated state formula. */
  const StateFormula* negand_;
};


/* ====================================================================== */
/* Comparison */

/*
 * A comparison state formula.
 */
struct Comparison : public StateFormula {
  /* Deletes this comparison. */
  virtual ~Comparison();

  /* Returns the first expression of this comparison. */
  const Expression& expr1() const { return *expr1_; }

  /* Returns the second expression of this comparison. */
  const Expression& expr2() const { return *expr2_; }

  /* Tests if this state formula contains probabilistic elements. */
  virtual bool probabilistic() const;

protected:
  /* Constructs a comparison. */
  Comparison(const Expression& expr1, const Expression& expr2);

private:
  /* The first expression. */
  const Expression* expr1_;
  /* The second expression. */
  const Expression* expr2_;
};


/* ====================================================================== */
/* LessThan */

/*
 * A less-than comparison state formula.
 */
struct LessThan : public Comparison {
  /* Constructs a less-than comparison. */
  LessThan(const Expression& expr1, const Expression& expr2);

  /* Returns this state formula subject to the given substitutions. */
  virtual const StateFormula& substitution(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const LessThan& substitution(const SubstitutionMap& subst) const;

  /* Returns a BDD representation for this state formula. */
  virtual DdNode* bdd(DdManager* dd_man) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;
};


/* ====================================================================== */
/* LessThanOrEqual */

/*
 * A less-than-or-equal comparison state formula.
 */
struct LessThanOrEqual : public Comparison {
  /* Constructs a less-than-or-equal comparison. */
  LessThanOrEqual(const Expression& expr1, const Expression& expr2);

  /* Returns this state formula subject to the given substitutions. */
  virtual const StateFormula& substitution(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const LessThanOrEqual&
  substitution(const SubstitutionMap& subst) const;

  /* Returns a BDD representation for this state formula. */
  virtual DdNode* bdd(DdManager* dd_man) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;
};


/* ====================================================================== */
/* GreaterThanOrEqual */

/*
 * A greater-than-or-equal comparison state formula.
 */
struct GreaterThanOrEqual : public Comparison {
  /* Constructs a greater-than-or-equal comparison. */
  GreaterThanOrEqual(const Expression& expr1, const Expression& expr2);

  /* Returns this state formula subject to the given substitutions. */
  virtual const StateFormula& substitution(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const GreaterThanOrEqual&
  substitution(const SubstitutionMap& subst) const;

  /* Returns a BDD representation for this state formula. */
  virtual DdNode* bdd(DdManager* dd_man) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;
};


/* ====================================================================== */
/* GreaterThan */

/*
 * A greater-than comparison state formula.
 */
struct GreaterThan : public Comparison {
  /* Constructs a greater-than comparison. */
  GreaterThan(const Expression& expr1, const Expression& expr2);

  /* Returns this state formula subject to the given substitutions. */
  virtual const StateFormula& substitution(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const GreaterThan& substitution(const SubstitutionMap& subst) const;

  /* Returns a BDD representation for this state formula. */
  virtual DdNode* bdd(DdManager* dd_man) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;
};


/* ====================================================================== */
/* Equality */

/*
 * An equality comparison state formula.
 */
struct Equality : public Comparison {
  /* Constructs an equality comparison. */
  Equality(const Expression& expr1, const Expression& expr2);

  /* Returns this state formula subject to the given substitutions. */
  virtual const StateFormula& substitution(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const Equality& substitution(const SubstitutionMap& subst) const;

  /* Returns a BDD representation for this state formula. */
  virtual DdNode* bdd(DdManager* dd_man) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;
};


/* ====================================================================== */
/* Inequality */

/*
 * An inequality comparison state formula.
 */
struct Inequality : public Comparison {
  /* Constructs an inequality comparison. */
  Inequality(const Expression& expr1, const Expression& expr2);

  /* Returns this state formula subject to the given substitutions. */
  virtual const StateFormula& substitution(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const Inequality& substitution(const SubstitutionMap& subst) const;

  /* Returns a BDD representation for this state formula. */
  virtual DdNode* bdd(DdManager* dd_man) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;
};


#endif /* FORMULAS_H */

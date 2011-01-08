/* -*-C++-*- */
/*
 * Rational numbers.
 *
 * Copyright (C) 2003 Carnegie Mellon University
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
 * $Id: rational.h,v 1.2 2003-11-07 04:26:15 lorens Exp $
 */
#ifndef RATIONAL_H
#define RATIONAL_H

#include <config.h>
#include <iostream>
#include <utility>


/* ====================================================================== */
/* Rational */

/*
 * A rational number.
 */
struct Rational {
  /* Returns the multipliers for the two integers. */
  static std::pair<int, int> multipliers(int n, int m);

  /* Constructs a rational number. */
  Rational(int n = 0) : numerator_(n), denominator_(1) {}

  /* Constructs a rational number. */
  Rational(int n, int m);

  /* Constructs a rational number. */
  Rational(const char* s);

  /* Returns the numerator of this rational number. */
  int numerator() const { return numerator_; }

  /* Returns the denominator of this rational number. */
  int denominator() const { return denominator_; }

  /* Returns the floating point representation of this rational number. */
  double double_value() const { return double(numerator_)/denominator_; }

private:
  /* The numerator. */
  int numerator_;
  /* The denominator. */
  int denominator_;
};

/* Less-than comparison operator for rational numbers. */
bool operator<(const Rational& q, const Rational& p);

/* Less-than-or-equal comparison operator for rational numbers. */
bool operator<=(const Rational& q, const Rational& p);

/* Equality comparison operator for rational numbers. */
bool operator==(const Rational& q, const Rational& p);

/* Inequality comparison operator for rational numbers. */
bool operator!=(const Rational& q, const Rational& p);

/* Greater-than-or-equal comparison operator for rational numbers. */
bool operator>=(const Rational& q, const Rational& p);

/* Greater-than comparison operator for rational numbers. */
bool operator>(const Rational& q, const Rational& p);

/* Addition operator for rational numbers. */
Rational operator+(const Rational& q, const Rational& p);

/* Subtraction operator for rational numbers. */
Rational operator-(const Rational& q, const Rational& p);

/* Multiplication operator for rational numbers. */
Rational operator*(const Rational& q, const Rational& p);

/* Division operator for rational numbers. */
Rational operator/(const Rational& q, const Rational& p);

/* Output operator for rational numbers. */
std::ostream& operator<<(std::ostream& os, const Rational& q);


#endif /* RATIONAL_H */

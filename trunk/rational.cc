/* -*-C++-*- */
/*
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
 * $Id: rational.cc,v 1.2 2003-11-07 04:26:11 lorens Exp $
 */
#include "rational.h"
#include <stdexcept>


/* ====================================================================== */
/* Rational */

/* Returns the greatest common devisor of the two integers. */
static int gcd(int n, int m) {
  int a = abs(n);
  int b = abs(m);
  while (b > 0) {
    int c = b;
    b = a % b;
    a = c;
  }
  return a;
}


/* Returns the least common multiplier of the two integers. */
static int lcm(int n, int m) {
  return n/gcd(n, m)*m;
}


/* Returns the multipliers for the two integers. */
std::pair<int, int> Rational::multipliers(int n, int m) {
  int f = lcm(n, m);
  return std::make_pair(f/n, f/m);
}


/* Constructs a rational number. */
Rational::Rational(int n, int m) {
  if (m == 0) {
    throw std::invalid_argument("division by zero");
  } else {
    int d = gcd(n, m);
    numerator_ = n/d;
    denominator_ = m/d;
    if (denominator_ < 0) {
      numerator_ *= -1;
      denominator_ *= -1;
    }
  }
}


/* Constructs a rational number. */
Rational::Rational(const char* s)
  : numerator_(0) {
  const char* si = s;
  for (; *si != '\0' && *si != '.' && *si != '/'; si++) {
    numerator_ = 10*numerator_ + (*si - '0');
  }
  if (*si == '/') {
    denominator_ = 0;
    for (si++; *si != '\0'; si++) {
      denominator_ = 10*denominator_ + (*si - '0');
    }
    if (denominator_ == 0) {
      throw std::invalid_argument("division by zero");
    }
    int d = gcd(numerator_, denominator_);
    numerator_ /= d;
    denominator_ /= d;
  } else if (*si == '.') {
    int a = numerator_;
    numerator_ = 0;
    denominator_ = 1;
    for (si++; *si != '\0'; si++) {
      numerator_ = 10*numerator_ + (*si - '0');
      denominator_ *= 10;
    }
    int d = gcd(numerator_, denominator_);
    numerator_ /= d;
    denominator_ /= d;
    numerator_ += a*denominator_;
  } else {
    denominator_ = 1;
  }
}


/* Less-than comparison operator for rational numbers. */
bool operator<(const Rational& q, const Rational& p) {
  std::pair<int, int> m =
    Rational::multipliers(q.denominator(), p.denominator());
  return q.numerator()*m.first < p.numerator()*m.second;
}


/* Less-than-or-equal comparison operator for rational numbers. */
bool operator<=(const Rational& q, const Rational& p) {
  std::pair<int, int> m =
    Rational::multipliers(q.denominator(), p.denominator());
  return q.numerator()*m.first <= p.numerator()*m.second;
}


/* Equality comparison operator for rational numbers. */
bool operator==(const Rational& q, const Rational& p) {
  std::pair<int, int> m =
    Rational::multipliers(q.denominator(), p.denominator());
  return q.numerator()*m.first == p.numerator()*m.second;
}


/* Inequality comparison operator for rational numbers. */
bool operator!=(const Rational& q, const Rational& p) {
  std::pair<int, int> m =
    Rational::multipliers(q.denominator(), p.denominator());
  return q.numerator()*m.first != p.numerator()*m.second;
}


/* Greater-than-or-equal comparison operator for rational numbers. */
bool operator>=(const Rational& q, const Rational& p) {
  std::pair<int, int> m =
    Rational::multipliers(q.denominator(), p.denominator());
  return q.numerator()*m.first >= p.numerator()*m.second;
}


/* Greater-than comparison operator for rational numbers. */
bool operator>(const Rational& q, const Rational& p) {
  std::pair<int, int> m =
    Rational::multipliers(q.denominator(), p.denominator());
  return q.numerator()*m.first > p.numerator()*m.second;
}


/* Addition operator for rational numbers. */
Rational operator+(const Rational& q, const Rational& p) {
  std::pair<int, int> m =
    Rational::multipliers(q.denominator(), p.denominator());
  return Rational(q.numerator()*m.first + p.numerator()*m.second,
		  q.denominator()*m.first);
}


/* Subtraction operator for rational numbers. */
Rational operator-(const Rational& q, const Rational& p) {
  std::pair<int, int> m =
    Rational::multipliers(q.denominator(), p.denominator());
  return Rational(q.numerator()*m.first - p.numerator()*m.second,
		  q.denominator()*m.first);
}


/* Multiplication operator for rational numbers. */
Rational operator*(const Rational& q, const Rational& p) {
  int d1 = gcd(q.numerator(), p.denominator());
  int d2 = gcd(p.numerator(), q.denominator());
  return Rational((q.numerator()/d1)*(p.numerator()/d2),
		  (q.denominator()/d2)*(p.denominator()/d1));
}


/* Division operator for rational numbers. */
Rational operator/(const Rational& q, const Rational& p) {
  if (p == 0) {
    throw std::invalid_argument("division by zero");
  }
  int d1 = gcd(q.numerator(), p.numerator());
  int d2 = gcd(p.denominator(), q.denominator());
  return Rational((q.numerator()/d1)*(p.denominator()/d2),
		  (q.denominator()/d2)*(p.numerator()/d1));
}


/* Output operator for rational numbers. */
std::ostream& operator<<(std::ostream& os, const Rational& q) {
  os << q.numerator();
  if (q.denominator() != 1) {
    os << '/' << q.denominator();
  }
  return os;
}

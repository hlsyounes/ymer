/* -*-C++-*- */
/*
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
 * $Id: rational.cc,v 1.1 2003-08-10 01:52:15 lorens Exp $
 */
#include "rational.h"
#include "exceptions.h"


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
    throw Exception("division by zero");
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
      throw Exception("division by zero");
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
    throw Exception("division by zero");
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

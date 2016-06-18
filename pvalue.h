/*
   Biograph computing kernel daemon
   Copyright (C) 2013-2016  Anthony Liekens

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PVALUE__H
#define PVALUE__H

#include <string>
#include <math.h>

#define MAXIT 100
#define EPS 3.0e-7
#define FPMIN 1.0e-30

void nrerror( std::string );
float betai(float a, float b, float x); // Returns the incomplete beta function Ix(a, b).
float betacf(float a, float b, float x); // Used by betai: Evaluates continued fraction for incomplete beta function by modiﬁed Lentz’s method (§5.2).
float gammln(float xx); // Returns the value ln[Γ(xx)] for xx > 0.
float pvalue( float t, float df ); // Compute p-value of t-statistic

#endif

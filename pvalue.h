#ifndef PVALUE__H
#define PVALUE__H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAXIT 100
#define EPS 3.0e-7
#define FPMIN 1.0e-30

void nrerror( char error_text[]);
float betai(float a, float b, float x); // Returns the incomplete beta function Ix(a, b).
float betacf(float a, float b, float x); // Used by betai: Evaluates continued fraction for incomplete beta function by modiﬁed Lentz’s method (§5.2).
float gammln(float xx); // Returns the value ln[Γ(xx)] for xx > 0.
float pvalue( float t, float df ); // Compute p-value of t-statistic

#endif

#include<math.h>

#include "complex.h"

ComplexNumber complex_pow(ComplexNumber a, int n) {
  int r;
  double term;
  ComplexNumber result = { 0, 0 };

  double part_real = pow(a.real, n), part_im = 1;
  double constant = 1;

  for(r = 0; r <= n; r++) {
    term = part_real * part_im * constant;
    term = r%4 < 2 ? term : -term;

    part_im *= a.im;
    part_real /= (a.real == 0) ? 1 : a.real;
    constant *= (n - r) / ((double) r + 1);

    if (r % 2) {
      result.im += term;
    } else {
      result.real += term;
    }
  }

  return result;
}

double diff(ComplexNumber a, ComplexNumber b) {
  return sqrt(pow(a.real - b.real, 2) + pow(a.im - b.im, 2));
}

ComplexNumber add(ComplexNumber a, ComplexNumber b) {
  return (ComplexNumber) {
    a.real + b.real,
    a.im + b.im,
  };
}

double magnitude(ComplexNumber a) {
  return sqrt((a.real * a.real) + (a.im * a.im));
}

double absf(double v) { return v < 0 ? -v : v; }


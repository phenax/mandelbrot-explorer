#include<math.h>

#include "complex.h"

ComplexNumber square(ComplexNumber a) {
  return (ComplexNumber) {
    a.real * a.real - a.im * a.im,
    2 * a.im * a.real,
  };
}

ComplexNumber cube(ComplexNumber a) {
  return (ComplexNumber) {
    pow(a.real, 3) - 3 * a.real * pow(a.im, 2),
    3 * pow(a.real, 2) * a.im - pow(a.im, 3),
  };
}

int fact(int x) {
  if (x <= 1) return 1;
  return x * fact(x - 1);
}

int comb(int n, int r) {
  return fact(n) / (fact(n - r) * fact(r));
}

ComplexNumber complex_pow(ComplexNumber a, int n) {
  int r;
  double term;
  ComplexNumber result = { 0, 0 };

  for(r = 0; r <= n; r++) {
    term = pow(a.real, n - r) * pow(a.im, r) * comb(n, r);
    term = r%4 < 2 ? term : -term;

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


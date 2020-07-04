#include<math.h>

#include "complex.h"

ComplexNumber square(ComplexNumber a) {
  return (ComplexNumber) {
    a.real * a.real - a.im * a.im,
    2 * a.im * a.real,
  };
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



typedef struct complex_t {
   double real, im;
} ComplexNumber;

ComplexNumber complex_pow(ComplexNumber a, int n);

ComplexNumber add(ComplexNumber a, ComplexNumber b);

double diff(ComplexNumber a, ComplexNumber b);

double magnitude(ComplexNumber a);

double absf(double v);

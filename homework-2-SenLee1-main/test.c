#include "FloatCalculate.h"
#include <assert.h>
#include <fenv.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

float random_float(void) {
  static const float min = 1e-15f;
  static const float max = 1e15f;
  float scale = (float)rand() / (float)RAND_MAX;
  float result = min + scale * (max - min);

  int sign = rand() % 2;
  return sign == 0 ? result : -result;
}

void float2bitstring(float f, char *bitstring) {
  uint32_t intRepresentation;
  memcpy(&intRepresentation, &f, sizeof(float));

  for (int i = 31; i >= 0; --i) {
    bitstring[31 - i] = (intRepresentation & (1u << i)) ? '1' : '0';
  }
  bitstring[32] = '\0';
}

int main(void) {
  // This is for your test, you should never call this function in your
  // submissions.
  fesetround(FE_TOWARDZERO);
  // Note that valgrind does not support fesetround,
  // so test will report incorrect if you are using valgrind.

  // srand(0x5eed);

  int count = 1000000;
  while (count--) {
    char a[33], b[33];
    char result[33] = {0};
    float fa = random_float(), fb = random_float();
    float2bitstring(fa, a);
    float2bitstring(fb, b);
    float_add(a, b, result);
    char solution[33];
    float fsolution = fa + fb;
    float2bitstring(fsolution, solution);
    if (memcmp(result, solution, 32) != 0) {
      printf("-------------------------------------count: %d\n", count);
      printf("a:        %s\n", a);
      printf("b:        %s\n", b);
      printf("result:   %s\n", result);
      printf("solution: %s\n", solution);
      printf("------------------------------------------\n");
      fflush(stdout);
      assert(0);
    }
  }

  return 0;
}

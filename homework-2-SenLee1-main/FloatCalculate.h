#ifndef HW2_FLOATCALCULATE_H
#define HW2_FLOATCALCULATE_H

#include <stddef.h>
#include <stdint.h>

// You can modify the following enum and struct
typedef enum {
  NORMALIZED_T = 0,
  DENORMALIZED_T=1,
  INFINITY_T=9999,
  ZERO_T=-1,
  NAN_T=999,
} FloatType;

typedef struct {
  uint32_t sign;
  uint32_t exponent;
  uint32_t mantissa;

  FloatType type;
} Float;

extern const size_t SIGN_BIT;
extern const size_t EXPONENT_BITS;
extern const size_t MANTISSA_BITS;

// You should not modify the signature of this function
void float_add(const char *a, const char *b, char *result);

//static Float parse_bitstring(const char *input);

//static void build_bitstring(Float input, char *output);

//static Float float_add_impl(Float a, Float b);

#endif // HW2_FLOATCALCULATE_H

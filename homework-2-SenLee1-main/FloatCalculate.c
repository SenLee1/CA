#include "FloatCalculate.h"
#include <stdbool.h>

#include<stdio.h>

const size_t SIGN_BIT = 1;
const size_t EXPONENT_BITS = 8;
const size_t MANTISSA_BITS = 23;

void swapFloat(Float* a,Float*b){
  Float tem=*a;
  *a=*b;
  *b=tem;
}

// static int32_t get_norm_bias(void) { return 1 - (1 << (EXPONENT_BITS - 1)); }//-127

// static int32_t get_denorm_bias(void) { return 1 + get_norm_bias(); }//-126

static bool test_rightmost_all_zeros(uint32_t number, size_t bits) {
  uint32_t mask = (1ull << bits) - 1;
  return (number & mask) == 0;
}

static bool test_rightmost_all_ones(uint32_t number, size_t bits) {
 uint32_t mask = (1ull << bits) - 1;
 return (number & mask) == mask;
}

// You can also design a function of your own.
static void build_bitstring(Float input, char *output){
  output[0]=(input.sign==1)?'1':'0';

  for(int i=1;i<=8;++i){
      output[i]=(input.exponent& (1ull<<(8-i)))?'1':'0';
  }
  for(int i=9;i<32;++i)
    output[i]=((input.mantissa)& (1ull<<(31-i)))?'1':'0';
  
}

// You can also design a function of your own.
static Float parse_bitstring(const char *input){
  Float ans;
  ans.sign= (input[0]=='0')?0:1;
  ans.exponent=0;
  ans.mantissa=0;
  for (int i=1;i<=8;++i){
    ans.exponent+=(input[i]=='1')?(1ull<<(EXPONENT_BITS-i)):0;/*(input[i+1]<<(EXPONENT_BITS-i-1));*/
  }

  if (test_rightmost_all_zeros(ans.exponent,EXPONENT_BITS)){
    ans.type= DENORMALIZED_T;
  }
  else{
    ans.type= NORMALIZED_T;
  }
  for(int i=0;i<23;++i)
    ans.mantissa+=(input[i+9]=='1')?(1ull<<(MANTISSA_BITS-i-1)):0;
    //ans.mantissa+=(input[9+i]<<(MANTISSA_BITS-i-1));
  return ans;
}

// You can also design a function of your own.
static Float float_add_impl(Float a, Float b){

  Float ans={0,0,0,NORMALIZED_T};
  if(a.sign==b.sign){
    bool swap=false;
    if(a.type==b.type){
      ans.sign=a.sign;
      ans.type=a.type;
      // int32_t type=a.type;
      uint32_t ansmantissa;
      if(a.exponent<b.exponent){
        swapFloat(&a,&b);
        swap=true;
      }
      else if(a.exponent==b.exponent&&a.mantissa<b.mantissa){
        swapFloat(&a,&b);
        swap=true;
      }
      int32_t E=a.exponent;
      uint32_t nmantissa=b.mantissa<<3;
      bool stick=false;
      int move=a.exponent-b.exponent;
      if(a.type==NORMALIZED_T){
        if (move!=0)
          nmantissa=(1ull<<25)+(nmantissa>>1);
        while(--move>0){
          if(!stick){
            if(nmantissa%2==1)
              stick=true;
          }
          nmantissa=nmantissa>>1;
        }
        if (stick){
          if(nmantissa%2==0)
          ++nmantissa;
        }
        ansmantissa=nmantissa+(a.mantissa<<3);
        if (ansmantissa>>26!=0){
          if(move==-1){
            ansmantissa+=(1ull<<26);
            ansmantissa=ansmantissa&((1ull<<27)-1);
            ansmantissa=ansmantissa>>1;
            ansmantissa+=1ull<<25;
            ++E;
          }
          else{
            ansmantissa=ansmantissa+(1ull<<26);
            ansmantissa=ansmantissa&((1ull<<27)-1);
            ansmantissa=ansmantissa>>1;
            ++E;

          }
        }
        else{
          if(move==-1){
            ++E;
            ansmantissa=ansmantissa>>1;
          }
        } 
        ans.mantissa=ansmantissa>>3;
        ans.exponent=E;
      }
      else{
        //move=
        ansmantissa=nmantissa+(a.mantissa<<3);
        if (ansmantissa>>26!=0){
          ansmantissa=((1ull<<26)-1) &ansmantissa;
          ans.type=NORMALIZED_T;
          ++E;
        }
        ans.mantissa=ansmantissa<<3;
        ans.exponent=E;
      } 
      if (swap)
        swapFloat(&a,&b);
    }
    else{//norm + denorm  =  norm or infity
      ans.type=NORMALIZED_T;
      if(a.type==DENORMALIZED_T){
        swapFloat(&a,&b);
        swap=true;
      }
      ans.sign=a.sign;
      int32_t E=a.exponent;
      bool stick=false;
      int move=a.exponent-b.exponent;
      uint32_t nmantissa=b.mantissa<<3;
      while(move-->0){
        if(!stick){
          if(nmantissa%2==1)
            stick=true;
        }
        nmantissa=nmantissa>>1;
      }
      if (stick){
        if(nmantissa%2==0)
          ++nmantissa;
      }
      int32_t ansmantissa=(a.mantissa<<3)+nmantissa;
      if (ansmantissa>>26!=0){
        ansmantissa=((1ull<<26)-1) &ansmantissa;
        ansmantissa=ansmantissa>>1;
        ++E;
      }
      ans.mantissa=ansmantissa;
      ans.exponent=E;
      if (swap)
        swapFloat(&a,&b);
    }
  }
  else{
    if(a.type==b.type&&a.type==NORMALIZED_T){
      int32_t tema=a.exponent;
      int32_t temb=b.exponent;
      if(tema==temb){
        if(a.mantissa>b.mantissa)
          ans.sign=a.sign;
        else {
          ans.sign=b.sign;
          swapFloat(&a,&b);
        }
      }
      else if(tema>temb)
        ans.sign=a.sign;
      else{
        ans.sign=b.sign;
        swapFloat(&a,&b);
      }
      int32_t E=a.exponent;
      bool stick=false;
      int move=a.exponent-b.exponent;
      uint32_t nmantissa=b.mantissa<<3;
      if(move!=0)
        nmantissa=(1ull<<25)+(nmantissa>>1);
      while(--move>0){
        if(!stick){
          if(nmantissa%2==1)
            stick=true;
        }
        nmantissa=nmantissa>>1;
      }
      if (stick){
        if(nmantissa%2==0)
          ++nmantissa;
      }
      int32_t ansmantissa=(a.mantissa<<3)-nmantissa;
      if(ansmantissa<0){ 
        ansmantissa+=(1ull<<26);
        int32_t tem=1ull<<25;
        int32_t deltlen=1;
        while(!(tem & ansmantissa)||tem==0){
          ++deltlen;
          tem=tem>>1;
        }
        ansmantissa=ansmantissa<<deltlen;
        E-=deltlen;
      }
      else if(move==-1){//same exponent: 
        int32_t temmantissa=ansmantissa;
        int change=0;
        while(temmantissa!=0){
          temmantissa=temmantissa>>1;
          ++change;
        }
        E-=26-change+1;
        ansmantissa=(ansmantissa<<(26-change+1))-(1ull<<26);
      }
      ans.exponent=E;
      ans.mantissa=ansmantissa>>3;
    }
    else if(a.type==b.type&&a.type==DENORMALIZED_T){
      ans.sign=(a.mantissa>b.mantissa)?a.sign:b.sign;
      int32_t E=(ans.sign==a.sign)?a.mantissa:b.mantissa;
      int32_t ansmantissa=a.mantissa-b.mantissa;
      ans.exponent=E;
      ans.mantissa=ansmantissa;
      ans.type=DENORMALIZED_T;
    }
    else{
      if(a.type==DENORMALIZED_T)
        swapFloat(&a,&b);
      ans.sign=a.sign;
      int32_t E=a.exponent;
      bool stick=false;
      int move=a.exponent-b.exponent;
      uint32_t nmantissa=b.mantissa<<3;
      while(move-->0){
        if(!stick){
          if(nmantissa%2==1)
            stick=true;
        }
        nmantissa=nmantissa>>1;
      }
      if (stick){
        if(nmantissa%2==0)
          ++nmantissa;
      }
      int32_t ansmantissa=(a.mantissa<<3)-nmantissa;
      if(ansmantissa<0){
        ansmantissa+=(1ull<<26);
        int32_t tem=1ull<<25;
        int32_t deltlen=1;
        while(!(tem & ansmantissa)||tem==0){
          ++deltlen;
          tem=tem>>1;
        }
        ansmantissa=ansmantissa<<deltlen;
        E-=deltlen;
      }
      ans.exponent=E;
      ans.mantissa=ansmantissa>>3;
    }
      
  }
  if(test_rightmost_all_ones(ans.exponent,EXPONENT_BITS)&&test_rightmost_all_zeros(ans.mantissa,MANTISSA_BITS))
    ans.type=INFINITY_T;
  else if(test_rightmost_all_ones(ans.exponent,EXPONENT_BITS)&&!test_rightmost_all_zeros(ans.mantissa,MANTISSA_BITS))
    ans.type=NAN_T;
  else if(test_rightmost_all_zeros(ans.exponent,EXPONENT_BITS)&&test_rightmost_all_zeros(ans.mantissa,MANTISSA_BITS))
    ans.type=ZERO_T;
  else if (test_rightmost_all_zeros(ans.exponent,EXPONENT_BITS)&&!test_rightmost_all_zeros(ans.mantissa,MANTISSA_BITS))
    ans.type=DENORMALIZED_T;
  else 
    ans.type=NORMALIZED_T;
  return ans;
}

// You should not modify the signature of this function
void float_add(const char *a, const char *b, char *result) {
  // TODO: Implement this function
  // A possible implementation of the function:
  Float fa = parse_bitstring(a);
  Float fb = parse_bitstring(b);
  Float fresult = float_add_impl(fa, fb);
  build_bitstring(fresult, result);
}


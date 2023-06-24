#pragma once
#include <gmpxx.h>
#include <complex>
#include <string>

const int _ignoreme=[](){
  mpf_set_default_prec(256);
  return 0;
}();

typedef mpf_class Number;

inline std::string mpf_to_string(const mpf_class& f){
  mp_exp_t expo;
  std::string digits=f.get_str(expo);

  if(expo==digits.size()){
    return digits;
  }

  if(expo>digits.size()){
    int zs=expo-digits.size();
    std::string pad(zs,'0');
    return digits+pad;
  }

  digits.insert(expo,".");
  return digits;
}

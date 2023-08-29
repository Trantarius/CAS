#include "Constants.hpp"

const Value Constants::PI=[](){
  Value v;
  v.mode=Value::PI;
  return v;
}();

const Value Constants::E=[](){
  Value v;
  v.mode=Value::E;
  return v;
}();

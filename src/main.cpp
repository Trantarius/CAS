#include <strings.hpp>
#include <io.hpp>
#include "Expr.hpp"

using namespace util;
using namespace std;

int main(){

  Expr add("x+y");
  print(add);

  Expr sub("x-y");
  print(sub);

  Expr mul("x*y");
  print(mul);

  Expr div("x/y");
  print(div);

  Expr pwr("x^y");
  print(pwr);

  Expr quad("2*x^2+5*x+7");
  print(quad);

  return 0;
}

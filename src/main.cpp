#include <strings.hpp>
#include <io.hpp>
#include "Expr.hpp"
#include "test.hpp"

using namespace util;
using namespace std;

int main(){

  test_all();

  VAR(x);
  VAR(y);

  Expr add=x+y;
  Expr mul=x*y;
  Expr sub=x-y;
  Expr div=x/y;
  Expr pwr=x^y;
  Expr neg=-x;

  return 0;
}

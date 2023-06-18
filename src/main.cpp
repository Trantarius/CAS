#include <strings.hpp>
#include <io.hpp>
#include "Expr.hpp"
#include "test.hpp"

using namespace util;

int main(){

  test_all();

  Expr orig("5x+y/(z+1)");

  Expr modified=substitute(orig,{{Expr("5x"),Expr("a")},{Expr("z+1"),Expr("b")}});
  print(orig);
  print(modified);


  return 0;
}

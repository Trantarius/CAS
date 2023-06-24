#include <strings.hpp>
#include <io.hpp>
#include "Expr.hpp"
#include "test.hpp"

using namespace util;

int main(){

  test_all();

  Expr orig("54364573452342364634x+y/(z+01)");

  Expr modified=substitute(orig,{{Expr("5x"),Expr("2.5a")},{Expr("z+1"),Expr("pi b")}});
  print(orig);
  print(modified);

  return 0;
}

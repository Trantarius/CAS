#include <strings.hpp>
#include <io.hpp>
#include "Expr.hpp"
#include "test.hpp"

using namespace util;

int main(){

  test_all();

  Expr ex("(a b)(2d)(x3)");
  print(ex);
  print(ex.tree_view());

  return 0;
}

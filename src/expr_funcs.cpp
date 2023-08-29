#include "Expr.hpp"
#include <misc.hpp>
#include <functional>

using namespace std;
using namespace util;

Expr substitute(Expr ex,ExprMap with){

  auto replace=[](Expr&& ex,const ExprMap& with){
    if(with.contains(ex)){
      return with.at(ex);
    }
    return ex;
  };

  function<Expr(Expr&&)> bound=bind(replace,placeholders::_1,with);
  ex.recurse(bound);
  return ex;
}


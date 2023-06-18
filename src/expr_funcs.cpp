#include "Expr.hpp"
#include <misc.hpp>
#include <functional>

using namespace std;
using namespace util;


Expr expr_recurse(Expr&& ex,function<Expr(Expr&&)> func){

  ex=func(move(ex));
  ex.root->recurse(func);
  return ex;
}

Expr substitute(Expr&& ex,ExprMap with){

  auto replace=[](Expr&& ex,const ExprMap& with){
    if(with.contains(ex)){
      return with.at(ex);
    }
    return ex;
  };

  function<Expr(Expr&&)> bound=bind(replace,placeholders::_1,with);
  return expr_recurse(move(ex),bound);
}

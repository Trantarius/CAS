#include "Expr.hpp"

using namespace std;
using namespace util;


Expr operator + (Expr a,Expr b){
  Sum sum;

  if(a.get_type()==Sum::id){
    sum.sub.swap(a.as<Sum>().sub);
  }
  else{
    sum.sub.push_back(move(a));
  }

  if(b.get_type()==Sum::id){
    sum.sub.splice(sum.sub.end(),b.as<Sum>().sub);
  }
  else{
    sum.sub.push_back(move(b));
  }

  return sum;
}

Expr operator * (Expr a,Expr b){
  Product prod;

  if(a.get_type()==Product::id){
    prod.sub.swap(a.as<Product>().sub);
  }
  else{
    prod.sub.push_back(move(a));
  }

  if(b.get_type()==Product::id){
    prod.sub.splice(prod.sub.end(),b.as<Product>().sub);
  }
  else{
    prod.sub.push_back(move(b));
  }

  return prod;
}

Expr operator - (Expr a,Expr b){
  return move(a)+-move(b);
}

Expr operator / (Expr a,Expr b){
  return move(a)*recip(move(b));
}

Expr operator - (Expr a){
  if(a.get_type()==Negate::id){
    return move(a.as<Negate>().sub);
  }else{
    Negate neg;
    neg.sub=move(a);
    return move(neg);
  }
}

Expr pow(Expr a,Expr b){
  Power ret;
  if(a.get_type()==Power::id){
    ret.base=move(a.as<Power>().base);
    ret.exp = move(a.as<Power>().exp) * move(b);
  }
  else{
    ret.base=move(a);
    ret.exp=move(b);
  }
  return move(ret);
}

Expr recip(Expr a){
  if(a.get_type()==Reciprocal::id){
    return move(a.as<Reciprocal>().sub);
  }else{
    Reciprocal ret;
    ret.sub=move(a);
    return move(ret);
  }
}

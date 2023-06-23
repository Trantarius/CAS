#include "Expr.hpp"

using namespace std;
using namespace util;

/*
Expr Expr::operator()(map<Expr,Expr> with) const{
  return substitute(Expr(*this),move(with));
}
*/

Expr operator + (Expr&& a,Expr&& b){
  Sum* sum=new Sum();

  if(a.get_type()==SUM){
    for(Expr& t : a.as<Sum>().sub){
      sum->sub.push_back(move(t));
    }
  }
  else{
    sum->sub.push_back(move(a));
  }

  if(b.get_type()==SUM){
    for(Expr& t : b.as<Sum>().sub){
      sum->sub.push_back(move(t));
    }
  }
  else{
    sum->sub.push_back(move(b));
  }

  return Expr(NodeRef(sum));
}

Expr operator * (Expr&& a,Expr&& b){
  Product* prod=new Product();

  if(a.get_type()==PRODUCT){
    for(Expr& t : a.as<Product>().sub){
      prod->sub.push_back(move(t));
    }
  }
  else{
    prod->sub.push_back(move(a));
  }

  if(b.get_type()==PRODUCT){
    for(Expr& t : b.as<Product>().sub){
      prod->sub.push_back(move(t));
    }
  }
  else{
    prod->sub.push_back(move(b));
  }

  return Expr(NodeRef(prod));
}

Expr operator - (Expr&& a,Expr&& b){
  return move(a)+-move(b);
}

Expr operator / (Expr&& a,Expr&& b){
  Reciprocal* rec=new Reciprocal();
  rec->sub=move(b);
  return move(a)*Expr(NodeRef(rec));
}

Expr operator ^ (const Expr& a,const Expr& b){
  Power* pwr=new Power();
  pwr->base=a;
  pwr->power=b;
  return Expr(NodeRef(pwr));
}

Expr operator - (Expr&& a){
  Negate* neg=new Negate();
  neg->sub=move(a);
  return Expr(NodeRef(neg));
}

Expr Expr::operator()(ExprMap with) const{
  return substitute(Expr(*this),with);
}

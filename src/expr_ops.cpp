#include "Expr.hpp"

using namespace std;
using namespace util;


Expr operator + (Expr a,Expr b){
  Expr::Sum* sum=new Expr::Sum();

  if(a.root->get_type()==Expr::SUM){
    for(Expr& t : a.as<Expr::Sum>().sub){
      sum->sub.push_back(move(t));
    }
  }
  else{
    sum->sub.push_back(move(a));
  }

  if(b.root->get_type()==Expr::SUM){
    for(Expr& t : b.as<Expr::Sum>().sub){
      sum->sub.push_back(move(t));
    }
  }
  else{
    sum->sub.push_back(move(b));
  }

  return Expr(Expr::NodeRef(sum));
}

Expr operator * (Expr a,Expr b){
  Expr::Product* prod=new Expr::Product();

  if(a.root->get_type()==Expr::PRODUCT){
    for(Expr& t : a.as<Expr::Product>().sub){
      prod->sub.push_back(move(t));
    }
  }
  else{
    prod->sub.push_back(move(a));
  }

  if(b.root->get_type()==Expr::PRODUCT){
    for(Expr& t : b.as<Expr::Product>().sub){
      prod->sub.push_back(move(t));
    }
  }
  else{
    prod->sub.push_back(move(b));
  }

  return Expr(Expr::NodeRef(prod));
}

Expr operator - (Expr a,Expr b){
  return move(a)+-move(b);
}

Expr operator / (Expr a,Expr b){
  Expr::Reciprocal* rec=new Expr::Reciprocal();
  rec->sub=move(b);
  return move(a)*Expr(Expr::NodeRef(rec));
}

Expr operator - (Expr a){
  Expr::Negate* neg=new Expr::Negate();
  neg->sub=move(a);
  return Expr(Expr::NodeRef(neg));
}

Expr Expr::operator()(ExprMap with) const{
  return substitute(Expr(*this),with);
}

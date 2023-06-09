#include "Expr.hpp"
#include <list>

using namespace std;
using namespace util;


string parenthesis(string in){
  return "("+in+")";
}

string Sum::as_text() const{
  string ret;
  for(auto it=sub.begin();it!=sub.end();it++){
    if(it!=sub.begin()){
      ret+=" + ";
    }

    switch(it->get_type()){

      case SUM:
        ret+=parenthesis(it->as_text());
        break;
      case NEGATE:
      case PRODUCT:
      case RECIPROCAL:
      case POWER:
      case VALUE:
      case VARIABLE:
        ret+=(it->as_text());
    }
  }
  return ret;
}

NodeRef Sum::duplicate() const{
  Sum* dupe=new Sum();
  for(auto it=sub.begin();it!=sub.end();it++){
    dupe->sub.push_back(it->duplicate());
  }
  return NodeRef(dupe);
}

bool Sum::operator==(const Node& b) const{
  if(b.get_type()!=SUM){
    return false;
  }

  list<Expr> bsub=((Sum&)b).sub;
  if(sub.size()!=bsub.size()){
    return false;
  }
  auto itA=sub.begin();
  auto itB=bsub.begin();
  while(itA!=sub.end() && itB!=bsub.end()){
    if(*itA!=*itB){
      return false;
    }
    itA++;
    itB++;
  }

  return true;
}


string Product::as_text() const{
  string ret;
  for(auto it=sub.begin();it!=sub.end();it++){
    if(it!=sub.begin()){
      ret+=" * ";
    }

    switch(it->get_type()){

      case SUM:
      case PRODUCT:
      case RECIPROCAL:
      case POWER:
        ret+=parenthesis(it->as_text());
        break;
      case NEGATE:
      case VALUE:
      case VARIABLE:
        ret+=(it->as_text());
    }
  }
  return ret;
}

NodeRef Product::duplicate() const{
  Product* dupe=new Product();
  for(auto it=sub.begin();it!=sub.end();it++){
    dupe->sub.push_back(it->duplicate());
  }
  return NodeRef(dupe);
}

bool Product::operator==(const Node& b) const{
  if(b.get_type()!=PRODUCT){
    return false;
  }

  list<Expr> bsub=((Product&)b).sub;
  if(sub.size()!=bsub.size()){
    return false;
  }
  auto itA=sub.begin();
  auto itB=bsub.begin();
  while(itA!=sub.end() && itB!=bsub.end()){
    if(*itA!=*itB){
      return false;
    }
    itA++;
    itB++;
  }

  return true;
}


string Reciprocal::as_text() const{
  switch(sub.get_type()){

    case SUM:
    case PRODUCT:
    case RECIPROCAL:
    case POWER:
      return "1/"+parenthesis(sub.as_text());
    case NEGATE:
    case VALUE:
    case VARIABLE:
      return "1/"+(sub.as_text());
  }
}

NodeRef Reciprocal::duplicate() const{
  Reciprocal* dupe=new Reciprocal();
  dupe->sub=sub.duplicate();
  return NodeRef(dupe);
}

bool Reciprocal::operator==(const Node& b) const{
  if(b.get_type()!=RECIPROCAL){
    return false;
  }
  return sub==((Reciprocal&)b).sub;
}

string Negate::as_text() const{
  switch(sub.get_type()){

    case SUM:
    case PRODUCT:
    case RECIPROCAL:
    case POWER:
      return "-"+parenthesis(sub.as_text());
    case NEGATE:
    case VALUE:
    case VARIABLE:
      return "-"+(sub.as_text());
  }
}

NodeRef Negate::duplicate() const{
  Negate* dupe=new Negate();
  dupe->sub=sub.duplicate();
  return NodeRef(dupe);
}

bool Negate::operator==(const Node& b) const{
  if(b.get_type()!=NEGATE){
    return false;
  }
  return sub==((Negate&)b).sub;
}

string Power::as_text() const{
  string ret;

  switch(base.get_type()){

    case SUM:
    case PRODUCT:
    case RECIPROCAL:
    case POWER:
    case NEGATE:
      ret+=parenthesis(base.as_text());
      break;
    case VALUE:
    case VARIABLE:
      ret+= (base.as_text());
  }

  ret+="^";

  switch(power.get_type()){

    case SUM:
    case PRODUCT:
    case RECIPROCAL:
    case POWER:
    case NEGATE:
      ret+=parenthesis(power.as_text());
      break;
    case VALUE:
    case VARIABLE:
      ret+= (power.as_text());
  }

  return ret;
}

NodeRef Power::duplicate() const{
  Power* dupe=new Power();
  dupe->base=base.duplicate();
  dupe->power=power.duplicate();
  return NodeRef(dupe);
}

bool Power::operator==(const Node& b) const{
  if(b.get_type()!=POWER){
    return false;
  }
  return base==((Power&)b).base && power==((Power&)b).power;
}

string Value::as_text() const{
  if(mode==PI){
    return "pi";
  }else if(mode==E){
    return "e";
  }else{
    return tostr(integer);
  }
}

NodeRef Value::duplicate() const{
  Value* dupe=new Value();
  dupe->mode=mode;
  dupe->integer=integer;
  return NodeRef(dupe);
}

bool Value::operator==(const Node& b) const{
  return b.get_type()==VALUE && ((Value&)b).mode==mode && (
    mode==INTEGER && integer==((Value&)b).integer ||
    mode!=INTEGER
  );
}

string Variable::as_text() const{
  return name;
}

NodeRef Variable::duplicate() const{
  Variable* dupe=new Variable();
  dupe->name=name;
  dupe->valtype=valtype;
  return NodeRef(dupe);
}

bool Variable::operator==(const Node& b) const{
  return b.get_type()==VARIABLE && name==((Variable&)b).name;
}

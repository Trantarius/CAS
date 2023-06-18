#include "Expr.hpp"
#include <misc.hpp>
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

size_t Sum::hash() const{

  size_t ret=0;
  for(int n=0;n<64;n+=8){
    ret^=SUM<<n;
  }

  int roll=0;
  for(const Expr& e : sub){
    size_t sh=Expr::hash(e);
    sh=bitroll(sh,roll);
    roll+=19;
    ret^=sh;
  }

  return ret;
}

void Sum::recurse(function<Expr(Expr&&)> func){
  for(auto it=sub.begin();it!=sub.end();it++){
    (*it)=func(move(*it));
    (*it).root->recurse(func);
  }
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

size_t Product::hash() const{

  size_t ret=0;
  for(int n=0;n<64;n+=8){
    ret^=PRODUCT<<n;
  }

  int roll=0;
  for(const Expr& e : sub){
    size_t sh=Expr::hash(e);
    sh=bitroll(sh,roll);
    roll+=19;
    ret^=sh;
  }

  return ret;
}

void Product::recurse(function<Expr(Expr&&)> func){
  for(auto it=sub.begin();it!=sub.end();it++){
    (*it)=func(move(*it));
    (*it).root->recurse(func);
  }
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

size_t Reciprocal::hash() const{

  size_t ret=0;
  for(int n=0;n<64;n+=8){
    ret^=RECIPROCAL<<n;
  }

  //making this invertible gives a hacky way of recognizing Reciprocal(Reciprocal(x))==x
  ret^=Expr::hash(sub);

  return ret;
}

void Reciprocal::recurse(function<Expr(Expr&&)> func){
  sub=func(move(sub));
  sub.root->recurse(func);
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

size_t Negate::hash() const{

  size_t ret=0;
  for(int n=0;n<64;n+=8){
    ret^=NEGATE<<n;
  }

  //making this invertible gives a hacky way of recognizing Negate(Negate(x))==x
  ret^=Expr::hash(sub);

  return ret;
}

void Negate::recurse(function<Expr(Expr&&)> func){
  sub=func(move(sub));
  sub.root->recurse(func);
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

size_t Power::hash() const{

  size_t ret=0;
  for(int n=0;n<64;n+=8){
    ret^=POWER<<n;
  }

  ret^=bitroll(Expr::hash(base),13);
  ret^=bitroll(Expr::hash(power),51);

  return ret;
}

void Power::recurse(function<Expr(Expr&&)> func){
  base=func(move(base));
  base.root->recurse(func);

  power=func(move(power));
  power.root->recurse(func);
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

size_t Value::hash() const{

  size_t ret=0;
  for(int n=0;n<64;n+=8){
    ret^=VALUE<<n;
  }
  ret=bitroll(ret,5);

  for(int n=0;n<64;n+=8){
    ret^=mode<<n;
  }

  if(mode==INTEGER || mode==REAL){
    ret^=integer;
  }

  return ret;
}

void Value::recurse(function<Expr(Expr&&)> func){
  //has no children, do nothing
}










string Variable::as_text() const{
  return name;
}

NodeRef Variable::duplicate() const{
  Variable* dupe=new Variable();
  dupe->name=name;
  return NodeRef(dupe);
}

bool Variable::operator==(const Node& b) const{
  return b.get_type()==VARIABLE && name==((Variable&)b).name;
}

size_t Variable::hash() const{

  size_t ret=0;
  for(int n=0;n<64;n+=8){
    ret^=VARIABLE<<n;
  }

  ret^=std::hash<std::string>()(name);

  return ret;
}

void Variable::recurse(function<Expr(Expr&&)> func){
  //has no children, do nothing
}

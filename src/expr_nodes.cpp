#include "Expr.hpp"
#include <misc.hpp>
#include <list>

using namespace std;
using namespace util;


string parenthesis(string in){
  return "("+in+")";
}

list<Expr::Token> Expr::Sum::to_tokens() const{
  list<Token> ret;
  for(auto it=sub.begin();it!=sub.end();it++){
    if(it!=sub.begin()){
      Token tok(Token::OPERATOR);
      tok.oper='+';
      ret.push_back(tok);
    }

    list<Token> toks=it->root->to_tokens();
    Token paren(Token::PARENTHESES);
    paren.subtokens=toks;

    switch(it->root->get_type()){

      case SUM:
        ret.push_back(paren);
        break;
      case NEGATE:
      case PRODUCT:
      case RECIPROCAL:
      case POWER:
      case VALUE:
      case VARIABLE:
        ret.splice(ret.end(),toks);
    }
  }
  return ret;
}

Expr::NodeRef Expr::Sum::duplicate() const{
  Sum* dupe=new Sum();
  for(auto it=sub.begin();it!=sub.end();it++){
    dupe->sub.push_back(Expr(*it));
  }
  return NodeRef(dupe);
}

bool Expr::Sum::operator==(const Node& b) const{
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

size_t Expr::Sum::hash() const{

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

void Expr::Sum::recurse(function<Expr(Expr&&)> func){
  for(auto it=sub.begin();it!=sub.end();it++){
    (*it)=func(move(*it));
    (*it).root->recurse(func);
  }
}







list<Expr::Token> Expr::Product::to_tokens() const{
  list<Token> ret;
  for(auto it=sub.begin();it!=sub.end();it++){
    if(it!=sub.begin()){
      Token tok(Token::OPERATOR);
      tok.oper='*';
      ret.push_back(tok);
    }

    list<Token> toks=it->root->to_tokens();
    Token paren(Token::PARENTHESES);
    paren.subtokens=toks;

    switch(it->root->get_type()){

      case SUM:
      case PRODUCT:
        ret.push_back(paren);
        break;
      case NEGATE:
      case RECIPROCAL:
      case POWER:
      case VALUE:
      case VARIABLE:
        ret.splice(ret.end(),toks);
    }
  }
  return ret;
}

Expr::NodeRef Expr::Product::duplicate() const{
  Product* dupe=new Product();
  for(auto it=sub.begin();it!=sub.end();it++){
    dupe->sub.push_back(Expr(*it));
  }
  return NodeRef(dupe);
}

bool Expr::Product::operator==(const Node& b) const{
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

size_t Expr::Product::hash() const{

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

void Expr::Product::recurse(function<Expr(Expr&&)> func){
  for(auto it=sub.begin();it!=sub.end();it++){
    (*it)=func(move(*it));
    (*it).root->recurse(func);
  }
}






list<Expr::Token> Expr::Reciprocal::to_tokens() const{

  list<Token> toks=sub.root->to_tokens();
  Token paren(Token::PARENTHESES);
  paren.subtokens=toks;

  Token op(Token::OPERATOR);
  op.oper='/';
  list<Token> ret;
  ret.push_front(op);


  switch(sub.root->get_type()){

    case SUM:
    case PRODUCT:
    case RECIPROCAL:
    case POWER:
      ret.push_back(paren);
      return ret;
    case NEGATE:
    case VALUE:
    case VARIABLE:
      ret.splice(ret.end(),toks);
      return ret;
  }
}

Expr::NodeRef Expr::Reciprocal::duplicate() const{
  Reciprocal* dupe=new Reciprocal();
  dupe->sub=sub;
  return NodeRef(dupe);
}

bool Expr::Reciprocal::operator==(const Node& b) const{
  if(b.get_type()!=RECIPROCAL){
    return false;
  }
  return sub==((Reciprocal&)b).sub;
}

size_t Expr::Reciprocal::hash() const{

  size_t ret=0;
  for(int n=0;n<64;n+=8){
    ret^=RECIPROCAL<<n;
  }

  //making this invertible gives a hacky way of recognizing Reciprocal(Reciprocal(x))==x
  ret^=Expr::hash(sub);

  return ret;
}

void Expr::Reciprocal::recurse(function<Expr(Expr&&)> func){
  sub=func(move(sub));
  sub.root->recurse(func);
}










list<Expr::Token> Expr::Negate::to_tokens() const{

  list<Token> toks=sub.root->to_tokens();
  Token paren(Token::PARENTHESES);
  paren.subtokens=toks;

  Token op(Token::OPERATOR);
  op.oper='-';
  list<Token> ret;
  ret.push_front(op);


  switch(sub.root->get_type()){

    case SUM:
    case PRODUCT:
    case RECIPROCAL:
    case POWER:
      ret.push_back(paren);
      return ret;
    case NEGATE:
    case VALUE:
    case VARIABLE:
      ret.splice(ret.end(),toks);
      return ret;
  }
}

Expr::NodeRef Expr::Negate::duplicate() const{
  Negate* dupe=new Negate();
  dupe->sub=sub;
  return NodeRef(dupe);
}

bool Expr::Negate::operator==(const Node& b) const{
  if(b.get_type()!=NEGATE){
    return false;
  }
  return sub==((Negate&)b).sub;
}

size_t Expr::Negate::hash() const{

  size_t ret=0;
  for(int n=0;n<64;n+=8){
    ret^=NEGATE<<n;
  }

  //making this invertible gives a hacky way of recognizing Negate(Negate(x))==x
  ret^=Expr::hash(sub);

  return ret;
}

void Expr::Negate::recurse(function<Expr(Expr&&)> func){
  sub=func(move(sub));
  sub.root->recurse(func);
}














list<Expr::Token> Expr::Power::to_tokens() const{

  list<Token> toks=base.root->to_tokens();
  Token paren(Token::PARENTHESES);
  paren.subtokens=toks;

  list<Token> ret;


  switch(base.root->get_type()){

    case SUM:
    case PRODUCT:
    case RECIPROCAL:
    case POWER:
    case NEGATE:
      ret.push_back(paren);
      break;
    case VALUE:
    case VARIABLE:
      ret.splice(ret.end(),toks);
      break;
  }

  Token op(Token::OPERATOR);
  op.oper='^';
  ret.push_back(op);

  toks=power.root->to_tokens();
  paren.subtokens=toks;

  switch(power.root->get_type()){

    case SUM:
    case PRODUCT:
    case RECIPROCAL:
    case POWER:
    case NEGATE:
      ret.push_back(paren);
      break;
    case VALUE:
    case VARIABLE:
      ret.splice(ret.end(),toks);
      break;
  }

  return ret;
}

Expr::NodeRef Expr::Power::duplicate() const{
  Power* dupe=new Power();
  dupe->base=base;
  dupe->power=power;
  return NodeRef(dupe);
}

bool Expr::Power::operator==(const Node& b) const{
  if(b.get_type()!=POWER){
    return false;
  }
  return base==((Power&)b).base && power==((Power&)b).power;
}

size_t Expr::Power::hash() const{

  size_t ret=0;
  for(int n=0;n<64;n+=8){
    ret^=POWER<<n;
  }

  ret^=bitroll(Expr::hash(base),13);
  ret^=bitroll(Expr::hash(power),51);

  return ret;
}

void Expr::Power::recurse(function<Expr(Expr&&)> func){
  base=func(move(base));
  base.root->recurse(func);

  power=func(move(power));
  power.root->recurse(func);
}














list<Expr::Token> Expr::Value::to_tokens() const{
  if(mode==PI){
    Token tok(Token::NAME);
    tok.name="pi";
    list<Token> ret;
    ret.push_back(tok);
    return ret;
  }else if(mode==E){
    Token tok(Token::NAME);
    tok.name="e";
    list<Token> ret;
    ret.push_back(tok);
    return ret;
  }else{
    Token tok(Token::NUMBER);
    tok.number=number;
    list<Token> ret;
    ret.push_back(tok);
    return ret;
  }
}

Expr::NodeRef Expr::Value::duplicate() const{
  Value* dupe=new Value();
  dupe->mode=mode;
  dupe->number=number;
  return NodeRef(dupe);
}

bool Expr::Value::operator==(const Node& b) const{
  return b.get_type()==VALUE && ((Value&)b).mode==mode && (
    mode==NUMBER && number==((Value&)b).number ||
    mode!=NUMBER
  );
}

size_t Expr::Value::hash() const{

  size_t ret=0;
  for(int n=0;n<64;n+=8){
    ret^=VALUE<<n;
  }
  ret=bitroll(ret,5);

  for(int n=0;n<64;n+=8){
    ret^=mode<<n;
  }

  if(mode==NUMBER){
    double n=number.get_d();
    int64_t i=*(int64_t*)&n;
    ret^=i;
  }

  return ret;
}

void Expr::Value::recurse(function<Expr(Expr&&)> func){
  //has no children, do nothing
}










list<Expr::Token> Expr::Variable::to_tokens() const{
  Token tok(Token::NAME);
  tok.name=name;
  list<Token> ret;
  ret.push_back(tok);
  return ret;
}

Expr::NodeRef Expr::Variable::duplicate() const{
  Variable* dupe=new Variable();
  dupe->name=name;
  return NodeRef(dupe);
}

bool Expr::Variable::operator==(const Node& b) const{
  return b.get_type()==VARIABLE && name==((Variable&)b).name;
}

size_t Expr::Variable::hash() const{

  size_t ret=0;
  for(int n=0;n<64;n+=8){
    ret^=VARIABLE<<n;
  }

  ret^=std::hash<std::string>()(name);

  return ret;
}

void Expr::Variable::recurse(function<Expr(Expr&&)> func){
  //has no children, do nothing
}

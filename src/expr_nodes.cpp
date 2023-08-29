#include "Expr.hpp"
#include "ExprNodes.hpp"
#include <misc.hpp>
#include <list>

using namespace std;
using namespace util;


string parenthesis(string in){
  return "("+in+")";
}




// Default

void Expr::recurse(function<Expr(Expr&&)> func){
  *this=func(move(*this));
  root->recurse(func);
}

list<Expr::Token> Expr::to_tokens() const{
  return root->to_tokens();
}

Expr::ExprTypeID Expr::get_type() const{
  return root->get_type();
}

Expr Expr::duplicate() const{
  return root->duplicate();
}

bool Expr::operator==(const Expr& b) const{
  return *root==b;
}

size_t Expr::hash() const{
  return root->hash();
}

bool Expr::is_null() const{
  return root==nullptr || root->is_null();
}

Expr* Expr::operator&(){
  return root.get();
}

const Expr* Expr::operator&() const {
  return root.get();
}

void Expr::operator=(Expr&& b){
  root=unique_ptr<Expr>(b.move_me());
}

void Expr::tree_view_rec(string tab,string branch,string& view) const{
  root->tree_view_rec(tab,branch,view);
}


// Sum

Expr* Sum::move_me() {
  Sum* donor=new Sum();
  donor->sub.swap(sub);
  return donor;
}

Expr::ExprTypeID Sum::get_type() const{
  return Sum::id;
}

void Sum::recurse(function<Expr(Expr&&)> func){
  for(auto it=sub.begin();it!=sub.end();it++){
    (*it)=func(move(*it));
    (*it).recurse(func);
  }
}

list<Expr::Token> Sum::to_tokens() const{
  list<Token> ret;
  for(auto it=sub.begin();it!=sub.end();it++){
    if(it!=sub.begin()){
      Token tok(Token::OPERATOR);
      tok.oper='+';
      ret.push_back(tok);
    }

    list<Token> toks=it->to_tokens();
    Token paren(Token::PARENTHESES);
    paren.subtokens=toks;

    switch(it->get_type()){

      case Sum::id:
        ret.push_back(paren);
        break;
      case Negate::id:
      case Product::id:
      case Reciprocal::id:
      case Power::id:
      case Value::id:
      case Variable::id:
        ret.splice(ret.end(),toks);
    }
  }
  return ret;
}

Expr Sum::duplicate() const{
  Sum* dupe=new Sum();
  for(auto it=sub.begin();it!=sub.end();it++){
    dupe->sub.push_back(it->duplicate());
  }
  return Expr(dupe);
}

bool Sum::operator==(const Expr& b) const{
  if(b.get_type()!=Sum::id){
    return false;
  }
  const list<Expr>& bsub=b.as<Sum>().sub;
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
    ret^=Sum::id<<n;
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

bool Sum::is_null() const {
  return sub.empty();
}

void Sum::operator=(Expr&& b){
  if(b.get_type()!=Sum::id){
    throw logic_error("Attempted to assign to a SubExpr with wrong ExprType");
  }

  sub.swap(b.as<Sum>().sub);
}

void Sum::tree_view_rec(string tab,string branch,string& view) const{
  view+=tab+branch+"Sum\n";

  if(branch=="└─"){
    tab+="  ";
  }
  else if(branch=="├─"){
    tab+="│ ";
  }

  auto it=sub.begin();
  for(;it!=--(sub.end());it++){
    it->tree_view_rec(tab,"├─",view);
  }
  it->tree_view_rec(tab,"└─",view);
}





// Product


Expr* Product::move_me() {
  Product* donor=new Product();
  donor->sub.swap(sub);
  return donor;
}

Expr::ExprTypeID Product::get_type() const{
  return Product::id;
}

void Product::recurse(function<Expr(Expr&&)> func){
  for(auto it=sub.begin();it!=sub.end();it++){
    (*it)=func(move(*it));
    (*it).recurse(func);
  }
}

list<Expr::Token> Product::to_tokens() const{
  list<Token> ret;
  for(auto it=sub.begin();it!=sub.end();it++){
    if(it!=sub.begin()){
      Token tok(Token::OPERATOR);
      tok.oper='*';
      ret.push_back(tok);
    }

    list<Token> toks=it->to_tokens();
    Token paren(Token::PARENTHESES);
    paren.subtokens=toks;

    switch(it->get_type()){

      case Sum::id:
      case Product::id:
        ret.push_back(paren);
        break;
      case Negate::id:
      case Reciprocal::id:
      case Power::id:
      case Value::id:
      case Variable::id:
        ret.splice(ret.end(),toks);
    }
  }
  return ret;
}

Expr Product::duplicate() const{
  Product* dupe=new Product();
  for(auto it=sub.begin();it!=sub.end();it++){
    dupe->sub.push_back(it->duplicate());
  }
  return Expr(dupe);
}

bool Product::operator==(const Expr& b) const{
  if(b.get_type()!=Product::id){
    return false;
  }

  const list<Expr>& bsub=b.as<Product>().sub;
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
    ret^=Product::id<<n;
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

bool Product::is_null() const {
  return sub.empty();
}

void Product::operator=(Expr&& b){
  if(b.get_type()!=Product::id){
    throw logic_error("Attempted to assign to a SubExpr with wrong ExprType");
  }

  sub.swap(b.as<Product>().sub);
}

void Product::tree_view_rec(string tab,string branch,string& view) const{
  view+=tab+branch+"Product\n";
  if(branch=="└─"){
    tab+="  ";
  }
  else if(branch=="├─"){
    tab+="│ ";
  }
  auto it=sub.begin();
  for(;it!=--(sub.end());it++){
    it->tree_view_rec(tab,"├─",view);
  }
  it->tree_view_rec(tab,"└─",view);
}




// Reciprocal

Expr* Reciprocal::move_me() {
  Reciprocal* donor=new Reciprocal();
  donor->sub=move(sub);
  return donor;
}

Expr::ExprTypeID Reciprocal::get_type() const{
  return Reciprocal::id;
}

void Reciprocal::recurse(function<Expr(Expr&&)> func){
  sub=func(move(sub));
  sub.recurse(func);
}

list<Expr::Token> Reciprocal::to_tokens() const{
  list<Token> toks=sub.to_tokens();
  Token paren(Token::PARENTHESES);
  paren.subtokens=toks;

  Token op(Token::OPERATOR);
  op.oper='/';
  list<Token> ret;
  ret.push_front(op);


  switch(sub.get_type()){

    case Sum::id:
    case Product::id:
    case Reciprocal::id:
    case Power::id:
      ret.push_back(paren);
      break;
    case Negate::id:
    case Value::id:
    case Variable::id:
      ret.splice(ret.end(),toks);
      break;
  }

  return ret;
}

Expr Reciprocal::duplicate() const{
  Reciprocal* dupe=new Reciprocal();
  dupe->sub=sub.duplicate();
  return Expr(dupe);
}

bool Reciprocal::operator==(const Expr& b) const{
  if(b.get_type()!=Reciprocal::id){
    return false;
  }
  return sub==b.as<Reciprocal>().sub;
}

size_t Reciprocal::hash() const{

  size_t ret=0;
  for(int n=0;n<64;n+=8){
    ret^=Reciprocal::id<<n;
  }

  //making this invertible gives a hacky way of recognizing Reciprocal(Reciprocal(x))==x
  ret^=Expr::hash(sub);

  return ret;
}

bool Reciprocal::is_null() const {
  return sub.is_null();
}

void Reciprocal::operator=(Expr&& b){
  if(b.get_type()!=Reciprocal::id){
    throw logic_error("Attempted to assign to a SubExpr with wrong ExprType");
  }

  sub=move(b.as<Reciprocal>().sub);
}

void Reciprocal::tree_view_rec(string tab,string branch,string& view) const{
  view+=tab+branch+"Reciprocal\n";
  if(branch=="└─"){
    tab+="  ";
  }
  else if(branch=="├─"){
    tab+="│ ";
  }
  sub.tree_view_rec(tab,"└─",view);
}





// Negate

Expr* Negate::move_me() {
  Negate* donor=new Negate();
  donor->sub=move(sub);
  return donor;
}

Expr::ExprTypeID Negate::get_type() const{
  return Negate::id;
}

void Negate::recurse(function<Expr(Expr&&)> func){
  sub=func(move(sub));
  sub.recurse(func);
}

list<Expr::Token> Negate::to_tokens() const{
  list<Token> toks=sub.to_tokens();
  Token paren(Token::PARENTHESES);
  paren.subtokens=toks;

  Token op(Token::OPERATOR);
  op.oper='-';
  list<Token> ret;
  ret.push_front(op);


  switch(sub.get_type()){

    case Sum::id:
    case Product::id:
    case Reciprocal::id:
    case Power::id:
      ret.push_back(paren);
      break;
    case Negate::id:
    case Value::id:
    case Variable::id:
      ret.splice(ret.end(),toks);
      break;
  }

  return ret;
}

Expr Negate::duplicate() const{
  Negate* dupe=new Negate();
  dupe->sub=sub.duplicate();
  return Expr(dupe);
}

bool Negate::operator==(const Expr& b) const{
  if(b.get_type()!=Negate::id){
    return false;
  }
  return sub==b.as<Negate>().sub;
}

size_t Negate::hash() const{

  size_t ret=0;
  for(int n=0;n<64;n+=8){
    ret^=Negate::id<<n;
  }

  //making this invertible gives a hacky way of recognizing Negate(Negate(x))==x
  ret^=Expr::hash(sub);

  return ret;
}

bool Negate::is_null() const {
  return sub.is_null();
}

void Negate::operator=(Expr&& b){
  if(b.get_type()!=Negate::id){
    throw logic_error("Attempted to assign to a SubExpr with wrong ExprType");
  }

  sub=move(b.as<Negate>().sub);
}

void Negate::tree_view_rec(string tab,string branch,string& view) const{
  view+=tab+branch+"Negate\n";
  if(branch=="└─"){
    tab+="  ";
  }
  else if(branch=="├─"){
    tab+="│ ";
  }
  sub.tree_view_rec(tab,"└─",view);
}





// Power

Expr* Power::move_me() {
  Power* donor=new Power();
  donor->base=move(base);
  donor->exp=move(exp);
  return donor;
}

Expr::ExprTypeID Power::get_type() const{
  return Power::id;
}

void Power::recurse(function<Expr(Expr&&)> func){
  base=func(move(base));
  base.recurse(func);

  exp=func(move(exp));
  exp.recurse(func);
}

list<Expr::Token> Power::to_tokens() const{
  list<Token> toks=base.to_tokens();
  Token paren(Token::PARENTHESES);
  paren.subtokens=toks;

  list<Token> ret;


  switch(base.get_type()){

    case Sum::id:
    case Product::id:
    case Reciprocal::id:
    case Power::id:
    case Negate::id:
      ret.push_back(paren);
      break;
    case Value::id:
    case Variable::id:
      ret.splice(ret.end(),toks);
      break;
  }

  Token op(Token::OPERATOR);
  op.oper='^';
  ret.push_back(op);

  toks=exp.to_tokens();
  paren.subtokens=toks;

  switch(exp.get_type()){

    case Sum::id:
    case Product::id:
    case Reciprocal::id:
    case Power::id:
    case Negate::id:
      ret.push_back(paren);
      break;
    case Value::id:
    case Variable::id:
      ret.splice(ret.end(),toks);
      break;
  }

  return ret;
}

Expr Power::duplicate() const{
  Power* dupe=new Power();
  dupe->base=base.duplicate();
  dupe->exp=exp.duplicate();
  return Expr(dupe);
}

bool Power::operator==(const Expr& b) const{
  if(b.get_type()!=Power::id){
    return false;
  }
  return base==b.as<Power>().base && exp==b.as<Power>().exp;
}

size_t Power::hash() const{

  size_t ret=0;
  for(int n=0;n<64;n+=8){
    ret^=Power::id<<n;
  }

  ret^=bitroll(Expr::hash(base),13);
  ret^=bitroll(Expr::hash(exp),51);

  return ret;
}

bool Power::is_null() const {
  return base.is_null() || exp.is_null();
}

void Power::operator=(Expr&& b){
  if(b.get_type()!=Power::id){
    throw logic_error("Attempted to assign to a SubExpr with wrong ExprType");
  }

  base=move(b.as<Power>().base);
  exp=move(b.as<Power>().exp);
}

void Power::tree_view_rec(string tab,string branch,string& view) const{
  view+=tab+branch+"Power\n";
  if(branch=="└─"){
    tab+="  ";
  }
  else if(branch=="├─"){
    tab+="│ ";
  }
  base.tree_view_rec(tab,"├─",view);
  exp.tree_view_rec(tab,"└─",view);
}






// Value

Expr* Value::move_me(){
  Value* donor=new Value();
  donor->mode=mode;
  donor->number=number;
  return donor;
}

Expr::ExprTypeID Value::get_type() const{
  return Value::id;
}

Expr Value::duplicate() const{
  Value* dupe=new Value();
  dupe->mode=mode;
  dupe->number=number;
  return Expr(dupe);
}

list<Expr::Token> Value::to_tokens() const{
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



bool Value::operator==(const Expr& b) const{
  return b.get_type()==Value::id && b.as<Value>().mode==mode && (
    mode==NUMBER && number==b.as<Value>().number ||
    mode!=NUMBER
  );
}

size_t Value::hash() const{

  size_t ret=0;
  for(int n=0;n<64;n+=8){
    ret^=Value::id<<n;
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

bool Value::is_null() const{
  return false;
}

void Value::operator=(Expr&& b){
  if(b.get_type()!=Value::id){
    throw logic_error("Attempted to assign to a SubExpr with wrong ExprType");
  }

  number=b.as<Value>().number;
  mode=b.as<Value>().mode;
}

void Value::tree_view_rec(string tab,string branch,string& view) const{
  view+=tab+branch+"Value: ";
  switch(mode){
    case PI:
      view+="PI\n";
      break;
    case E:
      view+="E\n";
      break;
    case NUMBER:
      view+=mpf_to_string(number)+"\n";
      break;
  }
}







// Variable

Expr* Variable::move_me(){
  Variable* donor=new Variable();
  donor->name=name;
  return donor;
}

Expr::ExprTypeID Variable::get_type() const {
  return Variable::id;
}

list<Expr::Token> Variable::to_tokens() const{
  Token tok(Token::NAME);
  tok.name=name;
  list<Token> ret;
  ret.push_back(tok);
  return ret;
}

Expr Variable::duplicate() const{
  Variable* dupe=new Variable();
  dupe->name=name;
  return Expr(dupe);
}

bool Variable::operator==(const Expr& b) const{
  return b.get_type()==Variable::id && name==b.as<Variable>().name;
}

size_t Variable::hash() const{

  size_t ret=0;
  for(int n=0;n<64;n+=8){
    ret^=Variable::id<<n;
  }

  ret^=std::hash<std::string>()(name);

  return ret;
}

bool Variable::is_null() const {
  return name.empty();
}

void Variable::operator=(Expr&& b){
  if(b.get_type()!=Variable::id){
    throw logic_error("Attempted to assign to a SubExpr with wrong ExprType");
  }

  name=b.as<Variable>().name;
}

void Variable::tree_view_rec(string tab,string branch,string& view) const{
  view+=tab+branch+"Variable: "+name+"\n";
}

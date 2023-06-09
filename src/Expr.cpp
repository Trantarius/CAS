#include "Expr.hpp"
#include <list>
#include <map>

using namespace std;
using namespace util;


const map<char,uchar> operator_precedence{
  {'+',1},
  {'*',2},
  {'/',3},
  {'^',4},
  {'-',5},//only for negation, not subtraction
  {CHAR_MAX,UINT64_MAX}//fake
};


bool is_name_char(char c){
  return (c>='A' && c<='Z') || (c>='a' && c<='z') || c=='_';
}

bool is_num_char(char c){
  return c>='0' && c<='9';
}

bool is_op_char(char c){
  return c=='+' || c=='-' || c=='*' || c=='/' || c=='^';
}

string trim_whitespace(string in){
  size_t start=in.find_first_not_of(' ');
  size_t end=in.find_last_not_of(' ');
  return in.substr(start,end-start+1);
}

/*
 * given the index of an opening parenthesis, find the index of the corresponding
 * closing parenthesis.
 */
size_t skip_paren(string text,size_t start){
  size_t idx=start+1;
  while(true){
    if(idx>=text.size()){
      throw std::runtime_error("unclosed parentheses");
    }else if(text[idx]=='('){
      idx=skip_paren(text,start);
    }else if(text[idx]==')'){
      break;
    }
    idx++;
  }
  return idx;
}

struct Token{
  const enum Type{NUMBER,NAME,OPERATOR,PARENTHESES} type;
  uint64_t number;
  string name;
  char oper;
  list<Token> subtokens;

  Token(Type type):type(type){}

  operator string(){
    switch(type){
      case NUMBER: return tostr(number);
      case NAME: return name;
      case OPERATOR: return string(&oper,1);
      case PARENTHESES:
        string ret="( ";
        for(Token token : subtokens){
          ret+=(string)token;
        }
        return ret+")";
    }
  }
};


list<Token> tokenize(string text,size_t start,size_t end){
  list<Token> tokens;
  for(size_t idx=start;idx<end;){

    if(text[idx]==' '){
      idx++;
    }

    else if(text[idx]=='('){
      size_t end=skip_paren(text,idx);
      Token tok(Token::PARENTHESES);
      tok.subtokens=tokenize(text,idx+1,end);
      tokens.push_back(tok);
      idx=end+1;
    }

    else if(is_num_char(text[idx])){
      string num;
      while(idx<end && is_num_char(text[idx])){
        num+=text[idx++];
      }
      Token tok(Token::NUMBER);
      tok.number=stoull(num);
      tokens.push_back(tok);
    }

    else if(is_name_char(text[idx])){
      string name;
      while(idx<end && is_name_char(text[idx])){
        name+=text[idx++];
      }
      Token tok(Token::NAME);
      tok.name=name;
      tokens.push_back(tok);
    }

    else if(is_op_char(text[idx])){
      Token tok(Token::OPERATOR);
      tok.oper=text[idx];
      tokens.push_back(tok);
      idx++;
    }

    else{
      throw runtime_error(string("Unknown character in expression (")+tostr(text[idx])+"): "+text[idx]);
    }
  }

  //convert any "a - b" into "a + -b"
  for(auto it=++tokens.begin();it!=--tokens.end();it++){
    if(it->type==Token::OPERATOR && it->oper=='-'){
      auto prev=--list<Token>::iterator(it);
      auto next=++list<Token>::iterator(it);
      if(prev->type!=Token::OPERATOR && next->type!=Token::OPERATOR){
        Token tok(Token::OPERATOR);
        tok.oper='+';
        tokens.insert(it,tok);
      }
    }
  }

  return tokens;
}

string tokens_string(list<Token> tokens){
  string str;
  for(Token token : tokens){
    str+=(string)token+" ";
  }
  return str;
}

list<list<Token>> split_tokens(list<Token> tokens,char oper){
  list<list<Token>> parts;
  parts.push_back(list<Token>());
  auto current_part=parts.begin();
  for(Token token : tokens){
    if(token.type==Token::OPERATOR && token.oper==oper){
      parts.push_back(list<Token>());
      current_part++;
    }else{
      current_part->push_back(token);
    }
  }
  return parts;
}

Expr expr_from_tokens(list<Token> tokens){
  if(tokens.empty()){
    throw logic_error("Can't make expression from empty tokens");
  }

  Expr ret;

  char highest_op=CHAR_MAX;
  for(Token token : tokens){
    if(token.type==Token::OPERATOR && operator_precedence.at(token.oper)<operator_precedence.at(highest_op)){
      highest_op=token.oper;
    }
  }

  if(highest_op==CHAR_MAX){
    if(tokens.size()!=1){
      throw runtime_error("Adjacent tokens without operator: "+tokens_string(tokens));
    }

    Token token=*tokens.begin();

    if(token.type==Token::NAME){
      if(token.name=="e"){
        Value val;
        val.mode=Value::E;
        return Expr(val);
      }
      else if(token.name=="pi"){
        Value val;
        val.mode=Value::PI;
        return Expr(val);
      }else{
        Variable var;
        var.name=token.name;
        return Expr(var);
      }
    }
    else if(token.type==Token::NUMBER){
      Value val;
      val.mode=Value::INTEGER;
      val.integer=token.number;
      return Expr(val);
    }
    else if(token.type==Token::PARENTHESES){
      return expr_from_tokens(token.subtokens);
    }
    else{
      throw runtime_error("unparseable tokens: "+tokens_string(tokens));
    }
  }

  else if(highest_op=='+'){

    Sum sum;
    for(list<Token> part : split_tokens(tokens,'+')){
      sum.sub.push_back(expr_from_tokens(part));
    }
    return Expr(sum);
  }

  else if(highest_op=='*'){

    Product product;
    for(list<Token> part : split_tokens(tokens,'*')){
      product.sub.push_back(expr_from_tokens(part));
    }
    return Expr(product);
  }

  else if(highest_op=='/'){
    list<list<Token>> parts=split_tokens(tokens,'/');

    Reciprocal recip;
    if(parts.size()>2){

      Product denom;
      for(auto it=++parts.begin();it!=parts.end();it++){
        denom.sub.push_back(expr_from_tokens(*it));
      }
      recip.sub=Expr(denom);

    }else{
      recip.sub=expr_from_tokens(*++parts.begin());
    }

    Product prod;
    prod.sub.push_back(expr_from_tokens(*parts.begin()));
    prod.sub.push_back(Expr(recip));

    return Expr(prod);
  }

  else if(highest_op=='^'){

    list<list<Token>> parts=split_tokens(tokens,'^');
    if(parts.size()>2){
      throw runtime_error("Chained ^ is not allowed");
    }

    Power power;
    power.base=expr_from_tokens(*parts.begin());
    power.power=expr_from_tokens(*++parts.begin());

    return Expr(power);
  }

  else if(highest_op=='-'){

    Token start=*tokens.begin();
    if(start.type!=Token::OPERATOR || start.oper!='-'){
      throw runtime_error("invalid expression: "+tokens_string(tokens));
    }
    tokens.pop_front();

    Negate neg;
    neg.sub=expr_from_tokens(tokens);
    return Expr(neg);
  }

  else{
    throw runtime_error("unparseable tokens: "+tokens_string(tokens));
  }
}

Expr::Expr(string text){
  *this=expr_from_tokens(tokenize(text,0,text.size()));
}




































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
  for(const Expr& part : sub){
    bool found=false;
    for(auto it=bsub.begin();it!=bsub.end();it++){
      if(*it==part){
        found=true;
        bsub.erase(it);
        break;
      }
    }
    if(!found){
      return false;
    }
  }
  return bsub.empty();
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
  for(const Expr& part : sub){
    bool found=false;
    for(auto it=bsub.begin();it!=bsub.end();it++){
      if(*it==part){
        found=true;
        bsub.erase(it);
        break;
      }
    }
    if(!found){
      return false;
    }
  }
  return bsub.empty();
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

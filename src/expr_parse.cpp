#include "Expr.hpp"
#include <map>

using namespace std;
using namespace util;
using parse_error = Expr::parse_error;
using Token = Expr::Token;


const map<char,uchar> operator_precedence{
  {'+',1},
  {'*',2},
  {'^',3},
  {'/',4},//only for reciprocal, not division
  {'-',4},//only for negation, not subtraction
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
      throw parse_error("unclosed parentheses");
    }else if(text[idx]=='('){
      idx=skip_paren(text,start);
    }else if(text[idx]==')'){
      break;
    }
    idx++;
  }
  return idx;
}














//convert 'a - b' into 'a + -b'
list<Token> sub_to_add_negate(list<Token> tokens){
  for(auto it=++tokens.begin();it!=--tokens.end();it++){
    if(it->type==Token::OPERATOR && it->oper=='-'){
      auto prev=--list<Token>::iterator(it);
      auto next=++list<Token>::iterator(it);
      if(prev->type!=Token::OPERATOR){
        Token tok(Token::OPERATOR);
        tok.oper='+';
        tokens.insert(it,tok);
      }
    }
  }
  return tokens;
}

//convert 'a / b' into 'a * /b'
list<Token> div_to_mul_recip(list<Token> tokens){
  for(auto it=++tokens.begin();it!=--tokens.end();it++){
    if(it->type==Token::OPERATOR && it->oper=='/'){
      auto prev=--list<Token>::iterator(it);
      auto next=++list<Token>::iterator(it);
      if(prev->type!=Token::OPERATOR){
        Token tok(Token::OPERATOR);
        tok.oper='*';
        tokens.insert(it,tok);
      }
    }
  }
  return tokens;
}

//convert '5n' to '5*n'
list<Token> adj_coefficient(list<Token> tokens){
  for(auto it=tokens.begin();it!=--tokens.end();it++){
    auto next=it;
    next++;
    if(it->type!=Token::OPERATOR && next->type!=Token::OPERATOR){
      Token tok(Token::OPERATOR);
      tok.oper='*';
      tokens.insert(next,tok);
      it++;
    }
  }
  return tokens;
}
















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
      throw parse_error(string("Unknown character in expression (")+tostr(text[idx])+"): "+text[idx]);
    }
  }


  tokens=adj_coefficient(tokens);
  tokens=sub_to_add_negate(tokens);
  tokens=div_to_mul_recip(tokens);

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
    throw parse_error("Can't make expression from empty tokens");
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
      throw parse_error("Adjacent tokens without operator: "+tokens_string(tokens));
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
      throw parse_error("unparseable tokens: "+tokens_string(tokens));
    }
  }

  else if(highest_op=='+'){

    Expr sum;
    list<list<Token>> parts=split_tokens(tokens,'+');
    auto it=parts.begin();
    sum=expr_from_tokens(*it);
    it++;
    for(;it!=parts.end();it++){
      sum = move(sum) + expr_from_tokens(*it);
    }
    return sum;
  }

  else if(highest_op=='*'){

    Expr prod;
    list<list<Token>> parts=split_tokens(tokens,'*');
    auto it=parts.begin();
    prod=expr_from_tokens(*it);
    it++;
    for(;it!=parts.end();it++){
      prod = move(prod) * expr_from_tokens(*it);
    }
    return prod;
  }

  else if(highest_op=='^'){

    list<Token> exptok;
    while(!(tokens.back().type==Token::OPERATOR && tokens.back().oper=='^')){
      exptok.push_front(tokens.back());
      tokens.pop_back();
    }
    tokens.pop_back();

    Power* pwr=new Power();
    pwr->base=expr_from_tokens(tokens);
    pwr->power=expr_from_tokens(exptok);

    return Expr(NodeRef(pwr));
  }

  else if(highest_op=='/'){

    Token start=*tokens.begin();
    if(start.type!=Token::OPERATOR || start.oper!='/'){
      throw parse_error("expected reciprocal: "+tokens_string(tokens));
    }
    tokens.pop_front();

    Reciprocal* rec=new Reciprocal();
    rec->sub=expr_from_tokens(tokens);
    return Expr(NodeRef(rec));
  }

  else if(highest_op=='-'){

    Token start=*tokens.begin();
    if(start.type!=Token::OPERATOR || start.oper!='-'){
      throw parse_error("expected negation: "+tokens_string(tokens));
    }
    tokens.pop_front();

    Negate* neg=new Negate();
    neg->sub=expr_from_tokens(tokens);
    return Expr(NodeRef(neg));
  }

  else{
    throw parse_error("unparseable tokens: "+tokens_string(tokens));
  }
}

Expr::Expr(string text){
  try{
    *this=expr_from_tokens(tokenize(text,0,text.size()));
  }
  catch(parse_error& err){
    throw parse_error(err.what()+string("\nInput: \'")+text+"\'");
  }
}

Expr::Expr(int64_t val){
  if(val>=0){
    Value* vl=new Value();
    vl->mode=Value::INTEGER;
    vl->integer=val;
    root=NodeRef(vl);
  }else{
    Value* vl=new Value();
    vl->mode=Value::INTEGER;
    vl->integer=-val;
    Negate* neg=new Negate();
    neg->sub=Expr(NodeRef(vl));
    root=NodeRef(neg);
  }
}

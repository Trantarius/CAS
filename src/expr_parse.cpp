#include "Expr.hpp"
#include "Constants.hpp"
#include <map>

using namespace std;
using namespace util;
using parse_error = Expr::parse_error;


const map<char,uchar> operator_precedence{
  {'+',1},
  {'*',2},
  {'^',3},
  {'/',4},//only for reciprocal, not division
  {'-',4},//only for negation, not subtraction
  {CHAR_MAX,UINT64_MAX}//fake
};


bool is_name_char(char c){
  return (c>='A' && c<='Z') || (c>='a' && c<='z') || c=='_' || c=='?';
}

bool is_num_char(char c){
  return c>='0' && c<='9' || c=='.';
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




























list<Expr::Token> Expr::string_to_tokens(string text,size_t start,size_t end){
  list<Token> tokens;
  for(size_t idx=start;idx<end;){

    if(text[idx]==' '){
      idx++;
    }

    else if(text[idx]=='('){
      size_t end=skip_paren(text,idx);
      Token tok(Token::PARENTHESES);
      tok.subtokens=string_to_tokens(text,idx+1,end);
      tokens.push_back(tok);
      idx=end+1;
    }

    else if(is_num_char(text[idx])){
      string num;
      while(idx<end && is_num_char(text[idx])){
        num+=text[idx++];
      }
      Token tok(Token::NUMBER);
      tok.number.set_str(num,10);
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



  //convert '5n' to '5*n'
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


  //convert 'a-b' to 'a+-b'
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

  //convert 'a/b' to 'a*/b'
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

string Expr::tokens_to_string(list<Token> tokens){

  //convert '5*n' to '5n'
  for(auto it=++tokens.begin();it!=--tokens.end();it++){
    auto next=it;
    next++;
    auto prev=it;
    prev--;
    if(it->type==Token::OPERATOR && it->oper=='*' && next->type==Token::NAME && prev->type==Token::NUMBER){
      it=--tokens.erase(it);
    }
  }

  //convert 'a+-b' to 'a-b'
  for(auto it=tokens.begin();it!=--tokens.end();it++){
    if(it->type==Token::OPERATOR && it->oper=='+'){
      auto next=++list<Token>::iterator(it);
      if( next->type==Token::OPERATOR && next->oper=='-'){
        it=--tokens.erase(it);
      }
    }
  }

  //convert 'a*/b' to 'a/b'
  for(auto it=tokens.begin();it!=--tokens.end();it++){
    if(it->type==Token::OPERATOR && it->oper=='*'){
      auto next=++list<Token>::iterator(it);
      if(next->type==Token::OPERATOR && next->oper=='/'){
        it=--tokens.erase(it);
      }
    }
  }

  string str;
  for(Token token : tokens){
    str+=(string)token;
  }
  return str;
}

list<list<Expr::Token>> Expr::split_tokens(list<Token> tokens,char oper){
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

Expr Expr::tokens_to_expr(list<Token> tokens){
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
      throw parse_error("Adjacent tokens without operator: "+tokens_to_string(tokens));
    }

    Token token=*tokens.begin();

    if(token.type==Token::NAME){
      if(token.name=="e"){
        return Constants::E;
      }
      else if(token.name=="pi"){
        return Constants::PI;
      }else{
        return Variable(token.name);
      }
    }
    else if(token.type==Token::NUMBER){
      return Value(token.number);
    }
    else if(token.type==Token::PARENTHESES){
      return tokens_to_expr(token.subtokens);
    }
    else{
      throw parse_error("unparseable tokens: "+tokens_to_string(tokens));
    }
  }

  else if(highest_op=='+'){

    Expr sum(new Sum());
    list<list<Token>> parts=split_tokens(tokens,'+');
    for(auto it=parts.begin();it!=parts.end();it++){
      sum = sum + tokens_to_expr(*it);
    }
    return sum;
  }

  else if(highest_op=='*'){

    Expr prod(new Product());
    list<list<Token>> parts=split_tokens(tokens,'*');
    for(auto it=parts.begin();it!=parts.end();it++){
      prod = prod * tokens_to_expr(*it);
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

    return pow(tokens_to_expr(tokens),tokens_to_expr(exptok));
  }

  else if(highest_op=='/'){

    Token start=*tokens.begin();
    if(start.type!=Token::OPERATOR || start.oper!='/'){
      throw parse_error("expected reciprocal: "+tokens_to_string(tokens));
    }
    tokens.pop_front();

    return recip(tokens_to_expr(tokens));
  }

  else if(highest_op=='-'){

    Token start=*tokens.begin();
    if(start.type!=Token::OPERATOR || start.oper!='-'){
      throw parse_error("expected negation: "+tokens_to_string(tokens));
    }
    tokens.pop_front();

    return -tokens_to_expr(tokens);
  }

  else{
    throw parse_error("unparseable tokens: "+tokens_to_string(tokens));
  }
}

list<Expr::Token> Expr::expr_to_tokens(const Expr& ex){
  list<Token> tokens=ex.to_tokens();

  return tokens;
}

Expr::Expr(string text){
  try{
    *this=tokens_to_expr(string_to_tokens(text,0,text.size()));
  }
  catch(parse_error& err){
    throw parse_error(err.what()+string("\nInput: \'")+text+"\'");
  }
}

Expr::Expr(Number val){
  Value* vl=new Value(val);
  root=unique_ptr<Expr>(vl);
}

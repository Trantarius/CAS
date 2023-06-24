#pragma once
#include <strings.hpp>
#include <list>
#include <memory>
#include <cstring>
#include <unordered_map>
#include <functional>

class Expr{

  struct Token;
  struct Node;
  typedef std::unique_ptr<Node> NodeRef;
  enum Type:char{
    SUM='s',NEGATE='n',PRODUCT='p',RECIPROCAL='r',POWER='e',VALUE='l',VARIABLE='b'
  };

  struct Token{
    enum Type{NUMBER,NAME,OPERATOR,PARENTHESES} type;
    uint64_t number;
    std::string name;
    char oper;
    std::list<Token> subtokens;

    Token(Type type):type(type){}

    operator std::string(){
      switch(type){
        case NUMBER: return util::tostr(number);
        case NAME: return name;
        case OPERATOR: return std::string(&oper,1);
        case PARENTHESES: return "("+tokens_to_string(subtokens)+")";
      }
    }
  };

  struct Node{
    virtual void recurse(std::function<Expr(Expr&&)> func)=0;
    virtual Type get_type() const=0;
    virtual std::list<Token> to_tokens() const=0;
    virtual NodeRef duplicate() const=0;
    virtual bool operator==(const Node& b) const=0;
    virtual size_t hash() const=0;
  };

  struct Sum;
  struct Product;
  struct Reciprocal;
  struct Negate;
  struct Power;
  struct Value;
  struct Variable;

  NodeRef root=nullptr;

  void recurse(std::function<Expr(Expr&&)>);

  // internal text functions
  static std::list<Token> string_to_tokens(std::string,size_t,size_t);
  static std::string tokens_to_string(std::list<Token>);
  static std::list<std::list<Token>> split_tokens(std::list<Token> tokens,char oper);
  static Expr tokens_to_expr(std::list<Token>);
  static std::list<Token> expr_to_tokens(const Expr&);

public:
  bool is_null() const{
    return root==nullptr;
  }

  std::string as_text() const{
    return tokens_to_string(expr_to_tokens(*this));
  }
private:
  template<typename T> T& as();

  #define ASFUNC(type,tval)                                                                 \
  template <> type & as<type>(){                                                            \
    if(is_null()){                                                                          \
      throw std::logic_error("Expression is null, cannot be converted to " #type );         \
    }else if(root->get_type()!=tval){                                                       \
      throw std::logic_error("Expression is not " #type );                                  \
    }else{                                                                                  \
      return (type&)*root;                                                                  \
    }                                                                                       \
  }


  ASFUNC(Sum,SUM);
  ASFUNC(Product,PRODUCT);
  ASFUNC(Reciprocal,RECIPROCAL);
  ASFUNC(Negate,NEGATE);
  ASFUNC(Power,POWER);
  ASFUNC(Value,VALUE);
  ASFUNC(Variable,VARIABLE);

  #undef ASFUNC

  Expr(NodeRef&& root):root(std::move(root)){}
  Expr(const NodeRef& root):root(root->duplicate()){}
  Expr(const Node& root):root(root.duplicate()){}

public:

  struct parse_error : std::logic_error{
    parse_error(std::string msg):std::logic_error(msg){}
  };

  Expr(){}
  Expr(Expr&& b):root(std::move(b.root)){}
  Expr(const Expr& b):root(b.root->duplicate()){}
  Expr(std::string text);
  Expr(int64_t val);




  // Operators

  void operator = (const Expr& b){
    root=b.root->duplicate();
  }
  void operator = (Expr&& b){
    root=std::move(b.root);
  }

  operator std::string() const{
    return as_text();
  }

  bool operator==(const Expr& b) const{
    return *root==*b.root;
  }
  bool operator!=(const Expr& b) const{
    return !(*root==*b.root);
  }

  static size_t hash(const Expr& ex){
    return ex.root->hash();
  }
  struct Hash{
    size_t operator()(const Expr& ex) const{
      return Expr::hash(ex);
    }
  };

  Expr operator()(std::unordered_map<Expr,Expr,Expr::Hash> with) const;

  template<typename T> requires std::is_arithmetic<T>::value
  double operator()(T num) const;

  friend Expr operator+(Expr,Expr);
  friend Expr operator-(Expr,Expr);
  friend Expr operator*(Expr,Expr);
  friend Expr operator/(Expr,Expr);
  friend Expr operator-(Expr);



  // Utility Functions

  friend Expr substitute(Expr ex,std::unordered_map<Expr,Expr,Expr::Hash> with);
  friend Expr reduce(Expr ex);

};


using ExprMap=std::unordered_map<Expr,Expr,Expr::Hash>;


#include "ExprNodes.hpp"

#define VAR(name) Expr name(#name)

inline Expr substitute(Expr ex,const Expr& what,const Expr& with){
  return substitute(std::move(ex),ExprMap{{what,with}});
}


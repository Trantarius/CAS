#pragma once
#include <strings.hpp>
#include <list>
#include <memory>
#include <cstring>

enum ValType{SCALAR,VECTOR,MATRIX};
enum ExprType{
  SUM,NEGATE,PRODUCT,RECIPROCAL,POWER,VALUE,VARIABLE
};


class Expr;
class Node;
typedef std::unique_ptr<Node> NodeRef;

struct Sum;
struct Product;
struct Reciprocal;
struct Negate;
struct Power;
struct Value;
struct Variable;

struct Node{

  virtual ExprType get_type() const=0;
  virtual std::string as_text() const=0;
  virtual NodeRef duplicate() const=0;
  virtual bool operator==(const Node& b) const=0;
};

struct Expr{

  struct parse_error : std::logic_error{
    parse_error(std::string msg):std::logic_error(msg){}
  };

  NodeRef root=nullptr;

  ExprType get_type() const{
    return root->get_type();
  }
  std::string as_text() const{
    return root->as_text();
  }
  Expr duplicate() const{
    return *this;
  }

  bool is_null() const{
    return root==nullptr;
  }

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


  Expr(){}
  Expr(Expr&& b):root(std::move(b.root)){}
  Expr(const Expr& b):root(b.root->duplicate()){}
  Expr(const NodeRef root):root(root->duplicate()){}
  Expr(const Node& root):root(root.duplicate()){}
  Expr(std::string text);

  void operator = (const Expr& b){
    root=b.root->duplicate();
  }
  void operator = (Expr&& b){
    root=std::move(b.root);
  }

  operator std::string() const{
    return as_text();
  }

  bool operator==(const Expr& b){
    return *root==*b.root;
  }
};


struct Sum : Node{

  std::list<Expr> sub;

  ExprType get_type() const{
    return SUM;
  }

  std::string as_text() const;
  NodeRef duplicate() const;
  bool operator==(const Node& b) const;
};

struct Product : Node{

  std::list<Expr> sub;

  ExprType get_type() const{
    return PRODUCT;
  }

  std::string as_text() const;
  NodeRef duplicate() const;
  bool operator==(const Node& b) const;
};

struct Reciprocal : Node{
  Expr sub;
  ExprType get_type() const{
    return RECIPROCAL;
  }
  std::string as_text() const;
  NodeRef duplicate() const;
  bool operator==(const Node& b) const;
};

struct Negate : Node{
  Expr sub;
  ExprType get_type() const{
    return NEGATE;
  }
  std::string as_text() const;
  NodeRef duplicate() const;
  bool operator==(const Node& b) const;
};

struct Power : Node{
  Expr base;
  Expr power;
  ExprType get_type() const{
    return POWER;
  }
  std::string as_text() const;
  NodeRef duplicate() const;
  bool operator==(const Node& b) const;
};

struct Value : Node{
  uint64_t integer;
  enum Mode{INTEGER,PI,E} mode;
  ExprType get_type() const{
    return VALUE;
  }
  std::string as_text() const;
  NodeRef duplicate() const;
  bool operator==(const Node& b) const;
};

struct Variable : Node{
  std::string name;
  ValType valtype=SCALAR;
  ExprType get_type() const{
    return VARIABLE;
  }
  std::string as_text() const;
  NodeRef duplicate() const;
  bool operator==(const Node& b) const;
};












#pragma once
#include "Expr.hpp"

struct Expr::Sum : Expr::Node{

  std::list<Expr> sub;

  Type get_type() const{
    return SUM;
  }

  std::list<Token> to_tokens() const;
  NodeRef duplicate() const;
  bool operator==(const Node& b) const;

  size_t hash() const;
private:
  void recurse(std::function<Expr(Expr&&)> func);
};

struct Expr::Product : Expr::Node{

  std::list<Expr> sub;

  Type get_type() const{
    return PRODUCT;
  }

  std::list<Token> to_tokens() const;
  NodeRef duplicate() const;
  bool operator==(const Node& b) const;
  size_t hash() const;
private:
  void recurse(std::function<Expr(Expr&&)> func);
};

struct Expr::Reciprocal : Expr::Node{
  Expr sub;
  Type get_type() const{
    return RECIPROCAL;
  }
  std::list<Token> to_tokens() const;
  NodeRef duplicate() const;
  bool operator==(const Node& b) const;
  size_t hash() const;
private:
  void recurse(std::function<Expr(Expr&&)> func);
};

struct Expr::Negate : Expr::Node{
  Expr sub;
  Type get_type() const{
    return NEGATE;
  }
  std::list<Token> to_tokens() const;
  NodeRef duplicate() const;
  bool operator==(const Node& b) const;
  size_t hash() const;
private:
  void recurse(std::function<Expr(Expr&&)> func);
};

struct Expr::Power : Expr::Node{
  Expr base;
  Expr power;
  Type get_type() const{
    return POWER;
  }
  std::list<Token> to_tokens() const;
  NodeRef duplicate() const;
  bool operator==(const Node& b) const;
  size_t hash() const;
private:
  void recurse(std::function<Expr(Expr&&)> func);
};

struct Expr::Value : Expr::Node{
  Number number;
  enum Mode:uint8_t{NUMBER='N',PI='P',E='E'} mode;
  Type get_type() const{
    return VALUE;
  }
  std::list<Token> to_tokens() const;
  NodeRef duplicate() const;
  bool operator==(const Node& b) const;
  size_t hash() const;
private:
  void recurse(std::function<Expr(Expr&&)> func);
};

struct Expr::Variable : Expr::Node{
  std::string name;
  Type get_type() const{
    return VARIABLE;
  }
  std::list<Token> to_tokens() const;
  NodeRef duplicate() const;
  bool operator==(const Node& b) const;
  Variable(){}
  Variable(std::string name):name(name){}
  size_t hash() const;
private:
  void recurse(std::function<Expr(Expr&&)> func);
};

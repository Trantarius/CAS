#pragma once
#include "Expr.hpp"

struct Sum : Node{

  std::list<Expr> sub;

  ExprType get_type() const{
    return SUM;
  }

  std::string as_text() const;
  NodeRef duplicate() const;
  bool operator==(const Node& b) const;

  size_t hash() const;
private:
  void recurse(std::function<Expr(Expr&&)> func);
};

struct Product : Node{

  std::list<Expr> sub;

  ExprType get_type() const{
    return PRODUCT;
  }

  std::string as_text() const;
  NodeRef duplicate() const;
  bool operator==(const Node& b) const;
  size_t hash() const;
private:
  void recurse(std::function<Expr(Expr&&)> func);
};

struct Reciprocal : Node{
  Expr sub;
  ExprType get_type() const{
    return RECIPROCAL;
  }
  std::string as_text() const;
  NodeRef duplicate() const;
  bool operator==(const Node& b) const;
  size_t hash() const;
private:
  void recurse(std::function<Expr(Expr&&)> func);
};

struct Negate : Node{
  Expr sub;
  ExprType get_type() const{
    return NEGATE;
  }
  std::string as_text() const;
  NodeRef duplicate() const;
  bool operator==(const Node& b) const;
  size_t hash() const;
private:
  void recurse(std::function<Expr(Expr&&)> func);
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
  size_t hash() const;
private:
  void recurse(std::function<Expr(Expr&&)> func);
};

struct Value : Node{
  union{
    uint64_t integer;
    double real;
  };
  enum Mode:uint8_t{INTEGER='I',REAL='R',PI='P',E='E'} mode;
  ExprType get_type() const{
    return VALUE;
  }
  std::string as_text() const;
  NodeRef duplicate() const;
  bool operator==(const Node& b) const;
  size_t hash() const;
private:
  void recurse(std::function<Expr(Expr&&)> func);
};

struct Variable : Node{
  std::string name;
  ExprType get_type() const{
    return VARIABLE;
  }
  std::string as_text() const;
  NodeRef duplicate() const;
  bool operator==(const Node& b) const;
  Variable(){}
  Variable(std::string name):name(name){}
  size_t hash() const;
private:
  void recurse(std::function<Expr(Expr&&)> func);
};

#pragma once
#include "Expr.hpp"


//sets required Expr::* overrides to null so that subexpressions are forced to implement them.
struct SubExpr : Expr{
  virtual Expr* move_me() override = 0;
  virtual void recurse(std::function<Expr(Expr&&)> func) override = 0;
  virtual std::list<Token> to_tokens() const override = 0;
  virtual ExprTypeID get_type() const override = 0;
  virtual Expr duplicate() const override = 0;
  virtual bool operator==(const Expr& b) const override = 0;
  virtual size_t hash() const override = 0;
  virtual bool is_null() const override = 0;
  Expr* operator &() override {return this;}
  const Expr* operator &() const override {return this;}
  virtual void operator =(Expr&& b) override = 0;
  virtual void tree_view_rec(std::string tab,std::string branch,std::string& view) const override = 0;
};


class Sum : public SubExpr{
  std::list<Expr> sub;

public:
  void recurse(std::function<Expr(Expr&&)> func) override;
  std::list<Token> to_tokens() const override;
  Expr* move_me() override;
  ExprTypeID get_type() const override;
  Expr duplicate() const override;
  bool operator==(const Expr& b) const override;
  size_t hash() const override;
  bool is_null() const override;
  void operator=(Expr&& b) override;
  void tree_view_rec(std::string tab,std::string branch,std::string& view) const override;

  static constexpr size_t id='s';

  friend Expr operator+(Expr,Expr);
};

class Product : public SubExpr{
  std::list<Expr> sub;

public:
  void recurse(std::function<Expr(Expr&&)> func) override;
  std::list<Token> to_tokens() const override;
  Expr* move_me() override;
  ExprTypeID get_type() const override;
  Expr duplicate() const override;
  bool operator==(const Expr& b) const override;
  size_t hash() const override;
  bool is_null() const override;
  void operator=(Expr&& b) override;
  void tree_view_rec(std::string tab,std::string branch,std::string& view) const override;

  static constexpr size_t id='p';

  friend Expr operator*(Expr,Expr);
};

class Reciprocal : public SubExpr{
  Expr sub;

public:
  void recurse(std::function<Expr(Expr&&)> func) override;
  std::list<Token> to_tokens() const override;
  Expr* move_me() override;
  ExprTypeID get_type() const override;
  Expr duplicate() const override;
  bool operator==(const Expr& b) const override;
  size_t hash() const override;
  bool is_null() const override;
  void operator=(Expr&& b) override;
  void tree_view_rec(std::string tab,std::string branch,std::string& view) const override;

  static constexpr size_t id='r';

  friend Expr recip(Expr);
};

class Negate : public SubExpr{
  Expr sub;

public:
  void recurse(std::function<Expr(Expr&&)> func) override;
  std::list<Token> to_tokens() const override;
  Expr* move_me() override;
  ExprTypeID get_type() const override;
  Expr duplicate() const override;
  bool operator==(const Expr& b) const override;
  size_t hash() const override;
  bool is_null() const override;
  void operator=(Expr&& b) override;
  void tree_view_rec(std::string tab,std::string branch,std::string& view) const override;

  static constexpr size_t id='n';

  friend Expr operator-(Expr);
};

class Power : public SubExpr{
  Expr base,exp;

public:
  void recurse(std::function<Expr(Expr&&)> func) override;
  std::list<Token> to_tokens() const override;
  Expr* move_me() override;
  ExprTypeID get_type() const override;
  Expr duplicate() const override;
  bool operator==(const Expr& b) const override;
  size_t hash() const override;
  bool is_null() const override;
  void operator=(Expr&& b) override;
  void tree_view_rec(std::string tab,std::string branch,std::string& view) const override;

  static constexpr size_t id='w';

  friend Expr pow(Expr,Expr);
};

class Value : public SubExpr{
  Number number;
  enum Mode:uint8_t{NUMBER='N',PI='P',E='E'} mode;

public:

  void recurse(std::function<Expr(Expr&&)> func) override{}
  std::list<Token> to_tokens() const override;
  Expr* move_me() override;
  ExprTypeID get_type() const override;
  Expr duplicate() const override;
  bool operator==(const Expr& b) const override;
  size_t hash() const override;
  bool is_null() const override;
  void operator=(Expr&& b) override;
  void tree_view_rec(std::string tab,std::string branch,std::string& view) const override;

  static constexpr size_t id='l';

  Value(){}
  Value(Number n):number(n),mode(NUMBER){}

  friend class Constants;
};

class Variable : public SubExpr{
  std::string name;

public:
  void recurse(std::function<Expr(Expr&&)> func) override{}
  std::list<Token> to_tokens() const override;
  Expr* move_me() override;
  ExprTypeID get_type() const override;
  Expr duplicate() const override;
  bool operator==(const Expr& b) const override;
  size_t hash() const override;
  bool is_null() const override;
  void operator=(Expr&& b) override;
  void tree_view_rec(std::string tab,std::string branch,std::string& view) const override;

  static constexpr size_t id='b';

  Variable(){}
  Variable(std::string name):name(name){}
};

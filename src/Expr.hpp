#pragma once
#include <strings.hpp>
#include <list>
#include <memory>
#include <cstring>
#include <unordered_map>
#include <functional>
#include <typeindex>
#include "Number.hpp"

class Expr{
protected:
  //used for string conversion only
  struct Token{
    enum Type{NUMBER,NAME,OPERATOR,PARENTHESES} type;
    Number number;
    std::string name;
    char oper;
    std::list<Token> subtokens;

    Token(Type type):type(type){}

    operator std::string(){
      switch(type){
        case NUMBER: return mpf_to_string(number);
        case NAME: return name;
        case OPERATOR: return std::string(&oper,1);
        case PARENTHESES: return "("+tokens_to_string(subtokens)+")";
      }
    }
  };

  //the actual expr subtype this wraps (if this is an Expr, not subclass of Expr)
  //subclasses ignore this
  std::unique_ptr<Expr> root;

  // internal text functions
  static std::list<Token> string_to_tokens(std::string,size_t,size_t);
  static std::string tokens_to_string(std::list<Token>);
  static std::list<std::list<Token>> split_tokens(std::list<Token> tokens,char oper);
  static Expr tokens_to_expr(std::list<Token>);
  static std::list<Token> expr_to_tokens(const Expr&);


public:
  typedef size_t ExprTypeID;

  //A pointer to an expr that holds this Expr's data. used for moving r-values.
  //guarantees the returned pointer is not managed, so it can become managed by the caller.
  virtual Expr* move_me() {return root.release();};
  virtual void recurse(std::function<Expr(Expr&&)> func);
  virtual std::list<Token> to_tokens() const;
  virtual ExprTypeID get_type() const;
  virtual Expr duplicate() const;
  virtual bool operator==(const Expr& b) const;
  virtual size_t hash() const;
  virtual bool is_null() const;
  virtual Expr* operator &();
  virtual const Expr* operator &() const;
  virtual void operator =(Expr&& b);
  virtual void tree_view_rec(std::string tab,std::string branch,std::string& view) const;

  std::string as_text() const{
    return tokens_to_string(to_tokens());
  }

  std::string tree_view() const{
    std::string ret;
    tree_view_rec("","",ret);
    return ret;
  }

  struct parse_error : std::logic_error{
    parse_error(std::string msg):std::logic_error(msg){}
  };

  Expr(){}
  Expr(Expr&& b):root(b.move_me()){}
  Expr(const Expr& b):Expr(b.duplicate()){}
  Expr(std::string text);
  Expr(Number val);
  template<typename T> requires std::is_arithmetic<T>::value
  Expr(T val):Expr(Number(val)){}
  template<typename T> requires std::is_base_of<Expr,T>::value
  Expr(T* rt):root(rt){}

  // Operators

  void operator = (const Expr& b){
    *this=b.duplicate();
  }

  operator std::string() const{
    return as_text();
  }
  bool operator!=(const Expr& b) const{
    return !(*this==b);
  }

  template<typename ExprType> requires std::is_base_of<Expr,ExprType>::value
  ExprType& as(){
    if(this->get_type()!=ExprType::id){
      throw std::logic_error("Expr tried to convert to wrong subtype");
    }
    return *(ExprType*)&*this;
  }

  template<typename ExprType> requires std::is_base_of<Expr,ExprType>::value
  const ExprType& as() const{
    if(this->get_type()!=ExprType::id){
      throw std::logic_error("Expr tried to convert to wrong subtype");
    }
    return *(ExprType*)&*this;
  }

  static size_t hash(const Expr& ex){
    return ex.hash();
  }
  struct Hash{
    size_t operator()(const Expr& ex) const{
      return Expr::hash(ex);
    }
  };

  Expr operator()(std::unordered_map<Expr,Expr,Expr::Hash> with) const;

  // Utility Functions

  friend Expr substitute(Expr ex,std::unordered_map<Expr,Expr,Expr::Hash> with);

  //remove negation and division in favor of -1*a or a^-1
  friend Expr generalize(Expr ex);//TODO
  friend Expr degeneralize(Expr ex);//TODO

  //perform basic immediate simplification (ie, evaluate operations, cancel out inverses)
  //also try to put it in sum of product of power form
  friend Expr reduce(Expr ex);//TODO


  friend class Rule;
};


Expr operator+(Expr,Expr);
Expr operator-(Expr,Expr);
Expr operator*(Expr,Expr);
Expr operator/(Expr,Expr);
Expr operator-(Expr);
Expr pow(Expr,Expr);
Expr recip(Expr);


using ExprMap=std::unordered_map<Expr,Expr,Expr::Hash>;


#include "ExprNodes.hpp"

#define VAR(name) Expr name(#name)

inline Expr substitute(Expr ex,const Expr& what,const Expr& with){
  return substitute(std::move(ex),ExprMap{{what,with}});
}

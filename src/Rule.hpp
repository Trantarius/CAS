#pragma once
#include "Expr.hpp"




struct Rule{

  struct Result{
    bool worked=false;
    Expr result;
  };
  virtual Result apply(const Expr& ex) const =0;

};

namespace Rules{

  //TODO
  struct Evaluation : Rule{
    Result apply(const Expr& ex) const override;
  };

  //TODO
  struct Distributive : Rule{
    Result apply(const Expr& ex) const override;
  };

  //TODO
  struct RevDistributive : Rule{
    Result apply(const Expr& ex) const override;
  };

  //TODO
  struct NegToMul : Rule{
    Result apply(const Expr& ex) const override;
  };

  //TODO
  struct DivToPow : Rule{
    Result apply(const Expr& ex) const override;
  };

  //TODO
  struct IntPowToMul : Rule{
    Result apply(const Expr& ex) const override;
  };

  //TODO
  struct MulInv : Rule{
    Result apply(const Expr& ex) const override;
  };

  //TODO
  struct AddInv : Rule{
    Result apply(const Expr& ex) const override;
  };
}

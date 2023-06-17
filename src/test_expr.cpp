#include "test.hpp"
#include "Expr.hpp"

TEST_CASE(expr_parse,

  Expr add("x+y");
  ASSERT_EQ(add.as_text(),"x + y");

  Expr sub("x-y");
  ASSERT_EQ(sub.as_text(),"x + -y");

  Expr mul("x*y");
  ASSERT_EQ(mul.as_text(),"x * y");

  Expr div("x/y");
  ASSERT_EQ(div.as_text(),"x * (1/y)");

  Expr pwr("x^y");
  ASSERT_EQ(pwr.as_text(),"x^y");

  Expr quad("2*(x^2)+5*x+7");
  ASSERT_EQ(quad.as_text(),"2 * (x^2) + 5 * x + 7");

  Expr acoff("(a b)(2d)x3");
  ASSERT_EQ(acoff.as_text(),"a * b * 2 * d * x * 3");
)


#define EZ(ex) ASSERT_EQ(ex,Expr(#ex));
TEST_CASE(expr_ops,
  VAR(x);
  VAR(y);
  VAR(z);


  EZ(x+y+z);
  EZ(x*y*z);
  EZ(x-y-z);
  EZ(x/y/z);
  EZ(-x);
  EZ((2*x)*(3*y)*(4*z));

  EZ(x*y+z);
  EZ(x+y*z);
  EZ(x*y-z);
  EZ(x*y/z);
  EZ(x/y+z);
  EZ(y+z/y);
  EZ((x+y)/z);
  EZ(-(x-y)/-z);
  EZ(x- -y);
)
#undef EZ

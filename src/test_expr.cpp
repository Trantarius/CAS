#include "test.hpp"
#include "Expr.hpp"

TEST_CASE(expr_parse,

  Expr add("x+y");
  ASSERT_EQ(add.as_text(),"x + y");

  Expr sub("x-y");
  ASSERT_EQ(sub.as_text(),"x + - y");

  Expr mul("x*y");
  ASSERT_EQ(mul.as_text(),"x * y");

  Expr div("x/y");
  ASSERT_EQ(div.as_text(),"x * / y");

  Expr pwr("x^y");
  ASSERT_EQ(pwr.as_text(),"x ^ y");

  Expr quad("2*(x^2)+5*x+7");
  ASSERT_EQ(quad.as_text(),"2 * x ^ 2 + 5 * x + 7");

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


TEST_CASE(expr_substitute,
  VAR(x);
  VAR(y);
  VAR(z);

  VAR(a);
  VAR(b);
  VAR(c);

  ExprMap map{{x,a},{y,b},{z,c}};

  ASSERT_EQ(substitute(x+y+z,map),a+b+c);
  ASSERT_EQ(substitute(x*y/z,map),a*b/c);
  ASSERT_EQ(substitute(a*x*y,map),a*a*b);
  ASSERT_EQ(substitute((x*2)+(y*4+(z*8)),map),(a*2)+(b*4+(c*8)));
)

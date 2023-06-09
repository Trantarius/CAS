#include <strings.hpp>
#include <io.hpp>
#include "Expr.hpp"
#include "test.hpp"

using namespace util;
using namespace std;

int main(){

  //test_all();
  print(str_rep('a'));
  print(str_rep("123"));
  print(str_rep(123));
  print(str_rep(std::string("123")));
  print(str_rep((const char*)("123")));

  return 0;
}

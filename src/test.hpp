#pragma once
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <strings.hpp>

struct failed_test : std::logic_error{
  failed_test(std::string msg):logic_error(msg){}
};

#define ASSERT_EQ(foo,bar)\
if(!((foo)==(bar))){\
  throw failed_test(util::str_rep(foo)+" != "+util::str_rep(bar)+" at " __FILE__ ":" + util::tostr(__LINE__));\
}

#define ASSERT(cond)                                                                    \
if(!(cond)){                                                                            \
  throw failed_test(#cond " is false at " __FILE__ ":" + util::tostr(__LINE__));     \
}

#define TEST_CASE(casename,test)                                                        \
void test_##casename (){                                                                \
  try{                                                                              \
    test                                                                            \
  }                                                                                 \
  catch(failed_test& ft){                                                           \
    throw ft;                                                                       \
  }                                                                                 \
  catch(std::exception& ex){                                                        \
    throw failed_test(std::string("Test threw ")+typeid(ex).name()+":\n"+ex.what()); \
  }                                                                                 \
  util::print(#casename " test passed");\
}


void test_expr_parse();

inline void test_all(){
  test_expr_parse();
}
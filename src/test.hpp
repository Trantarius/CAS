#pragma once
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <strings.hpp>
#include "Timer.hpp"

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

#define TEST_CASE(casename,...)                                                        \
void test_##casename (){                                                                \
  util::Timer testtimer;\
  double testtook;\
  try{                                                                              \
    testtimer.start();\
    __VA_ARGS__                                                                          \
    testtook=testtimer.stop();\
  }                                                                                 \
  catch(failed_test& ft){                                                           \
    throw ft;                                                                       \
  }                                                                                 \
  catch(std::exception& ex){                                                        \
    throw failed_test(std::string("Test threw ")+typeid(ex).name()+":\n"+ex.what()); \
  }                                                                                 \
  util::print(#casename " test passed in ",util::Timer::format(testtook));\
}


void test_expr_parse();
void test_expr_ops();
void test_expr_substitute();

inline void test_all(){
  test_expr_parse();
  test_expr_ops();
  test_expr_substitute();
}

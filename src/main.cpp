#include <strings.hpp>
#include <io.hpp>

int main(){
  util::print("hello");
  util::bloc<uchar> file=util::readfile("test/test.txt");
  return 0;
}

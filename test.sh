clang++ -w -std=c++20 -ICUtil $(find src -name "*.cpp") -o exe
./exe
rm exe

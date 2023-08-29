clang++ -w -std=c++20 -ICUtil $(find src -name "*.cpp") -lgmp -o exe && \
./exe
rm exe

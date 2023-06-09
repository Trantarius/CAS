g++ -w -O3 -std=c++20 -ICUtil $(find src -name "*.cpp") -o exe
echo
echo compiled
echo
./exe
rm exe

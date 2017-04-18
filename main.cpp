#include "include/format.hpp"
#include <iostream>

int main()
{
    constexpr auto tmp = "  {2} d {}  {} {1} end"_format(2_s, "hello"_s, 3.14_s);
    std::cout << '\'' << tmp.c_str << '\'' << std::endl;
}

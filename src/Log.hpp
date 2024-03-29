#pragma once
#include <iostream>

inline void printLog(const char* action, bool success)
{
    std::cout << (success ? "[OK]" : "[NG]") << ": " << action << std::endl;
}

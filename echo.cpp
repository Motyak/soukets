#include <iostream>

// g++ echo.cpp -o echo
int main()
{
    std::string input;
    while(true)
    {
        std:getline(std::cin, input);
        std::cout << input << std::endl;
    }
}

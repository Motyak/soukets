#include <iostream>

// g++ echo.cpp -o echo
int main()
{
    std::string input, line;

    while(true)
    {
        while (std::getline(std::cin, line))
            input += '\n' + line;
    
        std::cout << input << std::endl;
        std::cin.clear();
        std::clearerr(stdin);
        input = "";
    }
}

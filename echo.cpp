#include <iostream>

int main()
{
    std::string input, line;

    while(true)
    {
        while (std::getline(std::cin, line))
            input += '\n' + line;

        std::cout << input;
        std::cin.clear();
        std::clearerr(stdin);
        input = "";
    }
}

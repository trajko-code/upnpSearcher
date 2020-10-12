#include "InOut.hpp"
#include <limits>

bool InOut::In(std::string& inData)
{
    std::cin >> inData;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if(std::cin.fail())
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if(!(std::cin >> inData))
            return false;
    }

    return true;
}

void InOut::GetLine(std::string& inData)
{
    std::getline(std::cin, inData);
}

void InOut::Out(const std::string outData)
{
    std::cout << outData;
}
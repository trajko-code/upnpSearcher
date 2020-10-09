#pragma once

#include <string>
#include <iostream>

class InOut
{
    private:
        InOut() {}
    
    public:
        static bool In(std::string& inData);
        static void GetLine(std::string& inData);
        static void Out(const std::string outData);
};
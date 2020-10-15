#pragma once

#include <string>
#include <iostream>

class InOut
{   
    public:
        static bool In(std::string& inData);
        static void GetLine(std::string& inData);
        static void Out(const std::string outData);

    private:
        InOut() = default;
        ~InOut() = default;
        InOut(const InOut&) = delete;
        InOut& operator=(const InOut&) = delete;
};
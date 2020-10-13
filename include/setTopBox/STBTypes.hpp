#pragma once

#include <string>
#include <vector>

namespace setTopBox
{
    enum class DirectionType 
    { 
        IN, 
        OUT 
    };

    struct ArgumentDataFromTable{
        std::string type;
        bool sendEvents;
        std::string defaultValue;
        std::string min;
        std::string max;
        std::string step;
        std::vector<std::string> allowedValueList;
    };

    static std::vector<std::string> booleanValues = {"true", "TRUE", "false", "FALSE", "yes", "YES", "no", "NO", "0", "1"};
}
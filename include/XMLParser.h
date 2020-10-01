#pragma once

#include <string>

class XMLParser
{
    private:
        XMLParser() {}
    public:
        static std::string GetTagValue(std::string response, std::string tagName);
        static void RemoveBlanks(std::string& response);
};

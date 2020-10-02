#pragma once

#include <string>

class XMLParser
{
    private:
        XMLParser() {}
    public:
        static std::string GetTagValue(const std::string xmlMessage, std::string tagName);
        static std::string GetTagAttributeValue(const std::string xmlMessage, std::string tagName, std::string attributeName);
        static void RemoveBlanks(std::string& response);
};

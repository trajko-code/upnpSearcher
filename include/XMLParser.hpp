#pragma once

#include <string>

class XMLParser
{
    public:
        static std::string GetTagValue(const std::string xmlMessage, const std::string tagName);
        static std::string GetTagAttributeValue(const std::string xmlMessage, const std::string tagName, const std::string attributeName);
        static void RemoveBlanks(std::string& response);
    
    private:
        XMLParser() = default;
        ~XMLParser() = default;
        XMLParser(const XMLParser&) = delete;
        XMLParser& operator=(const XMLParser&) = delete;
};

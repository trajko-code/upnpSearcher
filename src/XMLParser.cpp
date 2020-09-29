#include "XMLParser.h"

std::string XMLParser::GetTagValue(std::string response, std::string tagName)
{
    unsigned short tagBegin = response.find("<" + tagName + ">") + tagName.length() + 2;
    unsigned short tagEnd = response.find("</" + tagName + ">", tagBegin);

    return response.substr(tagBegin, tagEnd - tagBegin);
}
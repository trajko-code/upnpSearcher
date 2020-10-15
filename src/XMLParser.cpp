#include "XMLParser.hpp"

std::string XMLParser::GetTagValue(const std::string xmlMessage, const std::string tagName)
{
    size_t tagBegin = xmlMessage.find("<" + tagName + ">");
    size_t tagValueBegin;

    if(tagBegin == std::string::npos)
    {
        tagBegin = xmlMessage.find("<" + tagName + " ");
        
        if(tagBegin == std::string::npos)
            return "";
        else
            tagValueBegin = xmlMessage.find(">", tagBegin) + 1;
    }
    else
    {
        tagValueBegin = tagBegin + tagName.length() + 2;
    }
        
    size_t tagValueEnd = xmlMessage.find("</" + tagName + ">", tagValueBegin);

    return xmlMessage.substr(tagValueBegin, tagValueEnd - tagValueBegin);
}

std::string XMLParser::GetTagAttributeValue(const std::string xmlMessage, const std::string tagName, const std::string attributeName)
{
    size_t tagAttrBegin = xmlMessage.find("<" + tagName + " ");
    if(tagAttrBegin == std::string::npos)
        return "";
    
    tagAttrBegin += tagName.length() + 2;

    std::string allTagAttributes = xmlMessage.substr(tagAttrBegin, xmlMessage.find('>', tagAttrBegin) - tagAttrBegin);

    size_t attrNameBegin = allTagAttributes.find(attributeName);

    if(attrNameBegin == std::string::npos)
        return "";

    size_t attrValBegin = attrNameBegin + attributeName.length() + 2;

    return allTagAttributes.substr(attrValBegin, allTagAttributes.find("\"", attrValBegin) - attrValBegin);
}

void XMLParser::RemoveBlanks(std::string& response)
{
    bool tag = false;
    uint beginIndex = 0;

    for(std::string::size_type i = 0; i < response.size(); i++)
    {
        if(response[i] == '<')
        {
            tag = true;
            response.erase(beginIndex, i - beginIndex);
            i = beginIndex + 1; 
        }    
        else if(response[i] == '>')
        {
            tag = false;
            beginIndex = i + 1;
        }    
        
        if(!tag && response[i] != ' ' && response[i] != '\r' && response[i] != '\n')
        {   
            beginIndex = i + 1;
        }
    }
}
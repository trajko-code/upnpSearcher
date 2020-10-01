#include "XMLParser.h"

std::string XMLParser::GetTagValue(std::string response, std::string tagName)
{
    unsigned short tagBegin = response.find("<" + tagName + ">") + tagName.length() + 2;
    unsigned short tagEnd = response.find("</" + tagName + ">", tagBegin);

    return response.substr(tagBegin, tagEnd - tagBegin);
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
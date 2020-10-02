#pragma once

#include <string>
#include "MySocket.h"

#define USER_AGENT "Linux/20.04 UPnP/2.0 upnpSearcher/1.0"

class HTTPCommunicator
{
    private:
        HTTPCommunicator() {}
        static std::string PostBodyFromResponse(std::string postResponse);
    public:
        static std::string GetXMLDescription(std::string XMLUrl, std::string address, std::string port);
        static std::string PostExecuteAction(std::string controlURL, std::string address, std::string port, std::string soapAction, std::string body);
};
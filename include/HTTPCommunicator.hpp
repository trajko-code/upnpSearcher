#pragma once

#include <string>
#include "MySocket.h"
#include "InOut.hpp"

#define USER_AGENT "Linux/20.04 UPnP/2.0 upnpSearcher/1.0"

class HTTPCommunicator
{
    public:
        static std::string GetXMLDescription(const std::string XMLUrl, const std::string address, const std::string port);
        static std::string PostExecuteAction(const std::string controlURL, const std::string address, const std::string port, const std::string soapAction, const std::string body);
        static std::string GetHeaderValue(const std::string httpMsg, const std::string headerName);

    private:
        HTTPCommunicator() = default;
        ~HTTPCommunicator() = default;
        HTTPCommunicator(const HTTPCommunicator&) = delete;
        HTTPCommunicator& operator=(const HTTPCommunicator&) = delete;

        static std::string PostBodyFromResponse(const std::string postResponse);
};
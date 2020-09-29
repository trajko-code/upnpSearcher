#pragma once

#include <string>
#include "MySocket.h"

class HTTPCommunicator
{
    private:
        HTTPCommunicator() {}

    public:
        static std::string GetXMLDescription(std::string XMLUrl, std::string address, std::string port);
};
#include <string>
#include <iostream>
#include <MySocket.h>
#include "STB.h"

#define ALIVE "ssdp:alive"
#define ALL "ssdp:all"
#define ROOTDEVICE "upnp:rootdevice"

class Searcher
{
    private:
        std::string friendlyName;
        std::vector<STB> discoveredSTB;
 
    public:
        Searcher(std::string fName);

        uint16_t SearchBcast(const std::string delay, int searchTime);
        uint16_t SearchBcast(const std::string delay, const std::string target, int searchTime);

        void SearchSTBDescription(std::string stbUuid);

    private:
        void FilterResponse(const std::string response);

        std::string GetHeaderValue(const std::string response, const std::string key);
};
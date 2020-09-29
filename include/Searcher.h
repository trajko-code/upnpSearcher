#include <string>
#include <iostream>
#include <list>
#include "MySocket.h"
#include "STB.h"

#define ALIVE "ssdp:alive"
#define BYEBYE "ssdp:byebye"
#define UPDATE "ssdp:update"
#define ALL "ssdp:all"
#define ROOTDEVICE "upnp:rootdevice"

class Searcher
{
    private:
        std::string friendlyName;
        std::list<std::unique_ptr<STB>> discoveredSTB;
 
    public:
        Searcher(std::string fName);

        uint16_t SearchBcast(const std::string delay, const int searchTime);
        uint16_t SearchBcast(const std::string delay, const std::string target, const int searchTime);

        bool SearchSTBDescription(STB& stb);
        void ShowDetectedSTBs() const;

    private:
        void FilterDiscoveryResponse(const std::string response);
        void FilterMulticastMessage(const std::string response);
        void FillSTBName(const std::string response, STB& stb);
        void FillServiceList(std::string response, STB& stb);
        void ParseServiceFromXML(std::string XMLservice, STB& stb);

        std::unique_ptr<STB> CreateNewSTB(const std::string uuid, const std::string location);
        std::string GetHeaderValue(const std::string response, const std::string key);
        std::string GetTagValue(std::string response, std::string tagName);
};
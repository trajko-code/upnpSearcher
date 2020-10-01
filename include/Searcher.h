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
        std::list<std::shared_ptr<STB>> discoveredSTB;
 
    public:
        Searcher(std::string friendlyName);

        uint16_t SearchBcast(const std::string delay, const int searchTime);
        uint16_t SearchBcast(const std::string delay, const std::string target, const int searchTime);

        int GetDetectedSTBsCount() const;
        void ShowDetectedSTBs() const;
        void ClearDetectedSTBs();
        std::shared_ptr<STB> GetSTB(int uuid);
        
    private:
        void FilterDiscoveryResponse(const std::string response);
        void FilterMulticastMessage(const std::string response);

        std::shared_ptr<STB> CreateNewSTB(const std::string uuid, const std::string location);
        std::string GetHeaderValue(const std::string response, const std::string key);
};
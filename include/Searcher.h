#include <string>
#include <list>
#include <iostream>
#include <MySocket.h>
#include "STB.h"

class Searcher
{
    private:
        std::string friendlyName;
        std::string uuid;
        std::list<STB> discoveredSTB;
 
    public:
        Searcher(std::string fName, std::string uuid);

        void SearchBcast(const std::string delay, int searchTime);
        void SearchBcast(const std::string delay, const std::string target, int searchTime);
        void SearchUnicast(std::string hostname, int port, std::string target);
        void SearchUnicast(uint32_t address, int port, std::string target);

    private:
        void FilterResponse(const std::string response);
        std::string GetHeaderValue(const std::string response, const std::string key);
};
#include <string>
#include <vector>
#include "MySocket.h"
#include "setTopBox/STB.hpp"
#include "InOut.hpp"

#define ALIVE "ssdp:alive"
#define BYEBYE "ssdp:byebye"
#define UPDATE "ssdp:update"
#define ALL "ssdp:all"
#define ROOTDEVICE "upnp:rootdevice"

class Searcher
{
    private:
        std::vector<std::shared_ptr<setTopBox::STB>> discoveredSTB;
 
    public:
        static Searcher& GetInstance() 
        {
            static Searcher instance;
            return instance;
        }

        uint16_t SearchBcast(const std::string delay, const int searchTime);
        uint16_t SearchBcast(const std::string delay, const std::string target, const int searchTime);

        int GetDetectedSTBsCount() const;
        void ShowDetectedSTBs() const;
        void ClearDetectedSTBs();
        std::shared_ptr<setTopBox::STB> GetSTB(unsigned int ordNumber);

    private:
        Searcher() = default;
        ~Searcher() = default;
        Searcher(const Searcher&) = delete;
        Searcher& operator=(const Searcher&) = delete;

        void FilterDiscoveryResponse(const std::string response);
        //void FilterMulticastMessage(const std::string response);

        void TryToAddNewSTB(const std::string usn, const std::string location);
};
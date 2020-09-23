#include <string>
#include <vector>
#include "Service.h"

class STB
{
    private:
        std::string uuid;
        std::string address;
        std::string port;
        std::string configXMLLocation;
        std::string nt;
        std::vector<Service> services;
    
    public:
        STB(std::string uuid, std::string address, std::string port, std::string xmlLocation, std::string nt);
        
        void AddService(const struct Service service);

        std::string GetUUID() const { return this->uuid; }
        std::string GetAddress() const { return this->address; }
        std::string GetPort() const { return this->port; }
        std::string GetXMLLocation() const { return this->configXMLLocation; }
        std::string GetNt() const { return this->nt; }
        
};
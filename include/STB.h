#include <iostream> 
#include <string>
#include <vector>

class STB
{
    public:
        STB(std::string fname, std::string uuid, std::string address, std::string port, std::string xmlLocation, std::string nt);
        STB(std::string uuid, std::string address, std::string port, std::string xmlLocation, std::string nt);
        
        void AddService(std::string type, std::string id, std::string controlURL, std::string eventURL, std::string descriptionURL);
        void ShowMyServices();

        std::string GetFriendlyName() const { return this->friendlyName; }
        std::string GetUUID() const { return this->uuid; }
        std::string GetAddress() const { return this->address; }
        std::string GetPort() const { return this->port; }
        std::string GetXMLLocation() const { return this->configXMLLocation; }
        std::string GetNt() const { return this->nt; }      

        void SetFriendlyName(std::string fname) { this-> friendlyName = fname; }

    private: 
        struct Service{
            std::string type;
            std::string id;
            std::string descriptionURL;
            std::string controlURL;
            std::string eventURL;

            std::string GetNameOfService() const
            {
                ushort nameBegin = this->type.find(":service:") + 9;
                ushort nameEnd = this->type.find(':', nameBegin);
                return this->type.substr(nameBegin, nameEnd - nameBegin);
            }

            std::string GetVersionOfService() const
            {
                ushort versionBegin = this->type.rfind(':') + 1;
                return this->type.substr(versionBegin, this->type.length() - versionBegin);
            }

            std::string GetServiceId() const
            {
                ushort idBegin = this->id.find(":serviceId:") + 11;
                return this->id.substr(idBegin, this->id.length() - idBegin);
            }
        };
        
        std::string friendlyName;
        std::string uuid;
        std::string address;
        std::string port;
        std::string configXMLLocation;
        std::string nt;
        std::vector<Service> services;
};
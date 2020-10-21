#include <string>
#include <vector>
#include <map>
#include <algorithm> 
#include "MySocket.h"
#include "XMLParser.hpp"
#include "HTTPCommunicator.hpp"
#include "InOut.hpp"
#include "setTopBox/KeyCodes.hpp"
#include "setTopBox/Service.hpp"
#include "Config.hpp"

namespace setTopBox
{
    class STB
    {
        private:
            std::string friendlyName;
            std::string uuid;
            std::string deviceType;
            std::string manufacturer;
            std::string serialNumber;
            std::string address;
            std::string port;
            std::string configXMLLocation;
            std::string verificationCode;
            std::vector<Service> services;
            bool paired;

        public:
            STB(std::string uuid, std::string address, std::string port, std::string xmlLocation);

            bool RequireDescription();
            void ShowDescription() const;
            void ShowMyServices() const;
            void ShowServiceActions(int serviceNumber);
            void FillServiceList(std::string XMLResponse);     
            void AddService(const std::string type, const std::string id, 
                const std::string controlURL, const std::string eventURL, const std::string descriptionURL);
            void SearchServiceDescription(const std::string serviceName); 

            bool ExecuteServiceAction(uint serviceNumber, uint actionNumber);

            bool PairToDevice();
            bool SendPairingRequest();
            bool SendPairingCheck(const std::string pin);
            bool CheckIsPaired();
            bool SetDeviceFriendlyName(const std::string fname);
            void ShowKeysName() const;
            bool SendKeyCommand(int key);

            std::string GetFriendlyName() const { return this->friendlyName; }
            std::string GetUUID() const { return this->uuid; }
            std::string GetDeviceType() const { return this->deviceType; }
            std::string GetManufacturer() const { return this->manufacturer; }
            std::string GetSerialNumber() const { return this->serialNumber; }
            std::string GetAddress() const { return this->address; }
            std::string GetPort() const { return this->port; }
            std::string GetXMLLocation() const { return this->configXMLLocation; }
            std::string GetVerificationCode() const { return this->verificationCode; }
            int GetDetectedServicesCount() const { return this->services.size(); }
            std::string GetServiceName(int serviceNumber) const;
            uint GetServiceActionsCount(int serviceNumber) const;

        private:
            void SetFriendlyName(std::string fname) { this->friendlyName = fname; }
            void SetDeviceType(std::string dtype) { this->deviceType = dtype; }
            void SetManufacturer(std::string manufacturer) { this->manufacturer = manufacturer; }    
            void SetSerialNumber(std::string snumber) { this->serialNumber = snumber; }
            void SetVerificationCode(std::string vcode) { this->verificationCode = vcode; }

            void ParseServiceFromXML(const std::string XMLservice);
            std::string ExecuteServiceAction(const std::string serviceName, 
                const std::string actionName, const std::string argumentList);
    };
}
#pragma once

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "XMLParser.hpp"
#include "HTTPCommunicator.hpp"
#include "InOut.hpp"
#include "setTopBox/KeyCodes.hpp"
#include "setTopBox/STBTypes.hpp"

namespace setTopBox
{ 
    typedef std::map<std::string, ArgumentDataFromTable> stateMap;

    class STB
    {
        public:
            STB(std::string uuid, std::string address, std::string port, std::string xmlLocation);

            bool RequireDescription();
            void ShowDescription() const;
            void ShowMyServices() const;
            void ShowServiceActions(int serviceNumber);
            void FillServiceList(const std::string XMLResponse);     
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
            uint GetDetectedServicesCount() const { return this->services.size(); }
            std::string GetServiceName(int serviceNumber) const;
            uint GetServiceActionsCount(int serviceNumber) const;

        private:
            void SetFriendlyName(std::string fname) { this->friendlyName = fname; }
            void SetDeviceType(std::string dtype) { this->deviceType = dtype; }
            void SetManufacturer(std::string manufacturer) { this->manufacturer = manufacturer; }    
            void SetSerialNumber(std::string snumber) { this->serialNumber = snumber; }
            void SetVerificationCode(std::string vcode) { this->verificationCode = vcode; }
            
            void ParseServiceFromXML(const std::string XMLservice);
            std::string ExecuteServiceAction(const std::string serviceName, const std::string actionName, const std::string argumentList);

        private:
            struct Argument
            {
                std::string name;
                DirectionType directionType;
                std::string relatedStateVariable;
                std::string type;
                std::string defaultValue;
                bool sendEvents;
                std::vector<std::string> allowedValueList;
                struct AllowedValueRange
                {
                    std::string minimum;
                    std::string maximum;
                    std::string step;
                } valueRange;

                Argument(std::string name, DirectionType directionType, std::string relatedStateVariable, 
                    std::string type, std::string defaultValue, bool sendEvents, 
                    std::vector<std::string> allowedList, struct AllowedValueRange valueRange);
                void ShowArgument() const;
                std::string GetAdditionalInfo() const;
                std::string GetName() const { return this->name; }
                std::string GetType() const { return this->type; }
                std::string GetDefaultValue() const { return this->defaultValue; }

                bool CorrectType(const std::string argumentValue) const;
                bool CorrectValue(const std::string argumentValue) const;
            };

            struct Action
            {
                std::string name;
                std::vector<Argument> InputParameters;
                std::vector<Argument> OutputParameters;

                Action(std::string name);
                std::string GetName() const { return this->name; }

                void ShowAction() const;
                void AddArgument(Argument& argument);
                void FillArgumentList(const std::string XMLAction, stateMap& stateTable);
                void ParseArgumentFromXML(const std::string argumentXML, stateMap& stateTable);
                
                bool Execute(const std::string STBAddress, const std::string STBPort, const std::string  serviceControlURL, const std::string serviceType);
                std::string MakeSOAPRequestBody(const std::string serviceType, const std::string argumentList);
                std::string MakeArgumentForSOAPBody();               
                void ParseSOAPResponse(const std::string SOAPResponse);
            };

            struct Service{
                std::string type;
                std::string id;
                std::string controlURL;
                std::string eventURL;
                std::string descriptionURL;
                std::vector<Action> actions;

                std::string GetNameOfService() const;
                std::string GetVersionOfService() const;
                std::string GetServiceId() const;
                std::string GetControlUrl() const { return this->controlURL; }
                std::string GetType() const { return this->type; }
                size_t GetActionCount() const { return this->actions.size(); }
    
                void ShowMyActions() const;
                bool RequireServiceDescription(const std::string STBAddress, const std::string STBPort);

                void FillActionList(const std::string XMLServiceResponse);
                std::unique_ptr<stateMap> GetStateMap(const std::string XMLStateTable);
                void ParseActionFromXML(const std::string serviceXML, stateMap& stateTable);

                bool ExecuteAction(const std::string STBAddress, const std::string STBPort, uint actionNum);
            };
        
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
    };
}
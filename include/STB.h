#include <iostream> 
#include <string>
#include <vector>
#include <map>
#include <algorithm> 
#include "MySocket.h"
#include "XMLParser.h"
#include "HTTPCommunicator.h"

enum class DirectionType { IN, OUT };
enum class ArgumentType { INT, FLOAT, CHAR, STRING, BOOLEAN, I4, UNKNOWN };

typedef std::map<std::string, ArgumentType> stateMap;

class STB
{
    public:
        STB(std::string friendlyName, std::string uuid, std::string address, std::string port, std::string xmlLocation);
        STB(std::string uuid, std::string address, std::string port, std::string xmlLocation);

        bool GetDescription();
        void ShowDescription() const;
        void FillServiceList(std::string XMLResponse);
        
        void AddService(std::string type, std::string id, std::string controlURL, std::string eventURL, std::string descriptionURL);
        void ShowMyServices() const;
        void ShowServiceActions(int serviceNumber);
        void SearchServiceDescription(std::string serviceName); 

        bool ExecuteServiceAction(uint serviceNumber, uint actionNumber);

        bool PairToDevice();
        bool CheckIsPaired();
        bool SetDeviceFriendlyName(std::string fname);
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
        void ParseServiceFromXML(std::string XMLservice);

    private:
        struct Argument
        {
            std::string name;
            DirectionType directionType;
            std::string relatedStateVariable;
            ArgumentType type;

            Argument(std::string name, DirectionType directionType, std::string relatedStateVariable, ArgumentType type);
            void ShowArgument() const;
            std::string GetTypeString() const;
            std::string GetName() const { return this->name; }
        };

        struct Action
        {
            std::string name;
            std::vector<Argument> InputParameters;
            std::vector<Argument> OutputParameters;

            Action(std::string name);
            std::string GetName() const { return this->name; }

            void ShowAction() const;
            void AddArgument(std::string name, DirectionType directionType, std::string relatedStateVariable, ArgumentType type);

            void FillArgumentList(std::string XMLAction, stateMap& stateTable);
            void ParseArgumentFromXML(std::string argumentXML, stateMap& stateTable);

            bool Execute(std::string STBAddress, std::string STBPort, std::string  serviceControlURL, std::string serviceType);
            std::string MakeSOAPRequestBody(std::string serviceType, std::string argumentList);
            std::string MakeArgumentForSOAPBody();
            bool correctArgumentType(std::string argumentType, std::string inputArgumentType);
            void ParseSOAPResponse(std::string SOAPResponse);
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
            bool GetServiceDescription(std::string STBAddress, std::string STBPort);

            void FillActionList(std::string XMLServiceResponse);
            std::unique_ptr<stateMap> GetStateMap(std::string XMLStateTable);
            void ParseActionFromXML(std::string serviceXML, stateMap& stateTable);

            bool ExecuteAction(std::string STBAddress, std::string STBPort, uint actionNum);
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
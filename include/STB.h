#include <iostream> 
#include <string>
#include <vector>
#include <map>
#include <algorithm> 
#include "MySocket.h"
#include "XMLParser.h"
#include "HTTPCommunicator.h"

enum class DirectionType { IN, OUT };
enum class ArgumentType { INT, FLOAT, CHAR, STRING, BOOLEAN, I4 };

typedef std::map<std::string, ArgumentType> stateMap;

class STB
{
    public:
        STB(std::string friendlyName, std::string uuid, std::string address, std::string port, std::string xmlLocation);
        STB(std::string uuid, std::string address, std::string port, std::string xmlLocation);

        bool GetDescription();
        void FillServiceList(std::string XMLResponse);
        
        void AddService(std::string type, std::string id, std::string controlURL, std::string eventURL, std::string descriptionURL);
        void ShowMyServices() const;
        void ShowServiceActions(int serviceNumber);
        void SearchServiceDescription(std::string serviceName); 

        std::string GetFriendlyName() const { return this->friendlyName; }
        std::string GetUUID() const { return this->uuid; }
        std::string GetAddress() const { return this->address; }
        std::string GetPort() const { return this->port; }
        std::string GetXMLLocation() const { return this->configXMLLocation; }
        int GetDetectedServicesCount() const { return this->services.size(); }
        std::string GetServiceName(int serviceNumber);

        void SetFriendlyName(std::string fname) { this-> friendlyName = fname; }
    private:
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
        };

        struct Action
        {
            std::string name;
            std::vector<Argument> InputParameters;
            std::vector<Argument> OutputParameters;

            Action(std::string name);
            void ShowAction() const;
            void AddArgument(std::string name, DirectionType directionType, std::string relatedStateVariable, ArgumentType type);

            void FillArgumentList(std::string XMLAction, stateMap& stateTable);
            void ParseArgumentFromXML(std::string argumentXML, stateMap& stateTable);
        };

        struct Service{
            std::string type;
            std::string id;
            std::string descriptionURL;
            std::string controlURL;
            std::string eventURL;
            std::vector<Action> actions;

            std::string GetNameOfService() const;
            std::string GetVersionOfService() const;
            std::string GetServiceId() const;
 
            void ShowMyActions() const;
            bool GetServiceDescription(std::string STBAddress, std::string STBPort);

            void FillActionList(std::string XMLServiceResponse);
            std::unique_ptr<stateMap> GetStateMap(std::string XMLStateTable);
            void ParseActionFromXML(std::string serviceXML, stateMap& stateTable);
        };
        
        std::string friendlyName;
        std::string uuid;
        std::string address;
        std::string port;
        std::string configXMLLocation;
        std::vector<Service> services;
};
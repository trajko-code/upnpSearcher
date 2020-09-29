#include <iostream> 
#include <string>
#include <vector>
#include <algorithm> 
#include "MySocket.h"

class STB
{
    public:
        STB(std::string fname, std::string uuid, std::string address, std::string port, std::string xmlLocation);
        STB(std::string uuid, std::string address, std::string port, std::string xmlLocation);
        
        void AddService(std::string type, std::string id, std::string controlURL, std::string eventURL, std::string descriptionURL);
        void ShowMyServices();
        void SearchServiceDescription(std::string serviceName);

        std::string GetFriendlyName() const { return this->friendlyName; }
        std::string GetUUID() const { return this->uuid; }
        std::string GetAddress() const { return this->address; }
        std::string GetPort() const { return this->port; }
        std::string GetXMLLocation() const { return this->configXMLLocation; }    

        void SetFriendlyName(std::string fname) { this-> friendlyName = fname; }

    private:
        enum class DirectionType { IN, OUT };
        enum class ArgumentType { INT, FLOAT, CHAR, STRING, BOOLEAN };

        struct Argument
        {
            std::string name;
            DirectionType directionType;
            std::string relatedStateVariable;
            ArgumentType type;

            Argument(std::string name, DirectionType directionType, std::string relatedStateVariable, ArgumentType type);
        };

        struct Action
        {
            std::string name;
            std::vector<Argument> InputParameter;
            std::vector<Argument> OutputParameter;

            void AddArgument(std::string name, DirectionType directionType, std::string relatedStateVariable, ArgumentType type);
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
 
            void AddAction(std::string name);
            bool GetServiceDescription(std:: string STBAddress, std::string STBPort);

            void FillActionList(std::string XMLresponse);
        };
        
        std::string friendlyName;
        std::string uuid;
        std::string address;
        std::string port;
        std::string configXMLLocation;
        std::vector<Service> services;
};
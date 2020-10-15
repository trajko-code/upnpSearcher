#include "setTopBox/STBTypes.hpp"
#include "setTopBox/Argument.hpp"
#include "HTTPCommunicator.hpp"
#include "XMLParser.hpp"
#include "InOut.hpp"

namespace setTopBox
{
    struct Action
    {
        private:
            friend class Service;
            friend class STB;

            std::string name;
            std::vector<Argument> InputParameters;
            std::vector<Argument> OutputParameters;

            Action(std::string name);
          
            void ShowAction() const;
            void AddArgument(Argument& argument);
            void FillArgumentList(const std::string XMLAction, stateMap& stateTable);
            void ParseArgumentFromXML(const std::string argumentXML, stateMap& stateTable);
            bool Execute(const std::string STBAddress, const std::string STBPort, 
                const std::string  serviceControlURL, const std::string serviceType);
            std::string MakeSOAPRequestBody(const std::string serviceType, const std::string argumentList);
            std::string MakeArgumentForSOAPBody();
            void ParseSOAPResponse(const std::string SOAPResponse);

            std::string GetName() const { return this->name; }
    };
}
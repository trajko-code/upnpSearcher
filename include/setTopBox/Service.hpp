#include "setTopBox/STBTypes.hpp"
#include "setTopBox/Action.hpp"
#include "HTTPCommunicator.hpp"
#include "XMLParser.hpp"

namespace setTopBox
{
    struct Service{
        private:
            friend class STB;

            std::string type;
            std::string id;
            std::string controlURL;
            std::string eventURL;
            std::string descriptionURL;
            std::vector<Action> actions;
            
            Service(std::string type, std::string id, std::string controlURL, 
                std::string eventURL, std::string descriptionURL);
            
            void ShowMyActions() const;
            bool RequireServiceDescription(const std::string STBAddress, const std::string STBPort);
            void FillActionList(const std::string XMLServiceResponse);
            std::unique_ptr<stateMap> GetStateMap(const std::string XMLStateTable);
            void ParseActionFromXML(const std::string serviceXML, stateMap& stateTable);
            bool ExecuteAction(const std::string STBAddress, const std::string STBPort, uint actionNum);

            std::string GetNameOfService() const;
            std::string GetVersionOfService() const;
            std::string GetServiceId() const;
            std::string GetControlUrl() const { return this->controlURL; }
            std::string GetType() const { return this->type; }
            size_t GetActionCount() const { return this->actions.size(); }
    };
}
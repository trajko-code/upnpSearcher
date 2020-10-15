#include "setTopBox/Service.hpp"

namespace setTopBox
{
    Service::Service(std::string type, std::string id, std::string controlURL,
        std::string eventURL, std::string descriptionURL)
        :type(type), id(id), controlURL(controlURL), 
        eventURL(eventURL), descriptionURL(descriptionURL)
    {
    }

    std::string Service::GetNameOfService() const
    {
        size_t nameBegin = this->type.find(":service:") + std::string(":service:").length();
        size_t nameEnd = this->type.find(':', nameBegin);
        return this->type.substr(nameBegin, nameEnd - nameBegin);
    }

    std::string Service::GetVersionOfService() const
    {
        size_t versionBegin = this->type.rfind(':') + 1;
        return this->type.substr(versionBegin, this->type.length() - versionBegin);
    }

    std::string Service::GetServiceId() const
    {
        size_t idBegin = this->id.find(":serviceId:") + std::string(":serviceId:").length();
        return this->id.substr(idBegin, this->id.length() - idBegin);
    }

    void Service::ShowMyActions() const
    {
        uint i = 1;
        for(auto const& action : this->actions)
        {
            InOut::Out("\t" + std::to_string(i++) + ". ");
            action.ShowAction();
        }
    }

    bool Service::RequireServiceDescription(const std::string STBAddress, const std::string STBPort)
    {
        std::string XMLResponse = HTTPCommunicator::GetXMLDescription(this->descriptionURL, STBAddress, STBPort);

        XMLParser::RemoveBlanks(XMLResponse);

        if(!XMLResponse.empty())
        {
            FillActionList(XMLResponse);
            return true;
        }
        else
            return false;
    }

    void Service::FillActionList(const std::string XMLServiceResponse)
    {
        std::string actionList = XMLParser::GetTagValue(XMLServiceResponse, "actionList");
        std::string XMLStateTable = XMLParser::GetTagValue(XMLServiceResponse, "serviceStateTable");
        std::unique_ptr<stateMap> stateTable = GetStateMap(XMLStateTable);
        std::string actionXML;
        std::string endActionTag = "</action>";

        while( actionList.length() > 0 )
        {
            actionXML = XMLParser::GetTagValue(actionList, "action");
            this->ParseActionFromXML(actionXML, *stateTable);

            int cropPos = actionList.find(endActionTag) + endActionTag.length();
            if(cropPos > actionList.length())
                return;

            actionList = actionList.substr(cropPos);
        }
    }

    std::unique_ptr<stateMap> Service::GetStateMap(std::string XMLStateTable)
    {
        std::unique_ptr<stateMap> map(new stateMap);

        std::string XMLStateVariable;
        std::string name;
        std::string dataType;
        std::string sendEvents;
        std::string valueRange;
        std::string allowedValueList;

        std::string endVariableTag = "</stateVariable>";
        std::string endAllowedValueTag = "</allowedValue>";

        while(XMLStateTable.length() > 0)
        {
            ArgumentDataFromTable data;

            XMLStateVariable = XMLParser::GetTagValue(XMLStateTable, "stateVariable");
            name = XMLParser::GetTagValue(XMLStateVariable, "name");
            data.type = XMLParser::GetTagValue(XMLStateVariable, "dataType");
            data.defaultValue = XMLParser::GetTagValue(XMLStateVariable, "defaulValue");
            valueRange = XMLParser::GetTagValue(XMLStateVariable, "allowedValueRange");
            if(!valueRange.empty())
            {
                data.min = XMLParser::GetTagValue(valueRange, "minimum");
                data.max = XMLParser::GetTagValue(valueRange, "maximum");
                data.step = XMLParser::GetTagValue(valueRange, "step");
            }
            allowedValueList = XMLParser::GetTagValue(XMLStateVariable, "allowedValueList");
            if(!allowedValueList.empty())  
                while(allowedValueList.length() > 0)
                {
                    data.allowedValueList.push_back(XMLParser::GetTagValue(allowedValueList, "allowedValue"));

                    int cropPos = allowedValueList.find(endAllowedValueTag) + endAllowedValueTag.length();
                    if(cropPos > allowedValueList.length())
                        break;
                    allowedValueList = allowedValueList.substr(cropPos);
                }
            sendEvents = XMLParser::GetTagValue(XMLStateVariable, "sendEventsAttribute");
            if(sendEvents.empty())
                sendEvents = XMLParser::GetTagAttributeValue(XMLStateTable, "stateVariable", "sendEvents");
            
            if(sendEvents.empty())
                data.sendEvents = false;
            else
                data.sendEvents = !sendEvents.compare("yes");

            if(map->find(name) == map->end())
                map->insert({name, data});

            int cropPos = XMLStateTable.find(endVariableTag) + endVariableTag.length();
            if(cropPos > XMLStateTable.length())
                return map;
                
            XMLStateTable = XMLStateTable.substr(cropPos);
        }

        return map;
    }

    void Service::ParseActionFromXML(const std::string actionXML, stateMap& stateTable)
    {
        std::string name = XMLParser::GetTagValue(actionXML, "name");
        Action action(name);
        action.FillArgumentList(actionXML, stateTable);
        this->actions.push_back(action);
    }

    bool Service::ExecuteAction(const std::string STBAddress, const std::string STBPort, uint actionNum)
    {
        return this->actions[actionNum].Execute(STBAddress, STBPort, this->GetControlUrl(), this->GetType());
    }
}
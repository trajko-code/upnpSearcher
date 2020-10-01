#include "STB.h"

#define BUFF_SIZE 1024

STB::STB(std::string friendlyName, std::string uuid, std::string address, std::string port, std::string xmlLocation)
    :STB(uuid, address, port, xmlLocation)
{
    this->friendlyName = friendlyName;
}

STB::STB(std::string uuid, std::string address, std::string port, std::string xmlLocation)
    :uuid(uuid), address(address), port(port), configXMLLocation(xmlLocation)
{
}

bool STB::GetDescription()
{
    std::string XMLResponse = HTTPCommunicator::GetXMLDescription(this->GetXMLLocation(), this->GetAddress(), this->GetPort());

    XMLParser::RemoveBlanks(XMLResponse);

    if(!XMLResponse.empty())
    {
        this->SetFriendlyName(XMLParser::GetTagValue(XMLResponse, "friendlyName"));
        FillServiceList(XMLResponse);

        return true;
    }
    else
        return false;
}

void STB::FillServiceList(std::string XMLResponse)
{
    std::string serviceList = XMLParser::GetTagValue(XMLResponse, "serviceList");
    std::string serviceXML;
    std::string endServiceTag = "</service>";

    while( serviceList.length() > 0 )
    {
        serviceXML = XMLParser::GetTagValue(serviceList, "service");
        this->ParseServiceFromXML(serviceXML);
        
        int cropPos = serviceList.find(endServiceTag) + endServiceTag.length();
        if(cropPos > serviceList.length())
           return;
        serviceList = serviceList.substr(cropPos);
    }
}

void STB::ParseServiceFromXML(std::string XMLservice)
{
    std::string type = XMLParser::GetTagValue(XMLservice, "serviceType");
    std::string id = XMLParser::GetTagValue(XMLservice, "serviceId");
    std::string controlURL = XMLParser::GetTagValue(XMLservice, "controlURL");
    std::string eventURL = XMLParser::GetTagValue(XMLservice, "eventSubURL");
    std::string descriptionURL = XMLParser::GetTagValue(XMLservice, "SCPDURL");

    this->AddService(type, id, controlURL, eventURL, descriptionURL);
}

void STB::AddService(std::string type, std::string id, std::string controlURL, std::string eventURL, std::string descriptionURL)
{
    this->services.push_back(Service{type, id, controlURL, eventURL, descriptionURL});
}

void STB::ShowMyServices()
{
    std::cout << "STB: " << this->friendlyName << std::endl;
    for(auto &service : this->services)
    {
        std::cout << service.GetNameOfService() << " : " << service.GetVersionOfService() << std::endl;
    }
}

void STB::SearchServiceDescription(std::string serviceName)
{
    auto service = std::find_if(this->services.begin(), this->services.end(), [&serviceName](const Service& s) { return !s.GetNameOfService().compare(serviceName); });
    if(service != this->services.end())
        service->GetServiceDescription(this->GetAddress(), this->GetPort());
}

STB::Argument::Argument(std::string name, DirectionType directionType, std::string relatedStateVariable, ArgumentType type)
    :name(name), directionType(directionType), relatedStateVariable(relatedStateVariable), type(type)
{
}

STB::Action::Action(std::string name)
    :name(name)
{
}

void STB::Action::AddArgument(std::string name, DirectionType directionType, std::string relatedStateVariable, ArgumentType type)
{
    Argument argument(name, directionType, relatedStateVariable, type);
    if(directionType == DirectionType::IN)
        this->InputParameters.push_back(argument);
    else
        this->OutputParameters.push_back(argument);
}

void STB::Action::FillArgumentList(std::string XMLAction, stateMap& stateTable)
{
    std::string argumentList = XMLParser::GetTagValue(XMLAction, "argumentList");
    std::string argumentXML;

    std::string endArgumentTag = "</argument>";

    while( argumentList.length() > 0 )
    {
        argumentXML = XMLParser::GetTagValue(argumentList, "argument");
        this->ParseArgumentFromXML(argumentXML, stateTable);
        int cropPos = argumentList.find(endArgumentTag) + endArgumentTag.length();
        if(cropPos > argumentList.length())
            return;
        argumentList = argumentList.substr(cropPos);
    }
}

void STB::Action::ParseArgumentFromXML(std::string argumentXML, stateMap& stateTable)
{
    std::string name = XMLParser::GetTagValue(argumentXML, "name");
    DirectionType direction = XMLParser::GetTagValue(argumentXML, "direction").compare("in") == 0 ? DirectionType::IN : DirectionType::OUT;
    std::string relatedStateVariable = XMLParser::GetTagValue(argumentXML, "relatedStateVariable");

    ArgumentType type = stateTable[relatedStateVariable];
        
    this->AddArgument(name, direction, relatedStateVariable, type);
}

std::string STB::Service::GetNameOfService() const
{
    ushort nameBegin = this->type.find(":service:") + 9;
    ushort nameEnd = this->type.find(':', nameBegin);
    return this->type.substr(nameBegin, nameEnd - nameBegin);
}

std::string STB::Service::GetVersionOfService() const
{
    ushort versionBegin = this->type.rfind(':') + 1;
    return this->type.substr(versionBegin, this->type.length() - versionBegin);
}

std::string STB::Service::GetServiceId() const
{
    ushort idBegin = this->id.find(":serviceId:") + 11;
    return this->id.substr(idBegin, this->id.length() - idBegin);
}

bool STB::Service::GetServiceDescription(std::string STBAddress, std::string STBPort)
{
    std::string XMLResponse = HTTPCommunicator::GetXMLDescription(descriptionURL, STBAddress, STBPort);

    XMLParser::RemoveBlanks(XMLResponse);

    if(!XMLResponse.empty())
    {
        FillActionList(XMLResponse);
        return true;
    }
    else
        return false;
}

void STB::Service::FillActionList(std::string XMLServiceResponse)
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

std::unique_ptr<stateMap> STB::Service::GetStateMap(std::string XMLStateTable)
{
    std::unique_ptr<stateMap> map(new stateMap);

    std::string XMLStateVariable;
    std::string name;
    std::string dataType;

    std::string endVariableTag = "</stateVariable>";

    while( XMLStateTable.length() > 0 )
    {
        XMLStateVariable = XMLParser::GetTagValue(XMLStateTable, "stateVariable");
        name = XMLParser::GetTagValue(XMLStateVariable, "name");
        dataType = XMLParser::GetTagValue(XMLStateVariable, "dataType");
        ArgumentType type;

        if(dataType.compare("string") == 0) 
            type = ArgumentType::STRING;
        else if(dataType.compare("i4") == 0) 
            type = ArgumentType::I4;
        else if(dataType.compare("int") == 0) 
            type = ArgumentType::INT;
        else if(dataType.compare("float") == 0) 
            type = ArgumentType::FLOAT;
        else if(dataType.compare("char") == 0) 
            type = ArgumentType::CHAR;
        else if(dataType.compare("boolean") == 0)
            type = ArgumentType::BOOLEAN;

        if(map->find(name) == map->end())
            map->insert({name, type});

        int cropPos = XMLStateTable.find(endVariableTag) + endVariableTag.length();

        if(cropPos > XMLStateTable.length())
            return map;
            
        XMLStateTable = XMLStateTable.substr(cropPos);
    }

    return map;
}

void STB::Service::ParseActionFromXML(std::string actionXML, stateMap& stateTable)
{
    std::string name = XMLParser::GetTagValue(actionXML, "name");
    Action action(name);
    action.FillArgumentList(actionXML, stateTable);
    this->actions.push_back(action);
}
#include "STB.h"

#define BUFF_SIZE 1024
#define XMLNS "schemas.xmlsoap.org/soap/envelope/"
#define ENCODING_STYLE "schemas.xmlsoap.org/soap/encoding/"

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

void STB::ShowMyServices() const
{
    int i = 1;
    for(auto &service : this->services)
    {
        std::cout << "\t" << i++ << ". " << service.GetNameOfService() << " : " << service.GetVersionOfService() << std::endl;
    }
}

void STB::ShowServiceActions(int serviceNumber) 
{
    if(this->services[serviceNumber].actions.size() < 1)
        this->services[serviceNumber].GetServiceDescription(this->GetAddress(), this->GetPort());
    
    this->services[serviceNumber].ShowMyActions();
}

void STB::SearchServiceDescription(std::string serviceName)  
{
    auto service = std::find_if(this->services.begin(), this->services.end(), [&serviceName](const Service& s) { return !s.GetNameOfService().compare(serviceName); });
    if(service != this->services.end())
        service->GetServiceDescription(this->GetAddress(), this->GetPort());
}

bool STB::ExecuteServiceAction(uint serviceNumber, uint actionNumber)
{
    return this->services[serviceNumber-1].ExecuteAction(this->GetAddress(), this->GetPort(), actionNumber);
}

std::string STB::GetServiceName(int serviceNumber)
{
    return this->services[serviceNumber].GetNameOfService();
}

STB::Argument::Argument(std::string name, DirectionType directionType, std::string relatedStateVariable, ArgumentType type)
    :name(name), directionType(directionType), relatedStateVariable(relatedStateVariable), type(type)
{
}

void STB::Argument::ShowArgument() const
{
    std::cout << "{ " << this->name << " " <<  this->GetTypeString() << " }";
}

std::string STB::Argument::GetTypeString() const
{
    if(this->type == ArgumentType::STRING) 
        return "string";
    else if(this->type == ArgumentType::I4) 
        return "i4";
    else if(this->type == ArgumentType::INT) 
        return "int";
    else if(this->type == ArgumentType::FLOAT) 
        return "float";
    else if(this->type == ArgumentType::CHAR) 
        return "char";
    else if(this->type == ArgumentType::BOOLEAN)
        return "boolean";
    else
        return "";
}

STB::Action::Action(std::string name)
    :name(name)
{
}

void STB::Action::ShowAction() const
{
    std::cout << this->name << std::endl;
    std::cout << "\t\t" << "input: ";
    for(auto const& arg : this->InputParameters)
    {
        arg.ShowArgument();
        std::cout << " ";
    }
    std::cout << std::endl;
    std::cout << "\t\t" << "output: ";
    for(auto const& arg : this->OutputParameters)
    {
        arg.ShowArgument();
        std::cout << " ";
    }
    std::cout << std::endl;
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

bool STB::Action::Execute(std::string STBAddress, std::string STBPort, std::string  serviceControlURL, std::string serviceType)
{
    try
    {
        std::string soapAction = serviceType + "#" + this->GetName();
        std::string body = MakeSOAPRequestBody(serviceType);

        std::string SOAPResponse = HTTPCommunicator::PostExecuteAction(serviceControlURL, STBAddress, STBPort, soapAction, body);
        std::cout << SOAPResponse << "\n";
        return ParseSOAPResponse(SOAPResponse); //??
    }
    catch(const std::string& ex)
    {
        std::cerr << ex << '\n';
        return false;
    }
    
}

std::string STB::Action::MakeSOAPRequestBody(std::string serviceType)
{ 
    std::string body =  "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" "
                        //"xmlns:s=\"http://" + XMLNS + "\"\n"
                        //"s:encodingStyle=\"http://" + ENCODING_STYLE + "\">\n"
                        "s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">\n"
                        "<s:Body>\n"
                        "<u:" + this->GetName() + " xmlns:u=\"" +  serviceType + "\">\n" 
                        + this->MakeArgumentForSOAPBody() +
                        "</u:" + this->GetName() + ">"
                        "</s:Body>\n"
                        "</s:Envelope>\n";
    return body;              
}

std::string STB::Action::MakeArgumentForSOAPBody()
{
    std::string arguments;
    std::string argumentName;
    std::string argumentType;
    std::string argumentValue;
    std::cout << "Enter arguments for the action \"" + this->GetName() +"\":\n";
    for(auto const& arg : this->InputParameters)
    {
        argumentName = arg.GetName();
        argumentType = arg.GetTypeString();
        std::cout << "Argument: " + argumentName + "(" + argumentType + ")  Value: ";
        std::cin >> argumentValue;
        
        if(this->correctArgumentType(argumentType, argumentValue))
            arguments += "<" + argumentName +">" + argumentValue + "</" + argumentName + ">\n";
        else
            throw std::string("Invalid input argument!");
    }
    return arguments;
}

bool STB::Action::correctArgumentType(std::string argumentType, std::string inputArgumentType)
{
        if(argumentType.compare("string") == 0)
            return true;
        else if(argumentType.compare("int") == 0 || argumentType.compare("i4") == 0) 
        {
            try
            {
                int type = std::stoi(inputArgumentType);
                return true;
            }
            catch(...)
            {
                return false;
            }
        }
        else if(argumentType.compare("float") == 0)
        {
            try
            {
                float type = std::stof(inputArgumentType);
                return true;
            }
            catch(...)
            {
                return false;
            }
        }
        else if(argumentType.compare("char") == 0)
            return inputArgumentType.length() == 1;
        else if(argumentType.compare("boolean") == 0)
        {
            return inputArgumentType.compare("true") == 0 || inputArgumentType.compare("TRUE") == 0 
                    || inputArgumentType.compare("false") == 0 || inputArgumentType.compare("FALSE") == 0 
                    || inputArgumentType.compare("yes") == 0 || inputArgumentType.compare("YES") == 0
                    || inputArgumentType.compare("no") == 0 || inputArgumentType.compare("NO") == 0
                    || inputArgumentType.compare("0") == 0 || inputArgumentType.compare("1") == 0;
        }
        else
            return false;
}

bool STB::Action::ParseSOAPResponse(std::string SOAPResponse)
{
    return true;
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

void STB::Service::ShowMyActions() const
{
    uint i = 1;
    for(auto const& action : this->actions)
    {
        std::cout<< "\t" << i++ << ". ";
        action.ShowAction();
    }
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
        else
            type = ArgumentType::UNKNOWN;
        

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

bool STB::Service::ExecuteAction(std::string STBAddress, std::string STBPort, uint actionName)
{
    return this->actions[actionName-1].Execute(STBAddress, STBPort, this->GetControlUrl(), this->GetType());
}
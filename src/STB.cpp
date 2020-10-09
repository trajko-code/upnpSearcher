#include "STB.h"
#include "Configuration.h"

#define BUFF_SIZE 1024
#define XMLNS "schemas.xmlsoap.org/soap/envelope/"
#define ENCODING_STYLE "schemas.xmlsoap.org/soap/encoding/"
#define REMOTE_PAIRING_SERVICE "X-CTC_RemotePairing"
#define PAIRING_REQUEST_ACTION "X-pairingRequest"
#define PAIRING_CHECK_ACTION "X-pairingCheck"
#define SET_FRIENDLY_NAME_ACTION "X-setFriendlyName"
#define REMOTE_CONTROL_SERVICE "X-CTC_RemoteControl"
#define REMOTE_KEY_ACTION "X-CTC_RemoteKey"

#pragma region STB
STB::STB(std::string friendlyName, std::string uuid, std::string address, std::string port, std::string xmlLocation)
    :STB(uuid, address, port, xmlLocation)
{
    this->friendlyName = friendlyName;
}

STB::STB(std::string uuid, std::string address, std::string port, std::string xmlLocation)
    :uuid(uuid), address(address), port(port), configXMLLocation(xmlLocation)
{
    this->paired = false;
}

bool STB::GetDescription()
{
    std::string XMLResponse = HTTPCommunicator::GetXMLDescription(this->GetXMLLocation(), this->GetAddress(), this->GetPort());

    XMLParser::RemoveBlanks(XMLResponse);

    if(!XMLResponse.empty())
    {
        this->SetFriendlyName(XMLParser::GetTagValue(XMLResponse, "friendlyName"));
        this->SetDeviceType(XMLParser::GetTagValue(XMLResponse, "deviceType"));
        this->SetManufacturer(XMLParser::GetTagValue(XMLResponse, "manufacturer"));
        this->SetSerialNumber(XMLParser::GetTagValue(XMLResponse, "serialNumber"));

        FillServiceList(XMLResponse);

        return true;
    }
    else
        return false;
}

void STB::ShowDescription() const
{
    InOut::Out("\n----DEVICE DESCRIPTION----\n");
    InOut::Out("Friendly name: " + this->GetFriendlyName() + '\n');
    InOut::Out("UUID: " + this->GetUUID() + '\n');
    InOut::Out("Address: " + this->GetAddress() + '\n');
    InOut::Out("Port: " + this->GetPort() + '\n');
    InOut::Out("Device type: " + this->GetDeviceType() + '\n');
    InOut::Out("Manufacturer: " + this->GetManufacturer() + '\n');
    InOut::Out("Serial number: " + this->GetSerialNumber() + '\n');
    InOut::Out("Configuration XML location: " + this->GetXMLLocation() + '\n');
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

    for(auto const& service : this->services)
    {
        if(service.id.compare(id) == 0)
            return;
    }
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
        InOut::Out("\t" + std::to_string(i++) + ". " + service.GetNameOfService() + " : " + service.GetVersionOfService() + '\n');
    }
}

void STB::ShowServiceActions(int serviceNumber) 
{
    if(this->services[serviceNumber].GetActionCount() < 1)
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
    return this->services[serviceNumber].ExecuteAction(this->GetAddress(), this->GetPort(), actionNumber);
}

std::string STB::ExecuteServiceAction(std::string serviceName, std::string actionName, std::string argumentList)
{
    auto service = std::find_if(this->services.begin(), this->services.end(), [serviceName](Service& s) { return !s.GetNameOfService().compare(serviceName); });
    
    if(service == this->services.end())
    {
        InOut::Out("ERROR: Service '" + serviceName + "' is not supported!\n");
        return "";
    }

    if(service->GetActionCount() < 1)
        service->GetServiceDescription(this->GetAddress(), this->GetPort());

    auto action = std::find_if(service->actions.begin(), service->actions.end(), [actionName](Action& a) { return !a.GetName().compare(actionName); });
    
    if(action == service->actions.end())
    {
        InOut::Out("ERROR: Action '" + actionName + "' is not supported!\n");
        return "";
    }

    std::string soapAction = service->GetType() + "#" + action->GetName();       
    std::string body = action->MakeSOAPRequestBody(service->GetType(), argumentList);

    return HTTPCommunicator::PostExecuteAction(service->GetControlUrl(), this->GetAddress(), this->GetPort(), soapAction, body);
}

bool STB::PairToDevice()
{
    if(this->paired && !this->GetVerificationCode().empty())
    {
        InOut::Out("Device already paired.\n");
        return true;
    }
    
    std::string argumentList = "<pairingDeviceID>" + Config::aplicationID + "</pairingDeviceID>"
                                "<friendlyName>" + Config::friendlyName + "</friendlyName>";

    std::string SOAPResponse = this->ExecuteServiceAction(REMOTE_PAIRING_SERVICE, PAIRING_REQUEST_ACTION, argumentList);

    if(SOAPResponse.empty())
        return false;
 
    std::string result = XMLParser::GetTagValue(SOAPResponse, "result");
    if(result.compare("0") != 0)
        return false;

    std::string pin;
    InOut::Out("Enter PIN: ");
    InOut::In(pin);

    argumentList = "<pairingDeviceID>" + Config::aplicationID + "</pairingDeviceID>"
                    "<verificationPIN>" + pin + "</verificationPIN>";

    SOAPResponse = this->ExecuteServiceAction(REMOTE_PAIRING_SERVICE, PAIRING_CHECK_ACTION, argumentList);

    if(SOAPResponse.empty())
        return false;

    std::string pairingResult = XMLParser::GetTagValue(SOAPResponse, "pairingResult");
    if(pairingResult.compare("0") != 0)
    {
        InOut::Out("ERROR: Verification failure!\n");
        return false;
    }
    else
    {
        this->SetVerificationCode(XMLParser::GetTagValue(SOAPResponse, "outputCode"));
        this->paired = true;
        InOut::Out("Succesfully paired to device.\n");
        return true;
    }
}

bool STB::SendPairingRequest()
{
    if(this->paired && !this->GetVerificationCode().empty())
    {
        InOut::Out("Device already paired.\n");
        return false;
    }
    
    std::string argumentList = "<pairingDeviceID>" + Config::aplicationID + "</pairingDeviceID>"
                                "<friendlyName>" + Config::friendlyName + "</friendlyName>";

    std::string SOAPResponse = this->ExecuteServiceAction(REMOTE_PAIRING_SERVICE, PAIRING_REQUEST_ACTION, argumentList);

    if(SOAPResponse.empty())
        return false;
 
    std::string result = XMLParser::GetTagValue(SOAPResponse, "result");
    if(result.compare("0") != 0)
        return false;
    
    return true;
}

bool STB::SendPairingCheck(const std::string pin)
{
    std::string argumentList = "<pairingDeviceID>" + Config::aplicationID + "</pairingDeviceID>"
                    "<verificationPIN>" + pin + "</verificationPIN>";

    std::string SOAPResponse = this->ExecuteServiceAction(REMOTE_PAIRING_SERVICE, PAIRING_CHECK_ACTION, argumentList);

    if(SOAPResponse.empty())
        return false;

    std::string pairingResult = XMLParser::GetTagValue(SOAPResponse, "pairingResult");
    if(pairingResult.compare("0") != 0)
    {
        InOut::Out("ERROR: Verification failure!\n");
        return false;
    }
    else
    {
        this->SetVerificationCode(XMLParser::GetTagValue(SOAPResponse, "outputCode"));
        this->paired = true;
        InOut::Out("Succesfully paired to device.\n");
        return true;
    }
}

bool STB::CheckIsPaired()
{
    if(!this->paired || this->GetVerificationCode().empty())
    {   
        return false;
    }

    std::string argumentList = "<pairingDeviceID>" + Config::aplicationID + "</pairingDeviceID>"
                                "<verificationCode>" + this->GetVerificationCode() + "</verificationCode>";       

    std::string SOAPResponse = this->ExecuteServiceAction(REMOTE_PAIRING_SERVICE, PAIRING_CHECK_ACTION, argumentList);

    if(SOAPResponse.empty())
        return false;

    std::string pairingResult = XMLParser::GetTagValue(SOAPResponse, "pairingResult");
    if(pairingResult.compare("0") == 0)
    {
        std::string outCode = XMLParser::GetTagValue(SOAPResponse, "outputCode");
        if(outCode.compare("-") == 0)
            return true;
    }
  
    this->SetVerificationCode("");
    this->paired = false;
    return false;
}

bool STB::SetDeviceFriendlyName(const std::string fname)
{
    if(!this->paired)
    {   
        InOut::Out("ERROR: Not paired with the device!\n");
        return false;
    }

    std::string argumentList = "<pairingDeviceID>" + Config::aplicationID + "</pairingDeviceID>"
                                "<verificationCode>" + this->GetVerificationCode() + "</verificationCode>"
                                "<stbFriendlyName>" + fname + "</stbFriendlyName>";

    std::string SOAPResponse = this->ExecuteServiceAction(REMOTE_PAIRING_SERVICE, SET_FRIENDLY_NAME_ACTION, argumentList);

    if(SOAPResponse.empty())
        return false;

    std::string result = XMLParser::GetTagValue(SOAPResponse, "result");
    if(result.compare("0") != 0)
        return false;
    
    this->SetFriendlyName(fname);
    InOut::Out("Device friendly name succesfully changed!\n");
    return true;
}

void STB::ShowKeysName() const
{
    int i=1;
    for(auto const& key : Config::keys)
        InOut::Out(std::to_string(i++) + " " + key.keyName + "    (" + key.description + ")\n");
}

bool STB::SendKeyCommand(int key)
{
    if(!this->paired)
    {   
        InOut::Out("ERROR: Not paired with the device!\n");
        return false;
    }

    std::string argumentList = "<keyCode>keyCode=" + Config::keys[key].keyValue + "</keyCode>"
                                "<deviceID>" + Config::aplicationID + "</deviceID>"
                                "<verificationCode>" + this->GetVerificationCode() + "</verificationCode>";

    std::string SOAPResponse = this->ExecuteServiceAction(REMOTE_CONTROL_SERVICE, REMOTE_KEY_ACTION, argumentList);

    return !SOAPResponse.empty();
}

std::string STB::GetServiceName(int serviceNumber) const
{
    return this->services[serviceNumber].GetNameOfService();
}

uint STB::GetServiceActionsCount(int serviceNumber) const
{
    return this->services[serviceNumber].actions.size();
}
#pragma endregion

#pragma region Argument
STB::Argument::Argument(std::string name, DirectionType directionType, std::string relatedStateVariable, ArgumentType type, std::string defaultValue, 
                        bool sendEvents, std::vector<std::string> allowedList, struct AllowedValueRange valueRange)
    :name(name), directionType(directionType), relatedStateVariable(relatedStateVariable), type(type), defaultValue(defaultValue), sendEvents(sendEvents),
    allowedValueList(allowedList), valueRange(valueRange)
{
}

void STB::Argument::ShowArgument() const
{
    InOut::Out("{" + this->name + " " + this->GetTypeString() + " events:" + (this->sendEvents?"yes":"no") + "}");
}

std::string STB::Argument::GetAdditionalInfo() const
{
    std::string info;
    if(!this->GetDefaultValue().empty())
        info += "Default value: " + this->GetDefaultValue() + " ";
    if(this->allowedValueList.size() > 0)
    {
        info += "Allowed values ( ";
        for(auto const& value : this->allowedValueList)
            info += value + " ";
        info += ")";
    }
    if(!this->valueRange.minimum.empty())
        info += "Value range: minimum - " + this->valueRange.minimum + " maximum - " + this->valueRange.maximum + "step - " + this->valueRange.step;
    info += "\n";
    return info;
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

bool STB::Argument::CorrectType(std::string inputValue) const
{
    if(this->type == ArgumentType::STRING)
            return true;
    else if(this->type == ArgumentType::INT || this->type == ArgumentType::I4) 
    {
        try
        {
            int type = std::stoi(inputValue);
            return true;
        }
        catch(...)
        {
            return false;
        }
    }
    else if(this->type == ArgumentType::FLOAT)
    {
        try
        {
            float type = std::stof(inputValue);
            return true;
        }
        catch(...)
        {
            return false;
        }
    }
    else if(this->type == ArgumentType::CHAR)
        return inputValue.length() == 1;
    else if(this->type == ArgumentType::BOOLEAN)
    {
        return inputValue.compare("true") == 0 || inputValue.compare("TRUE") == 0 
                || inputValue.compare("false") == 0 || inputValue.compare("FALSE") == 0 
                || inputValue.compare("yes") == 0 || inputValue.compare("YES") == 0
                || inputValue.compare("no") == 0 || inputValue.compare("NO") == 0
                || inputValue.compare("0") == 0 || inputValue.compare("1") == 0;
    }
    else
        return false;
}

bool STB::Argument::CorrectValue(std::string inputValue) const
{
    if(this->allowedValueList.size() > 0)
    {
        auto allowedValue = std::find_if(this->allowedValueList.begin(), this->allowedValueList.end(), [&inputValue](const std::string& value) { return !value.compare(inputValue); });
        if(allowedValue == this->allowedValueList.end())
            return false;
    }
    return true;
}
#pragma endregion

#pragma region Action
STB::Action::Action(std::string name)
    :name(name)
{
}

void STB::Action::ShowAction() const
{
    InOut::Out(this->name + '\n');
    InOut::Out("\t\tinput: ");
    for(auto const& arg : this->InputParameters)
    {
        arg.ShowArgument();
        InOut::Out(" ");
    }
    InOut::Out("\n\t\toutput: ");
    for(auto const& arg : this->OutputParameters)
    {
        arg.ShowArgument();
        InOut::Out(" ");
    }
    InOut::Out("\n");
}

void STB::Action::AddArgument(Argument& argument)
{
    if(argument.directionType == DirectionType::IN)
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

    ArgumentDataFromTable argData = stateTable[relatedStateVariable];

    Argument newArgument(name, direction, relatedStateVariable, argData.type, argData.defaultValue, argData.sendEvents,
                         argData.allowedValueList, Argument::AllowedValueRange{argData.min, argData.max, argData.step});

    this->AddArgument(newArgument);
}

bool STB::Action::Execute(std::string STBAddress, std::string STBPort, std::string  serviceControlURL, std::string serviceType)
{
    try
    {
        std::string soapAction = serviceType + "#" + this->GetName();
        std::string argumentList = this->MakeArgumentForSOAPBody();
        std::string body = MakeSOAPRequestBody(serviceType, argumentList);

        std::string SOAPResponse = HTTPCommunicator::PostExecuteAction(serviceControlURL, STBAddress, STBPort, soapAction, body);
        if(SOAPResponse.empty())
            return false;
        else
        {
            ParseSOAPResponse(SOAPResponse);
            return true;
        }
        
    }
    catch(const std::string& ex)
    {
        std::cerr << ex << '\n';
        return false;
    }   
}

std::string STB::Action::MakeSOAPRequestBody(std::string serviceType,std::string argumentList)
{ 
    std::string body =  "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" "
                        //"xmlns:s=\"http://" + XMLNS + "\"\n"
                        //"s:encodingStyle=\"http://" + ENCODING_STYLE + "\">\n"
                        "s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">\n"
                        "<s:Body>\n"
                        "<u:" + this->GetName() + " xmlns:u=\"" +  serviceType + "\">\n" 
                        + argumentList +
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
    InOut::Out("Enter arguments for the action \"" + this->GetName() + "\" ('/' if the argument is not used):\n");
    for(auto const& arg : this->InputParameters)
    {
        argumentName = arg.GetName();
        argumentType = arg.GetTypeString();
        InOut::Out("Argument: " + argumentName + "(" + argumentType + ") " + arg.GetAdditionalInfo() + "Value: ");
        InOut::In(argumentValue);
        
        if(argumentValue.compare("/") != 0)
        {
            if(arg.CorrectType(argumentValue) && arg.CorrectValue(argumentValue))
                arguments += "<" + argumentName +">" + argumentValue + "</" + argumentName + ">\n";
            else
                throw std::string("Invalid input argument!");
        }
    }
    return arguments;
}

void STB::Action::ParseSOAPResponse(std::string SOAPResponse)
{
    std::string value;
    InOut::Out("### RESPONSE OK ###\n");
    for(auto const& outArg : this->OutputParameters)
    {
        value = XMLParser::GetTagValue(SOAPResponse, outArg.GetName());
        if(!value.empty())
            InOut::Out(outArg.GetName() + " : " + value + '\n');
    }
}
#pragma endregion

#pragma region Service
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
        InOut::Out("\t" + std::to_string(i++) + ". ");
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
    std::string sendEvents;
    std::string valueRange;
    std::string allowedValueList;

    std::string endVariableTag = "</stateVariable>";
    std::string endAllowedValueTag = "</allowedValue>";

    while( XMLStateTable.length() > 0 )
    {
        ArgumentDataFromTable data;

        XMLStateVariable = XMLParser::GetTagValue(XMLStateTable, "stateVariable");
        name = XMLParser::GetTagValue(XMLStateVariable, "name");
        dataType = XMLParser::GetTagValue(XMLStateVariable, "dataType");
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
            
        if(dataType.compare("string") == 0) 
            data.type = ArgumentType::STRING;
        else if(dataType.compare("i4") == 0) 
            data.type = ArgumentType::I4;
        else if(dataType.compare("int") == 0) 
            data.type = ArgumentType::INT;
        else if(dataType.compare("float") == 0) 
            data.type = ArgumentType::FLOAT;
        else if(dataType.compare("char") == 0) 
            data.type = ArgumentType::CHAR;
        else if(dataType.compare("boolean") == 0)
            data.type = ArgumentType::BOOLEAN;
        else
            data.type = ArgumentType::UNKNOWN;
        
        if(map->find(name) == map->end())
            map->insert({name, data});

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

bool STB::Service::ExecuteAction(std::string STBAddress, std::string STBPort, uint actionNum)
{
    return this->actions[actionNum].Execute(STBAddress, STBPort, this->GetControlUrl(), this->GetType());
}
#pragma endregion
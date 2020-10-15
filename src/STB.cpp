#include "setTopBox/STB.hpp"

#define REMOTE_PAIRING_SERVICE "X-CTC_RemotePairing"
#define PAIRING_REQUEST_ACTION "X-pairingRequest"
#define PAIRING_CHECK_ACTION "X-pairingCheck"
#define SET_FRIENDLY_NAME_ACTION "X-setFriendlyName"
#define REMOTE_CONTROL_SERVICE "X-CTC_RemoteControl"
#define REMOTE_KEY_ACTION "X-CTC_RemoteKey"

namespace setTopBox
{
    STB::STB(std::string uuid, std::string address, std::string port, std::string xmlLocation)
        :uuid(uuid), address(address), port(port), configXMLLocation(xmlLocation), paired(false)
    {
    }

    bool STB::RequireDescription()
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
        InOut::Out("Verification code (if paired): " + this->GetVerificationCode() + '\n');
    }

    void STB::FillServiceList(const std::string XMLResponse)
    {
        std::string serviceList = XMLParser::GetTagValue(XMLResponse, "serviceList");
        std::string serviceXML;
        std::string endServiceTag = "</service>";

        while(serviceList.length() > 0)
        {
            serviceXML = XMLParser::GetTagValue(serviceList, "service");
            this->ParseServiceFromXML(serviceXML);
            
            int cropPos = serviceList.find(endServiceTag) + endServiceTag.length();
            if(cropPos > serviceList.length())
                return;
            serviceList = serviceList.substr(cropPos);
        }
    }

    void STB::ParseServiceFromXML(const std::string XMLservice)
    {
        std::string type = XMLParser::GetTagValue(XMLservice, "serviceType");
        std::string id = XMLParser::GetTagValue(XMLservice, "serviceId");
        std::string controlURL = XMLParser::GetTagValue(XMLservice, "controlURL");
        std::string eventURL = XMLParser::GetTagValue(XMLservice, "eventSubURL");
        std::string descriptionURL = XMLParser::GetTagValue(XMLservice, "SCPDURL");
    
        this->AddService(type, id, controlURL, eventURL, descriptionURL);
    }

    void STB::AddService(const std::string type, const std::string id,
        std::string controlURL, const std::string eventURL, const std::string descriptionURL)
    {
        if(!std::any_of(this->services.begin(), this->services.end(), [id](const Service& s) { return !s.id.compare(id); }))
            this->services.push_back(Service{type, id, controlURL, eventURL, descriptionURL});
    }

    void STB::ShowMyServices() const
    {
        int i = 1;
        for(const auto &service : this->services)
        {
            InOut::Out("\t" + std::to_string(i++) + ". " + service.GetNameOfService() + " : " + service.GetVersionOfService() + '\n');
        }
    }

    void STB::ShowServiceActions(int serviceNumber) 
    {
        if(this->services[serviceNumber].GetActionCount() < 1)
            this->services[serviceNumber].RequireServiceDescription(this->GetAddress(), this->GetPort());
        
        this->services[serviceNumber].ShowMyActions();
    }

    void STB::SearchServiceDescription(const std::string serviceName)  
    {
        auto service = std::find_if(this->services.begin(), this->services.end(), 
            [&serviceName](const Service& s) { return !s.GetNameOfService().compare(serviceName); });
        
        if(service != this->services.end())
            service->RequireServiceDescription(this->GetAddress(), this->GetPort());
    }

    bool STB::ExecuteServiceAction(uint serviceNumber, uint actionNumber)
    {
        return this->services[serviceNumber].ExecuteAction(this->GetAddress(), this->GetPort(), actionNumber);
    }

    std::string STB::ExecuteServiceAction(const std::string serviceName, const std::string actionName, const std::string argumentList)
    {
        auto service = std::find_if(this->services.begin(), this->services.end(), 
            [serviceName](Service& s) { return !s.GetNameOfService().compare(serviceName); });
        
        if(service == this->services.end())
        {
            InOut::Out("ERROR: Service '" + serviceName + "' is not supported!\n");
            return "";
        }

        if(service->GetActionCount() < 1)
            service->RequireServiceDescription(this->GetAddress(), this->GetPort());

        auto action = std::find_if(service->actions.begin(), service->actions.end(), 
            [actionName](Action& a) { return !a.GetName().compare(actionName); });
        
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
        if(!SendPairingRequest())
            return false;

        std::string pin;
        InOut::Out("Enter PIN: ");
        InOut::In(pin);

        return this->SendPairingCheck(pin);
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

        return !result.compare("0");
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
            return false;

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
        int i = 1;
        for(auto const& key : setTopBox::keyCodes)
            InOut::Out(std::to_string(i++) + " " + key.keyName + "    (" + key.description + ")\n");
    }

    bool STB::SendKeyCommand(int key)
    {
        if(!this->paired)
        {   
            InOut::Out("ERROR: Not paired with the device!\n");
            return false;
        }

        std::string argumentList = "<keyCode>keyCode=" + setTopBox::keyCodes[key].keyValue + "</keyCode>"
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
}
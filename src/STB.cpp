#include <STB.h>

#define BUFF_SIZE 1024

STB::STB(std::string fname, std::string uuid, std::string address, std::string port, std::string xmlLocation)
    :STB(uuid, address, port, xmlLocation)
{
    this->friendlyName = fname;
}

STB::STB(std::string uuid, std::string address, std::string port, std::string xmlLocation)
    :uuid(uuid), address(address), port(port), configXMLLocation(xmlLocation)
{
}

void STB::AddService(std::string type, std::string id, std::string controlURL, std::string eventURL, std::string descriptionURL)
{
    this->services.push_back(Service{type, id, controlURL, eventURL, descriptionURL});
}

void STB::ShowMyServices()
{
    std::cout << "STB: " << this->friendlyName << std::endl;
    for(auto const &service : this->services)
        std::cout << service.GetNameOfService() << " : " << service.GetVersionOfService() << std::endl;
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

void STB::Action::AddArgument(std::string name, DirectionType directionType, std::string relatedStateVariable, ArgumentType type)
{
    Argument argument(name, directionType, relatedStateVariable, type);
    if(directionType == DirectionType::IN)
        this->InputParameter.push_back(argument);
    else
        this->OutputParameter.push_back(argument);
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

bool STB::Service::GetServiceDescription(std:: string STBAddress, std::string STBPort)
{
    std::string msg = "GET " + this->descriptionURL + " HTTP/1.1\r\n"
                        "Connection: close\r\n"
                        "Host: " + STBAddress + ":" + STBPort + "\r\n"
                        "\r\n";
        
    MySocket sock(AF_INET, SOCK_STREAM);
    if(!sock.CreateSocket())
        return false;
    
    if(!sock.Connect(AF_INET, std::stoi(STBPort), inet_addr(STBAddress.c_str())))
        return false;

    sock.Send(msg.c_str(), msg.length(), 0);

    std::string XMLresponse = "";
    char recvBuf[BUFF_SIZE];
    memset(recvBuf, 0, BUFF_SIZE);

    while(sock.Recieve(recvBuf, BUFF_SIZE, 0) != 0)
    {
        if(!XMLresponse.empty())
            XMLresponse.pop_back();
        XMLresponse += recvBuf;
        memset(recvBuf, 0, BUFF_SIZE);
    }
    
    if(!XMLresponse.empty())
    {
        //std::cout << XMLresponse << std::endl;
        FillActionList(XMLresponse);

        return true;
    }
    else
        return false;
}

void STB::Service::FillActionList(std::string XMLresponse)
{
    
}
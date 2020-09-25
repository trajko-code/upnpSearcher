#include <STB.h>

STB::STB(std::string fname, std::string uuid, std::string address, std::string port, std::string xmlLocation, std::string nt)
    :STB(uuid, address, port, xmlLocation, nt)
{
    this->friendlyName = fname;
}

STB::STB(std::string uuid, std::string address, std::string port, std::string xmlLocation, std::string nt)
    :uuid(uuid), address(address), port(port), configXMLLocation(xmlLocation), nt(nt)
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
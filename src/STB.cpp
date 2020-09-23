#include <STB.h>

STB::STB(std::string uuid, std::string address, std::string port, std::string xmlLocation, std::string nt)
    :uuid(uuid), address(address), port(port), configXMLLocation(xmlLocation), nt(nt)
{
}

void STB::AddService(const struct Service service)
{
    this->services.push_back(service);
}
#include "Searcher.h"
#include <chrono> 

using namespace std::chrono;

Searcher::Searcher(std::string fName, std::string uuid)
    :friendlyName(fName), uuid(uuid)
{
}

void Searcher::SearchBcast(const std::string delay, int searchTime)
{
    SearchBcast(delay, "upnp:rootdevice", searchTime);
}

void Searcher::SearchBcast(const std::string delay, const std::string target, int searchTime)
{ 
    MySocket sock(AF_INET, SOCK_DGRAM, 1900);
    if(!sock.CreateSocket())
        return;
    if(!sock.SetSockOption(SO_REUSEADDR, 1))
        return;
    if(!sock.Bind())
        return;

    struct ip_mreq group;
    group.imr_multiaddr.s_addr = inet_addr("239.255.255.250");
    group.imr_interface.s_addr = INADDR_ANY;

    if(!sock.SetSockOption(IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)))
        return;

    char recvBuf[1024];

    memset(recvBuf, 0, sizeof(recvBuf));

    MySocket fromSock;
    auto start = high_resolution_clock::now(); 

    while (true)
    {
        sock.RecieveFrom(recvBuf, sizeof(recvBuf), 0, fromSock);

        FilterResponse(std::string(recvBuf));

        if(duration_cast<seconds>(high_resolution_clock::now() - start).count() > searchTime)
            return;
    }
}

void Searcher::SearchUnicast(std::string hostname, int port, std::string target)
{

}

void Searcher::SearchUnicast(uint32_t address, int port, std::string target)
{

}

void Searcher::FilterResponse(const std::string response)
{
    if(response.find("NOTIFY") == std::string::npos)
        return;

    std::string location;
    if((location = GetHeaderValue(response, "LOCATION")).empty())
        return;   

    std::string nt = GetHeaderValue(response, "NT");

    if(nt.empty())
        return;
    else if(nt.compare("urn:zenterio-net:service:X-CTC_RemotePairing:1") != 0)
        return;
    
    std::string nts = GetHeaderValue(response, "NTS");

    if(nts.empty())
        return;
    else if(nts.compare("ssdp:alive") != 0)
        return;

    std::string usn = GetHeaderValue(response, "USN");
    if(usn.empty())
        return;

    std::string uuid = usn.substr(5, 36);
    for(auto const &stb : this->discoveredSTB)
        if(stb.GetUUID().compare(uuid) == 0)
            return;
    
    this->discoveredSTB.push_back(STB(uuid, location, nt));

    std::cout<<"New STB detected!"<<std::endl;
    std::cout<<"UUID: "<<uuid<<std::endl;
    std::cout<<"Location: "<<location<<std::endl;
    std::cout<<"NT: "<<nt<<std::endl;
}

std::string Searcher::GetHeaderValue(const std::string response, const std::string key)
{
    size_t pos = 0;

    if((pos = response.find(key+":")) == std::string::npos)
        return "";

    pos += key.length() + 2;

    return response.substr(pos, response.find("\r\n", pos) - pos);
}

#include "Searcher.h"
#include <chrono> 
#include <algorithm>

using namespace std::chrono;

#define SSDP_MULTICAST_ADDRESS "239.255.255.250"
#define SSDP_PORT 1900
#define BUFF_SIZE 1024
#define RESPONSE_OK "HTTP/1.1 200 OK"

Searcher::Searcher(std::string friendlyName)
    :friendlyName(friendlyName)
{
}

uint16_t Searcher::SearchBcast(const std::string delay, const int searchTime)
{
    return SearchBcast(delay, ALL, searchTime);
}

uint16_t Searcher::SearchBcast(const std::string delay, const std::string target, const int searchTime)
{ 
    std::string msg = "M-SEARCH * HTTP/1.1\r\n"
                    "HOST: 239.255.255.250:1900\r\n"
                    "MAN: \"ssdp:discover\"\r\n"
                    "MX: " + delay + "\r\n"
                    "ST: " + target + "\r\n"
                    "\r\n";

    MySocket sock(AF_INET, SOCK_DGRAM, SSDP_PORT);
    if(!sock.CreateSocket())
        return 0;
    if(!sock.SetSockOption(SO_REUSEADDR, 1))
        return 0;
    if(!sock.Bind())
        return 0;

    struct ip_mreq group;
    group.imr_multiaddr.s_addr = inet_addr(SSDP_MULTICAST_ADDRESS);
    group.imr_interface.s_addr = INADDR_ANY;

    if(!sock.SetSockOption(IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)))
        return 0;

    MySocket toSock(AF_INET, SOCK_DGRAM, SSDP_PORT, inet_addr(SSDP_MULTICAST_ADDRESS));
    sock.SendTo(msg.c_str(), msg.length(), 0, toSock);

    char recvBuf[BUFF_SIZE];
    memset(recvBuf, 0, BUFF_SIZE);

    MySocket fromSock;
    auto start = high_resolution_clock::now(); 

    while (duration_cast<seconds>(high_resolution_clock::now() - start).count() < searchTime)
    {
        sock.RecieveFrom(recvBuf, BUFF_SIZE, 0, fromSock);
        FilterDiscoveryResponse(std::string(recvBuf));
    }

    if(!this->discoveredSTB.empty())
        this->discoveredSTB.remove_if([this](std::shared_ptr<STB>& stb) { return !stb->GetDescription(); });
        // for(auto& stb : this->discoveredSTB)
        //     if(!stb->SearchSTBDescription())
        //         this->discoveredSTB.remove(stb);

    return this->discoveredSTB.size();
}

int Searcher::GetDetectedSTBsCount() const 
{
    return this->discoveredSTB.size();
}

void Searcher::ShowDetectedSTBs() const
{
    int i = 1;
    for(auto const& stb : this->discoveredSTB)
    {
        std::cout<< "\t" << i << ". " <<stb->GetFriendlyName() << "  (" << stb->GetUUID() << ")" << '\n';
        i++;
    }
}

void Searcher::ClearDetectedSTBs()
{
    if(this->discoveredSTB.size() > 0 )
        this->discoveredSTB.clear();
}

void Searcher::FilterDiscoveryResponse(const std::string response)
{
    if(response.find(RESPONSE_OK) == std::string::npos)
        return;

    std::string location;
    if((location = GetHeaderValue(response, "LOCATION")).empty())
        return;   

    std::string server = GetHeaderValue(response, "SERVER");
    if(server.empty())
        return;
    else if(server.find("zss/") == std::string::npos)
        return;

    // std::string st = GetHeaderValue(response, "ST");
    // if(st.empty())
    //     return;
    // else if(st.find("urn:zenterio-net:") == std::string::npos)
    //     return;

    std::string usn = GetHeaderValue(response, "USN");
    if(usn.empty())
        return;
    
    //da li izvuci u novu funkciju koja proverava da li se nalazi uuid u vektoru?
    std::string uuid = usn.substr(5, 36);   
    for(auto const &stb : this->discoveredSTB)
        if(stb->GetUUID().compare(uuid) == 0)
            return;
    
    this->discoveredSTB.push_back(this->CreateNewSTB(uuid, location));
}

void Searcher::FilterMulticastMessage(const std::string response)
{
    if(response.find("NOTIFY *") == std::string::npos)
        return;
    
    std::string nts = GetHeaderValue(response, "NTS");
    if(nts.empty())
        return;
    else if(nts.compare(ALIVE) != 0) //processing alive notify
        return;                                             
    else if(nts.compare(BYEBYE) != 0) //processing byebye notify
        return;
    else if(nts.compare(UPDATE) != 0) //processing update notify
        return;  

    //Process ALIVE notify
    // std::string location;
    // if((location = GetHeaderValue(response, "LOCATION")).empty())
    //     return;   

    // std::string nt = GetHeaderValue(response, "NT");
    // if(nt.empty())
    //     return;
    // // else if(nt.compare(ROOTDEVICE) != 0)
    // //    return;
    
    // std::string usn = GetHeaderValue(response, "USN");
    // if(usn.empty())
    //     return;

    // std::string uuid = usn.substr(5, 36);  
    // for(auto const &stb : this->discoveredSTB)
    //     if(stb->GetUUID().compare(uuid) == 0)
    //         return;

    // std::unique_ptr<STB> newSTB = this->CreateNewSTB(uuid, location);
    // if(SearchSTBDescription(*newSTB))
    // {
    //     this->discoveredSTB.push_back(std::move(newSTB));
    //     std::cout<<"New STB detected on network!"<<'\n';
    // }
}

std::shared_ptr<STB> Searcher::CreateNewSTB(const std::string uuid, const std::string location)
{
    unsigned short addrBegin = location.find('/') + 2;
    unsigned short portBegin = addrBegin + 16;
    unsigned short xmlBegin = location.find('/', portBegin);
    std::string address = location.substr(addrBegin, 15);
    std::string port = location.substr(portBegin, xmlBegin - portBegin);
    std::string xmlLoc = location.substr(xmlBegin, location.length() - xmlBegin);

    return std::shared_ptr<STB>(new STB(uuid, address, port, xmlLoc));
}

std::shared_ptr<STB> Searcher::GetSTB(int ordinalNumber)
{
    int i = 0;
    for(auto const &stb : this->discoveredSTB )
        if(i++ == ordinalNumber)
            return stb;
    return nullptr;
}

std::string Searcher::GetHeaderValue(const std::string response, const std::string key)
{
    size_t pos = 0;

    if((pos = response.find(key+":")) == std::string::npos)
        return "";

    pos += key.length() + 2;

    return response.substr(pos, response.find("\r\n", pos) - pos);
}

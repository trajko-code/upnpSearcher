#include "Searcher.hpp"
#include <chrono> 
#include <algorithm>

#define SSDP_MULTICAST_ADDRESS "239.255.255.250"
#define SSDP_PORT 1900
#define SSDP_DISCOVER "\"ssdp:discover\""
#define BUFF_SIZE 1024
#define RESPONSE_OK "HTTP/1.1 200 OK"
#define UUID_LENGTH 36

uint16_t Searcher::SearchBcast(const std::string delay, const int searchTime)
{
    return SearchBcast(delay, ALL, searchTime);
}

uint16_t Searcher::SearchBcast(const std::string delay, const std::string target, const int searchTime)
{ 
    std::string msg = "M-SEARCH * HTTP/1.1\r\n"
                    "HOST: " + std::string(SSDP_MULTICAST_ADDRESS) + ":" + std::to_string(SSDP_PORT) + "\r\n"
                    "MAN: " + std::string(SSDP_DISCOVER) + "\r\n"
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
    auto start = std::chrono::high_resolution_clock::now(); 

    while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - start).count() < searchTime)
    {
        sock.RecieveFrom(recvBuf, BUFF_SIZE, 0, fromSock);
        FilterDiscoveryResponse(std::string(recvBuf));
    }

    if(!this->discoveredSTB.empty())
        std::remove_if(this->discoveredSTB.begin(), this->discoveredSTB.end(), [](std::shared_ptr<setTopBox::STB>& stb) { return !stb->RequireDescription(); });

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
        InOut::Out("\t" + std::to_string(i++) + ". " + stb->GetFriendlyName() + "  (" + stb->GetUUID() + ")\n");
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
    if((location = HTTPCommunicator::GetHeaderValue(response, "LOCATION")).empty())
        return;   

    std::string server = HTTPCommunicator::GetHeaderValue(response, "SERVER");
    if(server.find("zss/") == std::string::npos)
        return;

    std::string st = HTTPCommunicator::GetHeaderValue(response, "ST");
    if(st.find("urn:zenterio-net:") == std::string::npos)
        return;

    std::string usn = HTTPCommunicator::GetHeaderValue(response, "USN");
    if(usn.empty())
        return;

    this->TryToAddNewSTB(usn, location);
}

// void Searcher::FilterMulticastMessage(const std::string response)
// {
//     if(response.find("NOTIFY *") == std::string::npos)
//         return;
    
//     std::string nts = HTTPCommunicator::GetHeaderValue(response, "NTS");
//     if(nts.empty())
//         return;
//     else if(nts.compare(ALIVE) != 0) //processing alive notify
//         return;                                             
//     else if(nts.compare(BYEBYE) != 0) //processing byebye notify
//         return;
//     else if(nts.compare(UPDATE) != 0) //processing update notify
//         return;  

//     //Process ALIVE notify
//     // std::string location;
//     // if((location = GetHeaderValue(response, "LOCATION")).empty())
//     //     return;   

//     // std::string nt = GetHeaderValue(response, "NT");
//     // if(nt.empty())
//     //     return;
//     // // else if(nt.compare(ROOTDEVICE) != 0)
//     // //    return;
    
//     // std::string usn = GetHeaderValue(response, "USN");
//     // if(usn.empty())
//     //     return;

//     // std::string uuid = usn.substr(5, 36);  
//     // for(auto const &stb : this->discoveredSTB)
//     //     if(stb->GetUUID().compare(uuid) == 0)
//     //         return;

//     // std::unique_ptr<STB> newSTB = this->CreateNewSTB(uuid, location);
//     // if(SearchSTBDescription(*newSTB))
//     // {
//     //     this->discoveredSTB.push_back(std::move(newSTB));
//     //     std::cout<<"New STB detected on network!"<<'\n';
//     // }
// }

void Searcher::TryToAddNewSTB(const std::string usn, const std::string location)
{
    size_t uuidBegin = usn.find("uuid:") + 5;   
    std::string uuid = usn.substr(uuidBegin, UUID_LENGTH);
    size_t addrBegin = location.find('/') + 2;
    size_t portBegin = location.find(':', addrBegin) + 1;
    size_t xmlBegin = location.find('/', portBegin);
    std::string address = location.substr(addrBegin, portBegin - addrBegin - 1);
    std::string port = location.substr(portBegin, xmlBegin - portBegin);
    std::string xmlLoc = location.substr(xmlBegin, location.length() - xmlBegin);

    //pri promeni frendlyName kreira se novi rootdevice sa 1 servisom bez akcija na istoj adresi pa se zbog toga i adresa ispituje
    if(!std::any_of(this->discoveredSTB.begin(), this->discoveredSTB.end(), 
        [uuid, address](const std::shared_ptr<setTopBox::STB>& stb) { return !stb->GetUUID().compare(uuid) || !stb->GetAddress().compare(address); }))
    {   
        this->discoveredSTB.push_back(std::make_shared<setTopBox::STB>(uuid, address, port, xmlLoc));
    }
}

std::shared_ptr<setTopBox::STB> Searcher::GetSTB(unsigned int ordNumber)
{
    return this->discoveredSTB[ordNumber];
}
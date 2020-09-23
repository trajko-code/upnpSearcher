#include "Searcher.h"
#include <chrono> 
#include <algorithm>

using namespace std::chrono;

Searcher::Searcher(std::string fName)
    :friendlyName(fName)
{
}

uint16_t Searcher::SearchBcast(const std::string delay, int searchTime)
{
    return SearchBcast(delay, "upnp:rootdevice", searchTime);
}

uint16_t Searcher::SearchBcast(const std::string delay, const std::string target, int searchTime)
{ 
    MySocket sock(AF_INET, SOCK_DGRAM, 1900);
    if(!sock.CreateSocket())
        return 0;
    if(!sock.SetSockOption(SO_REUSEADDR, 1))
        return 0;
    if(!sock.Bind())
        return 0;

    struct ip_mreq group;
    group.imr_multiaddr.s_addr = inet_addr("239.255.255.250");
    group.imr_interface.s_addr = INADDR_ANY;

    if(!sock.SetSockOption(IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)))
        return 0;

    char recvBuf[1024];

    memset(recvBuf, 0, sizeof(recvBuf));

    MySocket fromSock;
    auto start = high_resolution_clock::now(); 

    while (duration_cast<seconds>(high_resolution_clock::now() - start).count() < searchTime)
    {
        sock.RecieveFrom(recvBuf, sizeof(recvBuf), 0, fromSock);

        FilterResponse(std::string(recvBuf));

        //std::cout<<recvBuf<<std::endl;  
    }

    return this->discoveredSTB.size();
}

void Searcher::SearchUnicast(std::string hostname, int port, std::string target)
{
    // char msg[] = "M-SEARCH * HTTP/1.1\r\n"
    //             "HOST: " + hostname + ":" + port + "\r\n"
    //             "MAN: \"ssdp:discover\"\r\n"
    //             "ST: " + target + "\r\n"
    //             "USER-AGENT: Linux/20.04 UPnP/2.0 MyProduct/1.0\r\n"
    //             "\r\n";

    MySocket sock(AF_INET, SOCK_DGRAM, 1900);
    if(!sock.CreateSocket())
        return;
    // if(!sock.SetSockOption(SO_REUSEADDR, 1))
    //     return;
    if(!sock.Bind())
        return;
    
}

void Searcher::SearchUnicast(uint32_t address, int port, std::string target)
{

}

void Searcher::SearchSTBDescription(std::string stbUuid)
{
    auto stb = std::find_if(this->discoveredSTB.begin(), this->discoveredSTB.end(), [&stbUuid](const STB& stb) {return !stb.GetUUID().compare(stbUuid);});

    if(stb != this->discoveredSTB.end())
    {
        std::string msg = "GET " + stb->GetXMLLocation() + " HTTP/1.1\r\n"
                        "Connection: close\r\n"
                        "Host: " + stb->GetAddress() + ":" + stb->GetPort() + "\r\n"
                        //"User-Agent: linux/20.04 UPnP/2.0 myApp/1\r\n"
                        //"CPFN.UPNP.ORG: " + this->friendlyName + "\r\n"
                        "\r\n";
        
        MySocket sock(AF_INET, SOCK_STREAM);
        if(!sock.CreateSocket())
            return;
        
        if(!sock.Connect(AF_INET, std::stoi(stb->GetPort()), inet_addr(stb->GetAddress().c_str())))
            return;

        sock.Send(msg.c_str(), msg.length(), 0);

        std::string response = "";
        char recvBuf[1024];
        memset(recvBuf, 0, sizeof(recvBuf));

        while(sock.Recieve(recvBuf, sizeof(recvBuf), 0) != 0)
        {
            response += recvBuf;
            memset(recvBuf, 0, sizeof(recvBuf));
        }

        std::cout<<"Recived message:"<<std::endl;
        std::cout<<response<<std::endl;
    }   
    else
    {
        std::cout<<"Not found"<<std::endl;
    }
}

void Searcher::FilterResponse(const std::string response)
{
    if(response.find("NOTIFY") == std::string::npos)  // "NOTIFY *"
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
    
    unsigned short addrBegin = location.find('/') + 2;
    unsigned short portBegin = addrBegin + 16;
    unsigned short xmlBegin = location.find('/', portBegin);
    std::string address = location.substr(addrBegin, 15);
    std::string port = location.substr(portBegin, xmlBegin - portBegin);
    std::string xmlLoc = location.substr(xmlBegin, location.length() - xmlBegin);
    
    this->discoveredSTB.push_back(STB(uuid, address, port, xmlLoc, nt));

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

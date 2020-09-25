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
    std::string msg = "M-SEARCH * HTTP/1.1\r\n"
                    "HOST: 239.255.255.250:1900\r\n"
                    "MAN: \"ssdp:discover\"\r\n"
                    "MX: " + delay + "\r\n"
                    "ST: " + target + "\r\n"
                    "\r\n";

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

    MySocket toSock(AF_INET, SOCK_DGRAM, 1900, inet_addr("239.255.255.250"));
    for(int i=0; i<3; i++)
        sock.SendTo(msg.c_str(), msg.length(), 0, toSock);

    char recvBuf[1024];
    memset(recvBuf, 0, sizeof(recvBuf));

    MySocket fromSock;
    auto start = high_resolution_clock::now(); 

    //while (duration_cast<seconds>(high_resolution_clock::now() - start).count() < searchTime)
    for(int i=0; i<15; i++)
    {
        sock.RecieveFrom(recvBuf, sizeof(recvBuf), 0, fromSock);

        FilterDiscoveryResponse(std::string(recvBuf));

        //std::cout<<recvBuf<<std::endl;  
    }

    return this->discoveredSTB.size();
}

bool Searcher::SearchSTBDescription(STB& stb)
{
    std::string msg = "GET " + stb.GetXMLLocation() + " HTTP/1.1\r\n"
                        "Connection: close\r\n"
                        "Host: " + stb.GetAddress() + ":" + stb.GetPort() + "\r\n"
                        "\r\n";
        
    MySocket sock(AF_INET, SOCK_STREAM);
    if(!sock.CreateSocket())
        return false;
    
    if(!sock.Connect(AF_INET, std::stoi(stb.GetPort()), inet_addr(stb.GetAddress().c_str())))
        return false;

    sock.Send(msg.c_str(), msg.length(), 0);

    std::string XMLresponse = "";
    char recvBuf[1024];
    memset(recvBuf, 0, sizeof(recvBuf));

    while(sock.Recieve(recvBuf, sizeof(recvBuf), 0) != 0)
    {
        if(!XMLresponse.empty())
            XMLresponse.pop_back();
        XMLresponse += recvBuf;
        memset(recvBuf, 0, sizeof(recvBuf));
    }
    
    if(!XMLresponse.empty())
    {
        FillSTBName(XMLresponse, stb);
        FillServiceList(XMLresponse, stb);

        return true;
    }
    else
        return false;
}

void Searcher::ShowDetectedSTBs() const
{
    for(auto const& stb : this->discoveredSTB)
    {
        std::cout<<stb->GetFriendlyName()<<std::endl;
    }
}

void Searcher::FilterDiscoveryResponse(const std::string response)
{
    if(response.find("NOTIFY") == std::string::npos)  // "NOTIFY *"
        return;

    std::string location;
    if((location = GetHeaderValue(response, "LOCATION")).empty())
        return;   

    std::string nt = GetHeaderValue(response, "NT");

    if(nt.empty())
        return;
    else if(nt.compare("upnp:rootdevice") != 0)
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
    unsigned short addrBegin = location.find('/') + 2;
    unsigned short portBegin = addrBegin + 16;
    unsigned short xmlBegin = location.find('/', portBegin);
    std::string address = location.substr(addrBegin, 15);
    std::string port = location.substr(portBegin, xmlBegin - portBegin);
    std::string xmlLoc = location.substr(xmlBegin, location.length() - xmlBegin);

    for(auto const &stb : this->discoveredSTB)
        if(stb->GetUUID().compare(uuid) == 0)
            return;

    std::unique_ptr<STB> newSTB(new STB(uuid, address, port, xmlLoc, nt));
    
    if(SearchSTBDescription(*newSTB))
        this->discoveredSTB.push_back(std::move(newSTB));
}

void Searcher::FillSTBName(const std::string response, STB& stb)
{
    stb.SetFriendlyName(GetTagValue(response, "friendlyName"));
}

void Searcher::FillServiceList(std::string response, STB& stb)
{
    std::string serviceList = GetTagValue(response, "serviceList").substr(1);
    std::string serviceXML;
    while( serviceList.length() > 0 )
    {
        serviceXML = GetTagValue(serviceList, "service");
        ParseServiceFromXML(serviceXML, stb);
        int cropPos = serviceXML.length() + 2 * sizeof("service") + 4;
        if(cropPos > serviceList.length())
            return;
        serviceList = serviceList.substr(cropPos);
    }
}

void Searcher::ParseServiceFromXML(std::string XMLservice, STB& stb)
{
    std::string type = GetTagValue(XMLservice, "serviceType");
    std::string id = GetTagValue(XMLservice, "serviceId");
    std::string controlURL = GetTagValue(XMLservice, "controlURL");
    std::string eventURL = GetTagValue(XMLservice, "eventSubURL");
    std::string descriptionURL = GetTagValue(XMLservice, "SCPDURL");

    stb.AddService(type, id, controlURL, eventURL, descriptionURL);
}

std::string Searcher::GetHeaderValue(const std::string response, const std::string key)
{
    size_t pos = 0;

    if((pos = response.find(key+":")) == std::string::npos)
        return "";

    pos += key.length() + 2;

    return response.substr(pos, response.find("\r\n", pos) - pos);
}

std::string Searcher::GetTagValue(std::string response, const std::string tagName)
{
    unsigned short tagBegin = response.find("<" + tagName + ">") + tagName.length() + 2;
    unsigned short tagEnd = response.find("</" + tagName + ">", tagBegin);

    return response.substr(tagBegin, tagEnd - tagBegin);
}

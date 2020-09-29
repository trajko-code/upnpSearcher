#include "HTTPCommunicator.h"

#define BUFF_SIZE 1024

std::string HTTPCommunicator::GetXMLDescription(std::string XMLUrl, std::string address, std::string port)
{
    std::string msg = "GET " + XMLUrl + " HTTP/1.1\r\n"
                        "Connection: close\r\n"
                        "Host: " + address + ":" + port + "\r\n"
                        "\r\n";
        
    MySocket sock(AF_INET, SOCK_STREAM);
    if(!sock.CreateSocket())
        return "";
    
    if(!sock.Connect(AF_INET, std::stoi(port), inet_addr(address.c_str())))
        return "";

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
    
    return XMLresponse;  
}
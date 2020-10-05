#include "HTTPCommunicator.h"
#include <iostream>

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

    while(sock.Recieve(recvBuf, BUFF_SIZE - 1, 0) != 0)
    {
        recvBuf[BUFF_SIZE - 1] = '\0';

        XMLresponse += recvBuf;
        memset(recvBuf, 0, BUFF_SIZE);
    }
    
    return XMLresponse;  
}

std::string HTTPCommunicator::PostExecuteAction(std::string controlURL, std::string address, std::string port, std::string soapAction, std::string body)
{
    std::string msg = "POST " + controlURL + " HTTP/1.1\r\n"
                        "HOST: " + address + ":" + port + "\r\n"
                        "CONTENT-LENGTH: " + std::to_string(body.length()) + "\r\n"
                        "CONTENT-TYPE: text/xml; charset=\"utf-8\"\r\n"
                        "USER-AGENT: " + USER_AGENT + "\r\n"
                        "SOAPACTION: \"" + soapAction + "\"\r\n"
                        "\r\n"
                        + body;
        
    MySocket sock(AF_INET, SOCK_STREAM);
    if(!sock.CreateSocket())
        return "";
    
    if(!sock.Connect(AF_INET, std::stoi(port), inet_addr(address.c_str())))
        return "";

    sock.Send(msg.c_str(), msg.length(), 0);

    std::string PostResponse = "";
    char recvBuf[BUFF_SIZE];
    memset(recvBuf, 0, BUFF_SIZE);

    while(sock.Recieve(recvBuf, BUFF_SIZE - 1, 0) != 0)
    {
        recvBuf[BUFF_SIZE - 1] = '\0';

        PostResponse += recvBuf;
        memset(recvBuf, 0, BUFF_SIZE);
    }
    
    size_t pos = PostResponse.find(" 200 OK");

    if(pos == std::string::npos)
    {
        size_t endLinePos = PostResponse.find("\r\n");
        std::cout << PostResponse.substr(0, endLinePos) << '\n';
        return "";
    }

    return PostBodyFromResponse(PostResponse);
}

std::string HTTPCommunicator::PostBodyFromResponse(std::string postResponse)
{
    size_t soapBegin = postResponse.find('<');

    if(soapBegin == std::string::npos)
        return postResponse;
    
    return postResponse.substr(soapBegin, postResponse.length() - soapBegin);
}
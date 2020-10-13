#include "HTTPCommunicator.hpp"

#define BUFF_SIZE 1024
#define CHARSET "\"utf-8\""
#define CONTENT_TYPE "text/xml"

std::string HTTPCommunicator::GetXMLDescription(const std::string XMLUrl, const std::string address, const std::string port)
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

std::string HTTPCommunicator::PostExecuteAction(const std::string controlURL, const std::string address, const std::string port, const std::string soapAction, const std::string body)
{
    std::string msg = "POST " + controlURL + " HTTP/1.1\r\n"
                        "HOST: " + address + ":" + port + "\r\n"
                        "CONTENT-LENGTH: " + std::to_string(body.length()) + "\r\n"
                        "CONTENT-TYPE: " + std::string(CONTENT_TYPE) + "; charset=" + std::string(CHARSET) + "\r\n"
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
        InOut::Out(PostResponse.substr(0, endLinePos) + '\n');
        return "";
    }

    return PostBodyFromResponse(PostResponse);
}

std::string HTTPCommunicator::GetHeaderValue(const std::string httpMsg, const std::string headerName)
{
    size_t pos = 0;

    if((pos = httpMsg.find(headerName + ":")) == std::string::npos)
        return "";

    pos += headerName.length();

    while(httpMsg[++pos] == ' ') {}

    return httpMsg.substr(pos, httpMsg.find("\r\n", pos) - pos);
}

std::string HTTPCommunicator::PostBodyFromResponse(const std::string postResponse)
{
    size_t soapBegin = postResponse.find('<');

    if(soapBegin == std::string::npos)
        return postResponse;
    
    return postResponse.substr(soapBegin, postResponse.length() - soapBegin);
}
#include "socket.h"
#include <iostream>
#include <thread>
#include <mutex>

constexpr size_t SEND_SIZE = 1024;
ServerSocket* ServerSocket::m_instance = nullptr;

ServerSocket* ServerSocket::GetInstance()
{
    static std::once_flag onceFlag;
    std::call_once(onceFlag, []() 
        {
            m_instance = new ServerSocket();
        });

    return m_instance;
}

/*
void ServerSocket::Send(const std::string& buf)
{
    const char* buffer = buf.c_str();
    int sendLen = 0;
    size_t length = buf.size();

    char sendBuf[SEND_SIZE + 1]{ 0 };
    while (true)
    {
        if (sendLen + 1024 > length)
        {
            sendLen += send(m_accept, buffer, length - sendLen, 0);
            break;
        }
        else
        {
            memcpy(sendBuf, buffer, SEND_SIZE);
            sendLen += send(m_accept, sendBuf, 1024, 0);
        }
    }
}

void ServerSocket::Send(char* buffer,int size)
{
    int  sendLen = 0;
    char sendBuf[SEND_SIZE + 1]{ 0 };
    while (true)
    {
        if (sendLen + 1024 > size)
        {
            sendLen += send(m_accept, buffer, size - sendLen, 0);
            break;
        }
        else
        {
            memcpy(sendBuf, buffer, SEND_SIZE);
            buffer += SEND_SIZE;
            sendLen += send(m_accept, sendBuf, 1024, 0);
        }
    }
}
*/

ServerSocket::ServerSocket():
    m_socket(0),
    m_accept(0)
{
}

void ServerSocket::AddBuffer(const std::string& buffer)
{
    m_buffers.emplace(buffer);
}

void ServerSocket::SendBuffer()
{
    while (true)
    {
        if (!m_buffers.empty())
        {
            auto strBuffer = m_buffers.front();
            m_buffers.pop();

            auto buffer = strBuffer.c_str();
            auto size = strBuffer.size();

            size_t  sendLen = 0;
            char sendBuf[SEND_SIZE + 1]{ 0 };
            while (true)
            {
                if (sendLen + 1024 > size)
                {
                    sendLen += send(m_accept, buffer, static_cast<int>(size - sendLen), 0);
                    break;
                }
                else
                {
                    memcpy(sendBuf, buffer, SEND_SIZE);
                    buffer += SEND_SIZE;
                    sendLen += send(m_accept, sendBuf, 1024, 0);
                }
            }
        }
    }
}

void ServerSocket::InitSocket()
{
    //初始化WSA  
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(sockVersion, &wsaData) != 0)
    {
        throw("init winsock failed!\n");
    }

    //创建套接字  
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == INVALID_SOCKET)
    {
        throw("create socket failed!\n");
    }

    //绑定IP和端口  
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(8888);
    sin.sin_addr.S_un.S_addr = INADDR_ANY;
    if (bind(m_socket, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
    {
        throw("bind failed!\n");
    }

    //开始监听  
    if (listen(m_socket, 5) == SOCKET_ERROR)
    {
        throw("listen failed!\n");
    }
}

void ServerSocket::AcceptSocket()
{
    sockaddr_in remoteAddr;
    int nAddrlen = sizeof(remoteAddr);

    std::cout << "waiting for connection...\n";

    m_accept = accept(m_socket, (SOCKADDR*)&remoteAddr, &nAddrlen);
    if (m_socket == INVALID_SOCKET)
    {
        throw("accept failied!\n");
    }

    std::cout << "Connection successful : " << inet_ntoa(remoteAddr.sin_addr) << '\n';

    std::thread t([this]() {this->SendBuffer(); });
    t.detach();
}

void ServerSocket::ClostSocket()
{
    closesocket(m_accept);
    closesocket(m_socket);
    WSACleanup();
}

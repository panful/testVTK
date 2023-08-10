#ifndef _SERVER_SOCKET_H_
#define _SERVER_SOCKET_H_

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <string>
#include <queue>

class ServerSocket
{
public:
    static ServerSocket* GetInstance();

private:
    static ServerSocket* m_instance;

public:
    //std::string Recv();
    //void Send(const std::string& buffer);
    //void Send(char* buffer, int size);

    /// @brief 添加一个buffer到Socket发送队列
    void AddBuffer(const std::string& buffer);

    void InitSocket();
    void AcceptSocket();
    void ClostSocket();

private:
    ServerSocket();
    ~ServerSocket() = default;

    /// @brief 从发送队列依次读取并发送
    void SendBuffer();

private:
    SOCKET m_socket;
    SOCKET m_accept;
    std::queue<std::string> m_buffers;
};

#endif // !_SERVER_SOCKET_H_

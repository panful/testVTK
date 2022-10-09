#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <WinSock2.h>
#include <string>

#include "window.h"

class Socket
{
public:
    static Socket* GetInstance();

    const std::string Recv();
    void Recv2();
    void Send(const char* buffer);

    void SetWindow(Window* w) { m_window = w; }

    void InitSocket();
    void ConnectSocket();
    void ClostSocket();
private:
    Socket();
    ~Socket() = default;
    Socket& operator=(const Socket&) = delete;
    Socket(const Socket&) = delete;


private:
    static Socket* m_instance;
    SOCKET m_socket;

    Window* m_window;
};

#endif // !_SOCKET_H_

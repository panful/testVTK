#include <iostream>
#include "window.h"
#include "socket.h"

int main(int c,char** v)
{
    std::clog << "   === server start ===\n";

    ServerSocket::GetInstance()->InitSocket();
    ServerSocket::GetInstance()->AcceptSocket();
    ServerSocket::GetInstance()->Run();

    WindowVTK* w = new WindowVTK();
    w->MyStart();

    return 0;
}

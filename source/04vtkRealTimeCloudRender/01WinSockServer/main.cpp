#include <iostream>
#include <thread>
#include <memory>
#include "window.h"
#include "socket.h"

int main(int c,char** v)
{
    std::cout << "start\n";

    ServerSocket::GetInstance()->InitSocket();
    ServerSocket::GetInstance()->AcceptSocket();

    Window w(1, 800, 600);

    return 0;
}

#include <iostream>
#include <QApplication>
#include "window.h"
#include "socket.h"

int main(int argc,char** argv)
{   
    std::clog << "   === client start ===\n";

    QApplication app(argc, argv);

    ClientSocket::GetInstance()->InitSocket();
    ClientSocket::GetInstance()->ConnectSocket();
    ClientSocket::GetInstance()->Run();


    //ClientSocket::GetInstance()->AddBuffer("test 123 +-*/ end");

    Window* window = new Window();
    window->resize(800, 600);
    window->show();

    return app.exec();
}

#include "window.h"
#include <QApplication>

#include "socket.h"

int main(int argc,char** argv)
{
    QApplication app(argc, argv);

    Window* window = new Window();
    window->resize(800, 600);
    window->show();

    Socket::GetInstance()->SetWindow(window);

    return app.exec();
}

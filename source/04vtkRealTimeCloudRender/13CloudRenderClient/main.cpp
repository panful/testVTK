#include <QApplication>
#include "clientWidget.h"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    VTKWidget w;
    w.setWindowTitle("Client");
    w.setMinimumSize(800, 600);
    w.setMaximumSize(800, 600);
    w.show();

    return app.exec();
}

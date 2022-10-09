#include <QApplication>
#include "widget.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    VTKWidget w;
    w.setWindowTitle("origin");
    w.setMinimumSize(800, 600);
    w.show();

    return app.exec();
}

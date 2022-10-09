#include <QApplication>
#include "window.h"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    Window w;
    w.setMinimumSize(800, 600);
    w.show();

    return app.exec();
}

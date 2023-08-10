#include <QApplication>
#include "window_qt.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    WindowQt w;
    w.setWindowTitle("Qt");
    w.setMinimumSize(800, 600);
    w.show();

    return app.exec();
}

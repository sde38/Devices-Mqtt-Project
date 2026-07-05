#include <QApplication>
#include "MainWindows.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindows w;
    w.show();

    return app.exec();
}
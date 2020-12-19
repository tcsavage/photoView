#include <iostream>
#include <string>

#include <QApplication>

#include "MainWindow.hpp"

int main(int argc, char* argv[]) {
    // Initialise Qt and QML.
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setOrganizationName("Tom Savage");
    QCoreApplication::setOrganizationDomain("tomsavage.dev");
    QCoreApplication::setApplicationName("Photo LUTs");
    QApplication app(argc, argv);
    MainWindow wnd;
    wnd.show();
    return app.exec();
}

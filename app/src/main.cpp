#include <iostream>
#include <string>

#include <QApplication>
#include <QIcon>

#include "MainWindow.hpp"

int main(int argc, char* argv[]) {
    // Initialise Qt and QML.
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setOrganizationName("Tom Savage");
    QCoreApplication::setOrganizationDomain("tomsavage.dev");
    QCoreApplication::setApplicationName("Photo View");
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/icons/appIcon"));
    MainWindow wnd;
    wnd.show();
    return app.exec();
}

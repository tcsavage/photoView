#include <iostream>
#include <string>

#include <QApplication>
#include <QIcon>

#include <image/opencl/Manager.hpp>

#include "PhotoWindow.hpp"

int main(int argc, char* argv[]) {
    // Initialise OpenCL.
    image::opencl::Manager oclMan;
    // Initialise Qt and QML.
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setOrganizationName("Tom Savage");
    QCoreApplication::setOrganizationDomain("tomsavage.dev");
    QCoreApplication::setApplicationName("Photo View");
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/icons/appIcon"));
    PhotoWindow wnd;
    if (argc > 1) {
        QString filename = argv[1];
        wnd.openImage(filename);
    }
    wnd.show();
    return app.exec();
}

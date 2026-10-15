#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("RideShare");
    app.setOrganizationName("RideShare Student Project");

    MainWindow window;
    window.show();

    return app.exec();
}

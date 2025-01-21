#include "mainwindow.h"
#include <QApplication>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    // Versteckt das Konsolenfenster unter Windows
    ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
} 
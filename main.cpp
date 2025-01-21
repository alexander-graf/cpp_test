#include <QApplication>
#include "mainwindow.h"

#ifdef _WIN32
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    int argc = 0;
    char **argv = nullptr;
#else
int main(int argc, char *argv[])
{
#endif
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
} 
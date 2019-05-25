#include "MainWindow.h"
#include <QApplication>

int run(int argc, char *argv[]);


int main(int argc, char *argv[])
{
    run(argc,argv);
}

int run(int argc, char *argv[])
{
    QApplication simulationApplication(argc, argv);
    MainWindow mainWindow;
    mainWindow.show();

    return simulationApplication.exec();
}

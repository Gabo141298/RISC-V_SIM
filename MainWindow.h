#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define STEP

#include <QMainWindow>
#include "simulationmanager.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_runButtonPressed_pressed();

    void on_selectHilillosButton_pressed();

    void on_pushButton_pressed();

public: signals:
    void stepIN();

private:
    Ui::MainWindow *ui;
    QString openFile();
    void centerAndResize();
    SimulationManager* simulationManager;
    QString dir;
    int quatum;
    void beginSimulation();

};

#endif // MAINWINDOW_H

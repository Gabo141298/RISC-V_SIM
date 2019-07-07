#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define STEP

#include <QLCDNumber>
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

public slots:
    void updateLeds(const int processor, const int hilillo);
    void displayResults(const QString processorsData, const QString hilillosData);
public: signals:
    void stepIN();

private:
    Ui::MainWindow *ui;
    QString openFile();
    void centerAndResize();
    SimulationManager* simulationManager;
    QString dir;
    QLCDNumber* lcd[3];
    int quatum;


};

#endif // MAINWINDOW_H

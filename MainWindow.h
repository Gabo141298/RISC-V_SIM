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

// View class
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief MainWindow constructor of thw view class
     * @param parent inherite from
     */
    explicit MainWindow(QWidget *parent = nullptr);
    /// Destructor
    ~MainWindow();

private slots:
    /// Start button
    void on_runButtonPressed_pressed();
    /// Button that selects a folder, where the hilillos are store
    void on_selectHilillosButton_pressed();
    /// Step by step button
    void on_pushButton_pressed();

public slots:
    /// Slot that listen when the leds need to update
    void updateLeds(const int processor, const int hilillo);
    /// Slot waiting when  the simulation is over, in order to display the results
    void displayResults(const QString processorsData, const QString hilillosData);
public: signals:
    /// When flag Step define, step by step starts, increments the barrier
    void stepIN();

private:
    Ui::MainWindow *ui;
    QString openFile();
    /// Centers and resize the main window
    void centerAndResize();
    /// Pointer to the model, that handles the simulation
    SimulationManager* simulationManager;
    /// Filepath of the hilillos
    QString dir;
    /// Array of each lcd light in the ui
    QLCDNumber* lcd[3];
    /// Quatum given by the user
    int quatum;


};

#endif // MAINWINDOW_H

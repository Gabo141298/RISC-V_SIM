#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

private:
    Ui::MainWindow *ui;
    QString openFile();
    void centerAndResize();
};

#endif // MAINWINDOW_H

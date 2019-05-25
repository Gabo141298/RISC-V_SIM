#include "MainWindow.h"
#include "ui_mainwindow.h"
#include <QIntValidator>
#include <QDebug>
#include <QFileDialog>
#include <QStyle>
#include <QDesktopWidget>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QMainWindow::setWindowTitle(QString("Risc-V Simulator"));
    centerAndResize();
    //this->ui->runButtonPressed->setEnabled(false);
    this->ui->numHilillosInput->setValidator(new QIntValidator(0,100,this));
    this->ui->quantumInput->setValidator(new QIntValidator(0,100,this));
    qDebug() << QCoreApplication::applicationDirPath() ;
    this->ui->selectedDirLabel->setText(QCoreApplication::applicationDirPath() );
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_runButtonPressed_pressed()
{
    qDebug("Presionado");
    this->ui->numHilillosInput->setReadOnly(true);
    this->ui->quantumInput->setReadOnly(true);

}

QString MainWindow::openFile()
{
  QString filename =
          QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                       "/home",
                                                       QFileDialog::ShowDirsOnly
                                                       | QFileDialog::DontResolveSymlinks);

  if( !filename.isNull() )
  {
    qDebug() << "selected file path : " << filename.toUtf8();
    return filename;
  }
  return nullptr;
}

void MainWindow::on_selectHilillosButton_pressed()
{
    QString path;
    if ((path = openFile()) == nullptr )
        return;

    this->ui->selectedDirLabel->setText(path);

}

void MainWindow::centerAndResize() {
    // get the dimension available on this screen
    QSize availableSize = QGuiApplication::primaryScreen()->geometry().size();;
    int width = availableSize.width();
    int height = availableSize.height();
    qDebug() << "Available dimensions " << width << "x" << height;
    width *= 0.6; // 90% of the screen size
    height *= 0.6; // 90% of the screen size
    qDebug() << "Computed dimensions " << width << "x" << height;
    QSize newSize( width, height );

    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            newSize,
            QGuiApplication::primaryScreen()->geometry()
        )
    );
}

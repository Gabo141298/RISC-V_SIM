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
    this->quatum = 10;
    ui->setupUi(this);
    QMainWindow::setWindowTitle(QString("Risc-V Simulator"));
    centerAndResize();
    this->ui->quantumInput->setText("10");
    this->ui->quantumInput->setValidator(new QIntValidator(10,100,this));
    qDebug() << QCoreApplication::applicationDirPath() ;
    this->ui->selectedDirLabel->setText(QCoreApplication::applicationDirPath() );

    this->ui->pushButton->setVisible(false);

    this->ui->lcdNumber->display(0);
    this->ui->lcdNumber_2->display(1);
    this->ui->lcdNumber_3->display(2);
    this->lcd[0] = this->ui->lcdNumber;
    this->lcd[1] = this->ui->lcdNumber_2;
    this->lcd[2] = this->ui->lcdNumber_3;

    #ifdef STEP
    this->ui->pushButton->setVisible(true);
    #endif

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateLeds(const int processor, const int hilillo)
{
    this->lcd[processor]->display(hilillo);
}

void MainWindow::displayResults(const QString processorsData, const QString hilillosData)
{
   QWidget* temp = new QWidget();
   QLabel* label = new QLabel(temp);
   QLabel* label2 = new QLabel(temp);

   QVBoxLayout *layout = new QVBoxLayout(temp);

   label->setWordWrap(true);
   label->setText(processorsData);

   layout->addWidget(label);

   label2->setWordWrap(true);
   label2->setText(hilillosData);
   layout->addWidget(label2);

   temp->setWindowModality(Qt::ApplicationModal);
   temp->show();

}

void MainWindow::on_runButtonPressed_pressed()
{
    this->quatum = this->ui->quantumInput->text().toInt();
    if ( this->quatum < 10 || this->dir.isNull() )
    {
        qDebug() << "Invalid data, Quatum must be at least 10, and dir path must contain hilillos";
        this->ui->quantumInput->setText("10");
        return;
    }
    qDebug() << "Begining simulation";

    // Creates a new model controller
    this->simulationManager = new SimulationManager(size_t(this->quatum), this->dir,size_t(3));

    // Connects, the model with the UI
    connect(this->simulationManager, &SimulationManager::changeLeds, this, &MainWindow::updateLeds);
    connect(this->simulationManager, &SimulationManager::sendResultsToUI, this, &MainWindow::displayResults);
    connect(this, &MainWindow::stepIN, this->simulationManager, &SimulationManager::incrementBarrier);
    simulationManager->beginSimulation();
}


QString MainWindow::openFile()
{
  // Dialog that gets the path given from the user
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
    // Gets the path from the user
    QString path;
    if ((path = openFile()) == nullptr )
       return;
    this->dir = path;
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

void MainWindow::on_pushButton_pressed()
{
    emit stepIN();
}

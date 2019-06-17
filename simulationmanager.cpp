#include "simulationmanager.h"
#include <QDirIterator>
#include <QDebug>
#include "processor.h"
#include <QObject>

SimulationManager::SimulationManager()
{

}

SimulationManager::SimulationManager(const int quatum, const QString dir, const size_t numberOfProccesors):
    quatum{quatum} ,
    dir{dir}
{
    this->hilillos.resize(10);
    this->processors.resize(numberOfProccesors);
}

void SimulationManager::beginSimulation()
{
    // Sequential read of the files
    readHilillos();
    // Launch the threads
    createProcessors();
    // Distribute program
    distributeHilillos();

}


void SimulationManager::createProcessors()
{
    for (size_t index = 0; index < this->processors.size(); ++index)
    {
        // Create processor threads and add to an array
        Processor *processorThread = this->processors.at(index) = new Processor(index);
        //connect(processorThread, &Processor::resultReady, this, &MyObject::handleResults);
        //QObject::connect(processorThread, &Processor::finished, processorThread, &QObject::deleteLater);
    }
}

void SimulationManager::distributeHilillos()
{
   auto iterator =  this->hilillos.begin();

   iterator.operator++();

}

void SimulationManager::readHilillos()
{
    QDirIterator directoryIterator(this->dir, QStringList());
    size_t fileID = 0;
    while (directoryIterator.hasNext())
    {
        QFile file(directoryIterator.next());
        if (QFileInfo(directoryIterator.filePath()).isFile())
            if (QFileInfo(directoryIterator.filePath()).suffix() == "txt")
            {
                if (!file.open(QIODevice::ReadOnly))
                {
                    qDebug() << "Couldnt open " << directoryIterator.filePath();
                    return;
                }

                QTextStream in(&file);
                while (!in.atEnd())
                {
                    QString line = in.readLine();
                    QStringList list = line.split(" ");

                    for ( const auto& iterator : list  )
                    {
                       this->hilillos.at(fileID).push_back(iterator.toInt());
                    }
                }
                ++fileID;
            }

    }

}





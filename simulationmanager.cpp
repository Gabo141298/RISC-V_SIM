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
    dir{dir},
    numOfProcessor{numberOfProccesors}
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
    // Start each processors
    processorRun();

}


void SimulationManager::createProcessors()
{

    for (size_t index = 0; index < this->processors.size(); ++index)
    {
        // Create processor threads and add to an array
        Processor *processorThread = this->processors.at(index) = new Processor(index);
        (void)processorThread;
        //connect(processorThread, &Processor::resultReady, this, &MyObject::handleResults);
        //QObject::connect(processorThread, &Processor::finished, processorThread, &QObject::deleteLater);
    }
}

void SimulationManager::distributeHilillos()
{
    // Iterate through the hilillos
    std::vector<std::vector<int>>::iterator iteratorBegin = this->hilillos.begin();
    const std::vector<std::vector<int>>::iterator iteratorEnd = this->hilillos.end();
    
    std::vector< std::vector<int>* > memoryHilillos(numOfProcessor);

    // Gets a pointer to the instruction memory of each processor
    for (size_t index = 0; index < this->processors.size(); ++index)
    {
        memoryHilillos[index] = this->processors[index]->getInstructionMemory();
    }
    // numOfProcessor cambiar todos los 3
    // Counter that stores the current memory location of each processor
    size_t counter[3] = {0,0,0};

    // Processor currently
    size_t pos = 0;

    // Iterate though the hilillos
    for (/*iteratorBegin*/; iteratorBegin != iteratorEnd; ++iteratorBegin)
    {
        for(size_t index = 0; index < iteratorBegin->size(); ++index)
        {
            qDebug() << "Asignig to processor" << pos%3 << "at mem pos" << counter[pos%3] << "a" << iteratorBegin->at(index);
            memoryHilillos[pos%3]->at(counter[pos%3]) = iteratorBegin->at(index);
            ++counter[pos%3];
        }
        ++pos;
    }
}

void SimulationManager::processorRun()
{
    pthread_barrier_t* barrier = new pthread_barrier_t();
    // Aca se puede hacer mas general, al igual que en varios lados, para que no sea un 3 s
    pthread_barrier_init(barrier,nullptr, unsigned( numOfProcessor) );
    for (size_t index = 0; index < this->processors.size(); ++index)
    {
       this->processors.at(index)->init_barrier(barrier);
       this->processors.at(index)->start();
    }
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





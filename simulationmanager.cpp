#include "simulationmanager.h"
#include <QDirIterator>
#include <QDebug>
#include "processor.h"
#include <QObject>
#include <QtAlgorithms>
#include <algorithm>
#include <iostream>



SimulationManager::SimulationManager()
{

}

SimulationManager::SimulationManager(const size_t quatum, const QString dir, const size_t numberOfProccesors):
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
    // Distribute progrhilillosam
    distributeHilillos();
    // Start each processors
    processorRun();

}


void SimulationManager::createProcessors()
{

    for (size_t index = 0; index < this->processors.size(); ++index)
    {
        // Create processor threads and add to an array
        Processor *processorThread = this->processors.at(index) = new Processor(index, this->quatum);
        connect(this->processors.at(index), &Processor::contextChange, this, &SimulationManager::contextSwitch);
    }
    // The processor 0 is the one that sends the results when all is over
    connect(this->processors.at(0), &Processor::emitResults, this, &SimulationManager::gatherResults);
}

void SimulationManager::contextSwitch(const int processor, const int hilillo)
{
    emit changeLeds(processor, hilillo);
}

void SimulationManager::gatherResults(const QString processorsData, const QString hilillosData)
{
    emit sendResultsToUI(processorsData, hilillosData);
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

    // Counter that stores the current memory location of each processor
    size_t counter[3] = {0,0,0};
    int id = 0;

    // Processor currently
    size_t pos = 0;

    // Iterate though the hilillos
    for (/*iteratorBegin*/; iteratorBegin != iteratorEnd; ++iteratorBegin)
    {
        // Removing empy std::vector
        if (iteratorBegin->size() > 0)
        {
            // Creates a temporal pcb, with the initial needed data
            Pcb* temp = new Pcb(counter[pos%3], id);
            this->processors.at(pos%3)->pushPcb(temp);

            qDebug() << "Init PCB" << "with hilillo" << id << "Beginning at mem pos" << counter[pos%3] << "to proccesor" << pos%3;

            // Copies each instruction into the corresponding instruction memory
            for(size_t index = 0; index < iteratorBegin->size(); ++index)
            {
                memoryHilillos[pos%3]->at(counter[pos%3]) = iteratorBegin->at(index);
                ++counter[pos%3];
            }
            ++pos;
            ++id;
        }
    }
}

void SimulationManager::processorRun()
{
    // Creates tje new barrier, that keeps the syncronus clocks
    barrier = new pthread_barrier_t();

    #ifdef STEP
    // Aca se puede hacer mas general, al igual que en varios lados, para que no sea un 3 s
    pthread_barrier_init(barrier,nullptr, unsigned( numOfProcessor) + 1 );
    #else
    // Init the barrier
    pthread_barrier_init(barrier,nullptr, unsigned( numOfProcessor));
    #endif

    // Initialice each processor with a barrier, and tell it to start
    for (size_t index = 0; index < this->processors.size(); ++index)
    {
       this->processors.at(index)->processors = this->processors;
       this->processors.at(index)->init_barrier(barrier);
       this->processors.at(index)->start();
    }
}

void SimulationManager::incrementBarrier()
{
    #ifdef STEP
    //qDebug() << "Wainting";
    pthread_barrier_wait(this->barrier);
    #endif
}

static bool VersionCompare(const QFile* i, const QFile* j)
{
    // Compares to file names
    return i->fileName() < j->fileName();
}

void SimulationManager::readHilillos()
{
    // List with all the files
    QList<QFile*> listFiles;
    // Iterator of the directory path
    QDirIterator directoryIterator(this->dir, QStringList());
    size_t fileID = 0;

    // While there is file
    while (directoryIterator.hasNext())
    {
        QFile* file = new QFile(directoryIterator.next());

        if (QFileInfo(file->fileName()).isFile())
        {
            // If its a file, and it is a txt
            if (QFileInfo(file->fileName()).suffix() == "txt")
            {
                qDebug() << "Hilillo: " << directoryIterator.filePath();
                // Adds to the list the files
                listFiles.push_back(file);
            }
        }
    }

    // Sorts the files
    std::sort(listFiles.begin(), listFiles.end(), VersionCompare);

    // Go through the list files
    while(!listFiles.empty())
    {
        QFile *fileName = listFiles.first();
        listFiles.pop_front();

        if (QFileInfo(fileName->fileName()).isFile())
            if (QFileInfo(fileName->fileName()).suffix() == "txt")
            {
                qDebug() << "Hilillo: " << fileName->fileName();
                if (!fileName->open(QIODevice::ReadOnly))
                {
                    qDebug() << "Couldnt open " << directoryIterator.filePath();
                    return;
                }

                QTextStream in(fileName);
                // Reads the file content
                while (!in.atEnd())
                {
                    QString line = in.readLine();
                    QStringList list = line.split(" ");

                    for ( const auto& iterator : list  )
                    {
                       // Appends to the corresponding hilillo
                       this->hilillos.at(fileID).push_back(iterator.toInt());
                    }
                }
                ++fileID;
            }


    }

}





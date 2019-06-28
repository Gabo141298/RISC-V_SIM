#include "simulationmanager.h"
#include <QDirIterator>
#include <QDebug>
#include "processor.h"
#include <QObject>
#include <QtAlgorithms>
#include <algorithm>

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
        Processor *processorThread = this->processors.at(index) = new Processor(index, this->quatum);
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
    size_t pc[3] = {0,0,0};
    int id = 0;

    // Processor currently
    size_t pos = 0;

    // Iterate though the hilillos
    for (/*iteratorBegin*/; iteratorBegin != iteratorEnd; ++iteratorBegin)
    {
        // Removing empy std::vector
        if (iteratorBegin->size() > 0)
        {
            Pcb* temp = new Pcb(counter[pos%3], id);
            this->processors.at(pos%3)->pushPcb(temp);

            qDebug() << "Init PCB" << "with hilillo" << id << "Beginning at mem pos" << counter[pos%3] << "to proccesor" << pos%3;

            for(size_t index = 0; index < iteratorBegin->size(); ++index)
            {
                //qDebug() << "Asignig to processor" << pos%3 << "at mem pos" << counter[pos%3] << "a" << iteratorBegin->at(index);
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
    pthread_barrier_t* barrier = new pthread_barrier_t();
    // Aca se puede hacer mas general, al igual que en varios lados, para que no sea un 3 s
    pthread_barrier_init(barrier,nullptr, unsigned( numOfProcessor) );
    for (size_t index = 0; index < this->processors.size(); ++index)
    {
       this->processors.at(index)->processors = this->processors;
       this->processors.at(index)->init_barrier(barrier);
       this->processors.at(index)->start();
    }
}

static bool VersionCompare(const QFile* i, const QFile* j)
{
    return i->fileName() < j->fileName(); //what ever you consider necessary... for one Version to be lessThan other
}

void SimulationManager::readHilillos()
{
    QList<QFile*> listFiles;
    QDirIterator directoryIterator(this->dir, QStringList());
    size_t fileID = 0;

    while (directoryIterator.hasNext())
    {
        QFile* file = new QFile(directoryIterator.next());

        if (QFileInfo(file->fileName()).isFile())
        {
            if (QFileInfo(file->fileName()).suffix() == "txt")
            {
                qDebug() << "Hilillo: " << directoryIterator.filePath();
                listFiles.push_back(file);
            }
        }
    }

    std::sort(listFiles.begin(), listFiles.end(), VersionCompare);

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

               // return;

                QTextStream in(fileName);
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





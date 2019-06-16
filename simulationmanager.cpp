#include "simulationmanager.h"
#include <QDirIterator>
#include <QDebug>

SimulationManager::SimulationManager()
{

}

SimulationManager::SimulationManager(const int quatum, const QString dir):
    quatum{quatum} ,
    dir{dir}
{
    this->hilillos.resize(10);
}

void SimulationManager::beginSimulation()
{
    readHilillos();

    for (size_t index =0; index < hilillos.size(); ++index)
    {
        if (!hilillos.at(index).empty())
            qDebug() << hilillos.at(index);
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



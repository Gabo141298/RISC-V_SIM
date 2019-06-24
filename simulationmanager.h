#ifndef SIMULATIONMANAGER_H
#define SIMULATIONMANAGER_H

#include <QString>
#include <vector>
#include <processor.h>

class SimulationManager
{
public:
    SimulationManager();
    SimulationManager(size_t quatum, const QString dir,const size_t numberOfProccesors);
    void beginSimulation();
private:
    size_t quatum;
    QString dir;
    size_t numOfProcessor;
    std::vector<Processor*> processors;
    // Change name to hilillo, es un vector que en cada entrada tiene un hilillos completo
    std::vector< std::vector <int> > hilillos;
    void readHilillos();
    void createProcessors();
    void distributeHilillos();
    void processorRun();
};

#endif // SIMULATIONMANAGER_H

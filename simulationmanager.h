#ifndef SIMULATIONMANAGER_H
#define SIMULATIONMANAGER_H

#include <QString>
#include <vector>

class SimulationManager
{
public:
    SimulationManager();
    SimulationManager(int quatum, const QString dir);
    void beginSimulation();
private:
    int quatum;
    QString dir;
    // Change name to hilillo, es un vector que en cada entrada tiene un hilillos completo
    std::vector< std::vector <int> > hilillos;
    void readHilillos();
};

#endif // SIMULATIONMANAGER_H

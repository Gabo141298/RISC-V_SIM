#ifndef SIMULATIONMANAGER_H
#define SIMULATIONMANAGER_H

#include <QString>
#include <vector>
#include <processor.h>
#include <QObject>
#undef STEP

class SimulationManager: public QObject
{
    Q_OBJECT
public:
    SimulationManager();
    SimulationManager(size_t quatum, const QString dir,const size_t numberOfProccesors);
    void beginSimulation();
signals:
   // void changeLeds(const int processor, const int hilillo);
public slots:
   // void contextSwitch(const int processor,const int hilillo);
private:
    size_t quatum;
    QString dir;
    size_t numOfProcessor;
    std::vector<Processor*> processors;
    pthread_barrier_t* barrier;
    // Change name to hilillo, es un vector que en cada entrada tiene un hilillos completo
    std::vector< std::vector <int> > hilillos;
    void readHilillos();
    void createProcessors();
    void distributeHilillos();
    void processorRun();

public slots:
    void incrementBarrier();
signals:
     void increment();

};


#endif // SIMULATIONMANAGER_H

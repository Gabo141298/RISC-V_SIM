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
    /// Default constructor
    SimulationManager();

    /**
     * @brief SimulationManager constructor
     * @param quatum the quatum of the simulation
     * @param dir the path of the hilillos
     * @param numberOfProccesors number of processor we wanna launch
     */
    SimulationManager(size_t quatum, const QString dir,const size_t numberOfProccesors);

    /// Begins the simulation, launch the 3 processor, and give them hilillos
    void beginSimulation();

signals:
    /// Tells the UI to update the leds
    void changeLeds(const int processor, const int hilillo);
    /// Tells the UI that the results are ready
    void sendResultsToUI(const QString processorsData, const QString hilillosData);
public slots:
    /// Recollects the results from the processors
    void gatherResults(const QString processorsData, const QString hilillosData);
    /// When a context switch occurs tell the main window to update the leds
    void contextSwitch(const int processor,const int hilillo);

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

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
    /// Quantum recollected in the UI
    size_t quatum;
    /// The path of the hilillos
    QString dir;
    /// Number of processors to be launch
    size_t numOfProcessor;
    /// An array of pointers to each processor
    std::vector<Processor*> processors;
    /// The barrier that keeps each processor clock, syncronous
    pthread_barrier_t* barrier;
    // Change name to hilillo, es un vector que en cada entrada tiene un hilillos completo
    std::vector< std::vector <int> > hilillos;
    /// Reads the hilillos from the path given from the UI
    void readHilillos();
    /// Creates the processors, give them memory, and make the respective connections
    void createProcessors();
    /// Round robbin algorithm, that balance the loads
    void distributeHilillos();
    /// Tell the processors to start
    void processorRun();

/// This are the signals and slots, that are used in the implementation on the step by step
/// button
public slots:
    void incrementBarrier();
signals:
     void increment();

};


#endif // SIMULATIONMANAGER_H

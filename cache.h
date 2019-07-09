#ifndef CACHE_H
#define CACHE_H

enum states
{
    shared,
    modified,
    invalid,
};

class Processor;

class Cache
{   
public:
    Cache();
    /// Arreglo asociado a los bloques en caché para saber qué bloque está en qué posición.
    int blockID[4] = {-1,-1,-1,-1};
    /// Arreglo asociado a los bloques en caché para saber el estado del bloque que se encuentra en cierta posición.
    states state[4] = {invalid, invalid, invalid, invalid};

    virtual ~Cache();
};

#endif // CACHE_H

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
    int blockID[4] = {0};
    states state[4] = {invalid};

    // Esto es porque me imagino que esto va a ser distinto
    virtual void solveFail(Processor* processor, const int &blockInMemory, const int &blockInCache) = 0;

    virtual ~Cache();
};

#endif // CACHE_H

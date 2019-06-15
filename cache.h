#ifndef CACHE_H
#define CACHE_H

enum states
{
    shared,
    modified,
    invalid,
};

class Cache
{   
public:
    Cache();
    int cacheMem[4][4] = {{0}};
    int blockID[4] = {0};
    states state[4] = {invalid};

    // Esto es porque me imagino que esto va a ser distinto
    virtual void solveFail(const int& pc) = 0;

    virtual ~Cache();
};

#endif // CACHE_H

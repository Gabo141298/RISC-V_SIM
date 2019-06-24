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
    int blockID[4] = {-1,-1,-1,-1};
    states state[4] = {invalid};

    virtual ~Cache();
};

#endif // CACHE_H

#ifndef CACHE_H
#define CACHE_H


class Cache
{   
public:
    Cache();
    int cacheMem[4][4] = {{0}};
    int blockID[4] = {0};
    int state[4] = {0};

    int getDataAt();
    // Esto es porque me imagino que esto va a ser distinto
    virtual void solveFail();

    virtual ~Cache();
};

#endif // CACHE_H

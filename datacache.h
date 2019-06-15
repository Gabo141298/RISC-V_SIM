#include "cache.h"

#ifndef DATACACHE_H
#define DATACACHE_H


class DataCache : public Cache
{
public:
    DataCache();
    int getDataAt();
    void solveFail(const int& pc) override;
};

#endif // DATACACHE_H

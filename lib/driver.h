#ifndef __DRIVER_H
#define __DRIVER_H

#include "types.h"

class Driver
{
public:
    Driver();
    ~Driver();

    virtual void Activate();
    virtual int Reset();
    virtual void Deactivate();
};

class DriverManager
{
private:
    Driver *drivers[255];
    int32_t numDrivers;

public:
    DriverManager();
    ~DriverManager();
    void AddDriver(Driver *);
    void ActivateAll();
};

#endif
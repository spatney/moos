#include "driver.h"

Driver::Driver()
{
}
Driver::~Driver()
{
}

void Driver::Activate() {}
int Driver::Reset()
{
    return 0;
}
void Driver::Deactivate()
{
}

DriverManager::DriverManager()
{
    numDrivers = 0;
}

DriverManager::~DriverManager()
{
}
void DriverManager::AddDriver(Driver *driver)
{
    drivers[numDrivers] = driver;
    numDrivers++;
}

void DriverManager::ActivateAll()
{
    for (int32_t i = 0; i < numDrivers; i++)
    {
        drivers[i]->Activate();
    }
}
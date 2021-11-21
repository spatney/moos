#ifndef __MOOS__DRIVERS__DRIVER_H
#define __MOOS__DRIVERS__DRIVER_H

#include <common/types.h>

namespace moos
{
    class OSTest;
    
    namespace drivers
    {
        class Driver
        {
        public:
            Driver();
            ~Driver();

            virtual void Activate();
            virtual common::int32_t Reset();
            virtual void Deactivate();
        };

        class DriverManager
        {
            friend OSTest; // for testing only, will remove

        private:
            Driver *drivers[255];
            common::int32_t numDrivers;

        public:
            DriverManager();
            ~DriverManager();
            void AddDriver(Driver *);
            void ActivateAll();
        };
    }
}

#endif
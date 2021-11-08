#ifndef __MOOS__DRIVERS__DRIVER_H
#define __MOOS__DRIVERS__DRIVER_H

#include <common/types.h>

namespace moos
{
    namespace drivers
    {
        class Driver
        {
        public:
            Driver();
            ~Driver();

            virtual void Activate();
            virtual moos::common::int32_t Reset();
            virtual void Deactivate();
        };

        class DriverManager
        {
        private:
            Driver *drivers[255];
            moos::common::int32_t numDrivers;

        public:
            DriverManager();
            ~DriverManager();
            void AddDriver(Driver *);
            void ActivateAll();
        };
    }
}

#endif
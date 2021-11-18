#ifndef __CORE__MEMORY_H
#define __CORE__MEMORY_H

#include <common/types.h>

namespace moos
{
    namespace core
    {
        struct MemoryBlock
        {
            MemoryBlock *next;
            MemoryBlock *prev;
            bool allocated;
            common::size_t size;
        };

        class MemoryManager
        {
        protected:
            MemoryBlock *head;

        public:
            static MemoryManager *activeMemoryManager;

            MemoryManager(
                common::size_t head,
                common::size_t size);

            ~MemoryManager();

            void *malloc(common::size_t size);
            void free(void *ptr);
            common::size_t GetFree();
        };
    }
}

void *operator new(unsigned size);
void *operator new[](unsigned size);

// placement new
void *operator new(unsigned size, void *ptr);
void *operator new[](unsigned size, void *ptr);

void operator delete(void *ptr, unsigned);
void operator delete[](void *ptr, unsigned);

#endif
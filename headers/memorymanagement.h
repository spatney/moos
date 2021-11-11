#ifndef __MEMORYMANAGEMENT_H
#define __MEMORYMANAGEMENT_H

#include <common/types.h>

namespace moos
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
    };
}

void* operator new(unsigned size);
void* operator new[](unsigned size);

// placement new
void* operator new(unsigned size, void* ptr);
void* operator new[](unsigned size, void* ptr);

void operator delete(void* ptr);
void operator delete[](void* ptr);

#endif
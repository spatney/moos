#include <core/memory.h>
#include <common/console.h>

using namespace moos::core;
using namespace moos::common;

MemoryManager *MemoryManager::activeMemoryManager = 0;

MemoryManager::MemoryManager(
    common::size_t head,
    common::size_t size)
{
    activeMemoryManager = this;

    if (size < sizeof(MemoryBlock))
    {
        this->head = 0;
    }
    else
    {
        this->head = (MemoryBlock *)head;
        this->head->allocated = false;
        this->head->next = 0;
        this->head->prev = 0;
        this->head->size = size - sizeof(MemoryBlock);
    }
}

MemoryManager::~MemoryManager()
{
    if (activeMemoryManager == this)
    {
        activeMemoryManager = 0;
    }
}

void *MemoryManager::malloc(common::size_t size)
{
    // Console::Write("Allocating %d bytes\n", size);
    MemoryBlock *emptyBlock = 0;
    for (
        auto block = head;
        block != 0 && emptyBlock == 0;
        block = block->next)
    {
        if (block->size >= size && !block->allocated)
        {
            emptyBlock = block;
        }
    }

    if (emptyBlock == 0)
    {
        Console::Write("Out of Memory!! \n");
        return 0;
    }

    if (emptyBlock->size < (size + sizeof(MemoryBlock) + 1))
    {
        emptyBlock->allocated = true;
    }
    else
    {
        auto newBlock = (MemoryBlock *)((size_t)emptyBlock + sizeof(MemoryBlock) + size);
        newBlock->allocated = false;
        newBlock->size = emptyBlock->size - sizeof(MemoryBlock) - size;
        newBlock->prev = emptyBlock;
        newBlock->next = emptyBlock->next;

        if (newBlock->next != 0)
        {
            newBlock->next->prev = newBlock;
        }

        emptyBlock->size = size;
        emptyBlock->next = newBlock;

        emptyBlock->allocated = true;
    }

    return (void *)(((size_t)emptyBlock) + sizeof(MemoryBlock));
}

size_t MemoryManager::GetFree()
{
    auto curr = this->head;
    size_t size = 0;

    while (curr != 0)
    {
        if (!curr->allocated)
        {
            size += curr->size;
        }

        curr = curr->next;
    }

    return size;
}

void MemoryManager::free(void *ptr)
{

    auto block = (MemoryBlock *)((size_t)ptr - sizeof(MemoryBlock));
    block->allocated = false;

    if (block->prev != 0 && !block->prev->allocated)
    {
        block->prev->next = block->next;

        block->prev->size += (block->size + sizeof(MemoryBlock));

        if (block->next != 0)
        {
            block->next->prev = block->prev;
        }

        block = block->prev;
    }

    if (block->next != 0 && !block->next->allocated)
    {

        block->size += (block->next->size + sizeof(MemoryBlock));
        block->next = block->next->next;

        if (block->next != 0)
        {
            block->next->prev = block;
        }
    }
}

void *operator new(unsigned size)
{
    if (moos::core::MemoryManager::activeMemoryManager == 0)
        return 0;
    return moos::core::MemoryManager::activeMemoryManager->malloc(size);
}

void *operator new[](unsigned size)
{
    if (moos::core::MemoryManager::activeMemoryManager == 0)
        return 0;
    return moos::core::MemoryManager::activeMemoryManager->malloc(size);
}

void *operator new(unsigned size, void *ptr)
{
    return ptr;
}

void *operator new[](unsigned size, void *ptr)
{
    return ptr;
}

void operator delete(void *ptr, unsigned)
{
    if (moos::core::MemoryManager::activeMemoryManager != 0)
        moos::core::MemoryManager::activeMemoryManager->free(ptr);
}

void operator delete[](void *ptr, unsigned)
{
    if (moos::core::MemoryManager::activeMemoryManager != 0)
        moos::core::MemoryManager::activeMemoryManager->free(ptr);
}
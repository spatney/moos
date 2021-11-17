#ifndef __MOOS__OSTEST_H
#define __MOOS__OSTEST_H

#define WAIT 50

#include <common/types.h>
#include <common/linkedlist.h>
#include <common/console.h>
#include <core/multitasking.h>
#include <core/memory.h>

namespace moos
{
    struct Data
    {
        common::int32_t d;
    };

    class Empty
    {
    };

    class OSTest
    {
    public:
        static void SleepDemo()
        {
            int count = 0;
            while (true)
            {
                common::Console::Write("BEEP %d\n", count++);
                common::Console::Sleep(1);
            }
        }

        static void HeapDemo()
        {
            auto e1 = new Empty();
            auto e2 = new Empty();
            auto l1 = new common::LinkedList();

            common::Console::Clear();

            common::Console::Write("Size of MemoryBlock: %d\n", sizeof(core::MemoryBlock));
            common::Console::Write("Size of Empty: %d\n", sizeof(Empty));
            common::Console::Write("Size of LinkedList: %d\n", sizeof(common::LinkedList));

            common::Console::Write("E1 CREATED: 0x%x\n", e1);
            common::Console::Write("E2 CREATED: 0x%x\n", e2);
            common::Console::Write("L1 CREATED: 0x%x\n", l1);

            common::Console::Write("DELETING E2 ...\n", l1);
            delete e2;

            auto l2 = new common::LinkedList();
            common::Console::Write("L2 CREATED: 0x%x\n", l2);

            auto e3 = new Empty();
            common::Console::Write("E3 CREATED: 0x%x\n", e3);
        }

        static void ListDemo()
        {
            auto *list = new common::LinkedList();

            common::Console::Write("Linked List Demo \n");

            list->AddFirst(createDataPointer(2));
            list->AddFirst(createDataPointer(1));
            list->AddLast(createDataPointer(3));
            list->AddLast(createDataPointer(4));
            list->AddFirst(createDataPointer(0));

            printlist(list);

            list->RemoveLast();
            list->RemoveFirst();
            list->RemoveLast();
            list->RemoveFirst();

            printlist(list);
        }

    private:
        static void printlist(common::LinkedList *list)
        {
            common::int32_t counter = 0;

            common::Console::Write("HEAD => ");

            for (auto it = list->begin(), end = list->end(); it != end; ++it)
            {
                auto data = *it;
                common::Console::Write("%d => ", ((Data *)data)->d);
            }

            common::Console::Write("TAIL\n");
        }

        static void *createDataPointer(common::int32_t n)
        {
            auto *data = new Data();
            data->d = n;
            return data;
        }
    };
}

#endif
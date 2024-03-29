#ifndef __MOOS__OSTEST_H
#define __MOOS__OSTEST_H

#define WAIT 50

#include <common/types.h>
#include <common/linkedlist.h>
#include <common/console.h>
#include <core/multitasking.h>
#include <core/memory.h>
#include <core/sharedptr.h>
#include <drivers/amd_am79c973.h>
#include <drivers/driver.h>
#include <hardware/pci.h>
#include <common/strings.h>
#include <drivers/ata.h>
#include <file_system/msdos_partition.h>

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
        static void RunTest(common::int32_t num)
        {
            switch (num)
            {
            case 1:
                common::Console::Write("Linked list demo\n\n");
                ListDemo();
                break;
            case 2:
                common::Console::Write("Virtual memory demo\n\n");
                HeapDemo();
                break;
            case 3:
                common::Console::Write("Shared pointer demo\n\n");
                SharedPtrDemo();
                break;
            case 4:
                common::Console::Write("Hard-disk demo\n\n");
                HardDiskTest();
                break;
            case 5:
                common::Console::Write("System call demo\n\n");
                SysCallTest();
                break;
            case 6:
                common::Console::Write("Halt kernal demo\n\n");
                HaltKernalTest();
                break;

            default:
                common::Console::Write("Invalid test number\n");
                break;
            }
        }

        static void MultiTaskingTest(
            core::TaskManager *taskManager,
            core::GlobalDescriptorTable *gdt)
        {
            auto t1 = new core::Task(gdt, []()
                                     {
                                         while (true)
                                         {
                                             OSTest::sysPrint("A ");
                                         } });
            auto t2 = new core::Task(gdt, []()
                                     {
                                         while (true)
                                         {
                                             OSTest::sysPrint("B ");
                                         } });
            taskManager->AddTask(t1);
            taskManager->AddTask(t2);
        }

        static void SysCallTest()
        {
            auto str = "Hey System! do something with this string?\n";
            asm("int $0x80"
                :
                : "a"(4), "b"(str));
        }

        static void HaltKernalTest()
        {
            asm("int $0x80"
                :
                : "a"(60), "b"(0));
        }

        static void HardDiskTest()
        {
            drivers::AdvancedTechnologyAttachment *selectedDisk = 0;

            drivers::AdvancedTechnologyAttachment ata0m(0x1F0, true);
            if (ata0m.Identify())
            {
                selectedDisk = &ata0m;
            }

            drivers::AdvancedTechnologyAttachment ata0s(0x1F0, false);
            if (ata0s.Identify())
            {
                selectedDisk = &ata0s;
            }

            if (selectedDisk == 0)
            {
                common::Console::Write("No useable disk found ... \n");
                return;
            }

            common::Console::Write("Reading a test file ... \n");

            auto contents = file_system::PartitionTable::ReadFileContents(selectedDisk, "FILE1");

            common::Console::Write("%s\n", contents);

            /*AdvancedTechnologyAttachment ata1m(0x170, true);
            Console::Write("ATA 1 Master: ");
            ata1m.Identify();
            AdvancedTechnologyAttachment ata1s(0x170, false);
            Console::Write("ATA 1 Alternate: ");
            ata1s.Identify();
            AdvancedTechnologyAttachment ata2m(0x1E8, true);
            Console::Write("ATA 2 Master: ");
            ata2m.Identify();
            AdvancedTechnologyAttachment ata2s(0x1E8, false);
            Console::Write("ATA 2 Alternate: ");
            ata2s.Identify();
            AdvancedTechnologyAttachment ata3m(0x168, true);
            Console::Write("ATA 3 Master: ");
            ata3m.Identify();
            AdvancedTechnologyAttachment ata3s(0x168, false);
            Console::Write("ATA 3 Alternate: ");
            ata3s.Identify();*/

            /*auto *data = (common::uint8_t *)"This text will be saved to the hard-disk!";
            auto *buffer = new common::uint8_t[common::StringUtil::strlen((common::int8_t *)data)];

            selectedDisk->Write28(0, data, common::StringUtil::strlen((common::int8_t *)data));
            selectedDisk->Flush();
            selectedDisk->Read28(0, buffer, common::StringUtil::strlen((common::int8_t *)data));

            delete buffer;
            delete data;*/

            delete selectedDisk;
        }
        static void NetworkCardDemo(
            drivers::DriverManager *manager)
        {
            auto networkCard = (drivers::amd_am79c973 *)manager->drivers[0];
            auto str = "Hello Network";
            common::Console::Write("Sending data ...\n");
            networkCard->Send((common::uint8_t *)str, common::StringUtil::strlen(str));
        }

        static void SharedPtrDemo()
        {
            {
                common::Console::Write("Free mem (before alloc): %d bytes\n", core::MemoryManager::activeMemoryManager->GetFree());
                auto sp = core::shared_ptr(new Empty());
                common::Console::Write("Free mem (after  alloc): %d bytes\n", core::MemoryManager::activeMemoryManager->GetFree());
            }

            common::Console::Write("going out of scope, should free the pointer ... \nFree mem: %d bytes\n",
                                   core::MemoryManager::activeMemoryManager->GetFree());
        }

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

            common::Console::Write("Size of MemoryBlock: %d\n", sizeof(core::MemoryBlock));
            common::Console::Write("Size of Empty: %d\n", sizeof(Empty));
            common::Console::Write("Size of LinkedList: %d\n", sizeof(common::LinkedList));

            common::Console::Write("E1 CREATED: 0x%x\n", e1);
            common::Console::Write("E2 CREATED: ");
            auto color = common::Console::SetColor(10);
            common::Console::Write("0x%x\n", e2);
            common::Console::SetColor(color);
            common::Console::Write("L1 CREATED: 0x%x\n", l1);

            common::Console::Write("DELETING E2 ...\n", l1);
            delete e2;

            auto l2 = new common::LinkedList();
            common::Console::Write("L2 CREATED: 0x%x\n", l2);

            auto e3 = new Empty();
            common::Console::Write("E3 CREATED: ");
            color = common::Console::SetColor(10);
            common::Console::Write("0x%x\n", e3);
            common::Console::SetColor(color);

            delete e1;
            delete l1;
            delete e3;
            delete l2;
        }

        static void ListDemo()
        {
            auto *list = new common::LinkedList();

            list->AddFirst(createDataPointer(2));
            list->AddFirst(createDataPointer(1));
            list->AddLast(createDataPointer(3));
            list->AddLast(createDataPointer(4));
            list->AddFirst(createDataPointer(0));

            printlist(list);
            common::Console::Write("Performing: RemoveLast, RemoveFirst, RemoveLast, RemoveFirst\n");

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

        static void sysPrint(const char *str)
        {
            asm("int $0x80"
                :
                : "a"(4), "b"(str));
        }
    };
}

#endif
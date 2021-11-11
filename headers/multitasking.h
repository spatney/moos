#ifndef __MULTITASKING_H
#define __MULTITASKING_H

#include <common/types.h>
#include <gdt.h>

namespace moos
{
    struct CPUState
    {
        common::uint32_t eax; // accumelator register
        common::uint32_t ebx; // base register
        common::uint32_t ecx; // counterregister
        common::uint32_t edx; // data register

        common::uint32_t esi;   // stack index
        common::uint32_t edi;   // data index
        common::uint32_t ebp;   // stack base pointer
                                
        /*common::uint32_t gs;
        common::uint32_t fs;
        common::uint32_t es;
        common::uint32_t ds; */
        common::uint32_t error; // error code

        common::uint32_t eip; // instruction pointer
        common::uint32_t cs;  // code segment
        common::uint32_t eflags;
        common::uint32_t esp; // stack pointer
        common::uint32_t ss;  // stack segment
    } __attribute__((packed));

    class Task
    {
        friend class TaskManager;

    private:
        common::uint8_t stack[4096]; // 4KB
        CPUState *cpuState;

    public:
        Task(
            GlobalDescriptorTable *gdt,
            void entryPoint());
        ~Task();
    };

    class TaskManager
    {
    private:
        Task *tasks[256];
        int numTasks;
        int activeTask;

    public:
        TaskManager();
        ~TaskManager();
        bool AddTask(Task *task);
        CPUState *Schedule(CPUState *cpuState);
    };
}

#endif
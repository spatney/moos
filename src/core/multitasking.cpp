#include <core/multitasking.h>

using namespace moos::core;
using namespace moos::common;

Task::Task(
    GlobalDescriptorTable *gdt,
    void entryPoint())
{
    cpuState = (CPUState *)(stack + 4096 - sizeof(CPUState));

    cpuState->eax = 0;
    cpuState->ebx = 0;
    cpuState->ecx = 0;
    cpuState->edx = 0;

    cpuState->esi = 0;
    cpuState->edi = 0;
    cpuState->ebp = 0;

    /*
    cpuState->gs = 0;
    cpuState->fs = 0;
    cpuState->es = 0;
    cpuState->ds = 0;
    */

    // cpuState->error = 0;

    //cpuState->esp = 0;
    cpuState->eip = (uint32_t)entryPoint;
    cpuState->cs = gdt->CodeSegmentSelector();
    // cpuState->ss = 0;
    cpuState->eflags = 0x202;
}
Task::~Task() {}

TaskManager::TaskManager()
{
    numTasks = 0;
    activeTask = -1;
}

TaskManager::~TaskManager()
{
}

bool TaskManager::AddTask(Task *task)
{
    if (numTasks >= 256)
    {
        return false;
    }

    tasks[numTasks++] = task;
    return true;
}

CPUState *TaskManager::Schedule(CPUState *cpuState)
{
    if (numTasks <= 0)
    {
        return cpuState;
    }

    if (activeTask >= 0)
    {
        tasks[activeTask]->cpuState = cpuState;
    }

    if (++activeTask >= numTasks)
    {
        activeTask %= numTasks;
    }

    return tasks[activeTask]->cpuState;
}

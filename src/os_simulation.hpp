#pragma once

#ifndef OS_SIMULATION_HPP
#define OS_SIMULATION_HPP

#include <queue>
#include <vector>
#include <cstdint>
#include <csignal>
#include <random>

struct SystemParameters {
    int_fast32_t maxProcGenPerCycle;

    uint64_t processGenerationCycles; 
    uint64_t memoryAccessingTries;

    uint64_t memorySize;
    uint64_t waitCyclesBeforeStart;

    uint64_t minProcMemory;
    uint64_t rangeProcMemory;

    double minProbExit;
    double rangeProbExit;

    double minProbAccessMem;
    double rangeProbAccessMem;

    double probProcessCreation;
};


class OsSimulation {

    struct ProcessImage {
        ProcessImage() = default;
        ProcessImage(uint_fast32_t id, uint64_t _memsize, double _probExit, double _probAccessMem);

        uint_fast32_t id;
        uint64_t virtMemsize;
        double probExit;      // Probability of finishing 
        double probAccessMem; // Probabiliry of accessing memory
    };

    struct MemoryPart {
        MemoryPart() = default;
        MemoryPart(uint64_t size, uint64_t offset);

        bool free = true;
        uint64_t size;
        uint64_t offset;
        ProcessImage process;
    };

private:

    bool exitInterruptFlag;
    

    uint_fast32_t procPerCycleCounter = 0;
    uint_fast32_t partsLoadedCount = 0;
    uint_fast32_t processCounter = 0;

    uint64_t cyclesCounter = 0;
    uint64_t procGenCyclesCounter;
    uint64_t waitCyclesGenCounter;

    const SystemParameters& m_refParameters;

    std::queue<ProcessImage> processes;
    std::vector<MemoryPart> memoryParts;

    std::mt19937 m_rand_gen;

    uint64_t maxProcMemory;
    double maxProbExit;
    double maxProcAccessMem;

public:

    // `parameteres` reference must live during `OsSimulation` object exists
    OsSimulation(const SystemParameters& ref_parameters);

    OsSimulation() = delete;
    OsSimulation(const OsSimulation&) = delete;
    OsSimulation operator=(const OsSimulation&) = delete;

    void exec();
    void switchOff();

private:

    bool switchOffEmited() const;
    void chancelSwitchOff();
    bool isAllProcessesFinished() const;
    bool hasTask();
    bool interruptMenu();
    ProcessImage newTask();
    void addTaskToQueue(const ProcessImage& proc);
    bool isExecAllowed();
    decltype (memoryParts.begin()) getMinMemoryPart(uint64_t size);
    bool executeProcess(MemoryPart& mem);

    double _double_random(double from, double range);
    uint64_t _uint64_random(uint64_t from, uint64_t range);

    void logMemoryParts() const;
};

#endif // OS_SIMULATION_HPP
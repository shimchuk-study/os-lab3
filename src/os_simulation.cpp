#include "os_simulation.hpp"

#include <ctime>
#include <iostream>
#include <iomanip>

OsSimulation::ProcessImage::
ProcessImage(uint_fast32_t _id, uint64_t _memsize, double _probExit, double _probAccessMem)
    : id(_id),
      virtMemsize(_memsize), 
      probExit(_probExit), 
      probAccessMem(_probAccessMem)
{}

OsSimulation::MemoryPart::
MemoryPart(uint64_t _size, uint64_t _offset)
    : size(_size), offset(_offset)
{}

static bool *pbInterrupt = nullptr;

static void sig_int_handler(int sig)
{
    (void)sig;
    if (pbInterrupt)
        *pbInterrupt = true;
}

OsSimulation::OsSimulation(const SystemParameters& ref_parameters)
    : waitCyclesGenCounter(ref_parameters.waitCyclesBeforeStart),
      procGenCyclesCounter(ref_parameters.processGenerationCycles),
      m_refParameters(ref_parameters), m_rand_gen(std::random_device()()),
      maxProcMemory(m_refParameters.minProcMemory + m_refParameters.rangeProcMemory),
      maxProbExit(m_refParameters.minProbExit + m_refParameters.rangeProbExit),
      maxProcAccessMem(m_refParameters.minProbAccessMem + m_refParameters.rangeProbAccessMem)
{
    exitInterruptFlag = false;
    pbInterrupt = &exitInterruptFlag;
    std::signal(SIGINT, sig_int_handler);

    memoryParts.reserve(m_refParameters.memorySize / m_refParameters.minProcMemory);

    // Split memory to pathes
    uint64_t memorySize = m_refParameters.memorySize;
    uint64_t adress = 0;
    // First chunk must have maximal size
    auto currAdress = adress;
    
    memorySize -= maxProcMemory;
    adress += maxProcMemory;
    if (memorySize < ref_parameters.minProcMemory) {
        adress += memorySize;
        memorySize = 0;
    } 

    memoryParts.emplace_back(adress - currAdress, currAdress);
    
    while (memorySize) {
        currAdress = adress;

        auto nextChunkSize = _uint64_random(
            ref_parameters.minProcMemory,
            maxProcMemory
        );

        if (nextChunkSize > memorySize) {
            nextChunkSize = memorySize;
        }

        memorySize -= nextChunkSize;
        adress += nextChunkSize;
        
        if (memorySize < ref_parameters.minProcMemory) {
            adress += memorySize;
            memorySize = 0;
        }

        memoryParts.emplace_back(adress - currAdress, currAdress);
    }

    logMemoryParts();
}


void OsSimulation::exec()
{
    while (!isAllProcessesFinished()) {
        if (switchOffEmited()) {
            if (interruptMenu())
                break;
            else
                chancelSwitchOff();
        }

        std::cout << "Cycle ---------------------- " << std::dec 
                  << cyclesCounter << std::endl;

        while (hasTask()) 
            addTaskToQueue(newTask());

        if (isExecAllowed()) {
            while (!processes.empty()) {
                auto part = getMinMemoryPart(processes.front().virtMemsize);
                if (part == memoryParts.end()) 
                    break;
                 
                part->free = false;
                part->process = processes.front();
                processes.pop();
                partsLoadedCount++;         

                std::cout << "[Loaded] process id " << std::dec << part->process.id
                          << " to part with address " << std::hex 
                          << part->offset << std::endl;
            }
            
            for (auto &x : memoryParts)
                if (!x.free) {
                    if (executeProcess(x)) {
                        x.free = true;
                        --partsLoadedCount;
                    }
                }
        }

        cyclesCounter++;
    }
}

bool OsSimulation::switchOffEmited() const
{
    return exitInterruptFlag;
}

void OsSimulation::chancelSwitchOff()
{
    exitInterruptFlag = false;
}

bool OsSimulation::isAllProcessesFinished() const
{
    return (
        !procGenCyclesCounter 
        && !waitCyclesGenCounter
        && !partsLoadedCount
        && processes.empty()
    );
}

bool OsSimulation::interruptMenu()
{
    static std::string line(22, '-');

    std::cout << line << std::endl 
              << "Process interrupted\n"
              << line << std::endl;
    return false; // Exit
}

bool OsSimulation::hasTask()
{
    bool has = false;

    if (!procGenCyclesCounter)
        return false;

    while (!has) {
        if (procPerCycleCounter++ == m_refParameters.maxProcGenPerCycle) {
            procPerCycleCounter = 0;
            procGenCyclesCounter--;
            break;
        }

        double prob = _double_random(0.0, 1.0);
        if (prob < m_refParameters.probProcessCreation)
            has = true;
    }

    return has;
}

OsSimulation::ProcessImage OsSimulation::newTask()
{
    uint64_t ms = _uint64_random(m_refParameters.minProcMemory, maxProcMemory);
    double prob_exit = _double_random(m_refParameters.minProbExit, maxProbExit);
    double prob_ma = _double_random(m_refParameters.minProbAccessMem, maxProcAccessMem);

    ProcessImage proc(processCounter++, ms, prob_exit, prob_ma);

    std::cout << "[New task created (id: " << std::dec << proc.id
              << ")]: " << std::hex << "ms: " << proc.virtMemsize
              << ", exit pb.: " << std::setprecision(4) << proc.probExit
              << ", mem. access pb.:" << std::setprecision(4) << proc.probAccessMem
              << std::endl;

    return proc;
}

void OsSimulation::addTaskToQueue(const ProcessImage& proc)
{
    processes.push(proc);
}

bool OsSimulation::isExecAllowed() 
{
    if (waitCyclesGenCounter) {
        --waitCyclesGenCounter;
        return false;
    }

    return true;
}

decltype (OsSimulation::memoryParts.begin())
OsSimulation::getMinMemoryPart(uint64_t size)
{
    auto minPart = memoryParts.begin();
    
    for (auto it = minPart; it != memoryParts.end(); ++it) {
        if (minPart->free) {
            if (
                it->free 
                && (it->size < minPart->size)
                && (it->size >= size)
            )
                minPart = it;
        }
        else
            minPart = it;
    }

    if (minPart != memoryParts.end()) {
        if (!minPart->free || minPart->size < size)
            return memoryParts.end();
    }
    
    return minPart;
}

bool OsSimulation::executeProcess(OsSimulation::MemoryPart& m)
{
    for (int i = 0; i < 5; ++i)
        if (_double_random(0.0, 1.0) < m.process.probAccessMem) {
            // Memory accessed
            auto virtAddress = _uint64_random(0x00, m.process.virtMemsize - 1);
            // Address transformation
            std::cout << "[Accessing memory 0x" << std::hex << m.offset
                      << " part, " << std::dec << m.process.id 
                      << " proc id] " 
                      << "0x" << std::hex << m.offset << " + 0x"
                      << std::hex << virtAddress << "(virtual adress) = 0x"
                      << std::hex << (virtAddress + m.offset) 
                      << "(physical address)" << std::endl;
        }
    
    if (_double_random(0.0, 1.0) < m.process.probExit) {
        std::cout << "[Finished] Proc id: " << std::dec << m.process.id
                  << ", part offset: " << std::hex << m.offset << std::endl;
        return true;
    }

    return false;
}


double OsSimulation::_double_random(double from, double to)
{
    std::uniform_real_distribution<> dist(from, to);
    return dist(m_rand_gen);
}

uint64_t OsSimulation::_uint64_random(uint64_t from, uint64_t to)
{
    std::uniform_int_distribution<> dist(from, to);
    
    return dist(m_rand_gen);

}

void OsSimulation::logMemoryParts() const
{
    using namespace std;

    cout << "Memory Parts" << endl;
    cout << "|"
         << setw(10) << "  Offset" << "|" 
         << setw(10) << "Size" << "|"  
         << setw(10) << "Free" << "|" << endl
         << "+" << setw(10) << setfill('-') << ""
         << "+" << setw(10) << ""
         << "+" << setw(10) << "" << "+" << endl;
    for (auto &x : memoryParts) {
        cout << "|" << "0x" << setfill('0') << setw(8) << hex << x.offset << "|"
             << setw(10) << setfill(' ') << hex << x.size << "|"
             << setw(10) << boolalpha << x.free << "|" << endl;
    }
    cout << endl;
    cout.clear();
}

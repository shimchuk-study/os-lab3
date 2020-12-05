#include <iostream>
#include <boost/program_options.hpp>

#include "os_simulation.hpp"

using namespace std;

namespace po = boost::program_options;


int main(int argc, char *argv[])
{
    ios::sync_with_stdio(false);
    
    SystemParameters sp;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Show help");

    po::options_description desc_required("Required options");
    desc_required.add_options()
        (
            "mem_size,M", 
            po::value(&sp.memorySize), 
            "Size of DRAM"
        )
        (
            "proc_gen_cycles,g", 
            po::value(&sp.processGenerationCycles), 
            "Process generation cycles"
        )
        (
            "wait,w",
            po::value(&sp.waitCyclesBeforeStart),
            "Wait cycles before starting processes"
        )
        (
            "min_proc_mem", 
            po::value(&sp.minProcMemory),
            "Min process memory"
        )
        (
            "range_proc_memory",
            po::value(&sp.rangeProcMemory),
            "Range process memory"
        )
        (
            "proc_gen_tries",
            po::value(&sp.maxProcGenPerCycle),
            "Try generate memory `arg` times per cycle"
        )
        (
            "mem_access_tries",
            po::value(&sp.memoryAccessingTries),
            "Try access memory `arg` times per cycle"
        )
        (
            "prob_proc_creation",
            po::value(&sp.probProcessCreation),
            "Min probability of creation process"
        )
        (
            "min_prob_exit",
            po::value(&sp.minProbExit),
            "Min probability of exiting process"
        )
        (
            "range_prob_exit",
            po::value(&sp.rangeProbExit),
            "Range of probability of exiting process"
        )
        (
            "min_prob_memaccess",
            po::value(&sp.minProbAccessMem),
            "Min probability of exiting process"
        )
        (
            "range_prob_memaccess",
            po::value(&sp.rangeProbAccessMem),
            "Range of probability of exiting process"
        )
    ;
    
    desc.add(desc_required);
    
    po::variables_map vm;
    
    try {
        po::command_line_parser parser(argc, argv);
        po::parsed_options parsedOptions = parser.options(desc).run();
        po::store(parsedOptions, vm);
        po::notify(vm);

        if (vm.count("help")) {
            cerr << desc << endl;
        }
        else {
            auto vec = parsedOptions.options;
            for (auto &x : vec) 
                if (x.value.front()[0] == '-') {
                    cerr << "Arguments can't be negative" << endl;
                    return EXIT_FAILURE;
                }
                
            auto options = desc_required.options();
            for (auto &op : options) {
                auto optName = op->canonical_display_name();
                if (!vm.count(optName)) {
                    cerr << "Fatal error: \"" << optName 
                         << "\" required option missed!" << endl;
                    return EXIT_FAILURE;
                }
            }

            // After prevision `for` all required options exists
            // and all values are assigned
            if (sp.memorySize < (sp.minProcMemory + sp.rangeProcMemory)) {
                cerr << "There is not enough memory for the largest process" << endl;
                return EXIT_FAILURE;
            }
        }
    }
    catch (exception& e) {
        cerr << e.what() << endl;
        cerr << "Invalid usage\n\n" << desc << endl;
    }
 
    OsSimulation sim(sp);
    sim.exec();

    return 0;
}
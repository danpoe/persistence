/*
 * Persist.cpp
 *
 *  Reused from Trencher and developed by tuan-phong.ngo@it.uu.se for PERSIST tool
 *
 */

/*
 * ----------------------------------------------------------------------------
 * "THE JUICE-WARE LICENSE" (Revision 42):
 * <derevenetc@cs.uni-kl.de> wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a glass of juice in return.
 * ----------------------------------------------------------------------------
 */

#include <cassert>
#include <ctime>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <iomanip>

#include <boost/chrono.hpp>

#include "Benchmarking.h"
#include "DotPrinter.h"
#include "Foreach.h"
#include "FenceInsertion.h"
#include "NaiveParser.h"
#include "Program.h"
#include "Reduction.h"
#include "RobustnessChecking.h"
#include "State.h"

void help() {
	std::cout
			<< "************************************************************************"
			<< std::endl
			<< "** Command format: ./PERSIST [-b] [-p|-f|-fa n|-dot|-pdot] file       **"
			<< std::endl
			<< "**====================================================================**"
			<< std::endl
			<< "**                                                             +++++  **"
			<< std::endl
			<< "** Options:                                                    +   +  **"
			<< std::endl
			<< "**  -b     With benchmarking mode                              +++++  **"
			<< std::endl
			<< "**  -p     Persistence checking without fence insertion        +      **"
			<< std::endl
			<< "**  -f     Precise fence insertion                             +      **"
			<< std::endl
			<< "**  -fa e n Fence insertion with Thread abstraction:                  **"
			<< std::endl
			<< "**          e is 1 (example 1) or 2 (example 2), n refinements        **"
			<< std::endl
			<< "**  -dot   Print input program in dot format                          **"
			<< std::endl
			<< "**  -pdot  Print instrumented program in dot format                   **"
			<< std::endl
			<< "**  file   Input file                                                 **"
			<< std::endl
			<< "************************************************************************"
			<< std::endl;

}

int main(int argc, char **argv) {
	if (argc <= 1) {
		help();
		return 1;
	}

	try {
		enum {
			PRSISTENCE,
			FENCES,
			PRINT_DOT,
			PRINT_PRSISTENCE_DOT,
			FENCE_ABSTRACTION
		} action = PRSISTENCE;

		bool benchmarking = false;
		int loopNum = 0;
		int fenceType = -1;

		for (int i = 1; i < argc; ++i) {
			std::string arg = argv[i];

			if (arg == "-p") {
				action = PRSISTENCE;
			} else if (arg == "-f") {
				action = FENCES;
			} else if (arg == "-fa") {
				action = FENCE_ABSTRACTION;
				i++;
				char *fenceTypeStr = argv[i];
				fenceType = atoi(fenceTypeStr);
				i++;
				char *loopStr = argv[i];
				loopNum = atoi(loopStr);
			} else if (arg == "-b") {
				benchmarking = true;
			} else if (arg == "-nb") {
				benchmarking = false;
			} else if (arg == "-dot") {
				action = PRINT_DOT;
			} else if (arg == "-pdot") {
				action = PRINT_PRSISTENCE_DOT;
			} else if (arg.size() >= 1 && arg[0] == '-') {
				throw std::runtime_error("unknown option: " + arg);
			} else {
				wmm::Program program;

				wmm::Statistics::instance().reset();
				{
					wmm::NaiveParser parser;
					std::ifstream in(argv[i]);
					if (!in) {
						throw std::runtime_error("can't open file: " + arg);
					}
					parser.parse(in, program);

					wmm::Statistics::instance().incThreadsCount(
							program.threads().size());
					foreach (wmm::Thread *thread, program.threads()){
					wmm::Statistics::instance().incStatesCount(
							thread->states().size());
					wmm::Statistics::instance().incTransitionsCount(
							thread->transitions().size());
				}
			}

				auto startTime = boost::chrono::system_clock::now();

				switch (action) {
				case PRSISTENCE: {
					bool feasible = wmm::isAttackFeasible(program, false);
					if (feasible) {
						std::cout << std::left << std::setfill('+')
								<< std::setw(47) << "+" << std::endl;
						std::cout << std::setfill(' ') << std::setw(46)
								<< "+ Program IS NOT persist" << "+"
								<< std::endl;
						std::cout << std::left << std::setfill('+')
								<< std::setw(47) << "+" << std::endl;
					} else {
						std::cout << std::left << std::setfill('+')
								<< std::setw(45) << "+" << std::endl;
						std::cout << std::setfill(' ') << std::setw(46)
								<< "+ Program IS persist" << "+" << std::endl;
						std::cout << std::left << std::setfill('+')
								<< std::setw(47) << "+" << std::endl;
					}
					break;
				}
				case FENCES: {
					auto fences = wmm::preComputeFences(program, false);
					std::cout << std::left << std::setfill('+') << std::setw(47)
							<< "+" << std::endl;
					std::cout << std::setfill(' ') << std::setw(33)
							<< "+ The size of optimal fence set" << " : "
							<< std::setw(10) << fences.size() << "+";
					foreach (const auto &fence, fences){
					std::cout << std::endl << "+   " << std::setw(30) << fence.first->name() << ": "
					<< std::setw(10) << fence.second->name() << '+';
				}
					std::cout << std::endl;
					std::cout << std::left << std::setfill('+') << std::setw(47)
							<< "+" << std::endl;
					break;
				}
				case FENCE_ABSTRACTION: {
					auto fences = wmm::preComputeFenceAbstraction(program,
							false, loopNum, fenceType);
					std::cout << std::left << std::setfill('+') << std::setw(47)
							<< "+" << std::endl;
					std::cout << std::setfill(' ') << std::setw(33)
							<< "+ The size of optimal fence set" << " : "
							<< std::setw(10) << fences.size() << "+";
					foreach (const auto &fence, fences){
					std::cout << std::endl << "+   " << std::setw(30) << fence.first->name() << ": "
					<< std::setw(10) << fence.second->name() << '+';
				}
					std::cout << std::endl;
					std::cout << std::left << std::setfill('+') << std::setw(47)
							<< "+" << std::endl;
					break;
				}
				case PRINT_DOT: {
					wmm::DotPrinter printer;
					printer.print(std::cout, program);
					break;
				}
				case PRINT_PRSISTENCE_DOT: {
					wmm::Program instrumentedProgram;
					wmm::reduce(program, instrumentedProgram, false);
					wmm::DotPrinter printer;
					printer.print(std::cout, instrumentedProgram);
					break;
				}
				default: {
					assert(!"NEVER REACHED");
				}
				}

				auto endTime = boost::chrono::system_clock::now();

				wmm::Statistics::instance().addRealTime(
						boost::chrono::duration_cast<boost::chrono::milliseconds>(
								endTime - startTime).count());

				if (benchmarking) {
					std::cout << wmm::Statistics::instance() << std::endl;
					std::cout << std::left << std::setfill('+') << std::setw(47)
							<< "+" << std::endl;
				}
			}
		}
	} catch (const std::exception &exception) {
		std::cerr << "PERSIST: " << exception.what() << std::endl;
		return 1;
	}

	return 0;
}

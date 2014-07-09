/*
 * Benchmarking.cpp
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

#include "Benchmarking.h"
#include <iomanip>

namespace wmm {

std::ostream &operator<<(std::ostream &out, const Statistics &statistics) {
	return out
		<< std::setfill(' ') << std::left << std::setw(33) <<  "+ Number of processes" << " : " << std::setw(10) << statistics.threadsCount() << "+" << "\n"
		<< std::setw(33) << "+ Number of states" << " : " << std::setw(10) << statistics.statesCount() << "+" << "\n"
		<< std::setw(33) << "+ Number of transitions" << " : " << std::setw(10) << statistics.transitionsCount() << "+" << "\n"
		<< std::setw(33) << "+ Number of fences" << " : " << std::setw(10) << statistics.fencesCount() << "+" << "\n"
		<< std::setw(33) << "+ Code2Code translation time (ms)" << " : " << std::setw(10) << statistics.spinTime() << "+" << "\n"
		<< std::setw(33) << "+ Compilation time (ms)" << " : " << std::setw(10) << statistics.compilerTime() << "+" << "\n"
		<< std::setw(33) << "+ Verification time (ms)" << " : " << std::setw(10) << statistics.verifierTime() << "+" << "\n"
		<< std::setw(33) << "+ Sequential running time (ms)" << " : " << std::setw(10) << statistics.seqRealTime() << "+" << "\n"
		<< std::setw(33) << "+ Running time (ms): " << " : " << std::setw(10) << statistics.realTime() << "+"
	;
}

}

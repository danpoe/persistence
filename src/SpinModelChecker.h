/*
 * SpinModelChecker.h
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

#pragma once

#include <string>
#include <map>
#include <vector>

#include "Thread.h"

namespace wmm {

class Program;



class SpinModelChecker {
	std::string spinCommandLine_;
	std::string compilerCommandLine_;
	std::string verifierCommandLine_;
	std::string simulationCommandLine_;

public:

	SpinModelChecker(int size, int type);

	//SpinModelChecker();



	void setSpinCommandLine(const std::string &commandLine) {
		spinCommandLine_ = commandLine;
	}
	const std::string &spinCommandLine() const {
		return spinCommandLine_;
	}

	void setCompilerCommandLine(const std::string &commandLine) {
		compilerCommandLine_ = commandLine;
	}
	const std::string &compilerCommandLine() const {
		return compilerCommandLine_;
	}

	void setVerifierCommandLine(const std::string &commandLine) {
		verifierCommandLine_ = commandLine;
	}
	const std::string &verifierCommandLine() const {
		return verifierCommandLine_;
	}

	void setSimulationCommandLine(const std::string &commandLine) {
		simulationCommandLine_ = commandLine;
	}
	const std::string &simulationCommandLine() const {
		return simulationCommandLine_;
	}

	bool check(const Program &program);

	std::vector<int> check(const Program &program, Thread *thread) const;

	std::vector<int> check2(const Program &program, Thread *thread) const;

//	const std::map<std::string, std::vector<int>> checkAbstract(
//			const Program &program) const;

	const std::map<std::string, std::vector<int>> checkAbstract2(
				const Program &program, Thread *thread) const;

	//bool checkAbstract1(const Program &program);

};

}

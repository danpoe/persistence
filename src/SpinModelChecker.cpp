/*
 * SpinModelChecker.cpp
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

#include "SpinModelChecker.h"

#include <cstdlib> /* system */
#include <cstring> /* strerror */
#include <fstream>
#include <iostream>

#include <boost/regex.hpp>
#include <boost/chrono.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/unordered_set.hpp>
#include <boost/algorithm/string.hpp>

#include "Benchmarking.h"
#include "SpinPrinter.h"
#include "SpinPrinter2.h"
#include "Foreach.h"
#include "DotPrinter.h"
//#include "Thread.h"

namespace wmm {

namespace {

boost::filesystem::path makeTempDir() {
	boost::filesystem::path path = boost::filesystem::temp_directory_path()
			/ boost::filesystem::unique_path();
	if (!boost::filesystem::create_directory(path)) {
		throw std::runtime_error(
				"could not create temporary directory: " + path.string());
	}
	return path.string();
}

long run(const std::string &commandLine) {
	auto startTime = boost::chrono::system_clock::now();
	int status = system(commandLine.c_str());
	auto endTime = boost::chrono::system_clock::now();

	if (status == -1) {
		throw std::runtime_error(
				(boost::format("could not run '%s': %s") % commandLine
						% strerror(errno)).str());
	}
	//we need commentting this because spin in parsing trace generates non-zero code
	//else if (status != 0) {
	//	throw std::runtime_error(
	//			(boost::format("'%s' finished with non-zero exit code: %d")
	//					% commandLine % status).str());
	//}

	return boost::chrono::duration_cast<boost::chrono::milliseconds>(
			endTime - startTime).count();
}

} // anonymous namespace

//SpinModelChecker::SpinModelChecker() {
//	std::string spin = "spin";
//	if (boost::filesystem::exists("spin.exe")) {
//		spin = boost::filesystem::absolute("spin.exe").string();
//	}
//
//	setSpinCommandLine("cd \"%1%\" && \"" + spin + "\" -a \"%2%\"");
//	setCompilerCommandLine(
//			//"cc -O2 -DNOBOUNDCHECK -DNOFAIR -DSAFETY -DVECTORSZ=4444 -DBITSTATE  -pipe -o \"%2%\" \"%3%\" 2> \"%3%.cc.stderr\"");
//			"cc  -DNOBOUNDCHECK -DNOFAIR -DSAFETY  -DVECTORSZ=4444  -DBITSTATE  -pipe -o \"%2%\" \"%3%\" 2> \"%3%.cc.stderr\"");
//	setVerifierCommandLine(
//			"cd \"%1%\" && \"%2%\" -m10000  > \"%2%.stdout\" 2> \"%2%.stderr\"");
//	setSimulationCommandLine(
//			"cd \"%1%\" && \"" + spin
//					+ "\" -B -p -t1 -k \"%2%\" \"%3%\" > \"%4%\"");
//}

SpinModelChecker::SpinModelChecker(int size, int type) {
	std::string spin = "spin";
	if (boost::filesystem::exists("spin.exe")) {
		spin = boost::filesystem::absolute("spin.exe").string();
	}

	setSpinCommandLine("cd \"%1%\" && \"" + spin + "\" -a \"%2%\"");

	if (type == 0) {
		setCompilerCommandLine(
				"cc  -DNOBOUNDCHECK -DNOFAIR -DSAFETY  -DVECTORSZ=4444  -DBITSTATE  -pipe -o \"%2%\" \"%3%\" 2> \"%3%.cc.stderr\"");
	} else {
		// To optimize performance, we choose different configurations
		if (size <= 4)
			setCompilerCommandLine(
					"cc  -DNOBOUNDCHECK -DNOFAIR -DSAFETY  -DVECTORSZ=4444  -DBITSTATE  -pipe -o \"%2%\" \"%3%\" 2> \"%3%.cc.stderr\"");
		else
			setCompilerCommandLine(
					"cc -O2 -DNOBOUNDCHECK -DNOFAIR -DSAFETY -DVECTORSZ=4444 -DBITSTATE  -pipe -o \"%2%\" \"%3%\" 2> \"%3%.cc.stderr\"");
	}

	if (size <= 3) {
		setVerifierCommandLine(
				"cd \"%1%\" && \"%2%\" -m1000  > \"%2%.stdout\" 2> \"%2%.stderr\"");
	} else
		setVerifierCommandLine(
				"cd \"%1%\" && \"%2%\" -m10000  > \"%2%.stdout\" 2> \"%2%.stderr\"");

	setSimulationCommandLine(
			"cd \"%1%\" && \"" + spin
					+ "\" -B -p -t1 -k \"%2%\" \"%3%\" > \"%4%\"");

}

bool SpinModelChecker::check(const Program &program) {

	boost::filesystem::path temp_dir = makeTempDir();
	boost::filesystem::path program_pml = temp_dir / "program.pml";
	boost::filesystem::path verifier_c = temp_dir / "pan.c";
	boost::filesystem::path verifier = temp_dir / "pan";
	boost::filesystem::path trail = temp_dir / "program.pml.trail";
	boost::filesystem::path graph = temp_dir / "graph.gv";

	//need to be removed before publishing
	//std::ofstream out1(graph.string().c_str());
	//wmm::DotPrinter printer_xx;
	//printer_xx.print(out1, program);

	std::ofstream out(program_pml.string().c_str());
	SpinPrinter printer;

	printer.print(out, program);
	if (!out) {
		throw std::runtime_error(
				"could not create file: " + program_pml.string());
	}
	out.close();

	long spinTime = run(
			(boost::format(spinCommandLine()) % temp_dir % program_pml).str());
	long compilerTime = run(
			(boost::format(compilerCommandLine()) % temp_dir % verifier
					% verifier_c).str());
	long verifierTime = run(
			(boost::format(verifierCommandLine()) % temp_dir % verifier).str());

	Statistics::instance().addSpinTime(spinTime);
	Statistics::instance().addCompilerTime(compilerTime);
	Statistics::instance().addVerifierTime(verifierTime);
	Statistics::instance().addSeqRealTime(
			spinTime + compilerTime + verifierTime);

	bool result = boost::filesystem::exists(trail);

	//for debuging

	//boost::filesystem::path counter_example = temp_dir
	//			/ "counter-example.stdout";

	//if (result) {
	//		run(
	//				(boost::format(simulationCommandLine()) % temp_dir % trail
	//						% program_pml % counter_example).str());
	//	}

	/////////////////////////////////////////////

	boost::filesystem::remove_all(temp_dir);

	return result;
}

// Using for -fa 2 x (x>0)
std::map<std::string, std::vector<int>> parseSimulationFile12(
		boost::filesystem::path counter_example,
		boost::filesystem::path program_pml, Thread *attacker) {

	std::map<std::string, std::vector<int>> violationPaths;

	std::vector<int> lineList;
	std::ifstream traceFile(counter_example.c_str());
	std::ifstream programFile(program_pml.c_str());
	std::string token, line;
	int lineBefore = -1, lineCur;
	boost::regex txt_regex("[0-9]+:");
	std::vector<std::string> progLines;

	traceFile >> std::skipws;

	//parse file to get a sequence of lines in source code (before [mem4[2]...)
	//for each lines: know thread, id of transition
	//put them in a map and return

	while (traceFile >> token) {
		//this is an edge in the violate execution
		if (boost::regex_match(token, txt_regex)) {
			traceFile >> token;  // proc
			traceFile >> token;  // proc ID
			traceFile >> token;  // prototype

			if (token == "terminates") {
			} else {
				traceFile >> token;  //file:line
				std::vector<std::string> strs;
				boost::split(strs, token, boost::is_any_of(":"));
				lineCur = atoi(strs[strs.size() - 1].c_str());
				if (lineBefore != lineCur)
					lineList.push_back(lineCur); //line number

				lineBefore = lineCur;

				traceFile >> token; //state1
				traceFile >> token; //state2
				traceFile >> token; //command1

				if (token == "[mem3[3]")
					break;  //we are at the end of violation
				else {
					//read until the end of line
					traceFile >> std::noskipws;
					char c;
					while (traceFile >> c) {
						if (c == '\n' || c == '\r') {
							break;
						}
					}
					traceFile >> std::skipws;
				}
			}

		} else { //the first part in counter-example file
			traceFile >> std::noskipws;
			char c;
			while (traceFile >> c) {
				if (c == '\n' || c == '\r') {
					break;
				}
			}
			traceFile >> std::skipws;
		}
	}

	while (std::getline(programFile, line))
		progLines.push_back(line);

	//for attacker
	for (std::vector<int>::reverse_iterator iterator1 = lineList.rbegin();
			iterator1 != lineList.rend(); iterator1++) {
		int num = *iterator1;
		std::string aLine = progLines[num - 1];
		//std::cout << num << "," << aLine << std::endl;
		std::vector<std::string> strs;
		boost::split(strs, aLine, boost::is_any_of(" ,,"));
		if (strs[0] == "/*" && strs[1] == attacker->name()) { //a transition of attacker
			int path_ = atoi(strs[2].c_str());
			if (path_ != -1)  // not in the original graph
				violationPaths[strs[1]].push_back(path_);
			else {
				break;
			}

		}
	}

	//for other threads
	foreach (int num, lineList){
	std::string aLine = progLines[num - 1];
	std::vector<std::string> strs;
	boost::split(strs, aLine, boost::is_any_of(" ,,"));
	if (strs[0] == "/*" && strs[1] != attacker->name()) { //a transition
		int path_ = atoi(strs[2].c_str());
		if (path_ != -1)// not in the original graph
		violationPaths[strs[1]].push_back(path_);
	}
}

	return violationPaths;

}

std::vector<int> parseSimulationFile(boost::filesystem::path counter_example,
		boost::filesystem::path program_pml, Thread *attacker) {

	std::vector<int> violationPath;

	std::vector<int> lineList;
	std::ifstream traceFile(counter_example.c_str());
	std::ifstream programFile(program_pml.c_str());
	std::string token, line;
	int lineBefore = -1, lineCur;
	boost::regex txt_regex("[0-9]+:");
	std::vector<std::string> progLines;

	traceFile >> std::skipws;

	//parse file to get a sequence of lines in source code (before [mem4[2]...)
	//for each lines: know thread, id of transition
	//put them in a map and return

	while (traceFile >> token) {
		//this is an edge in the violate execution
		if (boost::regex_match(token, txt_regex)) {
			traceFile >> token;  // proc
			traceFile >> token;  // proc ID
			traceFile >> token;  // prototype

			if (token == "terminates") {  //some processes finish early
			} else {
				traceFile >> token;  //file:line
				std::vector<std::string> strs;
				boost::split(strs, token, boost::is_any_of(":"));
				lineCur = atoi(strs[strs.size() - 1].c_str());
				if (lineBefore != lineCur)
					lineList.push_back(lineCur); //line number

				lineBefore = lineCur;

				traceFile >> token; //state1
				traceFile >> token; //state2
				traceFile >> token; //command1

				if (token == "[mem2[0]")  //error state
					// there is a line as: xxx: stop error, proc can be at the end of line... but because break, we do not consider this line
					break;//we are at the end of violation
				else {
					//read until the end of line
					traceFile >> std::noskipws;
					char c;
					while (traceFile >> c) {
						if (c == '\n' || c == '\r') {
							break;
						}
					}
					traceFile >> std::skipws;
				}
			}

		} else { //the first part in counter-example file or the line: Never claim moves to line ...
			traceFile >> std::noskipws;
			char c;
			while (traceFile >> c) {
				if (c == '\n' || c == '\r') {
					break;
				}
			}
			traceFile >> std::skipws;
		}
	}

	//std::cout << "we are here --------------------------" << std::endl;

	while (std::getline(programFile, line))
		progLines.push_back(line);

	//foreach (int num, lineList) {
	//std::cout << "aaaa" << std::endl;
	for (std::vector<int>::reverse_iterator iterator1 = lineList.rbegin();
			iterator1 != lineList.rend(); iterator1++) {
		int num = *iterator1;
		std::string aLine = progLines[num - 1];
		//std::cout << num << "," << aLine << std::endl;
		std::vector<std::string> strs;
		boost::split(strs, aLine, boost::is_any_of(" ,,"));
		if (strs[0] == "/*" && strs[1] == attacker->name()) { //a transition of attacker
			int path_ = atoi(strs[2].c_str());
			if (path_ != -1)  // not in the original graph
				violationPath.push_back(path_);
			else {
				return violationPath;
			}

		}
	}

	assert(!"NEVER REACHED");
	return violationPath;

}

// Using for -fa 2 0
std::vector<int> parseSimulationFile2(boost::filesystem::path counter_example,
		boost::filesystem::path program_pml, Thread *attacker) {

	std::vector<int> violationPath;

	std::vector<int> lineList;
	std::ifstream traceFile(counter_example.c_str());
	std::ifstream programFile(program_pml.c_str());
	std::string token, line;
	int lineBefore = -1, lineCur;
	boost::regex txt_regex("[0-9]+:");
	std::vector<std::string> progLines;

	traceFile >> std::skipws;

	//parse file to get a sequence of lines in source code (before [mem4[2]...)
	//for each lines: know thread, id of transition
	//put them in a map and return

	while (traceFile >> token) {
		//this is an edge in the violate execution
		if (boost::regex_match(token, txt_regex)) {
			traceFile >> token;  // proc
			traceFile >> token;  // proc ID
			traceFile >> token;  // prototype

			if (token == "terminates") {  //some processes finish early
			} else {
				traceFile >> token;  //file:line
				std::vector<std::string> strs;
				boost::split(strs, token, boost::is_any_of(":"));
				lineCur = atoi(strs[strs.size() - 1].c_str());
				if (lineBefore != lineCur)
					lineList.push_back(lineCur); //line number

				lineBefore = lineCur;

				traceFile >> token; //state1
				traceFile >> token; //state2
				traceFile >> token; //command1

				if (token == "[mem3[3]")  //error state
					// there is a line as: xxx: stop error, proc can be at the end of line... but because break, we do not consider this line
					break;//we are at the end of violation
				else {
					//read until the end of line
					traceFile >> std::noskipws;
					char c;
					while (traceFile >> c) {
						if (c == '\n' || c == '\r') {
							break;
						}
					}
					traceFile >> std::skipws;
				}
			}

		} else { //the first part in counter-example file or the line: Never claim moves to line ...
			traceFile >> std::noskipws;
			char c;
			while (traceFile >> c) {
				if (c == '\n' || c == '\r') {
					break;
				}
			}
			traceFile >> std::skipws;
		}
	}

	//std::cout << "we are here --------------------------" << std::endl;

	while (std::getline(programFile, line))
		progLines.push_back(line);

	//foreach (int num, lineList) {
	//std::cout << "aaaa" << std::endl;
	for (std::vector<int>::reverse_iterator iterator1 = lineList.rbegin();
			iterator1 != lineList.rend(); iterator1++) {
		int num = *iterator1;
		std::string aLine = progLines[num - 1];
		//std::cout << num << "," << aLine << std::endl;
		std::vector<std::string> strs;
		boost::split(strs, aLine, boost::is_any_of(" ,,"));
		if (strs[0] == "/*" && strs[1] == attacker->name()) { //a transition of attacker
			int path_ = atoi(strs[2].c_str());
			if (path_ != -1)  // not in the original graph
				violationPath.push_back(path_);
			else {
				return violationPath;
			}

		}
	}

	assert(!"NEVER REACHED");
	return violationPath;

}

std::vector<int> SpinModelChecker::check(const Program &program,
		Thread *thread) const {

	//Need to consider here
	boost::filesystem::path temp_dir = makeTempDir();
	boost::filesystem::path program_pml = temp_dir / "program.pml";
	boost::filesystem::path verifier_c = temp_dir / "pan.c";
	boost::filesystem::path verifier = temp_dir / "pan";
	boost::filesystem::path trail = temp_dir / "program.pml.trail";
	boost::filesystem::path graph = temp_dir / "graph.gv";
	boost::filesystem::path counter_example = temp_dir
			/ "counter-example.stdout";

	//need to be removed before publishing
//	std::ofstream out1(graph.string().c_str());
//	wmm::DotPrinter printer_xx;
//	printer_xx.print(out1, program);

	std::ofstream out(program_pml.string().c_str());
	SpinPrinter printer;

	printer.print(out, program);
	if (!out) {
		throw std::runtime_error(
				"could not create file: " + program_pml.string());
	}
	out.close();
	long spinTime = run(
			(boost::format(spinCommandLine()) % temp_dir % program_pml).str());
	//std::cout << "spin is done" << std::endl;
	long compilerTime = run(
			(boost::format(compilerCommandLine()) % temp_dir % verifier
					% verifier_c).str());
	//std::cout << "gcc is done" << std::endl;
	long verifierTime = run(
			(boost::format(verifierCommandLine()) % temp_dir % verifier).str());
	//std::cout << "verification is done" << std::endl;

	Statistics::instance().addSpinTime(spinTime);
	Statistics::instance().addCompilerTime(compilerTime);
	Statistics::instance().addVerifierTime(verifierTime);
	Statistics::instance().addSeqRealTime(
			spinTime + compilerTime + verifierTime);

	bool violate = boost::filesystem::exists(trail);
	std::vector<int> violationPath;

	if (violate) {
		//std::cout << "we are here +++++++++++++ssssssssssssss" << std::endl;
		run(
				(boost::format(simulationCommandLine()) % temp_dir % trail
						% program_pml % counter_example).str());
		//std::cout << "reading trace is done" << std::endl;
		violationPath = parseSimulationFile(counter_example, program_pml,
				thread);
		//std::cout << "parsing trace is done" << std::endl;
	}

	boost::filesystem::remove_all(temp_dir);

	return violationPath;
}

// Use for -fa 2 0
std::vector<int> SpinModelChecker::check2(const Program &program,
		Thread *thread) const {

	//Need to consider here
	boost::filesystem::path temp_dir = makeTempDir();
	boost::filesystem::path program_pml = temp_dir / "program.pml";
	boost::filesystem::path verifier_c = temp_dir / "pan.c";
	boost::filesystem::path verifier = temp_dir / "pan";
	boost::filesystem::path trail = temp_dir / "program.pml.trail";
	boost::filesystem::path graph = temp_dir / "graph.gv";
	boost::filesystem::path counter_example = temp_dir
			/ "counter-example.stdout";

	//need to be removed before publishing
//	std::ofstream out1(graph.string().c_str());
//	wmm::DotPrinter printer_xx;
//	printer_xx.print(out1, program);

	std::ofstream out(program_pml.string().c_str());
	SpinPrinter2 printer;

	printer.print(out, program);
	if (!out) {
		throw std::runtime_error(
				"could not create file: " + program_pml.string());
	}
	out.close();
	long spinTime = run(
			(boost::format(spinCommandLine()) % temp_dir % program_pml).str());
	//std::cout << "spin is done" << std::endl;
	long compilerTime = run(
			(boost::format(compilerCommandLine()) % temp_dir % verifier
					% verifier_c).str());
	//std::cout << "gcc is done" << std::endl;
	long verifierTime = run(
			(boost::format(verifierCommandLine()) % temp_dir % verifier).str());
	//std::cout << "verification is done" << std::endl;

	Statistics::instance().addSpinTime(spinTime);
	Statistics::instance().addCompilerTime(compilerTime);
	Statistics::instance().addVerifierTime(verifierTime);
	Statistics::instance().addSeqRealTime(
			spinTime + compilerTime + verifierTime);

	bool violate = boost::filesystem::exists(trail);
	std::vector<int> violationPath;

	if (violate) {
		//std::cout << "we are here +++++++++++++ssssssssssssss" << std::endl;
		run(
				(boost::format(simulationCommandLine()) % temp_dir % trail
						% program_pml % counter_example).str());
		//std::cout << "reading trace is done" << std::endl;
		violationPath = parseSimulationFile2(counter_example, program_pml,
				thread);
		//std::cout << "parsing trace is done" << std::endl;
	}

	boost::filesystem::remove_all(temp_dir);

	return violationPath;
}

const std::map<std::string, std::vector<int>> SpinModelChecker::checkAbstract2(
		const Program &program, Thread *thread) const {

	boost::filesystem::path temp_dir = makeTempDir();
	boost::filesystem::path program_pml = temp_dir / "program.pml";
	boost::filesystem::path verifier_c = temp_dir / "pan.c";
	boost::filesystem::path verifier = temp_dir / "pan";
	boost::filesystem::path trail = temp_dir / "program.pml.trail";
	boost::filesystem::path graph = temp_dir / "graph.gv";
	boost::filesystem::path counter_example = temp_dir
			/ "counter-example.stdout";

	//need to be removed before publishing
//	std::ofstream out1(graph.string().c_str());
//	wmm::DotPrinter printer_xx;
//	printer_xx.print(out1, program);

	std::ofstream out(program_pml.string().c_str());
	SpinPrinter2 printer;

	printer.print(out, program);
	if (!out) {
		throw std::runtime_error(
				"could not create file: " + program_pml.string());
	}
	out.close();
	long spinTime = run(
			(boost::format(spinCommandLine()) % temp_dir % program_pml).str());

//	if (program.threads().size() <= 4) {
//		verifierCommandLine_ = setVerifierCommandLine("cd \"%1%\" && \"%2%\" -m1000  > \"%2%.stdout\" 2> \"%2%.stderr\"", this);
//	} else
//		verifierCommandLine_ = "cd \"%1%\" && \"%2%\" -m10000  > \"%2%.stdout\" 2> \"%2%.stderr\"";

	long compilerTime = run(
			(boost::format(compilerCommandLine()) % temp_dir % verifier
					% verifier_c).str());
	long verifierTime = run(
			(boost::format(verifierCommandLine()) % temp_dir % verifier).str());

	Statistics::instance().addSpinTime(spinTime);
	Statistics::instance().addCompilerTime(compilerTime);
	Statistics::instance().addVerifierTime(verifierTime);
	Statistics::instance().addSeqRealTime(
			spinTime + compilerTime + verifierTime);

	bool violate = boost::filesystem::exists(trail);
	std::map<std::string, std::vector<int>> violationPaths;

	if (violate) {
		run(
				(boost::format(simulationCommandLine()) % temp_dir % trail
						% program_pml % counter_example).str());
		violationPaths = parseSimulationFile12(counter_example, program_pml,
				thread);
	}

	boost::filesystem::remove_all(temp_dir);

	return violationPaths;
}

}

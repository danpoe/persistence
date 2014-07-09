/*
 * Program.h
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

#include <map>
#include <memory>
#include <vector>

#include "Instruction.h"
#include "Expression.h"
#include "Thread.h"

namespace wmm {

class Program {
	int memorySize_;
	int intSpaceSize_;
	int boolSpaceSize_;

	std::map<std::string, std::unique_ptr<Thread>> name2thread_;
	std::vector<Thread *> threads_;

	Domain interestingAddress_;
	Space interestingSpace_;

	std::map<std::string, std::shared_ptr<Register>> name2register_;

	std::map<Domain, std::shared_ptr<Constant>> value2constant_;

public:

	Program(int memorySize = 10) :
			memorySize_(memorySize), intSpaceSize_(2), boolSpaceSize_(1), interestingAddress_(0), interestingSpace_(
					INVALID_SPACE) {
	}

	int memorySize() const {
		return memorySize_;
	}
	void setMemorySize(int size) {
		memorySize_ = size;
	}

	int intSpaceSize() const {
		return intSpaceSize_;
	}

	void setIntSpaceSize(int size) {
		intSpaceSize_ = size;
	}

	int boolSpaceSize() const {
		return boolSpaceSize_;
	}

	void setBoolSpaceSize(int size) {
		boolSpaceSize_ = size;
	}

	const std::vector<Thread *> &threads() const {
		return threads_;
	}
	Thread *getThread(const std::string &name);
	Thread *makeThread(const std::string &name);

	void addThread(Thread *thread);

	void setInterestingAddress(Domain address, Space space = Space()) {
		interestingAddress_ = address;
		interestingSpace_ = space;
	}

	Domain interestingAddress() const {
		return interestingAddress_;
	}
	Space interestingSpace() const {
		return interestingSpace_;
	}

	const std::shared_ptr<Register> &makeRegister(const std::string &name);

	const std::shared_ptr<Constant> &makeConstant(Domain value);
};

}

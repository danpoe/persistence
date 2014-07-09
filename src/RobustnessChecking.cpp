/*
 * RobustnessChecking.cpp
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

#include "RobustnessChecking.h"

#include "DotPrinter.h"
#include "Benchmarking.h"
#include "Foreach.h"
#include "Program.h"
#include "Reduction.h"
#include "SpinModelChecker.h"
#include "State.h"
#include "Transition.h"

namespace wmm {

//namespace {

bool isReachable(State *state, State *target,
		boost::unordered_set<State *> &visited) {

	//std::cout << "-----" << state->name() << "," << target->name() << std::endl;

	//if (state->name() == target->name())
	//	std::cout << "+++++++" << visited.size()<<std::endl;

	if (visited.find(state) != visited.end()) {
		//std::cout << "fffff" <<std::endl;
		return false;
	}

	if (state->name() == target->name()) {
		return true;
	}

	visited.insert(state);

	foreach (Transition *transition, state->out()){
	switch (transition->instruction()->mnemonic()) {
		case Instruction::READ:
		case Instruction::WRITE:
		case Instruction::LOCAL:
		case Instruction::CONDITION:
		case Instruction::NOOP:
		if (isReachable(transition->to(), target, visited)) {
			return true;
		}
		break;
		case Instruction::MFENCE:
		case Instruction::LOCK:
		case Instruction::UNLOCK:
		break;
		default: {
			assert(!"NEVER REACHED");
		}
	}
}

	return false;
}

//} // anonymous namespace

//We will return a path from write to read operations
//
boost::unordered_set<State *> isAttackFeasible(const Program &program,
		Thread *attacker, const boost::unordered_set<State *> &fenced) {
	boost::unordered_set<State *> result;
	wmm::SpinModelChecker checker(program.threads().size(), 0);
	wmm::Program augmentedProgram;
	//bool feasible = true;

	Statistics::instance().incPotentialAttacksCount();
	wmm::reduce(program, augmentedProgram, false, attacker, NULL, NULL, fenced);

	//we only get path form write to read opeartions of attacker
	std::vector<int> violationPath = checker.check(augmentedProgram, attacker);

	//std::cout << "violation path:";
	//foreach(int path, violationPath)std::cout << path << ",";
	//std::cout << std::endl;

	foreach (int step, violationPath){
	//foreach (Transition *trans, attacker->transitions() ) {
	for (auto next = attacker->transitions().begin(); next != attacker->transitions().end();
			next++) {
		//check result contains, and is a write
		Transition *trans = *next;
		if (trans->getID() == step && trans->instruction()->is<Write>( )) {
			State *state = trans->to();
			if (result.find(state) == result.end() ) {
				result.insert(state);
			}
			break;
			//nned to be improve
		}
	}
}

	return result;
}

bool isAttackFeasible(const Program &program, bool searchForTdrOnly,
		Thread *attacker, Transition *attackWrite, Transition *attackRead,
		const boost::unordered_set<State *> &fenced) {

	Statistics::instance().incPotentialAttacksCount();
	wmm::Program augmentedProgram;
	//wmm::SpinModelChecker checker;
	wmm::SpinModelChecker checker(program.threads().size(), -1);

	if (attackWrite && attackRead) {
		boost::unordered_set<State *> visited(fenced);
		if (!isReachable(attackWrite->to(), attackRead->to(), visited)) {
			Statistics::instance().incInfeasibleAttacksCount1();
			return false;
		}
	}

	wmm::reduce(program, augmentedProgram, searchForTdrOnly, attacker,
			attackWrite, attackRead, fenced);
	bool feasible = checker.check(augmentedProgram);

	if (feasible) {
		Statistics::instance().incFeasibleAttacksCount();
	} else {
		Statistics::instance().incInfeasibleAttacksCount2();
	}

	return feasible;
}

boost::unordered_set<Domain> getReadVariableOfThread(Thread *thread) {
	boost::unordered_set<Domain> readList;

	foreach (Transition *transition, thread->transitions()){
	if (transition->instruction()->is<Read>()) {
		Read *read = transition->instruction()->as<Read>();
		const std::shared_ptr<Expression> address = read->address();
		if (address->kind() == Expression::CONSTANT) {
			readList.insert(address->as<Constant>()->value());
		}
		//then we should keep all write operations of helpers
		else {
			readList.insert(-2);
		}
	}
}

	return readList;
}

wmm::Program abstractProgram2(const Program &program, Thread *attacker,
		boost::unordered_set<Domain> &readAttacker) {

	wmm::Program checkingProgram;
	checkingProgram.setMemorySize(program.memorySize());
	const std::shared_ptr<Constant> minus = checkingProgram.makeConstant(-17);
	bool keepAll = readAttacker.find(-2) != readAttacker.end();

	foreach (Thread *thread, program.threads()){
	if (thread->name() != attacker->name()) {
		Thread *checkingThread = checkingProgram.makeThread(thread->name());
		if (thread->initialState())
		checkingThread->setInitialState(
				checkingThread->makeState(
						thread->initialState()->name()));

		foreach (Transition *transition, thread->transitions()) {
			State *From = checkingThread->makeState(
					transition->from()->name());
			State *To = checkingThread->makeState(transition->to()->name());

			if (Write *write = transition->instruction()->as<Write>()) {
				const std::shared_ptr<Expression> address =
				write->address();
				const std::shared_ptr<Expression> value = write->value();
				if (address->kind() == Expression::CONSTANT) {
					if ((readAttacker.find(address->as<Constant>()->value())
									== readAttacker.end()) && !keepAll) {
						checkingThread->makeTransition(From, To,
								std::make_shared<Noop>(),
								transition->getOriginal(), true);
					} else {
						//check value here
						if (value->kind() == Expression::CONSTANT) {
							checkingThread->makeTransition(From, To,
									transition->instruction(),
									transition->getOriginal(),
									transition->getIsNotPrecise());

						} else {
							checkingThread->makeTransition(From, To,
									std::make_shared < Write
									> (minus, address),
									transition->getOriginal(), true);
						}
					}
				} else {
					if (value->kind() == Expression::CONSTANT) {
						checkingThread->makeTransition(From, To,
								std::make_shared < Write > (value, minus),
								transition->getOriginal(), true);
					} else {
						checkingThread->makeTransition(From, To,
								std::make_shared < Write > (minus, minus),
								transition->getOriginal(), true);
					}
				}

			} else if (transition->instruction()->as<Lock>()
					|| transition->instruction()->as<Unlock>()) {
				checkingThread->makeTransition(From, To,
						transition->instruction(),
						transition->getOriginal(), false);
			} else {
				checkingThread->makeTransition(From, To,
						std::make_shared<Noop>(), transition->getOriginal(),
						true);
			}
		}
	} else {
		checkingProgram.addThread(attacker);
	}

}

	return checkingProgram;
}

typedef struct myPair {
	int address;
	int value;
} MyPair;

// Only keep needed write operations (by checking with readAttacker) for other threads, merging all processes to one,
// merging all states to one
wmm::Program abstractProgram1(const Program &program, Thread *attacker,
		boost::unordered_set<Domain> &readAttacker) {

	wmm::Program checkingProgram;
	checkingProgram.setMemorySize(program.memorySize());
	const std::shared_ptr<Constant> minus = checkingProgram.makeConstant(-17);
	bool keepAll = readAttacker.find(-2) != readAttacker.end();
	std::vector<MyPair> pairs;
	bool exist_;

	Thread *envThread = checkingProgram.makeThread("enviromental thread");
	State *Alone = envThread->makeState("My state");

	envThread->setInitialState(Alone);
	//envThread->makeTransition(Out, In, std::make_shared<Noop>(), 1000, true);

	foreach (Thread *thread, program.threads()){
	if (thread->name() != attacker->name()) {

		foreach (Transition *transition, thread->transitions()) {
			MyPair mypair_;
			if (Write *write = transition->instruction()->as<Write>()) {
				const std::shared_ptr<Expression> address =
				write->address();
				const std::shared_ptr<Expression> value = write->value();

				if (address->kind() == Expression::CONSTANT) {
					if ((readAttacker.find(address->as<Constant>()->value())
									== readAttacker.end()) && !keepAll) {
//						envThread->makeTransition(In, Out,
//								std::make_shared<Noop>(),
//								transition->getOriginal(), true);
					} else {

						if (value->kind() == Expression::CONSTANT) {
							//check value here
							exist_ = false;
							for (auto next = pairs.begin(); next != pairs.end(); next++) {
								if ((next->address == address->as<Constant>()->value()) && (next->value == value->as<Constant>()->value())) {
									exist_ = true;
									break;
								}

							}
							if (!exist_) {
								//mypair_ = (MyPair *) malloc(sizeof(MyPair));
								mypair_.address = (int)(address->as<Constant>()->value());
								mypair_.value = (int)(value->as<Constant>()->value());
								pairs.push_back(mypair_);
								envThread->makeTransition(Alone, Alone,
										transition->instruction(),
										transition->getOriginal(),
										transition->getIsNotPrecise());

							}

						} else {
							//check value here
							exist_ = false;
							for (auto next = pairs.begin(); next != pairs.end(); next++) {
								if ((next->address == address->as<Constant>()->value()) && (next->value == -17)) {
									exist_ = true;
									break;
								}

							}
							if (!exist_) {
								mypair_.address = (int)(address->as<Constant>()->value());
								mypair_.value = -17;
								pairs.push_back(mypair_);
								//pairs.insert(myPair((int)(address->as<Constant>()->value()), -17));
								envThread->makeTransition(Alone, Alone,
										std::make_shared < Write
										> (minus, address),
										transition->getOriginal(), true);
							}
						}
					}
				} else {
					if (value->kind() == Expression::CONSTANT) {
						//check value here
						exist_ = false;
						for (auto next = pairs.begin(); next != pairs.end(); next++) {
							if ((next->address == -17) && (next->value == value->as<Constant>()->value())) {
								exist_ = true;
								break;
							}

						}
						if (!exist_) {
							mypair_.address = -17;
							mypair_.value = (int)(value->as<Constant>()->value());
							pairs.push_back(mypair_);

							//pairs.insert(myPair(-17, (int)(value->as<Constant>()->value())));
							envThread->makeTransition(Alone, Alone,
									std::make_shared < Write > (value, minus),
									transition->getOriginal(), true);
						}
					} else {
						//check value here
						exist_ = false;
						for (auto next = pairs.begin(); next != pairs.end(); next++) {
							if ((next->address == -17) && (next->value == -17)) {
								exist_ = true;
								break;
							}

						}
						if (!exist_) {
							//pairs.insert(myPair(-17, -17));
							mypair_.address = -17;
							mypair_.value = -17;
							pairs.push_back(mypair_);
							envThread->makeTransition(Alone, Alone,
									std::make_shared < Write > (minus, minus),
									transition->getOriginal(), true);
						}
					}
				}

			}
		}
	} else {
		checkingProgram.addThread(attacker);
	}

}

	return checkingProgram;
}

bool isFeasiblePath(Thread *thread, const std::vector<int> &path) {
	bool feasible = true;

	foreach (int transactionID, path){
	//foreach (Transition *transition, thread->transitions()) {
	for (auto next = thread->transitions().begin(); next != thread->transitions().end();
			next++) {
		Transition *transition = *next;
		if (transition->getID() == transactionID) {
			if (transition->getIsNotPrecise() == true) {
				feasible = false;
				break;
			}
		}
	}
}

	return feasible;
}

void updateCheckingThread(Thread *originalThread, Thread *checkingThread,
		const std::vector<int> &path) {

	std::vector<Transition *> checkingTranss = checkingThread->transitions();
	int index = 0;
	std::stringstream convert;
	bool first = true;

	std::vector<int> myvector = path;
	std::vector<int>::iterator next;

	//foreach (int transitionID, path) {
	for (std::vector<int>::iterator transitionIDIterator = myvector.begin();
			transitionIDIterator != myvector.end(); transitionIDIterator++) {

		foreach (Transition *checkingTrans, checkingTranss){
		if (checkingTrans->getID() == *transitionIDIterator) {

			convert.str("");
			convert << index;
			State *From = checkingThread->makeState(
					"chk_" + checkingTrans->from()->name() + "_"
					+ convert.str());
			index = index + 1;
			convert.str("");
			convert << index;
			State *To = checkingThread->makeState(
					"chk_" + checkingTrans->to()->name() + "_"
					+ convert.str());

			if (!checkingTrans->getIsNotPrecise())  //precise edges
			checkingThread->makeTransition(From, To,
					checkingTrans->instruction(),
					checkingTrans->getOriginal(),
					checkingTrans->getIsNotPrecise());
			else { //update instructions here
				   //foreach (Transition *originalTrans, originalThread->transitions()) {
				for (auto next = originalThread->transitions().begin(); next != originalThread->transitions().end();
						next++) {
					Transition *originalTrans = *next;
					if (originalTrans->getID()
							== checkingTrans->getOriginal()) {
						checkingThread->makeTransition(From, To,
								originalTrans->instruction(),
								checkingTrans->getOriginal(), false);
						break;
					}
				}
			}

			if (first) { //first transition of execution from start to end. This contains the input point
				checkingThread->setInitialState(
						checkingThread->makeState(
								"chk_"
								+ checkingThread->initialState()->name()
								+ "_0"));
				State *in = checkingTrans->from();
				foreach (Transition *tran, in->out()) {
					bool found = false;
					if (*transitionIDIterator == tran->getID())
					found = true;

					if (!found)
					checkingThread->makeTransition(From, tran->to(),
							tran->instruction(), tran->getOriginal(),
							tran->getIsNotPrecise());
				}

				State *out = checkingTrans->to();
				foreach (Transition *tran, out->out()) {
					bool found = false;
					next = transitionIDIterator + 1;
					if (next != myvector.end()) {
						if (*next == tran->getID())
						found = true;
					}

					if (!found)
					checkingThread->makeTransition(To, tran->to(),
							tran->instruction(), tran->getOriginal(),
							tran->getIsNotPrecise());
				}

				first = false;
			} else { //other edges (not the first) we only need to consider to point
				State *out = checkingTrans->to();
				foreach (Transition *tran, out->out()) {
					bool found = false;
					next = transitionIDIterator + 1;
					if (next != myvector.end()) {
						if (*next == tran->getID())
						found = true;
					}

					if (!found) {
						checkingThread->makeTransition(To, tran->to(),
								tran->instruction(), tran->getOriginal(),
								tran->getIsNotPrecise());
					}
				}
			}
		}
	}
}

}

void optimizeProgram(const Program &program, Thread *attacker) {
	foreach (Thread *thread, program.threads())if (thread->name() != attacker->name()) {
		State *initial = thread->initialState();
		boost::unordered_set<State *> reached, unreached;

		reached.insert(initial);
		unsigned size_before = 0;

		while (size_before < reached.size()) {
			size_before = reached.size();
			foreach (State *state, reached) {
				std::vector<Transition *> transOut = state->out();
				foreach (Transition *trans, transOut) {
					State *out = trans->to();
					reached.insert(out);
				}
			}
		}

		foreach(State *state, thread->states()) {
			if (reached.find(state) == reached.end()) {
				unreached.insert(state);
			}
		}

		foreach(State *state, unreached) {
			std::vector<Transition *> transOut = state->out();
			std::vector<Transition *> transIn = state->in();

			foreach(Transition *tran, transOut) {
				thread->deleteTransition(tran->getID());
			}

			foreach(Transition *tran, transIn) {
				thread->deleteTransition(tran->getID());
			}

			thread->deleteState(state);
		}

	}

}

boost::unordered_set<State *> isAttackFeasibleAbstraction2(
		const Program &program, Thread *attacker, int loopNum,
		const boost::unordered_set<State *> &fenced) {

	Statistics::instance().incPotentialAttacksCount();

	boost::unordered_set<State *> result;

	boost::unordered_set<Domain> readAttacker = getReadVariableOfThread(
			attacker);
	wmm::Program checkingProgram = abstractProgram2(program, attacker,
			readAttacker);
	//wmm::SpinModelChecker checker;
	wmm::SpinModelChecker checker(program.threads().size(), 2);

	bool feasible = true;
	int i;
	int maxloops = loopNum + 1;

	if (maxloops != 1) {
		for (i = 0; i < maxloops; i++) {

			wmm::Program augmentedProgram;
			feasible = true;

			wmm::reduce2(checkingProgram, augmentedProgram, false, attacker,
			NULL, NULL, fenced);
			//wmm::reduce(program, augmentedProgram, false, attacker, NULL, NULL, fenced);
			//do some optimization herre
			//optimizeProgram(augmentedProgram, attacker);
			std::map<std::string, std::vector<int>> violationPaths =
					checker.checkAbstract2(augmentedProgram, attacker);
			std::map<std::string, bool> isFeasiblePaths;

			if (violationPaths.size() != 0) {
				foreach(Thread *thread, checkingProgram.threads()){
				if (thread->name() != attacker->name()) {
					if (violationPaths.find(thread->name())
							!= violationPaths.end()) {
						if (!isFeasiblePath(thread,
										violationPaths[thread->name()])) {
							feasible = false;
							isFeasiblePaths[thread->name()] = false;
						} else
						isFeasiblePaths[thread->name()] = true;
					}
				}
			}

			result.clear();
			foreach (int step, violationPaths[attacker->name()]) {
				//foreach (Transition *trans, attacker->transitions() ) {
				for (auto next = attacker->transitions().begin(); next != attacker->transitions().end();
						next++) {
					//check result contains, and is a write
					Transition *trans = *next;
					if (trans->getID() == step && trans->instruction()->is<Write>( )) {
						State *state = trans->to();
						if (result.find(state) == result.end() ) {
							result.insert(state);
						}
						break;
						//nned to be improve
					}
				}
			}

			if (!feasible) {
				Thread *originalThread, *checkingThread;
				foreach(auto mymap, isFeasiblePaths) { // isFeasiblePaths contains violation of threads (not attacker), but can be feasible or not
					if (!mymap.second) {
						//find originalThread
						foreach(Thread *original, program.threads())
						if (original->name() == mymap.first) {
							originalThread = original;
							//findcheckingThread
							//foreach(Thread *checking, checkingProgram.threads())
							for (auto next = checkingProgram.threads().begin(); next != checkingProgram.threads().end();
									next++) {

								Thread *checking = *next;
								if (checking->name() == mymap.first) {
									checkingThread = checking;
									updateCheckingThread(originalThread,
											checkingThread,
											violationPaths[mymap.first]);
									break;
								}
							}
							break;
						}
					}
				}
			} else
			break;
		} else {
			Statistics::instance().incInfeasibleAttacksCount2();
			//result = violationPaths[attacker->name()];
			result.clear();
			return result;
		}
	} //for

} else {
	//std::cout << "We hare herer" << std::endl;

	//std::cout << "aaaaaaa checking fence:";
	//		foreach(State *state, fenced) std::cout << state->name();
	//	std::cout << std::endl;

	wmm::Program augmentedProgram;
	//feasible = true;
	wmm::reduce2(checkingProgram, augmentedProgram, false,
	//wmm::reduce(checkingProgram, augmentedProgram, false,
			attacker, NULL, NULL, fenced);
	//do some optimization herre
	//optimizeProgram(augmentedProgram, attacker);
	std::vector<int> violationPath = checker.check2(augmentedProgram, attacker);

	//std::vector<int> violationPath = checker.check(augmentedProgram, attacker);


	if (violationPath.size() > 0 ) {
		feasible = true;
	} else {
		feasible = false;
	}

	if (feasible) {
		Statistics::instance().incFeasibleAttacksCount();
	} else {
		Statistics::instance().incInfeasibleAttacksCount2();
	}

	//here attacker in checkingprogram is not attcker
	//Thread *attackerInChecking = checkingProgram.getThread(attacker->name() ) ;

	foreach (int step, violationPath) {
		//foreach (Transition *trans, attacker->transitions() ) {
		for (auto next = attacker->transitions().begin(); next != attacker->transitions().end();
				next++) {
			//check result contains, and is a write
			Transition *trans = *next;
			if (trans->getID() == step && trans->instruction()->is<Write>( )) {
				State *state = trans->to();
				if (result.find(state) == result.end() ) {
					result.insert(state);
				}
				break;
				//nned to be improve
			}
		}
	}
}

	if (feasible || (!feasible && i == maxloops)) {
		Statistics::instance().incFeasibleAttacksCount();

	} else {
		Statistics::instance().incInfeasibleAttacksCount2();

	}

	return result;

}

// Here we will meger all other threads to one, merge all states to one
// We cannot do refinement
boost::unordered_set<State *> isAttackFeasibleAbstraction1(
		const Program &program, Thread *attacker, int loopNum,
		const boost::unordered_set<State *> &fenced) {

	Statistics::instance().incPotentialAttacksCount();

	boost::unordered_set<State *> result;

	boost::unordered_set<Domain> readAttacker = getReadVariableOfThread(
			attacker);
	wmm::Program checkingProgram = abstractProgram1(program, attacker,
			readAttacker);
	//wmm::SpinModelChecker checker;
	wmm::SpinModelChecker checker(program.threads().size(), 1);

	bool feasible = true;
	//int i;
	//int maxloops = loopNum + 1;

	//std::cout << "We hare herer" << std::endl;

	//std::cout << "aaaaaaa checking fence:";
	//		foreach(State *state, fenced) std::cout << state->name();
	//	std::cout << std::endl;

	wmm::Program augmentedProgram;
	feasible = true;
	wmm::reduce(checkingProgram, augmentedProgram, false, attacker, NULL, NULL,
			fenced);
	//do some optimization herre
	//optimizeProgram(augmentedProgram, attacker);
	std::vector<int> violationPath = checker.check(augmentedProgram, attacker);

	if (violationPath.size() > 0) {
		feasible = true;
	} else {
		feasible = false;
	}

	if (feasible) {
		Statistics::instance().incFeasibleAttacksCount();
	} else {
		Statistics::instance().incInfeasibleAttacksCount2();
	}

	//here attacker in checkingprogram is not attcker
	//Thread *attackerInChecking = checkingProgram.getThread(attacker->name() ) ;

	foreach (int step, violationPath){
	//foreach (Transition *trans, attacker->transitions() ) {
	for (auto next = attacker->transitions().begin(); next != attacker->transitions().end();
			next++) {
		//check result contains, and is a write
		Transition *trans = *next;
		if (trans->getID() == step && trans->instruction()->is<Write>( )) {
			State *state = trans->to();
			if (result.find(state) == result.end() ) {
				result.insert(state);
			}
			break;
			//nned to be improve
		}
	}
}

	if (feasible) {
		Statistics::instance().incFeasibleAttacksCount();

	} else {
		Statistics::instance().incInfeasibleAttacksCount2();

	}

	return result;

}

}

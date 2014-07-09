/*
 * FenceInsertion.cpp
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

#include "FenceInsertion.h"

#include <boost/range/adaptor/map.hpp>
#include <boost/threadpool.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#include "Benchmarking.h"
#include "Foreach.h"
#include "Instruction.h"
#include "Program.h"
#include "RobustnessChecking.h"
#include "SortAndUnique.h"
#include "State.h"
#include "Thread.h"
#include "Transition.h"

#include "config.h"

#include <lpsolve/lp_lib.h>

namespace wmm {

namespace {

class Attack {
	const Program &program_;
	Thread *attacker_;
	Transition *write_;
	Transition *read_;

	bool feasible_;
	std::vector<State *> intermediary_;
	std::vector<std::vector<State*>> paths_;

public:

	Attack(const Program &program, Thread *attacker, Transition *write,
			Transition *read) :
			program_(program), attacker_(attacker), write_(write), read_(read), feasible_(
					false) {
	}

	const Program &program() const {
		return program_;
	}
	Thread *attacker() const {
		return attacker_;
	}
	Transition *write() const {
		return write_;
	}
	Transition *read() const {
		return read_;
	}

	bool feasible() const {
		return feasible_;
	}
	void setFeasible(bool value) {
		feasible_ = value;
	}

	const std::vector<State *> &intermediary() const {
		return intermediary_;
	}

	template<class T>
	void setIntermediary(const T &container) {
		intermediary_.clear();
		intermediary_.insert(intermediary_.end(), container.begin(),
				container.end());
	}

	const std::vector<std::vector<State*>> &getPaths() const {
		return paths_;
	}
	void setPaths(std::vector<std::vector<State*>> paths) {
		paths_ = paths;
	}
};

class AttackChecker {
	Attack &attack_;
	bool searchForTdrOnly_;
	int fenceType_;

public:

	AttackChecker(Attack &attack, bool searchForTdrOnly, int fenceType) :
			attack_(attack), searchForTdrOnly_(searchForTdrOnly), fenceType_(
					fenceType) {
	}

	void operator()() {
		if (isAttackFeasible(attack_.program(), searchForTdrOnly_,
				attack_.attacker(), attack_.write(), attack_.read())) {
			attack_.setFeasible(true);
			boost::unordered_set<State *> visited;
			boost::unordered_set<State *> intermediary;

			std::vector<std::vector<State*>> paths;
			std::vector<std::vector<State*>> newpaths;

			if (fenceType_ == 0) {
				dfs(attack_.write()->to(), attack_.read()->from(), visited,
						intermediary, paths);

				//we only keep dangerous paths from paths
				//first for a specific part, we find all nodes of other parts but not belong to this node
				foreach (std::vector<State*> path, paths){
				boost::unordered_set<State *> fences;
				foreach (std::vector<State*> other, paths) {
					if (other != path) {
						foreach (State* state, other) {
							bool found = false;
							//check path containt state or not
							foreach (State* statepath, path) {
								if (statepath->name() == state->name()) {
									found = true;
								}
							}
							if (!found) {
								fences.insert(state);
							}
						}
					}
				}

				if (isAttackFeasible(attack_.program(), searchForTdrOnly_,
								attack_.attacker(), attack_.write(), attack_.read(),
								fences)) {
					newpaths.push_back(path);
				}
			}
		} else {
			//In the case fenceType = 1, we insert a fence after the write operation
			std::vector<State*> path;
			path.push_back(attack_.write()->to());
			path.push_back(attack_.read()->from());
			newpaths.push_back(path);
		}
			attack_.setIntermediary(intermediary);
			attack_.setPaths(newpaths);
		}

	}

private:

	static void dfs(State *state, State *target,
			boost::unordered_set<State *> &visited,
			boost::unordered_set<State *> &intermediary,
			std::vector<std::vector<State*>> &result) {

		result.clear();

		if (state == target) {
			intermediary.insert(state);
			std::vector<State *> states;
			states.push_back(state);
			result.push_back(states);
			return;
		}

		if (visited.find(state) != visited.end()) {
			return;
		}

		visited.insert(state);

		foreach (Transition *transition, state->out()){
		std::vector<std::vector<State*>> temps;
		switch (transition->instruction()->mnemonic()) {
			case Instruction::READ:
			case Instruction::WRITE:
			case Instruction::LOCAL:
			case Instruction::CONDITION:
			case Instruction::NOOP:

			dfs(transition->to(), target, visited, intermediary, temps);

			if (intermediary.find(transition->to()) != intermediary.end()) {
				intermediary.insert(state);

				foreach (auto temp, temps) {
					temp.push_back(state);
					result.push_back(temp);
				}
			}
			break;
			case Instruction::MFENCE:
			break;
			default: {
				assert(!"NEVER REACHED");
			}
		}
	}
		return;
	}
};

class Division {

	const Program &program_;
	Thread *attacker_;
	std::vector<State *> fences_;

public:

	Division(const Program &program, Thread *attacker) :
			program_(program), attacker_(attacker) {
	}

	const Program &program() const {
		return program_;
	}
	Thread *attacker() const {
		return attacker_;
	}

	const std::vector<State *> &fences() const {
		return fences_;
	}

	template<class T>
	void setFences(const T &container) {
		fences_.clear();
		fences_.insert(fences_.end(), container.begin(), container.end());
	}
};

class DivisionChecker {
	Division &division_;
public:

	DivisionChecker(Division &division) :
			division_(division) {
	}

	void operator()() {
//
//		std::vector<State *> fence;
//		std::list<boost::unordered_set<State *>> W;
//		boost::unordered_set<State *> empty;

		std::vector<Transition *> reads;
		std::vector<Transition *> writes;

		std::vector<State *> fence;
		std::list<boost::unordered_set<State *>> W;
		boost::unordered_set<State *> empty;

		bool isDangerous = false;

		foreach (Transition *transition, division_.attacker()->transitions() ){
		if (transition->instruction()->is<Read>()) {
			//std::cout << "read" << std::endl;
			reads.push_back(transition);
		} else if (transition->instruction()->as<Write>()) {
			//std::cout << "write" << std::endl;
			writes.push_back(transition);
		}
	}

		foreach (Transition *write, writes){
		foreach (Transition *read, reads) {
			//std::cout << write->to()->name() << "," << read->from()->name() << std::endl;
			//attacks.push_back(Attack(program, thread, write, read));
			boost::unordered_set<State *> visited;
			if (isReachable(write->to(), read->from(), visited)) {
				//std::cout << "ooooooooo" << std::endl;
				isDangerous = true;
			}
		}
	}

		if (!isDangerous) {
			division_.setFences(fence);
			return;
		}

		W.push_back(empty);
		division_.setFences(fence);

		//std::cout << "attacker:" << division_.attacker()->name() << std::endl;
		while (W.size() != 0) {
			boost::unordered_set<State *> F = W.front();

//			std::cout << "W is: ";
//			foreach (boost::unordered_set<State *> ff, W){
//			foreach(State *state, ff)
//			{
//				std::cout << state->name() << "|";
//			}
//			std::cout << ";";
//		}
//			std::cout << std::endl;

			W.pop_front();

//			std::cout << "checking fence:";
//			foreach(State *state, F)
//				std::cout << state->name();
//			std::cout << std::endl;

			boost::unordered_set<State *> barriers = isAttackFeasible(
					division_.program(), division_.attacker(), F);

//			std::cout << "fence set to correct a counter-example: "
//					<< barriers.size();
//			foreach(State *state, barriers)std::cout << state->name();
//			std::cout << std::endl;

			if (barriers.size() != 0) {

				foreach (State *state, barriers){
				//boost::unordered_set<State *> nF (F);
				boost::unordered_set<State *> nF;
				foreach(State *state1, F)
				nF.insert(state1);

				nF.insert(state);

//				std::cout << "nF: ";
//				foreach(State *state2, nF)
//				std::cout << state2->name();
//				std::cout << std::endl;

				bool isIn = false;

				//boost::unordered_set<State *>::iterator next;

				for (auto next = W.begin();
						next != W.end(); next++) {
					//foreach (State* state_, path)
					boost::unordered_set<State *> candidate = *next;
					if (isSubSet(candidate, nF)) {
						isIn = true;
						break;
					}
				}
				if (!isIn)
				W.push_back(nF);
			}
		} else {
			division_.setFences(F);
			return;
		}
		//if feasible with F , else we have a counter example
	}

}

private:
bool isSubSet(boost::unordered_set<State *> s,
		boost::unordered_set<State *> S) {
	bool isSubSet = true;
	//std::vector<State *>::iterator next;

	for (auto next = s.begin(); next != s.end(); next++) {
		//foreach (State* state_, path)
		State *state = *next;
		if (S.find(state) == S.end()) {
			isSubSet = false;
			break; //improve
		}
	}

	//		foreach (State *state, s) {
	//			if (S.find(state) == S.end()) {
	//				isSubSet = false;
	//				break; //improve
	//			}
	//		}

	return isSubSet;
}

};

class DivisionAbstractionChecker {
	Division &division_;
	int numLoop_;
	int fenceType_;
public:

	DivisionAbstractionChecker(Division &division, int numLoop, int fenceType) :
			division_(division), numLoop_(numLoop), fenceType_(fenceType) {
	}

//need to be implemented here
	void operator()() {
		//check write and read of attacker

		std::vector<Transition *> reads;
		std::vector<Transition *> writes;

		std::vector<State *> fence;
		std::list<boost::unordered_set<State *>> W;
		boost::unordered_set<State *> empty;

		bool isDangerous = false;
		foreach (Transition *transition, division_.attacker()->transitions() ){
		if (transition->instruction()->is<Read>()) {
			reads.push_back(transition);
		} else if (transition->instruction()->as<Write>()) {
			writes.push_back(transition);
		}
	}

		foreach (Transition *write, writes){
		foreach (Transition *read, reads) {
			//attacks.push_back(Attack(program, thread, write, read));
			boost::unordered_set<State *> visited;
			if (isReachable(write->to(), read->from(), visited)) {
				isDangerous = true;
			}
		}
	}

		if (!isDangerous) {
			division_.setFences(fence);
			return;
		}

		W.push_back(empty);
		division_.setFences(fence);

		//std::cout << "attacker:" << division_.attacker()->name() << std::endl;
		while (W.size() != 0) {
			boost::unordered_set<State *> F = W.front();

			//std::cout << "W is: ";
			//foreach (boost::unordered_set<State *> ff, W){
			//foreach(State *state, ff)
			//{
			//	std::cout << state->name() << "|";
			//}
			//std::cout << ";";
			//	}
			//	std::cout << std::endl;

			W.pop_front();

			//std::cout << "checking fence:";
			//foreach(State *state, F)std::cout << state->name();
			//std::cout << std::endl;

			//We have two way to do abstraction:
			// 1: example 1
			// 2: example 2
			boost::unordered_set<State *> barriers;

			if (fenceType_ == 1)
				barriers = isAttackFeasibleAbstraction1(division_.program(),
						division_.attacker(), numLoop_, F);
			else if (fenceType_ == 2)
				barriers = isAttackFeasibleAbstraction2(division_.program(),
						division_.attacker(), numLoop_, F);
			else {
				throw std::runtime_error("\nDoes not support this type abstraction");
			}
			//std::cout << "fence set to correct a counter-example: "
			//		<< barriers.size();
			//foreach(State *state, barriers)std::cout << state->name();
			//std::cout << std::endl;

			if (barriers.size() != 0) {

				foreach (State *state, barriers){
				//boost::unordered_set<State *> nF (F);
				boost::unordered_set<State *> nF;
				foreach(State *state1, F)
				nF.insert(state1);

				nF.insert(state);

				//std::cout << "nF: ";
				//foreach(State *state2, nF)
				//std::cout << state2->name();
				//std::cout << std::endl;

				bool isIn = false;

				//boost::unordered_set<State *>::iterator next;

				for (auto next = W.begin();
						next != W.end(); next++) {
					//foreach (State* state_, path)
					boost::unordered_set<State *> candidate = *next;
					if (isSubSet(candidate, nF)) {
						isIn = true;
						break;
					}
				}

//					foreach(boost::unordered_set<State *> candidate, W) {
//						if (isSubSet(candidate, nF))
//							isIn = true;
//							//break;
//						//nned to improve here
//					}
				if (!isIn)
				W.push_back(nF);
			}
		} else {
			division_.setFences(F);
			return;
		}
		//if feasible with F , else we have a counter example
	}

}

private:
bool isSubSet(boost::unordered_set<State *> s,
		boost::unordered_set<State *> S) {
	bool isSubSet = true;
	//std::vector<State *>::iterator next;

	for (auto next = s.begin(); next != s.end(); next++) {
		//foreach (State* state_, path)
		State *state = *next;
		if (S.find(state) == S.end()) {
			isSubSet = false;
			break; //improve
		}
	}

	//		foreach (State *state, s) {
	//			if (S.find(state) == S.end()) {
	//				isSubSet = false;
	//				break; //improve
	//			}
	//		}

	return isSubSet;
}

};

//end of class
}

FenceSet computeFences(const Program &program) {
std::vector<Division> divisions;
	// create all devisions
foreach (Thread *thread, program.threads()){
divisions.push_back(Division(program, thread));
}

	// create all devisionchecker & run thread parallel
boost::threadpool::pool pool(boost::thread::hardware_concurrency());

//boost::threadpool::pool pool(1);

foreach (Division &division, divisions){
pool.schedule(DivisionChecker(division));
}

pool.wait();

FenceSet result;

	//Fence set is the union of minimal fence set for each thread

foreach (Division division, divisions){
foreach (State *state, division.fences() )
result.push_back(Fence(division.attacker() , state));
}

Statistics::instance().incFencesCount(result.size());

return result;

}

FenceSet preComputeFences(const Program &program, bool searchForTdrOnly) {

FenceSet result;
result = computeFences(program);
return result;

}

FenceSet computeFenceAbstraction(const Program &program, int loopNum,
int fenceType) {

std::vector<Division> divisions;
	// create all devisions
foreach (Thread *thread, program.threads()){
divisions.push_back(Division(program, thread));
}

	// create all devisionchecker & run thread parallel
boost::threadpool::pool pool(boost::thread::hardware_concurrency());

//boost::threadpool::pool pool(1);

foreach (Division &division, divisions){
pool.schedule(DivisionAbstractionChecker(division, loopNum, fenceType));
}

pool.wait();

FenceSet result;

//Fence set is the union of minimal fence set for each thread
foreach (Division division, divisions){
foreach (State *state, division.fences() )
result.push_back(Fence(division.attacker() , state));
}

Statistics::instance().incFencesCount(result.size());

return result;

}

FenceSet preComputeFenceAbstraction(const Program &program,
bool searchForTdrOnly, int loopNum, int fenceType) {
FenceSet result;
result = computeFenceAbstraction(program, loopNum, fenceType);
return result;
}

}

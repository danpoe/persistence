/*
 * Thread.cpp
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

#include "Thread.h"

#include "Foreach.h"
#include "State.h"
#include "Transition.h"

namespace wmm {

Thread::Thread(const std::string &name) :
		name_(name), initialState_(NULL) {
}

Thread::~Thread() {
	foreach (Transition *transition, transitions_) {
		delete transition;
	}
}

State *Thread::makeState(const std::string &name) {
	auto &result = name2state_[name];
	if (!result) {
		result.reset(new State(name));
		states_.push_back(result.get());
	}
	return result.get();
}

Transition *Thread::makeTransition(State *from, State *to,
		const std::shared_ptr<Instruction> &instruction, int original,
		bool isNotPrecise) {
	std::unique_ptr<Transition> result(
			new Transition(from, to, instruction, transitions_.size(), original,
					isNotPrecise));
	transitions_.push_back(result.get());

	from->out_.push_back(result.get());
	to->in_.push_back(result.get());

	return result.release();
}

Transition *Thread::makeTransition(State *from, State *to,
		const std::shared_ptr<Instruction> &instruction, bool isNotPrecise) {
	std::unique_ptr<Transition> result(
			new Transition(from, to, instruction, transitions_.size(),
					transitions_.size(), isNotPrecise));
	transitions_.push_back(result.get());

	from->out_.push_back(result.get());
	to->in_.push_back(result.get());

	return result.release();
}

void Thread::deleteTransition(int ID) {
	unsigned index = 0;

	foreach (Transition *trans, transitions_) {
		if (trans->getID() != ID) {
			index = index + 1;
		} else
			break;
	}

	if (index < transitions_.size()) {
		transitions_.erase(transitions_.begin() + index);
	}
}

void Thread::deleteState(State *state) {
	unsigned index = 0;

	foreach (State *state_, states_) {
		if (state_ != state) {
			index = index + 1;
		} else
			break;
	}

	if (index < states_.size()) {
		states_.erase(states_.begin() + index);
	}
}

}

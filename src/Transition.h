/*
 * Transition.h
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

#include <memory>

namespace wmm {

class State;
class Instruction;

class Transition {
	State *from_;
	State *to_;
	std::shared_ptr<Instruction> instruction_;
	int id_, original_;
	bool isNotPrecise_;

public:

	Transition(State *from, State *to,
			const std::shared_ptr<Instruction> &instruction, int id,
			int original, bool isNotPrecise);
	~Transition();

	State *from() const {
		return from_;
	}
	State *to() const {
		return to_;
	}
	const std::shared_ptr<Instruction> &instruction() const {
		return instruction_;
	}

	int getID() const {
		return id_;
	}

	void setID(int id) {
		id_ = id;
	}

	int getOriginal() const {
		return original_;
	}

	void setOriginal(int original) {
		original_ = original;
	}

	bool getIsNotPrecise() const {
		return isNotPrecise_;
	}

	void setIsNotPrecise(bool value) {
		isNotPrecise_ = value;
	}

};

}

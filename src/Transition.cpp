/*
 * Transition.cpp
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

#include "Transition.h"

#include <cassert>

#include "Instruction.h"

namespace wmm {

Transition::Transition(State *from, State *to,
		const std::shared_ptr<Instruction> &instruction, int id, int original,
		bool isNotPrecise) :
		from_(from), to_(to), instruction_(instruction), id_(id), original_(
				original), isNotPrecise_(isNotPrecise) {
	assert(from);
	assert(to);
	assert(instruction);
}

Transition::~Transition() {
}

}

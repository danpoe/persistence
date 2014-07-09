/*
 * Reduction.h
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

#include "config.h"

#include <boost/unordered_set.hpp>

namespace wmm {

class Program;
class State;
class Thread;
class Transition;

// For -p, -f, -fa 1 x
void reduce(const Program &program, Program &result, bool searchForTdrOnly,
		Thread *attacker = NULL, Transition *attackWrite = NULL,
		Transition *attackRead = NULL,
		const boost::unordered_set<State *> &fenced = boost::unordered_set<
				State *>());

// For -fa 2 x because of performace
void reduce2(const Program &program, Program &result, bool searchForTdrOnly,
		Thread *attacker = NULL, Transition *attackWrite = NULL,
		Transition *attackRead = NULL,
		const boost::unordered_set<State *> &fenced = boost::unordered_set<
				State *>());

}

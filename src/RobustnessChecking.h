/*
 * RobustnessChecking.h
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

bool isAttackFeasible(const Program &program, bool searchForTdrOnly,
                      Thread *attacker = NULL, Transition *attackWrite = NULL, Transition *attackRead = NULL,
		      const boost::unordered_set<State *> &fenced = boost::unordered_set<State *>());

boost::unordered_set<State *> isAttackFeasible(const Program &program, Thread *attacker = NULL,
		      const boost::unordered_set<State *> &fenced = boost::unordered_set<State *>());

//boost::unordered_set<State *> isAttackFeasibleAbstractionNew(const Program &program, Thread *attacker = NULL,
//		int loopNum=0, const boost::unordered_set<State *> &fenced = boost::unordered_set<State *>());

boost::unordered_set<State *> isAttackFeasibleAbstraction1(const Program &program, Thread *attacker = NULL,
		int loopNum=0, const boost::unordered_set<State *> &fenced = boost::unordered_set<State *>());

boost::unordered_set<State *> isAttackFeasibleAbstraction2(const Program &program, Thread *attacker = NULL,
		int loopNum=0, const boost::unordered_set<State *> &fenced = boost::unordered_set<State *>());

//boost::unordered_set<State *> isAttackFeasibleAbstractionNew3(const Program &program, Thread *attacker = NULL,
//		int loopNum=0, const boost::unordered_set<State *> &fenced = boost::unordered_set<State *>());
//
//bool isAttackFeasibleAbstraction(const Program &program, bool searchForTdrOnly, int loopNum,
//                      Thread *attacker = NULL, Transition *attackWrite = NULL, Transition *attackRead = NULL,
//		      const boost::unordered_set<State *> &fenced = boost::unordered_set<State *>());

bool isReachable(State *state, State *target, boost::unordered_set<State *> &visited);

}



/*
 * Reduction.cpp
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

#include "Reduction.h"

#include <algorithm>
#include <cassert>

#include "Expression.h"
#include "Foreach.h"
#include "Instruction.h"
#include "Program.h"
#include "State.h"
#include "Transition.h"

// we need to change size of space
// we need to add space for storedAddVar
// try to check with value from instruction

namespace wmm {

// For fence insertion, checking persistence, and abstraction 1
void reduce(const Program &program, Program &resultProgram,
		bool searchForTdrOnly, Thread *attacker, Transition *attackWrite,
		Transition *attackRead, const boost::unordered_set<State *> &fenced) {
	assert(attackWrite == NULL || attackWrite->instruction()->is<Write>());
	assert(attackRead == NULL || attackRead->instruction()->is<Read>());

	//resultProgram.setMemorySize(std::max(program.memorySize(), 1)); //phaseVar, storedAddVar, suc,

	resultProgram.setIntSpaceSize(2);
	resultProgram.setBoolSpaceSize(1);

	enum {
		DEFAULT_SPACE = Space(), INTEGER_SPACE, BOOL_SPACE
	};

	/*
	 * The pattern we catch: \pi1 (phase0), w1u1+UNLOCK (after here: phase1), w2u2 (after here: phase2), \pi2 t2 (after here - phase 3)
	 * \pi2 contains nop and read of p
	 * the pattern is in SC
	 *
	 * For TSO process:
	 *  write:
	 *  	+ ph==0, normal
	 *  	+ ph==1 && its address != storedAddVar, write ph=2
	 *  read:
	 *  	+ ph==0 || (ph==2 && address != x), normal
	 *  	+ ph==2 && its address = storeAddVar, suc=1
	 *  nop:
	 *  	+ ph==0 || ph==2, normal
	 *  other:
	 *  	+ ph==0, normal
	 *
	 *  For SC process: NOTE: LOCK AND ULOCK
	 *	write:
	 *		+ ph==0, normal
	 *		+ change ph==0 to ph=1, store x to storedAddVar, UNLOCK (sometime after write opeations we need UNLOCK)
	 *	other:
	 *		+ ph==0, normal
	 */

	//const std::shared_ptr<Constant>  minus = resultProgram.makeConstant(-1);
	const std::shared_ptr<Constant> zero = resultProgram.makeConstant(0);
	const std::shared_ptr<Constant> one = resultProgram.makeConstant(1);
	const std::shared_ptr<Constant> two = resultProgram.makeConstant(2);
	//const std::shared_ptr<Constant> three = resultProgram.makeConstant(3);

	const std::shared_ptr<Constant> storedAddVar = resultProgram.makeConstant(
			0); // in INTEGER_SPACE
	const std::shared_ptr<Constant> phaseVar = resultProgram.makeConstant(1); // in INTEGER_SPACE
	const std::shared_ptr<Constant> successVar = resultProgram.makeConstant(0);	// in BOOL_SPACE

	resultProgram.setInterestingAddress(successVar->value(), BOOL_SPACE);

	const std::shared_ptr<Register> storedAdd(
			resultProgram.makeRegister("_storeAdd"));
	const std::shared_ptr<Register> phase(resultProgram.makeRegister("_phase"));
	const std::shared_ptr<Register> value(resultProgram.makeRegister("_value"));

	const std::shared_ptr<Condition> check_not_blocked(
			new Condition(std::make_shared<NotBlocked>()));

	if (attacker == NULL) {

		foreach (Thread *thread, program.threads()){

		Thread *resultThread = resultProgram.makeThread(thread->name());
		if (thread->initialState())
		resultThread->setInitialState(
				resultThread->makeState(
						"orig_" + thread->initialState()->name()));

		foreach (Transition *transition, thread->transitions()) {

			State *originalFrom = resultThread->makeState(
					"orig_" + transition->from()->name());
			State *originalTo = resultThread->makeState(
					"orig_" + transition->to()->name());
			State *attackerFrom = resultThread->makeState(
					"att_" + transition->from()->name());
			State *attackerTo = resultThread->makeState(
					"att_" + transition->to()->name());

			if (Write *write = transition->instruction()->as<Write>()) {

				// phase 0
				resultThread->makeTransition(originalFrom, originalTo,
						std::make_shared < Atomic
						> (check_not_blocked,
								std::make_shared
								< Read > (phase, phaseVar, INTEGER_SPACE),
								std::make_shared< Condition> (std::make_shared < BinaryOperator> (BinaryOperator::EQ, phase, zero)),
								transition->instruction()),
						transition->getID(), transition->getIsNotPrecise());

				// phase 0 to 1
				resultThread->makeTransition(originalFrom, resultThread->makeState("end"),
						std::make_shared < Atomic
						> (check_not_blocked,
								std::make_shared
								< Read > (phase, phaseVar, INTEGER_SPACE),
								std::make_shared < Condition> (std::make_shared < BinaryOperator> (BinaryOperator::EQ, phase, zero)),
								std::make_shared< Read > (value, write->address(), DEFAULT_SPACE),
								std::make_shared< Condition> (
										std::make_shared < BinaryOperator> (BinaryOperator::NEQ, value, write->value())),
								std::make_shared< Write> (one, phaseVar, INTEGER_SPACE),
								std::make_shared< Write > (write->address(), storedAddVar, INTEGER_SPACE),
								transition->instruction(),
								std::make_shared<Unlock>()
						),
						transition->getID(), transition->getIsNotPrecise());

				if (fenced.find(transition->from()) != fenced.end()) {
					/* No transition from an extra fenced state. */
				} else {

					//phase 1 to 2
					resultThread->makeTransition(originalFrom, attackerTo,
							std::make_shared < Atomic
							> (check_not_blocked,
									std::make_shared< Read > (phase, phaseVar, INTEGER_SPACE),
									std::make_shared < Condition> (std::make_shared < BinaryOperator> (BinaryOperator::EQ, phase, one)),
									std::make_shared< Read > (storedAdd, storedAddVar, INTEGER_SPACE),
									std::make_shared < Condition> (std::make_shared < BinaryOperator> (BinaryOperator::NEQ, storedAdd, write->address())),
									std::make_shared< Write> (two, phaseVar, INTEGER_SPACE), transition->instruction()),
							transition->getID(), transition->getIsNotPrecise());

				}

			} else if (Read *read = transition->instruction()->as<Read>()) {

				// phase 0
				resultThread->makeTransition(originalFrom, originalTo,
						std::make_shared < Atomic
						> (check_not_blocked,
								std::make_shared< Read > (phase, phaseVar, INTEGER_SPACE),
								std::make_shared< Condition> (
										std::make_shared < BinaryOperator> (BinaryOperator::EQ, phase, zero)),
								transition->instruction()),
						transition->getID(), transition->getIsNotPrecise());

				if (fenced.find(transition->from()) != fenced.end()) {
					/* No transition from an extra fenced state. */
				} else {

					// phase 2
					resultThread->makeTransition(attackerFrom, attackerTo,
							std::make_shared < Atomic
							> (check_not_blocked,
									std::make_shared< Read > (phase, phaseVar, INTEGER_SPACE),
									std::make_shared< Read > (storedAdd, storedAddVar, INTEGER_SPACE),
									std::make_shared< Condition> (std::make_shared
											< BinaryOperator> (BinaryOperator::AND,
													std::make_shared < BinaryOperator> (BinaryOperator::EQ, phase, two),
													std::make_shared < BinaryOperator> (BinaryOperator::NEQ, storedAdd, read->address())
											)),
//															std::make_shared< Condition> (
//																	std::make_shared < BinaryOperator>
//																	(BinaryOperator::NEQ, storedAdd, read->address())),
									transition->instruction()),
							transition->getID(), transition->getIsNotPrecise());

					// phase 2 to end
					resultThread->makeTransition(attackerFrom, resultThread->makeState("final"),
							std::make_shared < Atomic> (check_not_blocked,
									std::make_shared< Read > (phase, phaseVar, INTEGER_SPACE),
									std::make_shared< Read > (storedAdd, storedAddVar, INTEGER_SPACE),
									std::make_shared< Condition> (std::make_shared
											< BinaryOperator> (BinaryOperator::AND,
													std::make_shared < BinaryOperator> (BinaryOperator::EQ, phase, two),
													std::make_shared < BinaryOperator> (BinaryOperator::EQ, storedAdd, read->address()))),
//															std::make_shared< Condition> (
//																	std::make_shared < BinaryOperator>
//																	(BinaryOperator::EQ, storedAdd, read->address())),
									//std::make_shared< Write > (three, phaseVar, INTEGER_SPACE),
									std::make_shared< Write> (one, successVar, BOOL_SPACE)),
							transition->getID(), transition->getIsNotPrecise());

				}

			} else if (transition->instruction()->as<Local>()
					|| transition->instruction()->as<Condition>()
					|| transition->instruction()->as<Noop>()) {

				if (fenced.find(transition->from()) != fenced.end()) {
					/* No transition from an extra fenced state. */
				} else {
					// phase 1
					resultThread->makeTransition(attackerFrom, attackerTo,
							std::make_shared < Atomic
							> (check_not_blocked, std::make_shared
									< Read > (phase, phaseVar, INTEGER_SPACE), std::make_shared
									< Condition
									> (std::make_shared < BinaryOperator
											> (BinaryOperator::EQ, phase, one)), transition->instruction()),
							transition->getID(), transition->getIsNotPrecise());

				}

				// phase 0

				resultThread->makeTransition(originalFrom, originalTo,
						std::make_shared < Atomic
						> (//check_not_blocked,
								std::make_shared
								< Read > (phase, phaseVar, INTEGER_SPACE), std::make_shared
								< Condition
								> (std::make_shared < BinaryOperator
										> (BinaryOperator::EQ, phase, zero)),
								transition->instruction()),
				//transition->instruction(),
						transition->getID(), transition->getIsNotPrecise());

				if (fenced.find(transition->from()) != fenced.end()) {
					/* No transition from an extra fenced state. */
				} else {
					// phase 2 to 3
					resultThread->makeTransition(attackerFrom, attackerTo,
							std::make_shared < Atomic
							> (//check_not_blocked,
									std::make_shared
									< Read > (phase, phaseVar, INTEGER_SPACE), std::make_shared
									< Condition
									> (std::make_shared < BinaryOperator
											> (BinaryOperator::EQ, phase, two)), transition->instruction(),
									transition->instruction()),
							transition->getID(), transition->getIsNotPrecise());
				}

			} else {
				// phase 0

				resultThread->makeTransition(originalFrom, originalTo,
						std::make_shared < Atomic
						> (//check_not_blocked,
								std::make_shared
								< Read > (phase, phaseVar, INTEGER_SPACE), std::make_shared
								< Condition
								> (std::make_shared < BinaryOperator
										> (BinaryOperator::EQ, phase, zero)),
								transition->instruction()),
				//transition->instruction(),
						transition->getID(), transition->getIsNotPrecise());

			}
		}
	}
} else {
	// This case we know attacker and helpers
	// Note: we do not need to know which pairs of write and read
	foreach (Thread *thread, program.threads()) {
		Thread *resultThread = resultProgram.makeThread(thread->name());
		if (thread->initialState())
		resultThread->setInitialState(
				resultThread->makeState(
						"orig_" + thread->initialState()->name()));

		/*
		 * Instrumentation for the helpers:
		 */

		if (thread->name() != attacker->name()) { //helpers
			foreach (Transition *transition, thread->transitions()) {

				State *originalFrom = resultThread->makeState(
						"orig_" + transition->from()->name());
				State *originalTo = resultThread->makeState(
						"orig_" + transition->to()->name());

				if (Write *write = transition->instruction()->as<Write>()) {
					// phase 0
					resultThread->makeTransition(originalFrom, originalTo,
							std::make_shared < Atomic
							> (check_not_blocked,
//									std::make_shared
//									< Read > (phase, phaseVar, INTEGER_SPACE),
//									std::make_shared< Condition> (std::make_shared < BinaryOperator> (BinaryOperator::EQ, phase, zero)),
									transition->instruction()),
							transition->getID(), transition->getIsNotPrecise());

					// phase 0 to 1
					resultThread->makeTransition(originalFrom, resultThread->makeState("end"),
							std::make_shared < Atomic
							> (check_not_blocked,
//									std::make_shared
//									< Read > (phase, phaseVar, INTEGER_SPACE),
//									std::make_shared < Condition> (std::make_shared < BinaryOperator> (BinaryOperator::EQ, phase, zero)),
									std::make_shared< Read > (value, write->address(), DEFAULT_SPACE),
									std::make_shared< Condition> (
											std::make_shared < BinaryOperator> (BinaryOperator::NEQ, value, write->value())),
									std::make_shared< Write> (one, phaseVar, INTEGER_SPACE),
									std::make_shared< Write > (write->address(), storedAddVar, INTEGER_SPACE),
									transition->instruction(),
									std::make_shared<Unlock>()
							),
							transition->getID(), transition->getIsNotPrecise());
				} else if (transition->instruction()->as<Read>()) {

					// phase 0
					resultThread->makeTransition(originalFrom, originalTo,
							std::make_shared < Atomic
							> (check_not_blocked,
//									std::make_shared< Read > (phase, phaseVar, INTEGER_SPACE),
//									std::make_shared< Condition> (
//											std::make_shared < BinaryOperator> (BinaryOperator::EQ, phase, zero)),
									transition->instruction()),
							transition->getID(), transition->getIsNotPrecise());
				} else {
					// phase 0
					resultThread->makeTransition(originalFrom, originalTo,
							//std::make_shared < Atomic
							//> (//check_not_blocked,
//									std::make_shared< Read > (phase, phaseVar, INTEGER_SPACE),
//									std::make_shared< Condition> (
//											std::make_shared < BinaryOperator> (BinaryOperator::EQ, phase, zero)),
							//transition->instruction()),
							transition->instruction(),
							transition->getID(), transition->getIsNotPrecise());
				}
			}
		} else { // Instrument for attacker
			foreach (Transition *transition, thread->transitions()) {

				State *originalFrom = resultThread->makeState(
						"orig_" + transition->from()->name());
				State *originalTo = resultThread->makeState(
						"orig_" + transition->to()->name());
				State *attackerFrom = resultThread->makeState(
						"att_" + transition->from()->name());
				State *attackerTo = resultThread->makeState(
						"att_" + transition->to()->name());

				if (Write *write = transition->instruction()->as<Write>()) {
					// phase 0
					resultThread->makeTransition(originalFrom, originalTo,
							std::make_shared < Atomic
							> (check_not_blocked,
									std::make_shared
									< Read > (phase, phaseVar, INTEGER_SPACE), std::make_shared
									< Condition
									> (std::make_shared < BinaryOperator> (BinaryOperator::EQ, phase, zero)),
									transition->instruction()),
							transition->getID(), transition->getIsNotPrecise());

					if (fenced.find(transition->from()) != fenced.end()) {
						/* No transition from an extra fenced state. */
					} else {
						State *barState = resultThread->makeState("bar_" + transition->from()->name());

						resultThread->makeTransition(originalFrom, barState,
								std::make_shared < Atomic> ( //check_not_blocked,
										std::make_shared< Read > (phase, phaseVar, INTEGER_SPACE),
										std::make_shared< Condition> (
												std::make_shared < BinaryOperator> (BinaryOperator::EQ, phase, one))),
								-1, transition->getIsNotPrecise());
						//phase 1 to 2
						resultThread->makeTransition(barState, attackerTo,
								std::make_shared < Atomic
								> (check_not_blocked,
//										std::make_shared< Read > (phase, phaseVar, INTEGER_SPACE),
//										std::make_shared < Condition> (std::make_shared < BinaryOperator> (BinaryOperator::EQ, phase, one)),
										std::make_shared< Read > (storedAdd, storedAddVar, INTEGER_SPACE),
										std::make_shared < Condition> (std::make_shared < BinaryOperator> (BinaryOperator::NEQ, storedAdd, write->address())),
										std::make_shared< Write> (two, phaseVar, INTEGER_SPACE), transition->instruction()),
								transition->getID(), transition->getIsNotPrecise());
					}

				} else if (Read *read = transition->instruction()->as<Read>()) {
					// phase 0
					resultThread->makeTransition(originalFrom, originalTo,
							std::make_shared < Atomic
							> (check_not_blocked,std::make_shared
									< Read > (phase, phaseVar, INTEGER_SPACE), std::make_shared
									< Condition
									> (std::make_shared < BinaryOperator
											> (BinaryOperator::EQ, phase, zero)),
									transition->instruction()),
							transition->getID(), transition->getIsNotPrecise());

					if (fenced.find(transition->from()) != fenced.end()) {
						/* No transition from an extra fenced state. */
					} else {
						// phase 2
						resultThread->makeTransition(attackerFrom, attackerTo,
								std::make_shared < Atomic
								> (check_not_blocked,
										//std::make_shared< Read > (phase, phaseVar, INTEGER_SPACE),
										std::make_shared< Read > (storedAdd, storedAddVar, INTEGER_SPACE),
//										std::make_shared< Condition> (std::make_shared
//												< BinaryOperator> (BinaryOperator::AND,
//														std::make_shared < BinaryOperator> (BinaryOperator::EQ, phase, two),
//														std::make_shared < BinaryOperator> (BinaryOperator::NEQ, storedAdd, read->address())
//												)),
										std::make_shared< Condition> (
												std::make_shared < BinaryOperator>
												(BinaryOperator::NEQ, storedAdd, read->address())),
										transition->instruction()),
								transition->getID(), transition->getIsNotPrecise());

						// phase 2 to end
						resultThread->makeTransition(attackerFrom, resultThread->makeState("final"),
								std::make_shared < Atomic> (check_not_blocked,
										//std::make_shared< Read > (phase, phaseVar, INTEGER_SPACE),
										std::make_shared< Read > (storedAdd, storedAddVar, INTEGER_SPACE),
//										std::make_shared< Condition> (std::make_shared
//												< BinaryOperator> (BinaryOperator::AND,
//														std::make_shared < BinaryOperator> (BinaryOperator::EQ, phase, two),
//														std::make_shared < BinaryOperator> (BinaryOperator::EQ, storedAdd, read->address()))),
										std::make_shared< Condition> (
												std::make_shared < BinaryOperator>
												(BinaryOperator::EQ, storedAdd, read->address())),
										//std::make_shared< Write > (three, phaseVar, INTEGER_SPACE),
										std::make_shared< Write> (one, successVar, BOOL_SPACE)),
								transition->getID(), transition->getIsNotPrecise());
					}

				} else if (transition->instruction()->as<Local>()
						|| transition->instruction()->as<Condition>()
						|| transition->instruction()->as<Noop>()) {
					// phase 0

					resultThread->makeTransition(originalFrom, originalTo,
							std::make_shared < Atomic
							> (//check_not_blocked,
									std::make_shared
									< Read > (phase, phaseVar, INTEGER_SPACE), std::make_shared
									< Condition
									> (std::make_shared < BinaryOperator
											> (BinaryOperator::EQ, phase, zero)),
									transition->instruction()),
							//transition->instruction(),
							transition->getID(), transition->getIsNotPrecise());

					if (fenced.find(transition->from()) != fenced.end()) {
						/* No transition from an extra fenced state. */
					} else {
						// phase 2
						resultThread->makeTransition(attackerFrom, attackerTo,
//								std::make_shared < Atomic
//								> (//check_not_blocked,
//										std::make_shared
//										< Read > (phase, phaseVar, INTEGER_SPACE), std::make_shared
//										< Condition
//										> (std::make_shared < BinaryOperator
//												> (BinaryOperator::EQ, phase, two)), transition->instruction()),
								transition->instruction(),
								transition->getID(), transition->getIsNotPrecise());
					}
				} else {
//					// phase 0
//					resultThread->makeTransition(originalFrom, originalTo,
//							transition->instruction(), transition->getID(),
//							transition->getIsNotPrecise());

					// phase 0

					resultThread->makeTransition(originalFrom, originalTo,
							std::make_shared < Atomic
							> (//check_not_blocked,
									std::make_shared
									< Read > (phase, phaseVar, INTEGER_SPACE), std::make_shared
									< Condition
									> (std::make_shared < BinaryOperator
											> (BinaryOperator::EQ, phase, zero)),
									transition->instruction()),
							//transition->instruction(),
							transition->getID(), transition->getIsNotPrecise());
				}

			}

		}
	}
}
}

// For abtraction 2 because of faster performance
void reduce2(const Program &program, Program &resultProgram,
		bool searchForTdrOnly, Thread *attacker, Transition *attackWrite,
		Transition *attackRead, const boost::unordered_set<State *> &fenced) {
	assert(attackWrite == NULL || attackWrite->instruction()->is<Write>());
	assert(attackRead == NULL || attackRead->instruction()->is<Read>());

	resultProgram.setMemorySize(std::max(program.memorySize(), 4)); //barVar, suc, wAVar, canUpdateVar

	enum {
		DEFAULT_SPACE = Space(), BUFFER_SPACE, WA_SPACE, SERVICE_SPACE
	};

	const std::shared_ptr<Constant> zero = resultProgram.makeConstant(0);
	const std::shared_ptr<Constant> one = resultProgram.makeConstant(1);

	const std::shared_ptr<Constant> wAVar = resultProgram.makeConstant(0);
	const std::shared_ptr<Constant> barVar = resultProgram.makeConstant(1);
	const std::shared_ptr<Constant> canUpdateVar = resultProgram.makeConstant(
			2);
	const std::shared_ptr<Constant> successVar = resultProgram.makeConstant(3);

	resultProgram.setInterestingAddress(successVar->value(), SERVICE_SPACE);

	const std::shared_ptr<Register> wA(resultProgram.makeRegister("_wA"));
	const std::shared_ptr<Register> snapshot(
			resultProgram.makeRegister("_snapshot"));
	const std::shared_ptr<Register> wAttackerFlag(
			resultProgram.makeRegister("_wAttackerFlag"));
	const std::shared_ptr<Register> share(resultProgram.makeRegister("_share"));

	const std::shared_ptr<Condition> check_not_blocked(
			new Condition(std::make_shared<NotBlocked>()));

	const std::shared_ptr<Snapshort> snapshort(new Snapshort());

	if (attacker == NULL) { // this case: attackWrite = attackRead = NULL
		//We build a full graph to check robustness
		// each thread can be attacker or helper
		// each read and write of attacker can be bar_A and r_A

		foreach (Thread *thread, program.threads()){

		Thread *resultThread = resultProgram.makeThread(thread->name());
		if (thread->initialState())
		resultThread->setInitialState(
				resultThread->makeState(
						"orig_" + thread->initialState()->name()));

		foreach (Transition *transition, thread->transitions()) {

			/*
			 * Instrumentation for the attacker:
			 * We have 3 sections: before bar_A, bar_A, after bar_A and before w_A, w_A, after w_A
			 * before bar_A: reads and writes to shared mem
			 * after bar_A: reads and writes to snapshot
			 *
			 * Instrumentation for the helpers:
			 * Before bar_A, helper reads and writes to shared mem, but not check oW. After bar_A, helper need check oW
			 */

			/*
			 * before bar_A: all thread are in SC and every thread can become attacker, then others become helpers
			 */

			State *originalFrom = resultThread->makeState(
					"orig_" + transition->from()->name());
			State *originalTo = resultThread->makeState(
					"orig_" + transition->to()->name());

			//For both attacker before bar_A, and helpers
			if (transition->instruction()->is<Read>()) {
				resultThread->makeTransition(originalFrom, originalTo,
						std::make_shared < Atomic
						> (check_not_blocked, transition->instruction()),
						transition->getID(), transition->getIsNotPrecise());
			} else if (transition->instruction()->as<Write>()) {
				resultThread->makeTransition(originalFrom, originalTo,
						std::make_shared < Atomic
						> (check_not_blocked, std::make_shared
								< Read > (wA, wAVar, SERVICE_SPACE), std::make_shared
								< Condition
								> (std::make_shared < BinaryOperator
										> (BinaryOperator::EQ, wA, zero)), transition->instruction()),
						transition->getID(), transition->getIsNotPrecise());

			} else {
				resultThread->makeTransition(originalFrom, originalTo,
						transition->instruction(), transition->getID(),
						transition->getIsNotPrecise());
			}

			//The following code is only for attacker
			State *attackerFrom = resultThread->makeState(
					"att_" + transition->from()->name());
			State *attackerTo = resultThread->makeState(
					"att_" + transition->to()->name());

			/*
			 * Find bar_A
			 */
			if (Write *write = transition->instruction()->as<Write>()) {
				resultThread->makeTransition(originalFrom,
						resultThread->makeState(
								"bar_" + transition->from()->name()),
						std::make_shared < Atomic > (
								//check_not_blocked,
								std::make_shared < Read
								> (share, barVar, SERVICE_SPACE), std::make_shared
								< Condition
								> (std::make_shared < BinaryOperator
										> (BinaryOperator::EQ, share, zero)), snapshort, std::make_shared
								< Write
								> (one, barVar, SERVICE_SPACE)), -1,
						transition->getIsNotPrecise());

				//do not update to memory, only update to buffer and WA
				resultThread->makeTransition(
						resultThread->makeState(
								"bar_" + transition->from()->name()),
						resultThread->makeState(
								"att_" + transition->to()->name()),
						std::make_shared < Atomic
						> (check_not_blocked, std::make_shared
								< Write
								> (write->value(), write->address(), BUFFER_SPACE), std::make_shared
								< Write
								> (one, write->address(), WA_SPACE), std::make_shared
								< Write
								> (zero, canUpdateVar, SERVICE_SPACE)
						),
						transition->getID(), transition->getIsNotPrecise());

				//update to buffer, WA, and mem, but do not set wA
				resultThread->makeTransition(
						resultThread->makeState(
								"bar_" + transition->from()->name()),
						resultThread->makeState(
								"att_" + transition->to()->name()),
						std::make_shared < Atomic
						> (check_not_blocked, std::make_shared
								< Write
								> (write->value(), write->address(), BUFFER_SPACE), std::make_shared
								< Write
								> (write->value(), write->address(), DEFAULT_SPACE), std::make_shared
								< Write
								> (one, write->address(), WA_SPACE), std::make_shared
								< Write
								> (one, canUpdateVar, SERVICE_SPACE)),
						transition->getID(), transition->getIsNotPrecise());

				//update to buffer, WA, and mem, and set wA
				resultThread->makeTransition(
						resultThread->makeState(
								"bar_" + transition->from()->name()),
						resultThread->makeState(
								"att_" + transition->to()->name()),
						std::make_shared < Atomic
						> (check_not_blocked, std::make_shared
								< Write
								> (write->value(), write->address(), BUFFER_SPACE), std::make_shared
								< Write
								> (write->value(), write->address(), DEFAULT_SPACE), std::make_shared
								< Write
								> (one, write->address(), WA_SPACE), std::make_shared
								< Write
								> (one, canUpdateVar, SERVICE_SPACE), std::make_shared
								< Write
								> (one, wAVar, SERVICE_SPACE)),
						transition->getID(), transition->getIsNotPrecise());

			}

			/*
			 * Attacker's execution.
			 */
			if (fenced.find(transition->from()) != fenced.end()) {
				/* No transition from an extra fenced state. */
			} else if (Write *write =
					transition->instruction()->as<Write>()) {
				if (!searchForTdrOnly) {
					//do not update to memory, only update to buffer and WA
					resultThread->makeTransition(attackerFrom, attackerTo,
							std::make_shared < Atomic
							> (check_not_blocked, std::make_shared
									< Write
									> (write->value(), write->address(), BUFFER_SPACE), std::make_shared
									< Write
									> (one, write->address(), WA_SPACE), std::make_shared
									< Write
									> (zero, canUpdateVar, SERVICE_SPACE)),
							transition->getID(),
							transition->getIsNotPrecise());

					//update to buffer, WA, and mem, but do not set wA
					resultThread->makeTransition(attackerFrom, attackerTo,
							std::make_shared < Atomic
							> (check_not_blocked,
									std::make_shared < Read > (share, canUpdateVar, SERVICE_SPACE),
									std::make_shared < Condition> (std::make_shared < BinaryOperator> (BinaryOperator::EQ, share, one)),

									std::make_shared
									< Write
									> (write->value(), write->address(), BUFFER_SPACE), std::make_shared
									< Write
									> (one, write->address(), WA_SPACE),

									std::make_shared
									< Write
									> (write->value(), write->address(), DEFAULT_SPACE), std::make_shared
									< Write
									> (one, canUpdateVar, SERVICE_SPACE)),
							transition->getID(),
							transition->getIsNotPrecise());

					//update to buffer, WA, and mem, and set wA
					resultThread->makeTransition(attackerFrom, attackerTo,
							std::make_shared < Atomic
							> (check_not_blocked,
									std::make_shared < Read > (share, canUpdateVar, SERVICE_SPACE),
									std::make_shared < Condition> (std::make_shared < BinaryOperator> (BinaryOperator::EQ, share, one)),

									std::make_shared
									< Write
									> (write->value(), write->address(), BUFFER_SPACE), std::make_shared
									< Write
									> (one, write->address(), WA_SPACE),

									std::make_shared
									< Write
									> (write->value(), write->address(), DEFAULT_SPACE), std::make_shared
									< Write
									> (one, canUpdateVar, SERVICE_SPACE), std::make_shared
									< Write
									> (one, wAVar, SERVICE_SPACE)),
							transition->getID(),
							transition->getIsNotPrecise());
				}
			} else if (Read *read = transition->instruction()->as<Read>()) {

				/* Reads either read from the buffer... */
				resultThread->makeTransition(attackerFrom, attackerTo,
						std::make_shared < Atomic > (
								//check_not_blocked,
								std::make_shared < Read
								> (read->reg(), read->address(), BUFFER_SPACE)),
						transition->getID(), transition->getIsNotPrecise());

				resultThread->makeTransition(attackerFrom,
						resultThread->makeState("final"),
						std::make_shared < Atomic
						> (check_not_blocked, std::make_shared
								< Read
								> (snapshot, read->address(), BUFFER_SPACE), std::make_shared
								< Read
								> (share, read->address(), DEFAULT_SPACE), std::make_shared
								< Read
								> (wAttackerFlag, read->address(), WA_SPACE),

								//we need to check wA==1 here --> it is a new bug in previous version
								std::make_shared < Read > (wA, wAVar, SERVICE_SPACE),
								std::make_shared < Condition> (
										std::make_shared < BinaryOperator> ( BinaryOperator::AND,
												std::make_shared
												< BinaryOperator> (BinaryOperator::AND,
														std::make_shared < BinaryOperator> (BinaryOperator::NEQ, share, snapshot),
														std::make_shared < BinaryOperator> (BinaryOperator::EQ, wAttackerFlag, zero)
												),

												std::make_shared < BinaryOperator> (BinaryOperator::EQ, wA, one))
								),

								std::make_shared< Write> (one, successVar, SERVICE_SPACE)),
						transition->getID(), transition->getIsNotPrecise());
			} else if (transition->instruction()->as<Mfence>()) {
				/* No transition: attacker can't execute fences. */
			} else if (transition->instruction()->as<Lock>()
					|| transition->instruction()->as<Unlock>()) {
				/* No transition: attacker can't execute locked instructions. */
			} else if (transition->instruction()->as<Local>()
					|| transition->instruction()->as<Condition>()
					|| transition->instruction()->as<Noop>()) {
				resultThread->makeTransition(attackerFrom, attackerTo,
						transition->instruction(), transition->getID(),
						transition->getIsNotPrecise());
			} else if (transition->instruction()->as<Atomic>()) {
				assert(
						!"Sorry, atomics in input programs are not supported.");
			} else {
				assert(!"NEVER REACHED");
			}
		}
	}
} else if (attackWrite != NULL && attackRead != NULL) { //in this case: we know the attacker, writeAttack, and readAttack
	foreach (Thread *thread, program.threads()) {
		Thread *resultThread = resultProgram.makeThread(thread->name());
		if (thread->initialState())
		resultThread->setInitialState(
				resultThread->makeState(
						"orig_" + thread->initialState()->name()));

		/*
		 * Instrumentation for the helpers:
		 * Before bar_A, helper reads and writes to shared mem, but not check oW. After bar_A, helper need check oW
		 */
		//if (thread->name().compare(attacker->name()) != 0 ) { //helpers
		if (thread->name() != attacker->name()) { //helpers
			foreach (Transition *transition, thread->transitions()) {
				State *originalFrom = resultThread->makeState(
						"orig_" + transition->from()->name());
				State *originalTo = resultThread->makeState(
						"orig_" + transition->to()->name());

				if (transition->instruction()->is<Read>()) {
					resultThread->makeTransition(originalFrom, originalTo,
							std::make_shared < Atomic
							> (check_not_blocked, transition->instruction()),
							transition->getID(),
							transition->getIsNotPrecise());
				} else if (transition->instruction()->as<Write>()) {
					resultThread->makeTransition(originalFrom, originalTo,
							std::make_shared < Atomic
							> (check_not_blocked, std::make_shared
									< Read
									> (wA, wAVar, SERVICE_SPACE), std::make_shared
									< Condition
									> (std::make_shared
											< BinaryOperator
											> (BinaryOperator::EQ, wA, zero)), transition->instruction()),
							transition->getID(),
							transition->getIsNotPrecise());
				} else {
					resultThread->makeTransition(originalFrom, originalTo,
							transition->instruction(), transition->getID(),
							transition->getIsNotPrecise());
				}
			}
		} else { // attacker
			/*
			 * Instrumentation for the attacker:
			 * We have 3 sections: before bar_A, bar_A, after bar_A and before w_A, w_A, after w_A
			 * before bar_A: reads and writes to shared mem
			 * after bar_A: reads and writes to snapshot
			 */
			foreach (Transition *transition, thread->transitions()) {
				State *originalFrom = resultThread->makeState(
						"orig_" + transition->from()->name());
				State *originalTo = resultThread->makeState(
						"orig_" + transition->to()->name());

				if (transition->instruction()->is<Read>()
						|| transition->instruction()->as<Write>()) {
					resultThread->makeTransition(originalFrom, originalTo,
							std::make_shared < Atomic
							> (check_not_blocked, transition->instruction()),
							transition->getID(),
							transition->getIsNotPrecise());
				} else {
					resultThread->makeTransition(originalFrom, originalTo,
							transition->instruction(), transition->getID(),
							transition->getIsNotPrecise());
				}

				State *attackerFrom = resultThread->makeState(
						"att_" + transition->from()->name());
				State *attackerTo = resultThread->makeState(
						"att_" + transition->to()->name());

				/*
				 * Find bar_A
				 */
				if (Write *write = transition->instruction()->as<Write>()) {
					if (transition->getOriginal()
							== attackWrite->getOriginal()) {
						resultThread->makeTransition(originalFrom,
								resultThread->makeState(
										"bar_"
										+ transition->from()->name()),
								std::make_shared < Atomic > (
										check_not_blocked,
										std::make_shared < Read
										> (share, barVar, SERVICE_SPACE), std::make_shared
										< Condition
										> (std::make_shared
												< BinaryOperator
												> (BinaryOperator::EQ, share, zero)), snapshort, std::make_shared
										< Write
										> (one, barVar, SERVICE_SPACE)),
								-1, transition->getIsNotPrecise());
						//do not update to memory, only update to buffer and WA
						resultThread->makeTransition(
								resultThread->makeState(
										"bar_" + transition->from()->name()),
								resultThread->makeState(
										"att_" + transition->to()->name()),
								std::make_shared < Atomic
								> (check_not_blocked, std::make_shared
										< Write
										> (write->value(), write->address(), BUFFER_SPACE), std::make_shared
										< Write
										> (one, write->address(), WA_SPACE), std::make_shared
										< Write
										> (zero, canUpdateVar, SERVICE_SPACE)
								),
								transition->getID(), transition->getIsNotPrecise());

						//update to buffer, WA, and mem, but do not set wA
						resultThread->makeTransition(
								resultThread->makeState(
										"bar_" + transition->from()->name()),
								resultThread->makeState(
										"att_" + transition->to()->name()),
								std::make_shared < Atomic
								> (check_not_blocked, std::make_shared
										< Write
										> (write->value(), write->address(), BUFFER_SPACE), std::make_shared
										< Write
										> (write->value(), write->address(), DEFAULT_SPACE), std::make_shared
										< Write
										> (one, write->address(), WA_SPACE), std::make_shared
										< Write
										> (one, canUpdateVar, SERVICE_SPACE)),
								transition->getID(), transition->getIsNotPrecise());

						//update to buffer, WA, and mem, and set wA
						resultThread->makeTransition(
								resultThread->makeState(
										"bar_" + transition->from()->name()),
								resultThread->makeState(
										"att_" + transition->to()->name()),
								std::make_shared < Atomic
								> (check_not_blocked, std::make_shared
										< Write
										> (write->value(), write->address(), BUFFER_SPACE), std::make_shared
										< Write
										> (write->value(), write->address(), DEFAULT_SPACE), std::make_shared
										< Write
										> (one, write->address(), WA_SPACE), std::make_shared
										< Write
										> (one, canUpdateVar, SERVICE_SPACE), std::make_shared
										< Write
										> (one, wAVar, SERVICE_SPACE)),
								transition->getID(), transition->getIsNotPrecise());
					}
				}

				/*
				 * Attacker's execution.
				 */

				if (fenced.find(transition->from()) != fenced.end()) {
					/* No transition from an extra fenced state. */
				} else if (Write *write = transition->instruction()->as<
						Write>()) {
					if (!searchForTdrOnly) {

						//do not update to memory, only update to buffer and WA
						resultThread->makeTransition(attackerFrom, attackerTo,
								std::make_shared < Atomic
								> (check_not_blocked, std::make_shared
										< Write
										> (write->value(), write->address(), BUFFER_SPACE), std::make_shared
										< Write
										> (one, write->address(), WA_SPACE), std::make_shared
										< Write
										> (zero, canUpdateVar, SERVICE_SPACE)),
								transition->getID(),
								transition->getIsNotPrecise());

						//update to buffer, WA, and mem, but do not set wA
						resultThread->makeTransition(attackerFrom, attackerTo,
								std::make_shared < Atomic
								> (check_not_blocked,
										std::make_shared < Read > (share, canUpdateVar, SERVICE_SPACE),
										std::make_shared < Condition> (std::make_shared < BinaryOperator> (BinaryOperator::EQ, share, one)),

										std::make_shared
										< Write
										> (write->value(), write->address(), BUFFER_SPACE), std::make_shared
										< Write
										> (one, write->address(), WA_SPACE),

										std::make_shared
										< Write
										> (write->value(), write->address(), DEFAULT_SPACE), std::make_shared
										< Write
										> (one, canUpdateVar, SERVICE_SPACE)),
								transition->getID(),
								transition->getIsNotPrecise());

						//update to buffer, WA, and mem, and set wA
						resultThread->makeTransition(attackerFrom, attackerTo,
								std::make_shared < Atomic
								> (check_not_blocked,
										std::make_shared < Read > (share, canUpdateVar, SERVICE_SPACE),
										std::make_shared < Condition> (std::make_shared < BinaryOperator> (BinaryOperator::EQ, share, one)),

										std::make_shared
										< Write
										> (write->value(), write->address(), BUFFER_SPACE), std::make_shared
										< Write
										> (one, write->address(), WA_SPACE),

										std::make_shared
										< Write
										> (write->value(), write->address(), DEFAULT_SPACE), std::make_shared
										< Write
										> (one, canUpdateVar, SERVICE_SPACE), std::make_shared
										< Write
										> (one, wAVar, SERVICE_SPACE)),
								transition->getID(),
								transition->getIsNotPrecise());
					}
				} else if (Read *read =
						transition->instruction()->as<Read>()) {
					/* Reads either read from the buffer... */
					resultThread->makeTransition(attackerFrom, attackerTo,
							std::make_shared < Atomic > (
									//check_not_blocked,
									std::make_shared < Read
									> (read->reg(), read->address(), BUFFER_SPACE)),
							transition->getID(),
							transition->getIsNotPrecise());

					if (transition->getOriginal()
							== attackRead->getOriginal()) {
						resultThread->makeTransition(attackerFrom,
								resultThread->makeState("final"),
								std::make_shared < Atomic
								> (check_not_blocked, std::make_shared
										< Read
										> (snapshot, read->address(), BUFFER_SPACE), std::make_shared
										< Read
										> (share, read->address(), DEFAULT_SPACE), std::make_shared
										< Read
										> (wAttackerFlag, read->address(), WA_SPACE),
										//we need to check wA==1 here --> it is a new bug in previous version
										std::make_shared < Read > (wA, wAVar, SERVICE_SPACE),
										std::make_shared < Condition> (
												std::make_shared < BinaryOperator> ( BinaryOperator::AND,
														std::make_shared
														< BinaryOperator> (BinaryOperator::AND,
																std::make_shared < BinaryOperator> (BinaryOperator::NEQ, share, snapshot),
																std::make_shared < BinaryOperator> (BinaryOperator::EQ, wAttackerFlag, zero)
														),

														std::make_shared < BinaryOperator> (BinaryOperator::EQ, wA, one))
										),

										std::make_shared
										< Write
										> (one, successVar, SERVICE_SPACE)),
								transition->getID(),
								transition->getIsNotPrecise());
					}
				} else if (transition->instruction()->as<Mfence>()) {
					/* No transition: attacker can't execute fences. */
				} else if (transition->instruction()->as<Lock>()
						|| transition->instruction()->as<Unlock>()) {
					/* No transition: attacker can't execute locked instructions. */
				} else if (transition->instruction()->as<Local>()
						|| transition->instruction()->as<Condition>()
						|| transition->instruction()->as<Noop>()) {
					resultThread->makeTransition(attackerFrom, attackerTo,
							transition->instruction(), transition->getID(),
							transition->getIsNotPrecise());
				} else if (transition->instruction()->as<Atomic>()) {
					assert(
							!"Sorry, atomics in input programs are not supported.");
				} else {
					assert(!"NEVER REACHED");
				}
			}
		}
	}
} else { // This case we know attacker and helpers, but we do not know which pairs of write and read
	foreach (Thread *thread, program.threads()) {
		Thread *resultThread = resultProgram.makeThread(thread->name());
		if (thread->initialState())
		resultThread->setInitialState(
				resultThread->makeState(
						"orig_" + thread->initialState()->name()));

		/*
		 * Instrumentation for the helpers:
		 */
		//if (thread->name().compare(attacker->name()) != 0 ) { //helpers
		if (thread->name() != attacker->name()) { //helpers
			foreach (Transition *transition, thread->transitions()) {
				State *originalFrom = resultThread->makeState(
						"orig_" + transition->from()->name());
				State *originalTo = resultThread->makeState(
						"orig_" + transition->to()->name());

				if (transition->instruction()->is<Read>()) {
					resultThread->makeTransition(originalFrom, originalTo,
							std::make_shared < Atomic
							> (check_not_blocked, transition->instruction()),
							transition->getID(),
							transition->getIsNotPrecise());
				} else if (transition->instruction()->as<Write>()) {
					resultThread->makeTransition(originalFrom, originalTo,
							std::make_shared < Atomic
							> (check_not_blocked, std::make_shared
									< Read
									> (wA, wAVar, SERVICE_SPACE), std::make_shared
									< Condition
									> (std::make_shared
											< BinaryOperator
											> (BinaryOperator::EQ, wA, zero)), transition->instruction()),
							transition->getID(),
							transition->getIsNotPrecise());
				} else {
					resultThread->makeTransition(originalFrom, originalTo,
							transition->instruction(), transition->getID(),
							transition->getIsNotPrecise());
				}
			}
		} else {
			// Instrument for attacker

			foreach (Transition *transition, thread->transitions()) {

				/*
				 * Instrumentation for the attacker:
				 * We have 3 sections: before bar_A, bar_A, after bar_A and before w_A, w_A, after w_A
				 * before bar_A: reads and writes to shared mem
				 * after bar_A: reads and writes to snapshot
				 *
				 * Instrumentation for the helpers:
				 * Before bar_A, helper reads and writes to shared mem, but not check oW. After bar_A, helper need check oW
				 */

				/*
				 * before bar_A: all thread are in SC and every thread can become attacker, then others become helpers
				 */

				State *originalFrom = resultThread->makeState(
						"orig_" + transition->from()->name());
				State *originalTo = resultThread->makeState(
						"orig_" + transition->to()->name());

				//For both attacker before bar_A, and helpers
				if (transition->instruction()->is<Read>()) {
					resultThread->makeTransition(originalFrom, originalTo,
							std::make_shared < Atomic
							> (check_not_blocked, transition->instruction()),
							transition->getID(), transition->getIsNotPrecise());
				} else if (transition->instruction()->as<Write>()) {
					resultThread->makeTransition(originalFrom, originalTo,
							std::make_shared < Atomic
							> (check_not_blocked, std::make_shared
									< Read > (wA, wAVar, SERVICE_SPACE), std::make_shared
									< Condition
									> (std::make_shared < BinaryOperator
											> (BinaryOperator::EQ, wA, zero)), transition->instruction()),
							transition->getID(), transition->getIsNotPrecise());

				} else {
					resultThread->makeTransition(originalFrom, originalTo,
							transition->instruction(), transition->getID(),
							transition->getIsNotPrecise());
				}

				//The following code is only for attacker
				State *attackerFrom = resultThread->makeState(
						"att_" + transition->from()->name());
				State *attackerTo = resultThread->makeState(
						"att_" + transition->to()->name());

				/*
				 * Find bar_A
				 */
				if (Write *write = transition->instruction()->as<Write>()) {
					resultThread->makeTransition(originalFrom,
							resultThread->makeState(
									"bar_" + transition->from()->name()),
							std::make_shared < Atomic > (
									check_not_blocked,
									std::make_shared < Read
									> (share, barVar, SERVICE_SPACE), std::make_shared
									< Condition
									> (std::make_shared < BinaryOperator
											> (BinaryOperator::EQ, share, zero)), snapshort, std::make_shared
									< Write
									> (one, barVar, SERVICE_SPACE)), -1,
							transition->getIsNotPrecise());

					//do not update to memory, only update to buffer and WA
					resultThread->makeTransition(
							resultThread->makeState(
									"bar_" + transition->from()->name()),
							resultThread->makeState(
									"att_" + transition->to()->name()),
							std::make_shared < Atomic
							> (check_not_blocked, std::make_shared
									< Write
									> (write->value(), write->address(), BUFFER_SPACE), std::make_shared
									< Write
									> (one, write->address(), WA_SPACE), std::make_shared
									< Write
									> (zero, canUpdateVar, SERVICE_SPACE)
							),
							transition->getID(), transition->getIsNotPrecise());

					//update to buffer, WA, and mem, but do not set wA
					resultThread->makeTransition(
							resultThread->makeState(
									"bar_" + transition->from()->name()),
							resultThread->makeState(
									"att_" + transition->to()->name()),
							std::make_shared < Atomic
							> (check_not_blocked, std::make_shared
									< Write
									> (write->value(), write->address(), BUFFER_SPACE), std::make_shared
									< Write
									> (write->value(), write->address(), DEFAULT_SPACE), std::make_shared
									< Write
									> (one, write->address(), WA_SPACE), std::make_shared
									< Write
									> (one, canUpdateVar, SERVICE_SPACE)),
							transition->getID(), transition->getIsNotPrecise());

					//update to buffer, WA, and mem, and set wA
					resultThread->makeTransition(
							resultThread->makeState(
									"bar_" + transition->from()->name()),
							resultThread->makeState(
									"att_" + transition->to()->name()),
							std::make_shared < Atomic
							> (check_not_blocked, std::make_shared
									< Write
									> (write->value(), write->address(), BUFFER_SPACE), std::make_shared
									< Write
									> (write->value(), write->address(), DEFAULT_SPACE), std::make_shared
									< Write
									> (one, write->address(), WA_SPACE), std::make_shared
									< Write
									> (one, canUpdateVar, SERVICE_SPACE), std::make_shared
									< Write
									> (one, wAVar, SERVICE_SPACE)),
							transition->getID(), transition->getIsNotPrecise());
				}

				/*
				 * Attacker's execution.
				 */
				if (fenced.find(transition->from()) != fenced.end()) {
					/* No transition from an extra fenced state. */
				} else if (Write *write =
						transition->instruction()->as<Write>()) {
					if (!searchForTdrOnly) {

						//do not update to memory, only update to buffer and WA
						resultThread->makeTransition(attackerFrom, attackerTo,
								std::make_shared < Atomic
								> (check_not_blocked, std::make_shared
										< Write
										> (write->value(), write->address(), BUFFER_SPACE), std::make_shared
										< Write
										> (one, write->address(), WA_SPACE), std::make_shared
										< Write
										> (zero, canUpdateVar, SERVICE_SPACE)),
								transition->getID(),
								transition->getIsNotPrecise());

						//update to buffer, WA, and mem, but do not set wA
						resultThread->makeTransition(attackerFrom, attackerTo,
								std::make_shared < Atomic
								> (check_not_blocked,
										std::make_shared < Read > (share, canUpdateVar, SERVICE_SPACE),
										std::make_shared < Condition> (std::make_shared < BinaryOperator> (BinaryOperator::EQ, share, one)),

										std::make_shared
										< Write
										> (write->value(), write->address(), BUFFER_SPACE), std::make_shared
										< Write
										> (one, write->address(), WA_SPACE),

										std::make_shared
										< Write
										> (write->value(), write->address(), DEFAULT_SPACE), std::make_shared
										< Write
										> (one, canUpdateVar, SERVICE_SPACE)),
								transition->getID(),
								transition->getIsNotPrecise());

						//update to buffer, WA, and mem, and set wA
						resultThread->makeTransition(attackerFrom, attackerTo,
								std::make_shared < Atomic
								> (check_not_blocked,
										std::make_shared < Read > (share, canUpdateVar, SERVICE_SPACE),
										std::make_shared < Condition> (std::make_shared < BinaryOperator> (BinaryOperator::EQ, share, one)),

										std::make_shared
										< Write
										> (write->value(), write->address(), BUFFER_SPACE), std::make_shared
										< Write
										> (one, write->address(), WA_SPACE),

										std::make_shared
										< Write
										> (write->value(), write->address(), DEFAULT_SPACE), std::make_shared
										< Write
										> (one, canUpdateVar, SERVICE_SPACE), std::make_shared
										< Write
										> (one, wAVar, SERVICE_SPACE)),
								transition->getID(),
								transition->getIsNotPrecise());
					}
				} else if (Read *read = transition->instruction()->as<Read>()) {

					/* Reads either read from the buffer... */
					resultThread->makeTransition(attackerFrom, attackerTo,
							std::make_shared < Atomic > (
									//check_not_blocked,
									std::make_shared < Read
									> (read->reg(), read->address(), BUFFER_SPACE)),
							transition->getID(), transition->getIsNotPrecise());

					resultThread->makeTransition(attackerFrom,
							resultThread->makeState("final"),
							std::make_shared < Atomic
							> (check_not_blocked, std::make_shared
									< Read
									> (snapshot, read->address(), BUFFER_SPACE), std::make_shared
									< Read
									> (share, read->address(), DEFAULT_SPACE), std::make_shared
									< Read
									> (wAttackerFlag, read->address(), WA_SPACE),

									//we need to check wA==1 here --> it is a new bug in previous version
									std::make_shared < Read > (wA, wAVar, SERVICE_SPACE),
									std::make_shared < Condition> (
											std::make_shared < BinaryOperator> ( BinaryOperator::AND,
													std::make_shared
													< BinaryOperator> (BinaryOperator::AND,
															std::make_shared < BinaryOperator> (BinaryOperator::NEQ, share, snapshot),
															std::make_shared < BinaryOperator> (BinaryOperator::EQ, wAttackerFlag, zero)
													),

													std::make_shared < BinaryOperator> (BinaryOperator::EQ, wA, one))
									),

									std::make_shared
									< Write
									> (one, successVar, SERVICE_SPACE)),
							transition->getID(), transition->getIsNotPrecise());
				} else if (transition->instruction()->as<Mfence>()) {
					/* No transition: attacker can't execute fences. */
				} else if (transition->instruction()->as<Lock>()
						|| transition->instruction()->as<Unlock>()) {
					/* No transition: attacker can't execute locked instructions. */
				} else if (transition->instruction()->as<Local>()
						|| transition->instruction()->as<Condition>()
						|| transition->instruction()->as<Noop>()) {
					resultThread->makeTransition(attackerFrom, attackerTo,
							transition->instruction(), transition->getID(),
							transition->getIsNotPrecise());
				} else if (transition->instruction()->as<Atomic>()) {
					assert(
							!"Sorry, atomics in input programs are not supported.");
				} else {
					assert(!"NEVER REACHED");
				}
			}
		}
	}
}

}

}

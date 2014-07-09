/*
 * SpinPrinter.cpp
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

//#include <iostream>
#include "SpinPrinter.h"

#include <algorithm> /* std::find */
#include <cassert>
#ifndef WMM_FRIENDLY_SPIN_NAMES
#include <sstream>
#endif

#include "Census.h"
#include "Foreach.h"
#include "Instruction.h"
#include "Program.h"
#include "State.h"
#include "Transition.h"

namespace wmm {

namespace {

template<class T>
std::string ident(const T *object) {
#ifdef WMM_FRIENDLY_SPIN_NAMES
	return object->name();
#else
	std::stringstream ss;
	ss << "_0x" << object;
	return ss.str();
#endif
}

template<class T>
std::string ident(const std::shared_ptr<T> &ptr) {
	return ident(ptr.get());
}

enum {
	DEFAULT_SPACE = Space(),
	INTEGER_SPACE,
	BOOL_SPACE
};

int mem_size;

void printExpression(std::ostream &out,
		const std::shared_ptr<Expression> &expression) {
	switch (expression->kind()) {
	case Expression::CONSTANT: {
		Constant *constant = expression->as<Constant>();
		out << constant->value();
		break;
	}
	case Expression::REGISTER: {
		Register *reg = expression->as<Register>();
		out << ident(reg);
		break;
	}
	case Expression::UNARY: {
		UnaryOperator *unary = expression->as<UnaryOperator>();
		out << unary->getOperatorSign() << '(';
		printExpression(out, unary->operand());
		out << ')';
		break;
	}
	case Expression::BINARY: {
		BinaryOperator *binary = expression->as<BinaryOperator>();
		out << '(';
		printExpression(out, binary->left());
		out << binary->getOperatorSign();
		printExpression(out, binary->right());
		out << ')';
		break;
	}
	case Expression::NOT_BLOCKED: {
		out << "(giant_lock==0||giant_lock==THREAD_ID)";
		break;
	}
	default: {
		assert(!"NEVER REACHED");
	}
	}
}

void printInstruction(std::ostream &out,
		const std::shared_ptr<Instruction> &instruction) {
	/* Guard. */
	if (Condition *condition = instruction->as<Condition>()) {
		out << ' ';
		printExpression(out, condition->expression());
		out << " ->";
	}

	out << ' ';

	/* Instruction itself. */
	switch (instruction->mnemonic()) {
	case Instruction::READ: {
		Read *read = instruction->as<Read>();
		out << ident(read->reg()) << "=mem" << read->space() << "[";
		printExpression(out, read->address());
		out << "];";
		break;
	}
	case Instruction::WRITE: {
		Write *write = instruction->as<Write>();
		//we put something here for non-deterministic generating a random value
		if (write->address()->kind() == Expression::CONSTANT) {
			Constant *constantAdd = write->address()->as<Constant>();
			if (constantAdd->value() == -17) {
				out
						//<< "do :: (randAdd >= 0 && randAdd <= 20) -> randAdd++	:: (randAdd >= 0 && randAdd <= 20) -> randAdd-- :: randAdd < "
					//	<< mem_size << " && randAdd >= 0 ->break od; "; //generate a random address
				<< " atomic { if :: 1 < " << mem_size << " -> randAdd = 1 :: 2 < " << mem_size << " -> randAdd = 2 :: 3 < " << mem_size << " -> randAdd = 3 "
				<<	":: 4 < " << mem_size << " -> randAdd = 4 :: 5 < " << mem_size << " -> randAdd = 5 :: 6 < " << mem_size << " -> randAdd = 6"
				<< " :: 7 < " << mem_size << " -> randAdd = 7 :: 8 < " << mem_size << " -> randAdd = 8 :: 9 < " << mem_size << " -> randAdd = 9"
				<< " :: 10 < " << mem_size << " -> randAdd = 10 :: else -> randAdd = 0 fi; } ";
				if (write->value()->kind() == Expression::CONSTANT) {
					Constant *constantValue = write->value()->as<Constant>();
					if (constantValue->value() == -17) {
						out
						//		<< "do :: (randValue >= 0 && randValue <= 20) -> randValue++	:: (randValue >= 0 && randValue <= 20) -> randValue-- :: randValue >= 0 -> break od; "; //generate a random value
						<< " atomic { if :: 1 -> randValue = 0  :: 1 -> randValue = 1 :: 1 -> randValue = 2 :: 1 -> randValue = 3 :: 1 -> randValue = 4 fi; } ";
						out << "mem" << write->space()
								<< "[randAdd]= randValue";
					} else {
						out << "mem" << write->space() << "[randAdd]=";
						printExpression(out, write->value());
					}
				} else {
					out << "mem" << write->space() << "[randAdd]=";
					printExpression(out, write->value());
				}
			} else {
				if (write->value()->kind() == Expression::CONSTANT) {
					Constant *constantValue = write->value()->as<Constant>();
					if (constantValue->value() == -17) {
						out
							//	<< "do :: (randValue >= 0 && randValue <= 20) -> randValue++	:: (randValue >= 0 && randValue <= 20) -> randValue-- :: randValue >= 0 -> break od; "; //generate a random value
						<< " atomic { if :: 1 -> randValue = 0  :: 1 -> randValue = 1 :: 1 -> randValue = 2 :: 1 -> randValue = 3 :: 1 -> randValue = 4 fi; } ";
						out << "mem" << write->space() << "[";
						printExpression(out, write->address());
						out << "]= randValue";
					} else {
						out << "mem" << write->space() << "[";
						printExpression(out, write->address());
						out << "]=";
						printExpression(out, write->value());
					}
				} else {
					out << "mem" << write->space() << "[";
					printExpression(out, write->address());
					out << "]=";
					printExpression(out, write->value());
				}
			}
		} else {
			if (write->value()->kind() == Expression::CONSTANT) {
				Constant *constantValue = write->value()->as<Constant>();
				if (constantValue->value() == -17) {
					out
							//<< "do :: (randValue >= 0 && randValue <= 20) -> randValue++	:: (randValue >= 0 && randValue <= 20) -> randValue-- :: randValue >= 0 -> break od; "; //generate a random value
					<< " atomic { if :: 1 -> randValue = 0  :: 1 -> randValue = 1 :: 1 -> randValue = 2 :: 1 -> randValue = 3 :: 1 -> randValue = 4 fi; } ";
					out << "mem" << write->space() << "[";
					printExpression(out, write->address());
					out << "]=randValue";
				} else {
					out << "mem" << write->space() << "[";
					printExpression(out, write->address());
					out << "]=";
					printExpression(out, write->value());
				}
			} else {
				out << "mem" << write->space() << "[";
				printExpression(out, write->address());
				out << "]=";
				printExpression(out, write->value());

			}
		}
		out << ';';
		break;
	}
	case Instruction::MFENCE: {
		out << "skip /*mfence*/;" << std::endl;
		break;
	}
	case Instruction::LOCAL: {
		Local *local = instruction->as<Local>();
		out << ident(local->reg()) << "=";
		printExpression(out, local->value());
		out << ";";
		break;
	}
	case Instruction::CONDITION: {
		out << "skip /*condition*/;";
		break;
	}
	case Instruction::ATOMIC: {
		Atomic *atomic = instruction->as<Atomic>();

		out << "atomic {";
		foreach (const auto &instr, atomic->instructions()){
		printInstruction(out, instr);
	}
		out << " }";
		break;
	}
	case Instruction::NOOP: {
		out << "skip /*no-op*/;";
		break;
	}
	case Instruction::LOCK: {
		out << "atomic { giant_lock==0 -> giant_lock=THREAD_ID; }";
		break;
	}
	case Instruction::UNLOCK: {
		out << "atomic { giant_lock==THREAD_ID -> giant_lock=0; }";
		break;
	}
//	case Instruction::SNAPSHORT: {
//		out << "atomic {";
//		for (int i = 0; i < mem_size; i++)
//			out << " mem" << BUFFER_SPACE << "[" << i << "]=mem" << DEFAULT_SPACE
//					<< "[" << i << "]; ";
//		out << " } ";
//		break;
//	}
	default: {
		assert(!"NEVER REACHED");
	}
	}
}

} // anonymous namespace

void SpinPrinter::print(std::ostream &out, const Program &program) const {
	/* Set of used spaces. */
	std::vector<Space> spaces;

	Census programCensus;
	programCensus.visit(program);

	mem_size = program.memorySize();

//	/* Shared memory. */
//	foreach (Space space, programCensus.spaces()){
//	if (space == DEFAULT_SPACE) {
//		out << "byte mem" << space << "[" << program.memorySize() << "] = "
//		<< Domain() << ';' << std::endl;
//	} else if (space == INTEGER_SPACE) {
//	out << "byte mem" << space << "[" << program.intSpaceSize() << "] = "
//	<< Domain() << ';' << std::endl;
//	} else // BOOL_SPACE
//		out << "bit mem" << space << "[" << program.boolSpaceSize() << "] = "
//			<< Domain() << ';' << std::endl;
//	}

	/* Shared memory. */
	foreach (Space space, programCensus.spaces()){
		if (space == DEFAULT_SPACE) {
			out << "byte mem" << DEFAULT_SPACE << "[" << program.memorySize() << "] = "
				<< Domain() << ';' << std::endl;
		} else if (space == INTEGER_SPACE) {
			out << "byte mem" << space << "[" << program.intSpaceSize() << "] = "
				<< Domain() << ';' << std::endl;
		}
	}

	// BOOL_SPACE
	out << "bit mem" << BOOL_SPACE << "[" << program.boolSpaceSize() << "] = "
			<< Domain() << ';' << std::endl;

	out << "byte giant_lock = 0;" << std::endl;

	out << "byte randAdd = 0;" << std::endl;
	out << "byte randValue = 0;" << std::endl;

	/* Threads. */
	int thread_id = 0;
	foreach (Thread *thread, program.threads()){
	out << "active proctype " << ident(thread) << "() {" << std::endl;

	out << "int THREAD_ID = " << ++thread_id << ';' << std::endl;

	Census threadCensus;
	threadCensus.visit(thread);

	/* Register declarations. */
	foreach (Expression *expression, threadCensus.expressions()) {
		if (Register *reg = expression->as<Register>()) {
			out << "byte " << ident(reg) << ';' << std::endl;
		}
	}

	/* Goto initial state. */
	if (thread->initialState()) {
		out << "goto " << ident(thread->initialState()) << ";" << std::endl;
	}

	/* Transitions. */
	foreach (State *state, thread->states()) {
		out << ident(state) << ": ";
		if (state->out().empty()) {
			out << "goto __done;" << std::endl;
		} else {
			out << "if" << std::endl;
			foreach (Transition *transition, state->out()) {
				//here original is ID, not original of orifinal graph
				out << "/* " << thread->name() << ","
				<< transition->getOriginal() << " */ ";
				out << "::";
				printInstruction(out, transition->instruction());
				out << " goto " << ident(transition->to()) << ';'
				<< std::endl;
			}
			out << "fi;" << std::endl;
		}
	}
	out << "__done: skip;" << std::endl;
	out << "}" << std::endl;
}

/* Never claim. */
	if (program.interestingSpace() != INVALID_SPACE) {
		out << "never {" << std::endl;
		out << "T0_init:" << std::endl;
		out << "if" << std::endl;
		out << ":: (mem" << program.interestingSpace() << '['
				<< program.interestingAddress() << "]) -> goto accept_all"
				<< std::endl;
		out << ":: (1) -> goto T0_init" << std::endl;
		out << "fi;" << std::endl;
		out << "accept_all: skip;" << std::endl;
		out << "}" << std::endl;
	}
}

}

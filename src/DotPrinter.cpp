/*
 * DotPrinter.cpp
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

#include "DotPrinter.h"

#include <cassert>

#include "Foreach.h"
#include "Instruction.h"
#include "Program.h"
#include "State.h"
#include "Transition.h"

namespace wmm {

namespace {

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
		out << reg->name();
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
		out << ' ' << binary->getOperatorSign() << ' ';
		printExpression(out, binary->right());
		out << ')';
		break;
	}
	case Expression::NOT_BLOCKED: {
		out << "not_blocked";
		break;
	}
	default: {
		assert(!"NEVER REACHED");
	}
	}
}

void printInstruction(std::ostream &out,
		const std::shared_ptr<Instruction> &instruction) {
	switch (instruction->mnemonic()) {
	case Instruction::READ: {
		Read *read = instruction->as<Read>();
		out << read->reg()->name() << ":= mem";
		if (read->space()) {
			out << read->space();
		}
		out << '[';
		printExpression(out, read->address());
		out << "];";
		break;
	}
	case Instruction::WRITE: {
		Write *write = instruction->as<Write>();
		out << "mem";
		if (write->space()) {
			out << write->space();
		}
		out << '[';
		printExpression(out, write->address());
		out << "]:=";
		printExpression(out, write->value());
		out << ';';
		break;
	}
	case Instruction::MFENCE: {
		out << "mfence;";
		break;
	}
	case Instruction::LOCAL: {
		Local *local = instruction->as<Local>();
		printExpression(out, local->reg());
		out << ":=";
		printExpression(out, local->value());
		out << ';';
		break;
	}
	case Instruction::CONDITION: {
		Condition *condition = instruction->as<Condition>();
		out << "check ";
		printExpression(out, condition->expression());
		out << ';';
		break;
	}
	case Instruction::ATOMIC: {
		Atomic *atomic = instruction->as<Atomic>();
		out << "atomic {\\n";
		foreach (const auto &instr, atomic->instructions()) {
			printInstruction(out, instr);
			out << "\\n";
		}
		out << "};";
		break;
	}
	case Instruction::NOOP: {
		break;
	}
	case Instruction::LOCK: {
		out << "lock";
		break;
	}
	case Instruction::UNLOCK: {
		out << "unlock";
		break;
	}
	case Instruction::SNAPSHORT: {
		out << "snapshort;";
		break;
	}
	default: {
		assert(!"NEVER REACHED");
	}
	}
}

} // namespace

void DotPrinter::print(std::ostream &out, const Program &program) const {
	out << "digraph threads {" << std::endl;
	foreach (const Thread *thread, program.threads()) {
		out << "subgraph cluster" << this << " {" << std::endl;
		foreach (const State *state, thread->states()) {
			out << "state" << state << "[shape=\"ellipse\",label=\""
					<< state->name() << "\"];" << std::endl;
		}
		foreach (const Transition *transition, thread->transitions()) {
			out << "state" << transition->from() << "->state"
					<< transition->to() << "[label=\"";
			printInstruction(out, transition->instruction());
			out << "\"];" << std::endl;
		}
		out << '}' << std::endl;
}
	out << "}" << std::endl;
}

}

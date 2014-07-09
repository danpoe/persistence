/*
 * Printer.h
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

#include <ostream>

namespace wmm {

class Program;

class Printer {
public:

	virtual ~Printer() {
	}

	virtual void print(std::ostream &out, const Program &program) const = 0;
};

}

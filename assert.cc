#include "assert.h"

#include "print.h"

static void Assert(bool b) {
	if (!b) {
		Print("Assert tripped.\n");
	}
}


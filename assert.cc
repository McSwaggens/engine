#include "assert.h"
#include "print.h"
#include "os.h"

static void AssertImpl(bool b, const char* str, AssertSourceLocation srcloc) {
	if (b) return;

	Print("%:%:%: error: Assert tripped in function %: %\n", CString(srcloc.file), srcloc.line, srcloc.column, CString(srcloc.function), CString(str));
	standard_output_buffer.Flush();
	Trap();
	ExitProgram();
}


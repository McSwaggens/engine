#ifndef ASSERT_H
#define ASSERT_H

#include "general.h"

#define GetInternalFunctionName() ToString()
#define GetInternalFileName()     ToString(__builtin_FILE())
#define GetInternalLineNumber()   __builtin_LINE()
#define GetInternalColumnNumber() __builtin_COLUMN()

struct AssertSourceLocation {
	s32 line;
	s32 column;
	const char* file;
	const char* function;
};

#define Assert(x) AssertImpl(x, #x)

static void AssertImpl(bool b, const char* str, AssertSourceLocation srcloc = {
	.function = __builtin_FUNCTION(),
	.file     = __builtin_FILE(),
	.line     = __builtin_LINE(),
	.column   = __builtin_COLUMN(),
});

#endif // ASSERT_H

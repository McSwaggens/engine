#include "general.h"
#include "math.h"
#include "window.h"

#include "vector.h"
#include "quaternion.h"

#include "assert.cc"
#include "alloc.cc"
#include "unix.cc"
#include "window.cc"

int main(int argc, char** argv) {
	InitWindowSystem();

	Window window = CreateWindow();

	while (!window.ShouldClose()) {
		window.Update();
	}

	window.Destroy();

	return 0;
}

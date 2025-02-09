program: *.cc *.h
	clang main.cc -framework cocoa -framework IOKit -lglfw3 -lvulkan -lMoltenVK -std=c++20 -Wno-writable-strings -o program

run: program
	./program

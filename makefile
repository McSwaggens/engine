program: *.cc *.h
	clang main.cc -O0 -g3 -framework cocoa -framework IOKit -lglfw3 -lvulkan -lMoltenVK -std=c++20 -Wno-writable-strings -o program

run: program
	./program

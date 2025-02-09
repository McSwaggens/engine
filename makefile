program: *.cc *.h
	clang main.cc -framework cocoa -framework IOKit -lglfw3 -lvulkan -std=c++20 -o program

run: program
	./program

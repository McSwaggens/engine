program: *.cc *.h
	clang main.cc -framework cocoa -framework IOKit -lglfw3 -std=c++20 -o program

run: program
	./program

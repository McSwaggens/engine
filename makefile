program: *.cc *.h
	clang main.cc -std=c++20 -o program

run: program
	./program

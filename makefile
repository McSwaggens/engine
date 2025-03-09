program_xxx: *.cc *.h
	clang \
		main.cc \
		-O0 -g3 \
		-lglfw -lvulkan \
		-std=c++20 \
		-Wno-writable-strings -Wno-reorder-init-list -Wno-vla-cxx-extension\
		-o program

		glslc -fshader-stage=vertex   vert.glsl -o vert.spv
		glslc -fshader-stage=fragment frag.glsl -o frag.spv

run: program_xxx
	pkill program || true
	./program

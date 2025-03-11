shaders: *.glsl
	glslc -fshader-stage=vertex   vert.glsl -o vert.spv
	glslc -fshader-stage=fragment frag.glsl -o frag.spv

program_macos_xxx: *.cc *.h
	clang \
		main.cc \
		-O0 -g3 \
		-lglfw -lvulkan \
		-std=c++20 \
		-Wno-writable-strings -Wno-reorder-init-list -Wno-vla-cxx-extension\
		-DMACOS\
		-o program

program_linux_xxx: *.cc *.h
	clang \
		main.cc \
		-O0 -g3 \
		-lglfw -lvulkan \
		-std=c++20 \
		-Wno-writable-strings -Wno-reorder-init-list -Wno-vla-cxx-extension\
		-o program

run_macos: program_macos_xxx shaders
	pkill program || true
	DYLD_LIBRARY_PATH=/Users/daniel/VulkanSDK/1.4.304.1/macOS/lib ./program

run_linux: program_linux_xxx shaders
	pkill program || true
	./program


run: run_linux

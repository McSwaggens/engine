program_xxx: *.cc *.h shaders
	clang \
		main.cc \
		-O0 -g3 \
		-lglfw -lvulkan -lm \
		-std=c++20 \
		-Wno-writable-strings -Wno-reorder-init-list -Wno-vla-cxx-extension \
		-DMACOS=0 \
		-o program

shaders: *.glsl
	glslc -fshader-stage=vertex   vert.glsl -o vert.spv
	glslc -fshader-stage=fragment frag.glsl -o frag.spv

run: program_xxx shaders
	pkill program || true
	DYLD_LIBRARY_PATH=/Users/daniel/VulkanSDK/1.4.304.1/macOS/lib ./program

tags: *.cc *.h
	ctags --language-force=c++ -R *.cc *.h /usr/include/vulkan/vulkan_core.h


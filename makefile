OS_NAME := $(shell uname)

IS_MACOS:=0
IS_LINUX:=0

ifeq ($(OS_NAME), Darwin)
	IS_MACOS=1
endif

ifeq ($(OS_NAME), Linux)
	IS_LINUX=1
endif

program_xxx: *.cc *.h shaders
	clang \
		main.cc \
		-O0 -g3 \
		-lglfw -lvulkan -lm \
		-std=c++20 \
		-Wno-writable-strings -Wno-reorder-init-list -Wno-vla-cxx-extension -Wno-undefined-internal \
		-DMACOS=$(IS_MACOS) \
		-DLINUX=$(IS_LINUX) \
		-o program

shaders: vert.hlsl frag.hlsl
	dxc -spirv -T vs_6_0 -E main -Fo vert.spv vert.hlsl
	dxc -spirv -T ps_6_0 -E main -Fo frag.spv frag.hlsl

run: program_xxx shaders
	pkill program || true
	DYLD_LIBRARY_PATH=/Users/daniel/VulkanSDK/1.4.304.1/macOS/lib ./program

tags: *.cc *.h
	ctags --language-force=c++ -R *.cc *.h /usr/include/vulkan/vulkan_core.h


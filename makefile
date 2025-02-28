program_xxx: *.cc *.h
	clang \
		main.cc \
		-O0 -g3 \
		-framework cocoa -framework IOKit \
		-lglfw3 -lvulkan -lMoltenVK \
		-std=c++20 \
		-Wno-writable-strings \
		-o program

		# -I /Users/daniel/VulkanSDK/1.4.304.1/macOS/include/ \
		# -L /Users/daniel/VulkanSDK/1.4.304.1/macOS/lib/ \

run: program_xxx
	echo $(DYLD_LIBRARY_PATH)
	./program

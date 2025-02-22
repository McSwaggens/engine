#include "general.h"
#include "math.h"
#include "window.h"

#include "vector.h"
#include "quaternion.h"

#include "list.h"

#include "assert.cc"
#include "alloc.cc"
#include "unix.cc"
#include "window.cc"
#include "print.cc"
#include "file_system.cc"

#include "vk_helper.h"

static VkInstance vk;

static void InitVulkan() {
	const char** glfw_exts = null;
	u32 glfw_ext_count = 0;
	glfw_exts = glfwGetRequiredInstanceExtensions(&glfw_ext_count);

	Print("glfw_ext_count = %\n", glfw_ext_count);
	for (u32 i = 0; i < glfw_ext_count; i++)
		Print("Extension: %\n", FromCString(glfw_exts[i]));

	VkApplicationInfo app_info = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "Engine",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "xxx",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = VK_API_VERSION_1_4,
	};

	char* extensions[4096] = {
		VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
	};

	VkInstanceCreateInfo inst_info = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
		.pApplicationInfo = &app_info,
		.enabledExtensionCount = 1,
		.ppEnabledExtensionNames = extensions,
		.enabledLayerCount = 0,
	};

	VkResult result = vkCreateInstance(&inst_info, null, &vk);
	Print("result = %\n", ToString(result));
	Assert(result == VK_SUCCESS);
}

int main(int argc, char** argv) {
	InitWindowSystem();

	Window window = CreateWindow();

	InitVulkan();

	List<u32> nums;
	for (u32 i = 0; i < 1<<12; i++) {
		Print("Pushing % to nums\n", i);
		standard_output_buffer.Flush();
		nums.Add(i);
	}

	for (u32 i = 0; i < nums.count; i++) {
		Print("nums[%] = %\n", i, nums[i]);
	}

	while (!window.ShouldClose()) {
		window.Update();


		standard_output_buffer.Flush();
	}

	window.Destroy();
	glfwTerminate();

	standard_output_buffer.Flush();

	return 0;
}

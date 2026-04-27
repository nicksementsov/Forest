#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

#include <vulkan/vulkan_raii.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <cstdlib>

constexpr uint32_t WIN_WIDTH = 800;
constexpr uint32_t WIN_HEIGHT = 600;

const std::vector<char const*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

class HelloTriangleApplication {
public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private:
	GLFWwindow* window;

	vk::raii::Context context;
	vk::raii::Instance instance = nullptr;

	void initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Forest", nullptr, nullptr);
	}

	void initVulkan() {
		createInstance();
	}

	void createInstance() {
		// constexpr vk::ApplicationInfo appInfo{.pApplicationName = "Forest", .applicationVersion = VK_MAKE_VERSION(0, 1, 0), .pEngineName = "Forest Engine", .engineVersion = VK_MAKE_VERSION(0, 1, 0), .apiVersion = vk::ApiVersion14};
		constexpr vk::ApplicationInfo appInfo{
			.pApplicationName   = "Forest",
			.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 ),
			.pEngineName        = "No Engine",
			.engineVersion      = VK_MAKE_VERSION( 1, 0, 0 ),
			.apiVersion         = vk::ApiVersion14
		};

		// Validation Layers
		std::vector<char const*> requiredLayers;
		if (enableValidationLayers) {
			requiredLayers.assign(validationLayers.begin(), validationLayers.end());
		}

		auto layerProperties = context.enumerateInstanceLayerProperties();
		auto unsupportedLayerIt = std::ranges::find_if(
			requiredLayers,
			[&layerProperties](auto const &requiredLayer) 
			{ 
				return std::ranges::none_of(
					layerProperties,
					[requiredLayer](auto const &layerProperty)
					{
						return strcmp(layerProperty.layerName, requiredLayer) == 0;
					});
			});
		if (unsupportedLayerIt != requiredLayers.end())
		{
			throw std::runtime_error("Required layer not supported: " + std::string(*unsupportedLayerIt));
		}

		// Extensions
		auto requiredExtensions = getRequiredInstanceExtensions();
		
		auto extensionProperties = context.enumerateInstanceExtensionProperties();
		auto unsupportedPropertyIt = std::ranges::find_if(
			requiredExtensions,
			[&extensionProperties](auto const &requiredExtension)
			{
				return std::ranges::none_of(
					extensionProperties,
					[requiredExtension](auto const &extensionProperty)
					{
						return strcmp(extensionProperty.extensionName, requiredExtension) == 0;
					});
			});
		if (unsupportedPropertyIt != requiredExtensions.end())
		{
			throw std::runtime_error("Required extension not supported: " + std::string(*unsupportedPropertyIt));
		}

        vk::InstanceCreateInfo createInfo{
        	.pApplicationInfo = &appInfo,
        	.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
        	.ppEnabledLayerNames = requiredLayers.data(),
        	.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
        	.ppEnabledExtensionNames = requiredExtensions.data()
        };
        instance = vk::raii::Instance(context, createInfo);
	}

	void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
	}

	void cleanup() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	std::vector<const char *> getRequiredInstanceExtensions() {
		uint32_t glfwExtensionCount = 0;
		auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
		if (enableValidationLayers)
		{
			extensions.push_back(vk::EXTDebugUtilsExtensionName);
		}

		return extensions;
	}
};

int main()
{
	try
	{
		HelloTriangleApplication app;
		app.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
#include"../rengine.hpp"
using namespace re;


void Device::createInstance(){

	VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "REngine beta";
		appInfo.pEngineName = "REngine";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_3;


		auto ext = getRequiredExtensions();

		VkInstanceCreateInfo instanceCreateInfo{};
			instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			instanceCreateInfo.pApplicationInfo = &appInfo;
			instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(ext.size());
			instanceCreateInfo.ppEnabledExtensionNames = ext.data();

		if (enableValidation) {
			const char* validationLayers[] = { "VK_LAYER_KHRONOS_validation" };
			instanceCreateInfo.enabledLayerCount = 1;
			instanceCreateInfo.ppEnabledLayerNames = validationLayers;
			re::ReLog::warning(re::Prefix::VULKAN, "validation layers is enabled");

		}
		else {

			re::ReLog::warning(re::Prefix::VULKAN, "validation layers is disabled");
			instanceCreateInfo.enabledLayerCount = 0;
		}
		VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance_);
		if (result != VK_SUCCESS) {
			re::ReLog::error(re::Prefix::VULKAN, std::runtime_error("failed to create instance"));
		}

}
void Device::createSurface(GLFWwindow* window) {
	VkResult result = glfwCreateWindowSurface(instance_, window, nullptr, &surface_);
	if (result != VK_SUCCESS) {
		re::ReLog::error(re::Prefix::VULKAN, std::runtime_error("failed to create surface"));
	}

}
void Device::pickPhysicalDevice() {
	uint32_t pDeviceCount = 0;
	vkEnumeratePhysicalDevices(instance_, &pDeviceCount, nullptr);
	std::vector<VkPhysicalDevice> devices(pDeviceCount);
	vkEnumeratePhysicalDevices(instance_, &pDeviceCount, devices.data());
	auto bestDeviceIt = std::max_element(devices.begin(), devices.end(),
		[this](VkPhysicalDevice a, VkPhysicalDevice b) {
			return rateDevice(a) < rateDevice(b);
		}
	);

	if (bestDeviceIt != devices.end() && rateDevice(*bestDeviceIt) > 0) {
		physicalDevice_ = *bestDeviceIt;
	}
	else {
		re::ReLog::error(re::Prefix::VULKAN, std::runtime_error("failed to find a suitable GPU"));
	}

	{
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(physicalDevice_, &physicalDeviceProperties);
		std::string gpuName(physicalDeviceProperties.deviceName, VK_MAX_PHYSICAL_DEVICE_NAME_SIZE);
		re::ReLog::warning(re::Prefix::VULKAN, "selected gpu: " + gpuName);

	}


}
int Device::rateDevice(VkPhysicalDevice device) {
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	int score = 0;
	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		score += 1000;
	}
	else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
		score += 500;
	}

	score += deviceProperties.limits.maxImageDimension2D;
	
	return score;
}
void Device::findQueueFamilies() {
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueFamilyCount, queueFamilies.data());

	for (size_t i = 0; i < queueFamilies.size(); i++)
	{
		VkQueueFamilyProperties queueFamily = queueFamilies[i];
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice_, i, surface_, &presentSupport);
		if (presentSupport)
		{
			presentQueueFamilyIndex_ = i;
		}
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			graphicsQueueFamilyIndex_ = i;
		}
	}
//TODO:ADD VALIDATION FOR DOUBLE FINDING INDEXES
}
void Device::createLogicalDevice() {
	VkPhysicalDeviceFeatures deviceFeatures = {};
	const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	
	if (!graphicsQueueFamilyIndex_.has_value() || !presentQueueFamilyIndex_.has_value()) {
		re::ReLog::error(re::Prefix::VULKAN, std::runtime_error("failed to find suitable queue families (not has_value())"));

	}
	std::set<uint32_t> uniqueQueueFamilies= {graphicsQueueFamilyIndex_.value(), presentQueueFamilyIndex_.value()};
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	float queuePriority = 1.0f;

	for (const auto queueFamily : uniqueQueueFamilies){
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

	VkResult result = vkCreateDevice(physicalDevice_, &deviceCreateInfo, nullptr, &device_);

	if (result != VK_SUCCESS) {
		re::ReLog::error(re::Prefix::VULKAN, std::runtime_error("failed to create logical device GPU"));
	}


	vkGetDeviceQueue(device_, graphicsQueueFamilyIndex_.value(), 0, &graphicsQueue_);
	vkGetDeviceQueue(device_, presentQueueFamilyIndex_.value(), 0, &presentQueue_);
}


VkDevice Device::get() const {
	return device_;
}
VkPhysicalDevice Device::getPhysicalDevice() const {
	return physicalDevice_;
}
VkInstance Device::getInstance() const {
	return instance_;
}
VkSurfaceKHR Device::getSurface() const {
	return surface_;
}
uint32_t Device::getGraphicsQueueFamily() const {
	return graphicsQueueFamilyIndex_.value();
}
uint32_t Device::getPresentQueueFamily() const {
	return presentQueueFamilyIndex_.value();
}
VkQueue Device::getGraphicsQueue() const {
	return graphicsQueue_;
}
VkQueue Device::getPresentQueue() const {
	return presentQueue_;
}

Device::~Device() {
	if (allocator_ != VK_NULL_HANDLE) {
		vmaDestroyAllocator(allocator_);
	}
	if (device_ != VK_NULL_HANDLE) {
		vkDestroyDevice(device_, nullptr);
	}
	if (surface_ != VK_NULL_HANDLE) {
		vkDestroySurfaceKHR(instance_, surface_, nullptr);
	}
	if (instance_ != VK_NULL_HANDLE) {
		vkDestroyInstance(instance_, nullptr);
	}

}

std::vector<const char*> Device::getRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidation) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}


void Device::createAllocator() {
	VmaAllocatorCreateInfo allocatorInfo{};
		allocatorInfo.physicalDevice = physicalDevice_;
		allocatorInfo.device = device_;
		allocatorInfo.instance = instance_;
		allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;

	if (vmaCreateAllocator(&allocatorInfo, &allocator_)!=VK_SUCCESS) {
		re::ReLog::error(re::Prefix::VULKAN, std::runtime_error("failed to create vma allocator"));
	}

}
VmaAllocator Device::getAllocator()const {
	return allocator_;

}

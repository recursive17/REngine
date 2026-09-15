#include"../rengine.hpp"
#include<algorithm>
using namespace re;

void Swapchain::create(Device* device, GLFWwindow* window) {
	device_ = device;

	VkPhysicalDevice physicalDevice = device->getPhysicalDevice();
	VkSurfaceKHR surface = device->getSurface();

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
	std::vector<VkSurfaceFormatKHR> formats(formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());
	VkSurfaceFormatKHR surfaceFormat = chooseSurfaceFormat(formats);

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
	std::vector<VkPresentModeKHR> presentModes(presentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());
	VkPresentModeKHR presentMode = choosePresentMode(presentModes);

	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);
	VkExtent2D extent = chooseExtent(capabilities, window);

	uint32_t imageCount=capabilities.minImageCount+1;
	if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
		imageCount = capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;


	uint32_t graphicsFamily = device->getGraphicsQueueFamily();
	uint32_t presentFamily = device->getPresentQueueFamily();
	uint32_t queueFamilyIndices[] = {graphicsFamily,presentFamily};


	if (graphicsFamily != presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}
	createInfo.preTransform = capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;


	VkResult result = vkCreateSwapchainKHR(device->get(), &createInfo, nullptr, &swapchain_);
	if (result != VK_SUCCESS) {
		re::ReLog::error(re::Prefix::VULKAN, std::runtime_error("failed to create swapchain"));
	}


	uint32_t actualImageCount;
	vkGetSwapchainImagesKHR(device->get(), swapchain_, &actualImageCount, nullptr);
	images_.resize(actualImageCount);
	vkGetSwapchainImagesKHR(device->get(), swapchain_, &actualImageCount, images_.data());

	imageFormat_ = surfaceFormat.format;
	extent_ = extent;
}
void Swapchain::createImageViews() {
	imageViews_.resize(images_.size());

	for (size_t i = 0; i < images_.size(); ++i) {
		VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = images_[i];
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = imageFormat_;
			viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

		VkResult result = vkCreateImageView(device_->get(), &viewInfo, nullptr, &imageViews_[i]);
		if (result != VK_SUCCESS) {
			re::ReLog::error(re::Prefix::VULKAN, std::runtime_error("failed to create image view"));
		}
	}
}
void Swapchain::recreate(Device* device, GLFWwindow* window) {

	vkDeviceWaitIdle(device_->get());

	cleanup();
	create(device, window);
	createImageViews();
}
void Swapchain::cleanup() {
	for (auto view : imageViews_) {
		vkDestroyImageView(device_->get(), view, nullptr);
	}
	imageViews_.clear();
	if (swapchain_ != VK_NULL_HANDLE) {
		vkDestroySwapchainKHR(device_->get(), swapchain_, nullptr);
		swapchain_ = VK_NULL_HANDLE;
	}
}

VkSwapchainKHR Swapchain::get() const {
	return swapchain_;
}
VkFormat Swapchain::getFormat() const {
	return imageFormat_;
}
VkExtent2D Swapchain::getExtent() const {
	return extent_;
}
const std::vector<VkImageView>& Swapchain::getImageViews() const {
	return imageViews_;
}
size_t Swapchain::getImageCount() const {
	return images_.size();
}

Swapchain::~Swapchain(){
	cleanup();
}

VkSurfaceFormatKHR Swapchain::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& avaiableFormats) {
	for (const auto& format : avaiableFormats) {
		if (format.format == VK_FORMAT_R8G8B8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return format;
		}
	}
	return avaiableFormats[0];
}
VkPresentModeKHR Swapchain::choosePresentMode(const std::vector<VkPresentModeKHR>& avaiableModes) {
	for (const auto& mode : avaiableModes) {
		if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return mode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}
VkExtent2D Swapchain::chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	int width, height;

	glfwGetFramebufferSize(window, &width, &height);

	VkExtent2D actualExtent{ static_cast<uint32_t>(width),static_cast<uint32_t>(height) };
	actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
	actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

	return actualExtent;
}
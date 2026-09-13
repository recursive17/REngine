#pragma once
#include <iostream>
#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>
#include<vector>
#include<optional>
#include<set>
namespace re {


	class Device {
		
	public:

		void createInstance(GLFWwindow* window);
		void createSurface(GLFWwindow* window);
		void pickPhysicalDevice();
		void findQueueFamilies();
		void createLogicalDevice();


		VkDevice get() const;
		VkPhysicalDevice getPhysicalDevice() const;
		VkInstance getInstance() const;
		VkSurfaceKHR getSurface() const;
		uint32_t getGraphicsQueueFamily() const;
		uint32_t getPresentQueueFamily() const;
		VkQueue getGraphicsQueue()const;
		VkQueue getPresentQueue()const;
		
		~Device();
	
	private:
		std::vector<const char*> getRequiredExtensions();

	private:
		VkInstance				instance_					{ VK_NULL_HANDLE };
		VkSurfaceKHR			surface_					{ VK_NULL_HANDLE };
		VkPhysicalDevice		physicalDevice_				{ VK_NULL_HANDLE };
		VkDevice				device_						{ VK_NULL_HANDLE };
		VkQueue					presentQueue_				{ VK_NULL_HANDLE };
		VkQueue					graphicsQueue_				{ VK_NULL_HANDLE };
		std::optional<uint32_t> graphicsQueueFamilyIndex_	{ std::nullopt	 };
		std::optional<uint32_t> presentQueueFamilyIndex_	{ std::nullopt   };
		bool					enableValidation			{ true };



	};

	class Swapchain {
	public:

		void create(Device* device, GLFWwindow* window);
		void createImageViews();
		void recreate(Device* device,GLFWwindow* window);
		void cleanup();

		VkSwapchainKHR get() const;
		VkFormat getFormat() const;
		VkExtent2D getExtent() const;
		const std::vector<VkImageView>& getImageViews() const;
		size_t getImageCount() const;

		~Swapchain();
	
	private:
		VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& avaiableFormats);
		VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& avaiableModes);
		VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

	private:
		VkSwapchainKHR			 swapchain_		{ VK_NULL_HANDLE };
		std::vector<VkImage>	 images_;
		std::vector<VkImageView> imageViews_;
		VkFormat				 imageFormat_;
		VkExtent2D				 extent_; 

	};

	class RenderPass {
	public:

		void create(Device* device, Swapchain* swapchain);
		
		VkRenderPass get() const;

		~RenderPass();

	private:
		VkRenderPass			 renderPass_;
	};

	class Pipeline {
	public:

		void create(Device* device, RenderPass* renderPass, Swapchain* swapchain);

		VkPipeline get() const;
		VkPipelineLayout getLayout() const;

		~Pipeline();
	private:

		VkShaderModule createShaderModule(const std::vector<char>& code);
		std::vector<char> readFile(const std::string& path);

	private:
		VkPipelineLayout pipelineLayout_;
		VkPipeline pipeline_;

	};



	class CommandContext {
	public:
		void createCommandPool(Device* device);
		void createCommandBuffer(size_t count);

		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer cmdBuffer);

		VkCommandPool getPool() const;
		VkCommandBuffer getBuffer(size_t index) const;

		~CommandContext();

	private:
		VkCommandPool				 commandPool_;
		std::vector<VkCommandBuffer> commandBuffer_;
	};

	class FrameContext {
	public:
		void createSyncObjects(Device* device, size_t frameCount);

		void beginFrame(Swapchain* swapchain, uint32_t* imageIndexOut);
		void endFrame(Device* device, Swapchain* swapchain, uint32_t imageIndex, VkCommandBuffer cmdBuffer);

		void advanceFrame();
		size_t getCurrentFrame() const;

		std::vector<VkFramebuffer> createFramebuffer(Device* device, Swapchain* swapchain, RenderPass* renderPass);

		~FrameContext();

	private:
		std::vector<VkSemaphore>	 imageAvaiableSemaphores_;
		std::vector<VkSemaphore>	 renderFinishedSemaphores_;
		std::vector<VkFence>		 inFlightFence_;
		std::vector<VkFramebuffer>	 framebuffers_;
		size_t						 currentFrame_{ 0 };
	};

	class Buffer {
	public:
		void create(Device* device, VkDeviceSize deviceSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
		void copyFrom(Buffer* srcBuffer, CommandContext* cmdContext, VkDeviceSize size);
		void map(void** dataOut);
		void unmap();

		VkBuffer get() const;
		VkDeviceMemory getMemory() const;

		~Buffer();

	private:
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	private:
		VkBuffer		 buffer_;
		VkDeviceMemory	 memory_;
		VkDeviceSize	 size_;

	};



}

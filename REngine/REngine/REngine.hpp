#pragma once
#include <iostream>
#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>
#include<vector>
#include<optional>
#include<set>
#include<vk_mem_alloc.h>
namespace re {
	struct Vertex {
		float pos[2];
		float color[4];
	};

	struct ReVec2 {
		float x;
		float y;
	};
	struct ReVec3 {
		float x;
		float y;
		float z;
	};
	//color
	struct ReColor {
		float r, g, b, a;
	};

	void init(GLFWwindow* window);
	void render();
	class Device {
		
	public:

		void createInstance();
		void createSurface(GLFWwindow* window);
		void pickPhysicalDevice();
		void findQueueFamilies();
		void createLogicalDevice();
		void createAllocator();


		VkDevice get() const;
		VkPhysicalDevice getPhysicalDevice() const;
		VkInstance getInstance() const;
		VkSurfaceKHR getSurface() const;
		uint32_t getGraphicsQueueFamily() const;
		uint32_t getPresentQueueFamily() const;
		VkQueue getGraphicsQueue()const;
		VkQueue getPresentQueue()const;
		VmaAllocator getAllocator()const;

		
		
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
		VmaAllocator			allocator_					{ VK_NULL_HANDLE };

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
		Device* device_{ nullptr };

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
		Device*					 device_;
	};

	class Pipeline {
	public:

		//void create(Device* device, RenderPass* renderPass, Swapchain* swapchain);
		void createSolid(Device* device, RenderPass* renderPass, Swapchain* swapchain);

		VkPipeline get() const;
		VkPipelineLayout getLayout() const;

		~Pipeline();
	private:
		void createInternal(Device* device, RenderPass* renderPass, Swapchain* swapchain, const std::string& vertPath, const std::string& fragPath);

		VkShaderModule createShaderModule(const std::vector<char>& code);
		std::vector<char> readFile(const std::string& path);

	private:
		VkPipelineLayout	pipelineLayout_		{ VK_NULL_HANDLE };
		VkPipeline			pipeline_			{ VK_NULL_HANDLE };
		Device*				device_				{ nullptr };

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
		VkCommandPool				 commandPool_{ VK_NULL_HANDLE };
		std::vector<VkCommandBuffer> commandBuffer_;
		Device*						 device_;
	};

	class FrameContext {
	public:
		void createSyncObjects(Device* device, size_t frameCount);

		void beginFrame(Swapchain* swapchain, uint32_t* imageIndexOut);
		void endFrame(Device* device, Swapchain* swapchain, uint32_t imageIndex, VkCommandBuffer cmdBuffer);

		void advanceFrame();
		size_t getCurrentFrame() const;

		std::vector<VkFramebuffer> createFramebuffer(Device* device, Swapchain* swapchain, RenderPass* renderPass);
		VkFramebuffer getFramebuffer(uint32_t index) const;
		~FrameContext();

	private:
		std::vector<VkSemaphore>	 imageAvaiableSemaphores_;
		std::vector<VkSemaphore>	 renderFinishedSemaphores_;
		std::vector<VkFence>		 inFlightFence_;
		std::vector<VkFramebuffer>	 framebuffers_;
		size_t						 currentFrame_{ 0 };
		Device*						 device_;
	};

	class Buffer {
	public:
		void create(Device* device, VkDeviceSize deviceSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
		void copyFrom(Buffer* srcBuffer, CommandContext* cmdContext, VkDeviceSize size);
		void map(void** dataOut);
		void unmap();

		VkBuffer get() const;

		~Buffer();


	private:
		VkBuffer		 buffer_		{ VK_NULL_HANDLE };
		VmaAllocation	 allocation_	{ VK_NULL_HANDLE };
		VkDeviceSize	 size_			{ 0 };
		Device*			 device_		{ nullptr };

	};



}

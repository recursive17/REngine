#include "../rengine.hpp"
using namespace re;

void TextureManager::init(Device* device, CommandContext* cmdContext) {
	device_ = device;
	cmdContext_ = cmdContext;

    VkDescriptorSetLayoutBinding binding{};
    binding.binding = 0;
    binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    binding.descriptorCount = 1;
    binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &binding;

    if (vkCreateDescriptorSetLayout(device_->get(), &layoutInfo, nullptr, &setLayout_) != VK_SUCCESS) {
        re::ReLog::error(re::Prefix::VULKAN, "failed to create texture set layout");
    }

    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize.descriptorCount = MAX_TEXTURES;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = MAX_TEXTURES;

    if (vkCreateDescriptorPool(device_->get(), &poolInfo, nullptr, &pool_) != VK_SUCCESS) {
        re::ReLog::error(re::Prefix::VULKAN, "failed to create texture descriptor pool");
    }
}

Texture* TextureManager::load(const std::string& name, const std::string& path) {
    if (auto it = textures_.find(name); it != textures_.end()) {
        return it->second.get();
    }

    auto tex = std::make_unique<Texture>();
    tex->loadFromFile(device_, cmdContext_, pool_, setLayout_, path);
    Texture* raw = tex.get();
    textures_[name] = std::move(tex);
    re::ReLog::success(re::Prefix::RENDER, "Texture loaded: " + name);
    return raw;
}

Texture* TextureManager::loadFromMemory(const std::string& name, std::span<const unsigned char> bytes) {
    if (auto it = textures_.find(name); it != textures_.end()) {
        return it->second.get();
    }

    auto tex = std::make_unique<Texture>();
    tex->loadFromMemory(device_, cmdContext_, pool_, setLayout_, bytes);
    Texture* raw = tex.get();
    textures_[name] = std::move(tex);
    return raw;
}

Texture* TextureManager::get(const std::string& name) const {
    auto it = textures_.find(name);
    return it != textures_.end() ? it->second.get() : nullptr;
}

TextureManager::~TextureManager() {
    textures_.clear();
    if (pool_) vkDestroyDescriptorPool(device_->get(), pool_, nullptr);
    if (setLayout_) vkDestroyDescriptorSetLayout(device_->get(), setLayout_, nullptr);
}
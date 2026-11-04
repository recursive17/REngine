#include "../rengine.hpp"
#include <stb_image.h>
#include<cstring>
using namespace re;

static void transitionImageLayout(VkCommandBuffer cmd, VkImage image,
    VkImageLayout oldLayout, VkImageLayout newLayout) {
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags srcStage, dstStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
        newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }

    vkCmdPipelineBarrier(cmd, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void Texture::loadFromFile(Device* device, CommandContext* cmdContext,
    VkDescriptorPool pool, VkDescriptorSetLayout layout,
    const std::string& path) {
    device_ = device;
    cmdContext_ = cmdContext;

    int w, h, channels;

    stbi_uc* pixels = stbi_load(path.c_str(), &w, &h, &channels, STBI_rgb_alpha);
    if (!pixels) {
        re::ReLog::error(re::Prefix::VULKAN, "failed to load texture: " + path);
    }

    createFromPixels(pixels, w, h);
    stbi_image_free(pixels);

    createImageView();
    createSampler();
    allocateDescriptorSet(pool, layout);
}

void Texture::loadFromMemory(Device* device, CommandContext* cmdContext,
    VkDescriptorPool pool, VkDescriptorSetLayout layout,
    std::span<const unsigned char> fileBytes) {
    device_ = device;
    cmdContext_ = cmdContext;

    int w, h, channels;
    stbi_uc* pixels = stbi_load_from_memory(fileBytes.data(),
        static_cast<int>(fileBytes.size()), &w, &h, &channels, STBI_rgb_alpha);
    if (!pixels) {
        re::ReLog::error(re::Prefix::VULKAN, "failed to load texture from memory");
    }

    createFromPixels(pixels, w, h);
    stbi_image_free(pixels);

    createImageView();
    createSampler();
    allocateDescriptorSet(pool, layout);
}

void Texture::createFromPixels(const unsigned char* pixels, int w, int h) {
    width_ = static_cast<uint32_t>(w);
    height_ = static_cast<uint32_t>(h);
    VkDeviceSize imageSize = VkDeviceSize(width_) * height_ * 4;

    Buffer staging;
    staging.create(device_, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_AUTO);
    void* data;
    staging.map(&data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    staging.unmap();

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent = { width_, height_, 1 };
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

    if (vmaCreateImage(device_->getAllocator(), &imageInfo, &allocInfo,
        &image_, &allocation_, nullptr) != VK_SUCCESS) {
        re::ReLog::error(re::Prefix::VULKAN, "failed to create texture image");
    }

    VkCommandBuffer cmd = cmdContext_->beginSingleTimeCommands();

    transitionImageLayout(cmd, image_, VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VkBufferImageCopy region{};
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.layerCount = 1;
    region.imageExtent = { width_, height_, 1 };
    vkCmdCopyBufferToImage(cmd, staging.get(), image_,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    transitionImageLayout(cmd, image_, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    cmdContext_->endSingleTimeCommands(cmd);
}

void Texture::createImageView() {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image_;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device_->get(), &viewInfo, nullptr, &imageView_) != VK_SUCCESS) {
        re::ReLog::error(re::Prefix::VULKAN, "failed to create texture image view");
    }
}

void Texture::createSampler() {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;   
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.maxLod = 0.0f;

    if (vkCreateSampler(device_->get(), &samplerInfo, nullptr, &sampler_) != VK_SUCCESS) {
        re::ReLog::error(re::Prefix::VULKAN, "failed to create texture sampler");
    }
}

void Texture::allocateDescriptorSet(VkDescriptorPool pool, VkDescriptorSetLayout layout) {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = pool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    if (vkAllocateDescriptorSets(device_->get(), &allocInfo, &descriptorSet_) != VK_SUCCESS) {
        re::ReLog::error(re::Prefix::VULKAN, "failed to allocate texture descriptor set");
    }

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = imageView_;
    imageInfo.sampler = sampler_;

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = descriptorSet_;
    write.dstBinding = 0;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.descriptorCount = 1;
    write.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(device_->get(), 1, &write, 0, nullptr);
}

Texture::~Texture() {
    if (!device_) return;
    if (sampler_)   vkDestroySampler(device_->get(), sampler_, nullptr);
    if (imageView_) vkDestroyImageView(device_->get(), imageView_, nullptr);
    if (image_)     vmaDestroyImage(device_->getAllocator(), image_, allocation_);
}

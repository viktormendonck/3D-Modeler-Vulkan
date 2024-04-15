#pragma once

#include <Renderer/VECamera.h>

#include <vulkan/vulkan.h>

namespace VE{
    struct FrameInfo{
        int frameIndex;
        float frameTime;
        VECamera& camera;
        VkCommandBuffer commandBuffer;
        VkDescriptorSet globalDescriptorSet;
    };
}
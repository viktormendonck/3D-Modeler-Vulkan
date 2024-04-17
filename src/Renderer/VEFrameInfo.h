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

    #define MAX_LIGHTS 8

    struct PointLight{
        glm::vec4 color{1.f};
        glm::vec4 position{0.f};
    };

    struct GlobalUbo{
        glm::mat4 ProjectionMatrix{1.f};
        glm::mat4 viewMatrix{1.f};
        glm::vec4 ambientLightColor{1.f,1.f,1.f,.02f}; //using alpha for ambient light intensity
        PointLight pointLights[MAX_LIGHTS];
        int numPointLights{0};
    };
}
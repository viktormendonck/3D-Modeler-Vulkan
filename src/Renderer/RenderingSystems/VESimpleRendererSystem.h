#pragma once

#include <vulkan/vulkan.h>
#include "Renderer/VEDevice.hpp"
#include "Renderer/VEWindow.hpp"
#include "Renderer/VEPipeLine.hpp"
#include "Renderer/VECamera.h"
#include "Renderer/VEFrameInfo.h"



#include "Game/GameObject.h"

#include <memory>
#include <vector>

namespace VE{
    class SimpleRenderingSystem{
    public:
        SimpleRenderingSystem(VEDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalDescriptorSet);
        ~SimpleRenderingSystem();

        SimpleRenderingSystem(const SimpleRenderingSystem&) = delete;
        SimpleRenderingSystem& operator=(const SimpleRenderingSystem&) = delete;

        void RenderGameObjects(FrameInfo& frameInfo, std::vector<ModelObject>& gameObjects);
    private:
        void CreatePipelineLayout(VkDescriptorSetLayout globalDescriptorSet);
        void CreatePipeline(VkRenderPass renderPass);
        
        void Update(float deltaTime);
        float m_CurrentWaitTime = 0.0f; // for the tri update
        float m_WaitTime = 1.f; // for the tri update
        
        VEDevice& m_Device;

        std::unique_ptr<VEPipeline> m_Pipeline;
        VkPipelineLayout m_PipelineLayout;
        

    };
}
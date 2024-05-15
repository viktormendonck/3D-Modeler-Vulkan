#pragma once

#include <vulkan/vulkan.h>
#include "Renderer/VEDevice.hpp"
#include "Renderer/VEWindow.hpp"
#include "Renderer/VEPipeline.hpp"
#include "Renderer/VECamera.h"
#include "Renderer/VEFrameInfo.h"



#include "Game/GameObject.h"

#include <memory>
#include <vector>

namespace VE{
    class SimpleRenderingSystem{
    public:
        SimpleRenderingSystem(VEDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalDescriptorSet);
        SimpleRenderingSystem();
        ~SimpleRenderingSystem();
        
        
        //default rule of 5
        SimpleRenderingSystem(const SimpleRenderingSystem&) = default;
        SimpleRenderingSystem& operator=(const SimpleRenderingSystem&) = default;
        SimpleRenderingSystem(SimpleRenderingSystem&&) = default;
        SimpleRenderingSystem& operator=(SimpleRenderingSystem&&) = default;

        void RenderGameObjects(FrameInfo& frameInfo, std::vector<ModelObject>& gameObjects);
    private:
        void CreatePipelineLayout(VkDescriptorSetLayout globalDescriptorSet);
        void CreatePipeline(VkRenderPass renderPass);
        
        float m_CurrentWaitTime = 0.0f; // for the tri update
        float m_WaitTime = 1.f; // for the tri update
        
        VEDevice& m_Device;

        std::unique_ptr<VEPipeline> m_Pipeline;
        VkPipelineLayout m_PipelineLayout;
        

    };
}
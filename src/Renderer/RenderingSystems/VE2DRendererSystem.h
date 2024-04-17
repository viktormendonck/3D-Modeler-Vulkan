#pragma once

#include <vulkan/vulkan.h>
#include "Renderer/VEDevice.hpp"
#include "Renderer/VEWindow.hpp"
#include "Renderer/VEPipeLine.hpp"
#include "Renderer/VECamera.h"
#include "Renderer/VEFrameInfo.h"
#include "Game/PointLight.h"



#include "Game/GameObject.h"

#include <memory>
#include <vector>

namespace VE{
    class Simple2DRenderingSystem{
    public:
        Simple2DRenderingSystem(VEDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalDescriptorSet);
        ~Simple2DRenderingSystem();

        Simple2DRenderingSystem(const Simple2DRenderingSystem&) = delete;
        Simple2DRenderingSystem& operator=(const Simple2DRenderingSystem&) = delete;


        void RenderGameObjects(FrameInfo& frameInfo,std::vector<ModelObject>& gameObjects);
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
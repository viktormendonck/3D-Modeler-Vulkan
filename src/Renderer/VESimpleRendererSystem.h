#pragma once

#include <vulkan/vulkan.h>
#include "VEDevice.hpp"
#include "VEWindow.hpp"
#include "VEPipeLine.hpp"
#include "VECamera.h"



#include "../Game/GameObject.h"

#include <memory>
#include <vector>

namespace VE{
    class SimpleRenderingSystem{
    public:
        SimpleRenderingSystem(VEDevice& device, VkRenderPass renderPass);
        ~SimpleRenderingSystem();

        SimpleRenderingSystem(const SimpleRenderingSystem&) = delete;
        SimpleRenderingSystem& operator=(const SimpleRenderingSystem&) = delete;

        void RenderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects, const VECamera& camera);
    private:
        void CreatePipelineLayout();
        void CreatePipeline(VkRenderPass renderPass);
        
        void Update(float deltaTime);
        float m_CurrentWaitTime = 0.0f; // for the tri update
        float m_WaitTime = 1.f; // for the tri update
        
        VEDevice& m_Device;

        std::unique_ptr<VEPipeline> m_Pipeline;
        VkPipelineLayout m_PipelineLayout;
        

    };
}
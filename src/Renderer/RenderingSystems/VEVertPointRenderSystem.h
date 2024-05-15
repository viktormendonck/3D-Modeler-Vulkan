#pragma once

#include <vulkan/vulkan.h>
#include "Renderer/VEDevice.hpp"
#include "Renderer/VEWindow.hpp"
#include "Renderer/VEPipeline.hpp"
#include "Renderer/VECamera.h"
#include "Renderer/VEFrameInfo.h"
#include "Game/PointLight.h"



#include "Game/GameObject.h"

#include <memory>
#include <vector>

namespace VE{
    class VertexPointSystem{
    public:
        VertexPointSystem(VEDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalDescriptorSet);
        ~VertexPointSystem();

        //default rule of 5
        VertexPointSystem(const VertexPointSystem&) = default;
        VertexPointSystem& operator=(const VertexPointSystem&) = default;
        VertexPointSystem(VertexPointSystem&&) = default;
        VertexPointSystem& operator=(VertexPointSystem&&) = default;


        void Update(GlobalUbo& globalUbo);
        void Render(GLFWwindow* window,FrameInfo& frameInfo,ModelObject* model);
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
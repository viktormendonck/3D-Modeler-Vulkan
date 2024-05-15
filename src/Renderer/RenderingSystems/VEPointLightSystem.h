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
    class PointLightSystem{
    public:
        PointLightSystem(VEDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalDescriptorSet,std::vector<PointLightObject>& pointLights);
        PointLightSystem();
        ~PointLightSystem();

        //default rule of 5
        PointLightSystem(const PointLightSystem&) = default;
        PointLightSystem& operator=(const PointLightSystem&) = default;
        PointLightSystem(PointLightSystem&&) = default;
        PointLightSystem& operator=(PointLightSystem&&) = default;


        void Update(GlobalUbo& globalUbo);
        void Render(FrameInfo& frameInfo);
    private:
        void CreatePipelineLayout(VkDescriptorSetLayout globalDescriptorSet);
        void CreatePipeline(VkRenderPass renderPass);
        
        void Update(float deltaTime);
        float m_CurrentWaitTime = 0.0f; // for the tri update
        float m_WaitTime = 1.f; // for the tri update
        
        VEDevice& m_Device;

        std::unique_ptr<VEPipeline> m_Pipeline;
        VkPipelineLayout m_PipelineLayout;
        std::vector<PointLightObject>& m_PointLights;
        

    };
}
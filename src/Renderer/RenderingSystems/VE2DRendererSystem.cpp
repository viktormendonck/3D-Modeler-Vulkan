#include "VE2DRendererSystem.h"
#include <stdexcept>
#include <array>
#include <thread>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace VE{   

    struct Simple2DPushConstantData
    {
        glm::mat4 modelMatrix{1.f};
    };

    Simple2DRenderingSystem::Simple2DRenderingSystem(VEDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalDescriptorSet)
        : m_Device(device)
    {
        CreatePipelineLayout(globalDescriptorSet);
        CreatePipeline(renderPass);
    }

    Simple2DRenderingSystem::~Simple2DRenderingSystem()
    {
        vkDestroyPipelineLayout(m_Device.device(), m_PipelineLayout, nullptr);
    }

   

    void Simple2DRenderingSystem::CreatePipelineLayout(VkDescriptorSetLayout globalDescriptorSet)
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(Simple2DPushConstantData);        

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};

        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(m_Device.device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }
    
    
    void Simple2DRenderingSystem::CreatePipeline(VkRenderPass renderPass)
    {
        if (!m_PipelineLayout) throw std::runtime_error("cannot create pipeline without its layout");


        PipelineConfigInfo pipelineConfig{};
        VEPipeline::DefaultPipelineConfigInfo(pipelineConfig);

        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = m_PipelineLayout;

        m_Pipeline = std::make_unique<VEPipeline>(
            m_Device,
            "Shaders/Simple2DShader.vert.spv",
            "Shaders/Simple2DShader.frag.spv",
            pipelineConfig
        );
    }

     void Simple2DRenderingSystem::RenderGameObjects(FrameInfo &frameInfo, std::vector<ModelObject> &gameObjects)
    {
    
        m_Pipeline->Bind(frameInfo.commandBuffer);
        for (auto& obj : gameObjects)
        {
            Simple2DPushConstantData push{};
            push.modelMatrix = obj.GetTransform().GetTransformationMatrix();

            vkCmdPushConstants(frameInfo.commandBuffer,
                m_PipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(Simple2DPushConstantData),
                &push);

            obj.GetModel()->Bind(frameInfo.commandBuffer);
            obj.GetModel()->Draw(frameInfo.commandBuffer);
        }
    }
}
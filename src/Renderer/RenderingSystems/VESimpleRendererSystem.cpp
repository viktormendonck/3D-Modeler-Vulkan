#include "VESimpleRendererSystem.h"
#include <stdexcept>
#include <array>
#include <thread>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace VE{
    
    struct SimplePushConstantData
    {
        glm::mat4 modelMatrix{1.f};
        glm::mat4 normalMatrix{1.f};
    };

    
    SimpleRenderingSystem::SimpleRenderingSystem(VEDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalDescriptorSet)
        : m_Device(device)
    {
        CreatePipelineLayout(globalDescriptorSet);
        CreatePipeline(renderPass);
    }

    SimpleRenderingSystem::~SimpleRenderingSystem()
    {
        vkDestroyPipelineLayout(m_Device.device(), m_PipelineLayout, nullptr);
    }

    

    void SimpleRenderingSystem::CreatePipelineLayout(VkDescriptorSetLayout globalDescriptorSet)
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);        

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalDescriptorSet};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size()); 
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data(); 
        pipelineLayoutInfo.pushConstantRangeCount = 1; 
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange; 
        if (vkCreatePipelineLayout(m_Device.device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }
    
    
    void SimpleRenderingSystem::CreatePipeline(VkRenderPass renderPass)
    {
        if (!m_PipelineLayout) throw std::runtime_error("cannot create pipeline without its layout");


        PipelineConfigInfo pipelineConfig{};
        VEPipeline::DefaultPipelineConfigInfo(pipelineConfig);
        

        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = m_PipelineLayout;

        //TODO fix what the fuck is going on in the make_unique here
        m_Pipeline = std::make_unique<VEPipeline>(
            m_Device,
            "Shaders/SimpleShader.vert.spv",
            "Shaders/SimpleShader.frag.spv",
            pipelineConfig
        );
    }

    void SimpleRenderingSystem::RenderGameObjects(FrameInfo &frameInfo, std::vector<ModelObject> &gameObjects)
    {
        m_Pipeline->Bind(frameInfo.commandBuffer);
        
        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS, 
            m_PipelineLayout, 0 ,1,
            &frameInfo.globalDescriptorSet,0,nullptr
        );

        for (auto& obj : gameObjects)
        {
            SimplePushConstantData push{};
            push.modelMatrix = obj.GetTransform().GetTransformationMatrix();
            push.normalMatrix = obj.GetTransform().GetNormalMatrix();
            vkCmdPushConstants(frameInfo.commandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
            obj.GetModel()->Bind(frameInfo.commandBuffer);
            obj.GetModel()->Draw(frameInfo.commandBuffer);
        }
    }
}
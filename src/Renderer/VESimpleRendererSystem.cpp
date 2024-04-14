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
        glm::mat4 transform{1.f};
        alignas(16) glm::vec3 color;
    };

    
    SimpleRenderingSystem::SimpleRenderingSystem(VEDevice& device, VkRenderPass renderPass)
        : m_Device(device)
    {
        CreatePipelineLayout();
        CreatePipeline(renderPass);
    }

    SimpleRenderingSystem::~SimpleRenderingSystem()
    {
        vkDestroyPipelineLayout(m_Device.device(), m_PipelineLayout, nullptr);
    }
    

    void SimpleRenderingSystem::CreatePipelineLayout()
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);        


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

    void SimpleRenderingSystem::RenderGameObjects(VkCommandBuffer commandBuffer,std::vector<GameObject>& GameObjects, const VECamera& camera)
    {
        m_Pipeline->Bind(commandBuffer);
        
        auto projectionView = camera.GetProjectionMatrix() * camera.GetViewMatrix();

        for (auto& obj : GameObjects)
        {
            SimplePushConstantData push{};
            push.color = obj.GetColor();
            push.transform = projectionView * obj.GetTransform().GetTransformationMatrix();
            vkCmdPushConstants(commandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
            obj.GetModel()->Bind(commandBuffer);
            obj.GetModel()->Draw(commandBuffer);
        }
    }
}
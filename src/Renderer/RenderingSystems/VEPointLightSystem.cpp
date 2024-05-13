#include "VEPointLightSystem.h"
#include <stdexcept>
#include <array>
#include <thread>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace VE{   

    struct PointLightPushConstantData
    {
        glm::vec4 position{};
        glm::vec4 color{};
        float radius{};
    };
    
    PointLightSystem::PointLightSystem(VEDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalDescriptorSet,std::vector<PointLightObject>& pointLights)
        : m_Device(device),
        m_PointLights(pointLights)
    {
        CreatePipelineLayout(globalDescriptorSet);
        CreatePipeline(renderPass);
    }

    PointLightSystem::~PointLightSystem()
    {
        vkDestroyPipelineLayout(m_Device.device(), m_PipelineLayout, nullptr);
    }

    

    void PointLightSystem::CreatePipelineLayout(VkDescriptorSetLayout globalDescriptorSet)
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(PointLightPushConstantData);        

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
    
    
    void PointLightSystem::CreatePipeline(VkRenderPass renderPass)
    {
        if (!m_PipelineLayout) throw std::runtime_error("cannot create pipeline without its layout");


        PipelineConfigInfo pipelineConfig{};
        VEPipeline::DefaultPipelineConfigInfo(pipelineConfig);
        
        pipelineConfig.attributeDescriptions.clear();
        pipelineConfig.bindingDescriptions.clear();
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = m_PipelineLayout;

        //TODO fix what the fuck is going on in the make_unique here
        m_Pipeline = std::make_unique<VEPipeline>(
            m_Device,
            "Shaders/PointLightShader.vert.spv",
            "Shaders/PointLightShader.frag.spv",
            pipelineConfig
        );
    }

    void PointLightSystem::Update(GlobalUbo& ubo)
    {
        int lightIndex{};
        for (auto& light: m_PointLights)
        {
            ubo.pointLights[lightIndex].position = glm::vec4{light.GetTransform().pos,1.f};
            ubo.pointLights[lightIndex].color = glm::vec4{light.GetColor(),light.GetIntensity()};
            lightIndex++;
        }
        ubo.numPointLights = lightIndex;
    }

    void PointLightSystem::Render(FrameInfo &frameInfo)
    {
        m_Pipeline->Bind(frameInfo.commandBuffer);
        
        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS, 
            m_PipelineLayout, 0 ,1,
            &frameInfo.globalDescriptorSet,0,nullptr
        );
        for (auto& light: m_PointLights)
        {
            PointLightPushConstantData push{};
            push.position = glm::vec4{light.GetTransform().pos,1.f};
            push.color = glm::vec4{light.GetColor(),light.GetIntensity()};
            push.radius = light.GetRadius();
            vkCmdPushConstants(
                frameInfo.commandBuffer,
                m_PipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(PointLightPushConstantData),
                &push
            );
            vkCmdDraw(frameInfo.commandBuffer,6,1,0,0);
        }
        
    }
}
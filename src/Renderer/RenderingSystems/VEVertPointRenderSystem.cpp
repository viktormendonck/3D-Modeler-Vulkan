#include "VEVertPointRenderSystem.h"
#include <stdexcept>
#include <array>
#include <thread>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace VE{   

    struct VertexPointPushConstantData
    {
        glm::vec4 position{};
        glm::vec4 color{};
        float radius{};
        glm::mat4 modelMatrix{1.f};
    };
    
    VertexPointSystem::VertexPointSystem(VEDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalDescriptorSet)
        : m_Device{device}
    {
        CreatePipelineLayout(globalDescriptorSet);
        CreatePipeline(renderPass);
    }

    VertexPointSystem::~VertexPointSystem()
    {
        vkDestroyPipelineLayout(m_Device.device(), m_PipelineLayout, nullptr);
    }
    

    void VertexPointSystem::CreatePipelineLayout(VkDescriptorSetLayout globalDescriptorSet)
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(VertexPointPushConstantData);        

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
    
    
    void VertexPointSystem::CreatePipeline(VkRenderPass renderPass)
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

    

    void VertexPointSystem::Render(GLFWwindow* window,FrameInfo &frameInfo,ModelObject* model)
    {
        m_Pipeline->Bind(frameInfo.commandBuffer);
        
        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS, 
            m_PipelineLayout, 0 ,1,
            &frameInfo.globalDescriptorSet,0,nullptr
        );
        
        for (auto& vert: model->GetModel()->GetVertices())
        {
            VertexPointPushConstantData push{};
            push.position =  glm::vec4{vert.position,1.f} * model->GetTransform().GetTransformationMatrix();
            push.color = glm::vec4{vert.color,1};
            if (vert.selected == true){
                push.color = glm::vec4{1,0,0,1};
            }
            push.radius = 0.2f;
            push.modelMatrix = model->GetTransform().GetTransformationMatrix();
            vkCmdPushConstants(
                frameInfo.commandBuffer,
                m_PipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(VertexPointPushConstantData),
                &push
            );
            if (glfwGetKey(window,GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || vert.selected == true)
                vkCmdDraw(frameInfo.commandBuffer,6,1,0,0);
        }

         m_Pipeline->Bind(frameInfo.commandBuffer);
        
        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS, 
            m_PipelineLayout, 0 ,1,
            &frameInfo.globalDescriptorSet,0,nullptr
        );
    }
}
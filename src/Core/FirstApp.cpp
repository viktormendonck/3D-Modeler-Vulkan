#include "FirstApp.hpp"
#include <stdexcept>

namespace VE{
    FirstApp::FirstApp()
    {
        createPipelineLayout();
        createPipeline();
        createCommandBuffers();
    }

    FirstApp::~FirstApp()
    {
        vkDestroyPipelineLayout(m_Device.device(), m_PipelineLayout, nullptr);
    }

    void VE::FirstApp::run()
    {
        while (!m_Window.ShouldClose())
        {
            glfwPollEvents();
        }
    }
    void FirstApp::createPipelineLayout()
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0; 
        pipelineLayoutInfo.pSetLayouts = nullptr; 
        pipelineLayoutInfo.pushConstantRangeCount = 0; 
        pipelineLayoutInfo.pPushConstantRanges = nullptr; 
        if (vkCreatePipelineLayout(m_Device.device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }
    void FirstApp::createPipeline()
    {
        PipelineConfigInfo pipelineConfig{
            VEPipeline::DefaultPipelineConfigInfo(m_SwapChain.width(), m_SwapChain.height())
        };

        pipelineConfig.renderPass = m_SwapChain.getRenderPass();
        pipelineConfig.pipelineLayout = m_PipelineLayout;

        //TODO fix what the fuck is going on in the make_unique here
        m_Pipeline = std::make_unique<VEPipeline>(
            m_Device,
            "Shaders/SimpleShader.vert.spv",
            "Shaders/SimpleShader.frag.spv",
            pipelineConfig
        );
    }
    void FirstApp::createCommandBuffers()
    {

    }
    void FirstApp::drawFrame()
    {

    }
}
#include "ModelingApp.hpp"
#include <stdexcept>
#include <array>
#include <chrono>
#include <thread>

namespace VE{
    ModelingApp::ModelingApp()
    {
        LoadModels();
        CreatePipelineLayout();
        RecreateSwapChain();
        CreateCommandBuffers();
    }

    ModelingApp::~ModelingApp()
    {
        vkDestroyPipelineLayout(m_Device.device(), m_PipelineLayout, nullptr);
    }
    
    void VE::ModelingApp::run()
    {
        const int targetFrameTimeMS = 1000 / 60;
        auto lastFrameStartTime = std::chrono::high_resolution_clock::now();

        while (!m_Window.ShouldClose())
        {
            glfwPollEvents();
	        const auto start = std::chrono::high_resolution_clock::now();
	        const float delta{ std::chrono::duration<float>(start - lastFrameStartTime).count() };
	        lastFrameStartTime = start;

            Update(delta);
            DrawFrame();
	        const auto sleepTime = start + std::chrono::milliseconds(targetFrameTimeMS) - std::chrono::high_resolution_clock::now();
	        std::this_thread::sleep_for(sleepTime);
        }
        vkDeviceWaitIdle(m_Device.device());
    }

    void ModelingApp::LoadModels()
    {
        std::vector<VEModel::Vertex> vertices = {};
        Sierpinski(0, {{0.0f, -0.9f, 0.0f},{1.0f,0.0f,0.0f}}, {{0.9f, 0.9f, 0.0f},{0.0f,1.0f,0.0f}}, {{-0.9f, 0.9f, 0.0f},{0.0f,0.0f,1.0f}}, vertices);
        m_Model = std::make_unique<VEModel>(m_Device, vertices);
    }
    void ModelingApp::CreatePipelineLayout()
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
    void ModelingApp::RecreateSwapChain()
    {
        auto extent = m_Window.GetExtent();
        while (extent.width == 0 || extent.height == 0) // pause during minimisation
        {
            extent = m_Window.GetExtent();
            glfwWaitEvents();
        }
        vkDeviceWaitIdle(m_Device.device());
        if (m_SwapChain) {
            m_SwapChain = std::make_unique<VESwapChain>(m_Device, extent, std::move(m_SwapChain));
        }
        else{
            m_SwapChain = std::make_unique<VESwapChain>(m_Device, extent);
        }
        CreatePipeline();
    }
    
    void ModelingApp::CreatePipeline()
    {
        PipelineConfigInfo pipelineConfig{
            VEPipeline::DefaultPipelineConfigInfo(m_SwapChain->width(), m_SwapChain->height())
        };

        pipelineConfig.renderPass = m_SwapChain->getRenderPass();
        pipelineConfig.pipelineLayout = m_PipelineLayout;

        //TODO fix what the fuck is going on in the make_unique here
        m_Pipeline = std::make_unique<VEPipeline>(
            m_Device,
            "Shaders/SimpleShader.vert.spv",
            "Shaders/SimpleShader.frag.spv",
            pipelineConfig
        );
    }
    
    void ModelingApp::CreateCommandBuffers()
    {
        m_CommandBuffers.resize(m_SwapChain->imageCount());
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_Device.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

        if (vkAllocateCommandBuffers(m_Device.device(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error{"Failed to allocate command buffers!"};
        }

        
    }

    void ModelingApp::DrawFrame()
    {
        uint32_t imageIndex;
        VkResult result = m_SwapChain->acquireNextImage(&imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR){
            RecreateSwapChain();
            return;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("Failed to acquire next image!");
        }
        
        RecordCommandBuffer(imageIndex);
        result = m_SwapChain->submitCommandBuffers(&m_CommandBuffers[imageIndex], &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window.WasWindowResized())
        {
            m_Window.ResetWindowResizedFlag();
            RecreateSwapChain();
            return;
        }
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to submit command buffer!");
        }

    }


    void ModelingApp::RecordCommandBuffer(int imageIndex)
    {
        VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            if (vkBeginCommandBuffer(m_CommandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to begin recording command buffer!");
            }
            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = m_SwapChain->getRenderPass();
            renderPassInfo.framebuffer = m_SwapChain->getFrameBuffer(imageIndex);
            //set render area
            renderPassInfo.renderArea.offset = {0,0};
            renderPassInfo.renderArea.extent = m_SwapChain->getSwapChainExtent();
            //set clear values
            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {0.009f, 0.009f, 0.01f, 1.0f}; // set background color
            clearValues[1].depthStencil = {1.0f, 0};
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(m_CommandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            m_Pipeline->Bind(m_CommandBuffers[imageIndex]);
            m_Model->Bind(m_CommandBuffers[imageIndex]);
            m_Model->Draw(m_CommandBuffers[imageIndex]);

            vkCmdEndRenderPass(m_CommandBuffers[imageIndex]);
            if (vkEndCommandBuffer(m_CommandBuffers[imageIndex]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to record command buffer!");
            }
    }

    void ModelingApp::Update(float deltaTime)
    {
       
    }

   void ModelingApp::Sierpinski(int step, VEModel::Vertex a, VEModel::Vertex b, VEModel::Vertex c, std::vector<VEModel::Vertex>& points) {
    if (step == 0) {
        points.push_back(a);
        points.push_back(b);
        points.push_back(c);
        return;
    }

    VEModel::Vertex ab{{(a.position.x + b.position.x) / 2, (a.position.y + b.position.y) / 2, 0.0f},{1.0f,0.0f,0.0f}};
    VEModel::Vertex bc{{(b.position.x + c.position.x) / 2, (b.position.y + c.position.y) / 2, 0.0f},{0.0f,1.0f,0.0f}};
    VEModel::Vertex ca{{(c.position.x + a.position.x) / 2, (c.position.y + a.position.y) / 2, 0.0f},{0.0f,0.0f,1.0f}};

    Sierpinski(step - 1, a, ab, ca, points);
    Sierpinski(step - 1, ab, b, bc, points);
    Sierpinski(step - 1, ca, bc, c, points);
}
}
#include "VERenderer.h"
#include <stdexcept>
#include <array>
#include <chrono>
#include <thread>


namespace VE{
    VERenderer::VERenderer(VEWindow& window, VEDevice& device)
    : m_Window(window), m_Device(device)
    {
        RecreateSwapChain();
        CreateCommandBuffers();
    }

    VERenderer::~VERenderer()
    {
        FreeCommandBuffers();
    }

    VkCommandBuffer VERenderer::BeginFrame()
    {
        if (m_IsFrameStarted)
        {
            throw std::runtime_error("Called BeginFrame while frame is already in progress!");
        }
        
        VkResult result = m_SwapChain->acquireNextImage(&m_CurrentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR){
            RecreateSwapChain();
            return nullptr;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("Failed to acquire next image!");
        }
        m_IsFrameStarted = true;
        VkCommandBuffer commandBuffer = GetCurrentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to begin recording command buffer!");
        }
        return commandBuffer;
    }

    void VERenderer::EndFrame()
    {
        if (!m_IsFrameStarted)
        {
            throw std::runtime_error("Called EndFrame while frame is not in progress!");
        }
        VkCommandBuffer commandBuffer = GetCurrentCommandBuffer();
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to record command buffer!");
        }
        auto result = m_SwapChain->submitCommandBuffers(&commandBuffer, &m_CurrentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window.WasWindowResized())
        {
            m_Window.ResetWindowResizedFlag();
            RecreateSwapChain();
        } else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to submit command buffer!");
        }
        m_IsFrameStarted = false;
        m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % VESwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void VERenderer::BeginSwapChainRenderpass(VkCommandBuffer commandBuffer)
    {
        
        if (!m_IsFrameStarted)
        {
            throw std::runtime_error("Can't begin render pass when frame is not started!");
        }
        if (commandBuffer != GetCurrentCommandBuffer())
        {
            throw std::runtime_error("Can't begin render pass on command buffer from a different frame!");
        }
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_SwapChain->GetRenderPass();
        renderPassInfo.framebuffer = m_SwapChain->getFrameBuffer(m_CurrentImageIndex);
        //set render area
        renderPassInfo.renderArea.offset = {0,0};
        renderPassInfo.renderArea.extent = m_SwapChain->getSwapChainExtent();
        //set clear values
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.009f, 0.009f, 0.01f, 1.0f}; // set background color
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_SwapChain->width());
        viewport.height = static_cast<float>(m_SwapChain->height());
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, m_SwapChain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor (commandBuffer, 0, 1, &scissor);
    }

    void VERenderer::EndSwapChainRenderpass(VkCommandBuffer commandBuffer)
    {
        if (!m_IsFrameStarted)
        {
            throw std::runtime_error("Can't end render pass when frame is not started!");
        }
        if (commandBuffer != GetCurrentCommandBuffer())
        {
            throw std::runtime_error("Can't end render pass on command buffer from a different frame!");
        }
        vkCmdEndRenderPass(commandBuffer);
    }

    void VERenderer::RecreateSwapChain()
    {
        auto extent = m_Window.GetExtent();
        while (extent.width == 0 || extent.height == 0) // pause during minimisation
        {
            extent = m_Window.GetExtent();
            glfwWaitEvents();
        }
        vkDeviceWaitIdle(m_Device.device());
        if (m_SwapChain) {
            std::shared_ptr<VESwapChain> oldSwapChain = std::move(m_SwapChain);
            m_SwapChain = std::make_unique<VESwapChain>(m_Device, extent, oldSwapChain);

            if (!oldSwapChain->compareSwapFormats(*m_SwapChain.get()))
            {
                throw std::runtime_error("Swap chain image (or depth) format has changed!");
            }
        }
        else{
            m_SwapChain = std::make_unique<VESwapChain>(m_Device, extent);
        }
        //TODO: recreate pipeline
    }
    
    
    void VERenderer::CreateCommandBuffers()
    {
        m_CommandBuffers.resize(VESwapChain::MAX_FRAMES_IN_FLIGHT);
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

    void VERenderer::FreeCommandBuffers()
    {
        vkFreeCommandBuffers(
            m_Device.device(),
            m_Device.getCommandPool(),
            static_cast<uint32_t>(m_CommandBuffers.size()),
            m_CommandBuffers.data()
        );
        m_CommandBuffers.clear();
    }

   
    VkCommandBuffer VERenderer::GetCurrentCommandBuffer() const
    {
        if (!m_IsFrameStarted)
        {
            throw std::runtime_error("Cannot get command buffer when frame is not started");
        }
        return m_CommandBuffers[m_CurrentFrameIndex];
    }
    int VERenderer::getFrameIndex() const
    {
        if (!m_IsFrameStarted)
        {
            throw std::runtime_error("Cannot get frame index when frame is not started");
        }
        return m_CurrentFrameIndex;
    }
} // namespace VE
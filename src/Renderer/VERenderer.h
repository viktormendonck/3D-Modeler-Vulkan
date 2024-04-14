#pragma once

#include <vulkan/vulkan.h>
#include "VEDevice.hpp"
#include "VEWindow.hpp"
#include "VESwapChain.hpp"

#include <memory>
#include <vector>


namespace VE{
    class VERenderer{
    public:

        VERenderer(VEWindow& window, VEDevice& device);
        ~VERenderer();

        VERenderer(const VERenderer&) = delete;
        VERenderer& operator=(const VERenderer&) = delete;
        
        VkCommandBuffer BeginFrame();
        void EndFrame();
        void BeginSwapChainRenderpass(VkCommandBuffer commandBuffer);
        void EndSwapChainRenderpass(VkCommandBuffer commandBuffer);
        bool isFrameStarted() const { return m_IsFrameStarted; }
        VkCommandBuffer GetCurrentCommandBuffer() const;
        VkRenderPass GetSwapChainRenderPass() const { return m_SwapChain->GetRenderPass(); }
        float GetAspectRatio() const { return m_SwapChain->extentAspectRatio(); }
        int getFrameIndex() const;


    private:
        void CreateCommandBuffers();
        void FreeCommandBuffers();
        void RecreateSwapChain();
       
        
        void Update(float deltaTime);
        float m_CurrentWaitTime = 0.0f; // for the tri update
        float m_WaitTime = 1.f; // for the tri update
        
        VEWindow& m_Window;
        VEDevice& m_Device;
        std::unique_ptr<VESwapChain> m_SwapChain;
        std::vector<VkCommandBuffer> m_CommandBuffers;

        uint32_t m_CurrentImageIndex;
        int m_CurrentFrameIndex{0};
        bool m_IsFrameStarted{false};
    };
}
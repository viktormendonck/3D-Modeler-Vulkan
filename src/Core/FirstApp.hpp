#pragma once

#include <core/VEDevice.hpp>
#include <core/VEWindow.hpp>
#include <core/VEPipeLine.hpp>
#include <core/VESwapChain.hpp>

#include <memory>
#include <vector>

namespace VE{
    class FirstApp{
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        FirstApp();
        ~FirstApp();

        FirstApp(const FirstApp&) = delete;
        FirstApp& operator=(const FirstApp&) = delete;

        void run();
    private:
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void drawFrame();
        VEWindow m_Window{WIDTH, HEIGHT, "Hello Vulkan!"};
        VEDevice m_Device{m_Window};
        VESwapChain m_SwapChain{m_Device, m_Window.GetExtent()};
        std::unique_ptr<VEPipeline> m_Pipeline;
        
        VkPipelineLayout m_PipelineLayout;
        std::vector<VkCommandBuffer> m_CommandBuffers;
    };
}
#pragma once

#include <vulkan/vulkan.h>
#include "VEDevice.hpp"
#include "VEWindow.hpp"
#include "VEPipeLine.hpp"
#include "VESwapChain.hpp"
#include "VEModel.hpp"

#include <memory>
#include <vector>

namespace VE{
    class ModelingApp{
    public:
        static constexpr uint32_t WIDTH = 1920;
        static constexpr uint32_t HEIGHT = 1080;

        ModelingApp();
        ~ModelingApp();

        ModelingApp(const ModelingApp&) = delete;
        ModelingApp& operator=(const ModelingApp&) = delete;

        void run();
    private:
        void LoadModels();
        void CreatePipelineLayout();
        void CreatePipeline();
        void CreateCommandBuffers();
        void DrawFrame();
        void RecreateSwapChain();
        void RecordCommandBuffer(int imageIndex);
        
        void Update(float deltaTime);
        float m_CurrentWaitTime = 0.0f; // for the tri update
        float m_WaitTime = 1.f; // for the tri update
        
        void Sierpinski(int step, VEModel::Vertex a, VEModel::Vertex b, VEModel::Vertex c, std::vector<VEModel::Vertex>& points);


        VEWindow m_Window{WIDTH, HEIGHT, "Hello Vulkan!"};
        VEDevice m_Device{m_Window};
        std::unique_ptr<VESwapChain> m_SwapChain;
        std::unique_ptr<VEModel> m_Model;
        std::unique_ptr<VEPipeline> m_Pipeline;
        
        VkPipelineLayout m_PipelineLayout;
        std::vector<VkCommandBuffer> m_CommandBuffers;

    };
}
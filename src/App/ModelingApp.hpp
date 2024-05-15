#pragma once

#include <vulkan/vulkan.h>
#include "Renderer/VEDevice.hpp"
#include "Renderer/VEWindow.hpp"
#include "Renderer/VERenderer.h"
#include "Renderer/VEModel.hpp"
#include "Renderer/VECamera.h"
#include "Renderer/RenderingSystems/VESimpleRendererSystem.h"
#include "Renderer/RenderingSystems/VEPointLightSystem.h"
#include "Renderer/RenderingSystems/VE2DRendererSystem.h"
#include "Renderer/RenderingSystems/VEVertPointRenderSystem.h"
#include "Renderer/VEDescriptors.h"

#include "Game/GameObject.h"

#include <memory>
#include <vector>

namespace VE{
    class ModelingApp{
    public:
        static constexpr uint32_t WIDTH = 1024;
        static constexpr uint32_t HEIGHT = 1024;

        ModelingApp();
        ~ModelingApp();

        ModelingApp(const ModelingApp&) = delete;
        ModelingApp& operator=(const ModelingApp&) = delete;

        void run();
    private:
        void LoadGameObjects();
        void Init();
        void Update(float deltaTime);
        void Render(FrameInfo& FrameInfo,bool ssSelecting);
        float m_CurrentWaitTime = 0.0f; // for the tri update
        float m_WaitTime = 1.f; // for the tri update
        
        VEWindow m_Window{WIDTH, HEIGHT, "Hello Vulkan!"};
        VEDevice m_Device{m_Window};
        VERenderer m_Renderer{m_Window, m_Device};

        std::unique_ptr<VEDescriptorPool> m_GlobalDescriptorPool;
        std::vector<ModelObject> m_GameObjects;
        std::vector<ModelObject> m_2DGameObjects;
        std::vector<PointLightObject> m_PointLights;

        std::vector<std::unique_ptr<VEBuffer>> m_UboBuffers{VESwapChain::MAX_FRAMES_IN_FLIGHT};
        std::unique_ptr<SimpleRenderingSystem> m_SimpleRenderer;
        std::unique_ptr<PointLightSystem> m_PointLightRendererSystem;
        std::unique_ptr<Simple2DRenderingSystem> m_2DRendererSystem;
        std::unique_ptr<VertexPointSystem> m_VertRendererSystem;
        
        std::vector<VkDescriptorSet> m_GlobalDescriptorSets{VESwapChain::MAX_FRAMES_IN_FLIGHT};
    };
}
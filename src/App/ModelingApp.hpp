#pragma once

#include <vulkan/vulkan.h>
#include "Renderer/VEDevice.hpp"
#include "Renderer/VEWindow.hpp"
#include "Renderer/VERenderer.h"
#include "Renderer/VEModel.hpp"
#include "Renderer/VECamera.h"
#include "Renderer/VESimpleRendererSystem.h"

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
        
        void Update(float deltaTime);
        void Render(const VECamera& camera, SimpleRenderingSystem& simpleRenderer);
        float m_CurrentWaitTime = 0.0f; // for the tri update
        float m_WaitTime = 1.f; // for the tri update
        
        void Sierpinski(int step, VEModel::Vertex a, VEModel::Vertex b, VEModel::Vertex c, std::vector<VEModel::Vertex>& points);
        std::shared_ptr<VEModel> CreateCubeModel(VEDevice& device, glm::vec3 offset);

        VEWindow m_Window{WIDTH, HEIGHT, "Hello Vulkan!"};
        VEDevice m_Device{m_Window};
        VERenderer m_Renderer{m_Window, m_Device};
        std::vector<GameObject> m_GameObjects;

    };
}
#include "ModelingApp.hpp"
#include <stdexcept>
#include <array>
#include <chrono>
#include <thread>
#include <renderer/VESimpleRendererSystem.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace VE{
    

    
    ModelingApp::ModelingApp()
    {
        LoadGameObjects();
    }

    ModelingApp::~ModelingApp()
    {
    }
    
    void VE::ModelingApp::run()
    {
        SimpleRenderingSystem simpleRenderer{m_Device, m_Renderer.GetSwapChainRenderPass()};
        const int targetFrameTimeMS = 1000 / 60;
        auto lastFrameStartTime = std::chrono::high_resolution_clock::now();

        while (!m_Window.ShouldClose())
        {
            glfwPollEvents();
	        const auto start = std::chrono::high_resolution_clock::now();
	        const float delta{ std::chrono::duration<float>(start - lastFrameStartTime).count() };
	        lastFrameStartTime = start;

            Update(delta);
            //this is the rendering part
	        if (VkCommandBuffer commandBuffer = m_Renderer.BeginFrame())
            {
                m_Renderer.BeginSwapChainRenderpass(commandBuffer);
                simpleRenderer.RenderGameObjects(commandBuffer, m_GameObjects);
                m_Renderer.EndSwapChainRenderpass(commandBuffer);
                m_Renderer.EndFrame();
            }
            
            const auto sleepTime = start + std::chrono::milliseconds(targetFrameTimeMS) - std::chrono::high_resolution_clock::now();
	        std::this_thread::sleep_for(sleepTime);
        }
        vkDeviceWaitIdle(m_Device.device());
    }

    void ModelingApp::LoadGameObjects()
    {
        std::vector<VEModel::Vertex> vertices = {};
        Sierpinski(4, {{0.0f, -0.2f, 0.0f},{1.0f,0.0f,0.0f}}, {{0.2f, 0.2f, 0.0f},{0.0f,1.0f,0.0f}}, {{-0.2f, 0.2f, 0.0f},{0.0f,0.0f,1.0f}}, vertices);
        auto model= std::make_shared<VEModel>(m_Device, vertices);
        GameObject sierpinski{GameObject::CreateGameObject(model, glm::vec3{0.8f, 0.0f, 0.80f})};
        Transform2dComponent& sierpinskiTransform = sierpinski.GetTransform();
        sierpinskiTransform.position = {0.2f, 0.0f};
        sierpinskiTransform.scale = {2.f,2.5f};
        sierpinskiTransform.rotation = glm::half_pi<float>();
        
        m_GameObjects.push_back(std::move(sierpinski));
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
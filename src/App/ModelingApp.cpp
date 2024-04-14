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
#include <Renderer/VECamera.h>
#include <Renderer/VEModel.hpp>
#include <Game/InputManager.h>

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
        VECamera camera{};
        InputManager inputManager{};
        TransformComponent CameraTransform{}; 
        
        const int targetFrameTimeMS = 1000 / 60;
        auto lastFrameStartTime = std::chrono::high_resolution_clock::now();


        while (!m_Window.ShouldClose())
        {
            glfwPollEvents();
            const auto start = std::chrono::high_resolution_clock::now();
	        const float delta{ std::chrono::duration<float>(start - lastFrameStartTime).count() };
	        lastFrameStartTime = start;
            inputManager.Update(m_Window.GetWindow(), delta, CameraTransform);
            camera.SetViewYXZ(CameraTransform.translation, CameraTransform.rotation);       
            
            float ar = m_Renderer.GetAspectRatio();
            camera.SetPerspectiveProjection(glm::radians(45.0f), ar, 0.1f, 10.0f);

            
            Update(delta);
            
            Render(camera, simpleRenderer);
            
            const auto sleepTime = start + std::chrono::milliseconds(targetFrameTimeMS) - std::chrono::high_resolution_clock::now();
	        std::this_thread::sleep_for(sleepTime);
        }
        vkDeviceWaitIdle(m_Device.device());
    }

    void ModelingApp::LoadGameObjects()
    {
        std::shared_ptr<VEModel> flatVaseModel{VEModel::CreateModelFromFile(m_Device, "data/models/untitled.obj")};
        auto flatVase = GameObject::CreateGameObject();
        flatVase.SetModel(flatVaseModel);
        flatVase.GetTransform().translation = {-0.5f, 0.5f, 4.0f};
        flatVase.GetTransform().scale = glm::vec3(3.f,3.f,3.f);
        flatVase.GetTransform().rotation = glm::vec3(0,0,0.0f);
        m_GameObjects.push_back(std::move(flatVase));

        
    }
    
    void ModelingApp::Update(float deltaTime)
    {
       
    }

    void ModelingApp::Render(const VECamera& camera, SimpleRenderingSystem& simpleRenderer)
    {
        if (VkCommandBuffer commandBuffer = m_Renderer.BeginFrame())
        {
                m_Renderer.BeginSwapChainRenderpass(commandBuffer);
                simpleRenderer.RenderGameObjects(commandBuffer, m_GameObjects,camera);
                m_Renderer.EndSwapChainRenderpass(commandBuffer);
                m_Renderer.EndFrame();
        }
    }

   
}
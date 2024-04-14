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
        std::shared_ptr<VEModel> cubeModel = CreateCubeModel(m_Device, {0.0f, 0.0f, 0.0f});
        auto cube = GameObject::CreateGameObject();
        cube.SetModel(cubeModel);
        cube.GetTransform().translation = {0.0f, 0.0f, 4.0f};
        cube.GetTransform().scale = glm::vec3(0.5f,0.5f,0.5f);
        cube.GetTransform().rotation = glm::vec3(glm::quarter_pi<float>(),glm::pi<float>()-glm::quarter_pi<float>(),0.0f);
        m_GameObjects.push_back(std::move(cube));
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
    
    std::shared_ptr<VEModel> ModelingApp::CreateCubeModel(VEDevice& device, glm::vec3 offset) {

        VEModel::ModelBuilder modelBuilder{};

        modelBuilder.vertices = {
            // left face (white)
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
        
            // right face (yellow)
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
        
            // top face (orange, remember y axis points down)
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        
            // bottom face (red)
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        
            // nose face (blue)
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        
            // tail face (green)
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        };
        for (auto& v : modelBuilder.vertices) {
            v.position += offset;
        }   
    
        modelBuilder.indices = {0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
                                12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21};
        return std::make_shared<VEModel>(device, modelBuilder);
    }
}
#include "ModelingApp.hpp"
#include <stdexcept>
#include <array>
#include <chrono>
#include <thread>
#include <memory>
#include <Renderer/RenderingSystems/VESimpleRendererSystem.h>
#include <Renderer/RenderingSystems/VEPointLightSystem.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <Renderer/VECamera.h>
#include <Renderer/VEModel.hpp>
#include <Game/InputManager.h>
#include <Game/GameObject.h>
#include <Game/PointLight.h>
#include <Renderer/VEBuffer.h>
#include <Renderer/VEUtils.h>

namespace VE{
    
    ModelingApp::ModelingApp()
    {
        m_GlobalDescriptorPool = VEDescriptorPool::Builder(m_Device)
            .SetMaxSets(VESwapChain::MAX_FRAMES_IN_FLIGHT)
            .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VESwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();
        LoadGameObjects();
    }

    ModelingApp::~ModelingApp()
    {
    }
    void ModelingApp::Init()
    {
        for (int i{}; i< m_UboBuffers.size(); i++)
        {
            m_UboBuffers[i] = std::make_unique<VEBuffer>(
                m_Device,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            );
            m_UboBuffers[i]->Map();
        }
        auto globalDescriptorSetLayout = VEDescriptorSetLayout::Builder(m_Device)
            .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .build();
        
        for (int i{}; i < m_GlobalDescriptorSets.size();++i){
            auto bufferInfo = m_UboBuffers[i]->DescriptorInfo();
            VEDescriptorWriter(*globalDescriptorSetLayout,*m_GlobalDescriptorPool)
                .WriteBuffer(0, &bufferInfo)
                .Build(m_GlobalDescriptorSets[i]);
        }

        m_SimpleRenderer = std::make_unique<SimpleRenderingSystem>(m_Device, m_Renderer.GetSwapChainRenderPass(),globalDescriptorSetLayout->GetDescriptorSetLayout());
        m_2DRendererSystem = std::make_unique<Simple2DRenderingSystem>(m_Device, m_Renderer.GetSwapChainRenderPass(),globalDescriptorSetLayout->GetDescriptorSetLayout());
        m_PointLightRendererSystem = std::make_unique<PointLightSystem>(m_Device, m_Renderer.GetSwapChainRenderPass(),globalDescriptorSetLayout->GetDescriptorSetLayout(),m_PointLights);
        m_VertRendererSystem = std::make_unique<VertexPointSystem>(m_Device, m_Renderer.GetSwapChainRenderPass(),globalDescriptorSetLayout->GetDescriptorSetLayout());
    }

    void VE::ModelingApp::run()
    {
        Init();
        bool shouldUpdateBuffer{false};
        VECamera camera{};
        InputManager inputManager{};
        TransformComponent CameraTransform{}; 
        
        inputManager.SetBaseColor(glm::vec3(0.8f, 0.8f, 0.8f));
        inputManager.SetSelectionModel(&m_GameObjects[0]);

        const int targetFrameTimeMS{1000 / 60};
        auto lastFrameStartTime{std::chrono::high_resolution_clock::now()};
        camera.GetTransform().pos = {0.0f, -.5f, -5.0f};

        while (!m_Window.ShouldClose())
        {
            if (shouldUpdateBuffer)
            {
                m_GameObjects[0].GetModel()->UpdateVertices();
                shouldUpdateBuffer = false;
            }
            //allow glfw to process events
            glfwPollEvents();
            //update deltatime
            const auto start{std::chrono::high_resolution_clock::now()};
	        const float delta{ std::chrono::duration<float>(start - lastFrameStartTime).count() };
	        lastFrameStartTime = start;
            //update camera
            inputManager.UpdateCameraMovement(m_Window.GetWindow(), delta, camera.GetTransform());

            camera.CalculateViewMatrix();       
            float ar = m_Renderer.GetAspectRatio();
            camera.SetPerspectiveProjection(glm::radians(45.0f), ar, 0.1f, 100000.0f);
            //the actual frame
            if (VkCommandBuffer commandBuffer = m_Renderer.BeginFrame())
            {
                //update global uniform buffer
                FrameInfo frameInfo{m_Renderer.GetFrameIndex(), delta, camera, commandBuffer, m_GlobalDescriptorSets[m_Renderer.GetFrameIndex()]};
                GlobalUbo ubo{};
                ubo.ProjectionMatrix = camera.GetProjectionMatrix();
                ubo.viewMatrix = camera.GetViewMatrix();
                ubo.numPointLights = static_cast<int>(m_PointLights.size());
                for (int i{}; i < m_PointLights.size(); ++i)
                {
                    ubo.pointLights[i] = PointLight(glm::vec4(m_PointLights[i].GetTransform().pos,0), glm::vec4(m_PointLights[i].GetColor(),m_PointLights[i].GetIntensity()));
                }
                m_UboBuffers[frameInfo.frameIndex]->WriteToBuffer(&ubo);
                m_UboBuffers[frameInfo.frameIndex]->Flush();

                shouldUpdateBuffer = inputManager.Update(m_Window, delta,camera.GetTransform(), ubo);
                //update any gameobjects
                Update(delta);

                //render shit
                Render(frameInfo,inputManager.IsSelecting());
            }
            //make code run at certain fps
            const auto sleepTime{start + std::chrono::milliseconds(targetFrameTimeMS) - std::chrono::high_resolution_clock::now()};
	        std::this_thread::sleep_for(sleepTime);
        }
        vkDeviceWaitIdle(m_Device.device());
    }

    void ModelingApp::LoadGameObjects()
    {
        //point lights, if you make em might as well use em
        glm::vec3 baseLightColor{246.0f/255.0f, 200.0f/255.0f, 180.0f/255.0f};
        m_PointLights.push_back({ {2.0f, -1.0f, -3.0f}, baseLightColor, 2.0f,0.3f});
        m_PointLights.push_back({ {-2.0f, -1.0f, 3.0f},baseLightColor, .5f,0.1f});
        m_PointLights.push_back({ {-2.0f, -1.0f, -3.0f}, baseLightColor, 1.0f,0.2f});


        std::shared_ptr<VEModel> startCubeModel{VEModel::CreateModelFromFile(m_Device, "data/models/icosphere.obj")};
        ModelObject StartCubeObject{startCubeModel};
        StartCubeObject.GetTransform().pos = {0.f, 0.f, 0.0f};
        //StartCubeObject.GetTransform().scale = glm::vec3(0.5f,0.5f,0.5f);
        StartCubeObject.GetTransform().scale = glm::vec3(1.f,1.f,1.f);
        
        m_GameObjects.push_back(std::move(StartCubeObject));
        
    }

    void ModelingApp::Update(float deltaTime)
    {

    }

    void ModelingApp::Render(FrameInfo& FrameInfo,bool isSelecting)
    {
        m_Renderer.BeginSwapChainRenderpass(FrameInfo.commandBuffer);
        m_SimpleRenderer->RenderGameObjects(FrameInfo, m_GameObjects);
        //m_PointLightRendererSystem->Render(FrameInfo);
        m_VertRendererSystem->Render(FrameInfo,&m_GameObjects[0],isSelecting);
        m_2DRendererSystem->RenderGameObjects(FrameInfo, m_2DGameObjects);
        m_Renderer.EndSwapChainRenderpass(FrameInfo.commandBuffer);
        m_Renderer.EndFrame();
    }
}
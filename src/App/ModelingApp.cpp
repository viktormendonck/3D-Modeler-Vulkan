#include "ModelingApp.hpp"
#include <stdexcept>
#include <array>
#include <chrono>
#include <thread>
#include <Renderer/RenderingSystems/VESimpleRendererSystem.h>
#include <Renderer/RenderingSystems/VEPointLightSystem.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
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
    
    void VE::ModelingApp::run()
    {
        std::vector<std::unique_ptr<VEBuffer>> uboBuffers{VESwapChain::MAX_FRAMES_IN_FLIGHT};
        for (int i{}; i< uboBuffers.size(); i++)
        {
            uboBuffers[i] = std::make_unique<VEBuffer>(
                m_Device,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            );
            uboBuffers[i]->Map();
        }

        auto m_GlobalDescriptorSetLayout = VEDescriptorSetLayout::Builder(m_Device)
            .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .build();
        
        std::vector<VkDescriptorSet> m_GlobalDescriptorSets{VESwapChain::MAX_FRAMES_IN_FLIGHT};
        for (int i{}; i < m_GlobalDescriptorSets.size();++i){
            auto bufferInfo = uboBuffers[i]->DescriptorInfo();
            VEDescriptorWriter(*m_GlobalDescriptorSetLayout,*m_GlobalDescriptorPool)
                .WriteBuffer(0, &bufferInfo)
                .Build(m_GlobalDescriptorSets[i]);
        }

        SimpleRenderingSystem simpleRenderer{m_Device, m_Renderer.GetSwapChainRenderPass(),m_GlobalDescriptorSetLayout->GetDescriptorSetLayout()};
        PointLightSystem pointLightSystem{m_Device, m_Renderer.GetSwapChainRenderPass(),m_GlobalDescriptorSetLayout->GetDescriptorSetLayout(),m_PointLights};
        VECamera camera{};
        InputManager inputManager{};
        TransformComponent CameraTransform{}; 
        
        const int targetFrameTimeMS{1000 / 60};
        auto lastFrameStartTime{std::chrono::high_resolution_clock::now()};


        while (!m_Window.ShouldClose())
        {
            glfwPollEvents();
            const auto start{std::chrono::high_resolution_clock::now()};
	        const float delta{ std::chrono::duration<float>(start - lastFrameStartTime).count() };
	        lastFrameStartTime = start;
            inputManager.Update(m_Window.GetWindow(), delta, camera.GetTransform());
            camera.CalculateViewMatrix();       
            
            float ar = m_Renderer.GetAspectRatio();
            camera.SetPerspectiveProjection(glm::radians(45.0f), ar, 0.1f, 1000.0f);

            if (VkCommandBuffer commandBuffer = m_Renderer.BeginFrame())
            {

                FrameInfo frameInfo{m_Renderer.GetFrameIndex(), delta, camera, commandBuffer, m_GlobalDescriptorSets[m_Renderer.GetFrameIndex()]};
                GlobalUbo ubo{};
                ubo.ProjectionMatrix = camera.GetProjectionMatrix();
                ubo.viewMatrix = camera.GetViewMatrix();
                ubo.numPointLights = static_cast<int>(m_PointLights.size());
                for (int i{}; i < m_PointLights.size(); ++i)
                {
                    ubo.pointLights[i] = PointLight(glm::vec4(m_PointLights[i].GetTransform().pos,0), glm::vec4(m_PointLights[i].GetColor(),m_PointLights[i].GetIntensity()));
                }
                uboBuffers[frameInfo.frameIndex]->WriteToBuffer(&ubo);
                uboBuffers[frameInfo.frameIndex]->Flush();

                Update(delta);

                Render(simpleRenderer,pointLightSystem,frameInfo);
            }
            const auto sleepTime{start + std::chrono::milliseconds(targetFrameTimeMS) - std::chrono::high_resolution_clock::now()};
	        std::this_thread::sleep_for(sleepTime);
        }
        vkDeviceWaitIdle(m_Device.device());
    }

    void ModelingApp::LoadGameObjects()
    {
        PointLightObject light0{ glm::vec3(-1.0f,-1.f,-1.f), glm::vec3(0.0f, 1.0f, 0.0f), 1.0f,0.2f};
        PointLightObject light1{ glm::vec3(1.0f,-1.0f,1.0f), glm::vec3(0.0f, 0.0f, 1.0f), 1.0f,0.2f};
        PointLightObject light2{ glm::vec3(-1.0f,-1.0f,1.0f),glm::vec3(1.0f, 0.0f, 0.0f), 1.0f,0.2f};
        m_PointLights.push_back(light0);
        m_PointLights.push_back(light1);
        m_PointLights.push_back(light2);

        std::shared_ptr<VEModel> groundPlaneModel{VEModel::CreateModelFromFile(m_Device, "data/models/quad.obj")};
        ModelObject groundPlane{groundPlaneModel};
        groundPlane.GetTransform().pos = {0.0f, 0.5f, 0.0f};
        groundPlane.GetTransform().scale = glm::vec3(3.f,1.f,3.f);
        m_GameObjects.push_back(std::move(groundPlane));

        std::shared_ptr<VEModel> flatVaseModel{VEModel::CreateModelFromFile(m_Device, "data/models/untitled.obj")};
        ModelObject flatVase{flatVaseModel};
        flatVase.SetModel(flatVaseModel);
        flatVase.GetTransform().pos = {-0.5f, 0.5f, 0.0f};
        flatVase.GetTransform().scale = glm::vec3(3.f,3.f,3.f);
        flatVase.GetTransform().rotation = glm::vec3(0,4,0.0f);
        m_GameObjects.push_back(std::move(flatVase));

        
    }
    
    void ModelingApp::Update(float deltaTime)
    {
       m_GameObjects[1].GetTransform().rotation.y += glm::radians(45.0f) * deltaTime;
       
    }

    void ModelingApp::Render(SimpleRenderingSystem& simpleRenderer,PointLightSystem& pointlightRenderer,FrameInfo& FrameInfo)
    {
        m_Renderer.BeginSwapChainRenderpass(FrameInfo.commandBuffer);
        simpleRenderer.RenderGameObjects(FrameInfo, m_GameObjects);
        pointlightRenderer.Render(FrameInfo);
        m_Renderer.EndSwapChainRenderpass(FrameInfo.commandBuffer);
        m_Renderer.EndFrame();
    }
    

   
}
#pragma once
#include "GameObject.h"
#include "Renderer/VEWindow.hpp"
#include <Renderer/VEFrameInfo.h>


namespace VE
{
    class InputManager
    {
    public:
        
        void UpdateCameraMovement(GLFWwindow* window, float deltaTime, TransformComponent& CameraTransform);
        void Update(VEWindow& window, float deltaTime,TransformComponent& CameraTransform, const GlobalUbo& ubo);

        void SetSelectionModel(ModelObject* model) { 
            m_SelectionModel = model;
            m_VertSize = m_SelectionModel->GetModel()->GetVertexCount();
            }
        void SetBaseColor(const glm::vec3& color) { m_BaseColor = color; }
    private:
        void UpdateVertSelection(VEWindow& window, const GlobalUbo& ubo);
        void UpdateSelectionMovement(GLFWwindow* window, float deltaTime,TransformComponent& CameraTransform);
        void SelectNearbyVerts();
        void DeselctAllVerts();

        std::vector<glm::vec2> GetScreenSpaceVerts(VEWindow& window, const GlobalUbo& ubo);

        const float m_MoveSpeed{3.0f};
        const float m_RotateSpeed{0.2f};
        double m_LastMouseX{}, m_LastMouseY{};
        bool m_ClickedPreviousFrame{false};
        
        ModelObject* m_SelectionModel;
        int m_VertSize{};
        int m_CurrentSelectedVert{0};
        glm::vec3 m_BaseColor{0.8f, 0.8f, 0.8f};
        glm::vec3 m_SelectedColor{1.0f, 0.0f, 0.0f};
        
        enum class MoveMode
        {
            GLOBAL,
            VIEW,
        };
        MoveMode m_SelectionMode{MoveMode::GLOBAL};
    };
} // namespace VE
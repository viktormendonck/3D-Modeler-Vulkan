#pragma once
#include "GameObject.h"
#include "Renderer/VEWindow.hpp"


namespace VE
{
    class InputManager
    {
    public:
        
        void Update(GLFWwindow* window, float deltaTime, TransformComponent& CameraTransform);


        void SetSelectionModel(std::shared_ptr<VEModel> model) { m_SelectionModel = model; }
        void SetBaseColor(const glm::vec3& color) { m_BaseColor = color; }
    private:
        void UpdateCameraMovement(GLFWwindow* window, float deltaTime, TransformComponent& CameraTransform);
        void UpdateVertSelection(GLFWwindow *window);
        void UpdateSelectionMovement(GLFWwindow* window, float deltaTime,TransformComponent& CameraTransform);
        void SelectNearbyVerts();
        void DeselctAllVerts();
        const float m_MoveSpeed{3.0f};
        const float m_RotateSpeed{0.2f};
        double m_LastMouseX{}, m_LastMouseY{};
        bool m_ClickedPreviousFrame{false};
        
        std::shared_ptr<VEModel> m_SelectionModel;
        std::vector<bool> m_SelectedVerts;
        int currentSelectedVert{0};
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
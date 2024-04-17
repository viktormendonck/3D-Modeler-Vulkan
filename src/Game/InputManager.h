#pragma once
#include "GameObject.h"
#include "Renderer/VEWindow.hpp"


namespace VE
{
    class InputManager
    {
    public:
        void UpdateCameraMovement(GLFWwindow* window, float deltaTime, TransformComponent& transform);
    private:
        float m_MoveSpeed{3.0f};
        float m_RotateSpeed{0.2f};
        double m_LastMouseX{}, m_LastMouseY{};
        bool m_ClickedPreviousFrame{false};
    };
} // namespace VE
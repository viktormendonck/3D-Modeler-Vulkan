#pragma once
#include "GameObject.h"
#include "Renderer/VEWindow.hpp"


namespace VE
{
    class InputManager
    {
    public:
        void Update(GLFWwindow* window, float deltaTime, TransformComponent& transform);
    private:
        float m_MoveSpeed{3.0f};
        float m_RotateSpeed{1.5f};
    
        
    };
} // namespace VE
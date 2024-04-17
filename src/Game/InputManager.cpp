#include "InputManager.h"
#include <limits>

namespace VE
{
    void InputManager::UpdateCameraMovement(GLFWwindow *window, float deltaTime, TransformComponent& transform)
    {
        float yaw = transform.rotation.y;
        const glm::vec3 forwardDir{sin(yaw), 0.f, cos(yaw)};
        const glm::vec3 rightDir{forwardDir.z, 0.f, -forwardDir.x};
        const glm::vec3 upDir{0.f, -1.f, 0.f};

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);
            const float deltaX = static_cast<float>(mouseX - m_LastMouseX);
            const float deltaY = static_cast<float>(mouseY - m_LastMouseY);
            m_LastMouseX = mouseX;
            m_LastMouseY = mouseY;
            if (m_ClickedPreviousFrame) {
                transform.rotation.x -= m_RotateSpeed * deltaTime * deltaY;
                transform.rotation.y += m_RotateSpeed * deltaTime * deltaX;
            }
            m_ClickedPreviousFrame = true;
        } else {
            m_ClickedPreviousFrame = false;
        }

        glm::vec3 moveDir{0.f};
        bool isSprinting = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) moveDir += forwardDir;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) moveDir -= forwardDir;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) moveDir += rightDir;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) moveDir -= rightDir;
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) moveDir += upDir;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) moveDir -= upDir;

        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
            float moveSpeed = m_MoveSpeed;
            if (isSprinting) {
                moveSpeed *=2;
            }
            transform.pos += (moveSpeed * deltaTime * glm::normalize(moveDir));
        }
        
    }
}
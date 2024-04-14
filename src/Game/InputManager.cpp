#include "InputManager.h"
#include <limits>

namespace VE
{
    void InputManager::Update(GLFWwindow *window, float deltaTime, TransformComponent& transform)
    {
       glm::vec3 rotate{0};
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) rotate.y += 1.f;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) rotate.y -= 1.f;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) rotate.x += 1.f;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) rotate.x -= 1.f;

        if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
            transform.rotation += m_RotateSpeed * deltaTime * glm::normalize(rotate);
        }
        
        float yaw = transform.rotation.y;

        const glm::vec3 forwardDir{sin(yaw), 0.f, cos(yaw)};
        const glm::vec3 rightDir{forwardDir.z, 0.f, -forwardDir.x};
        const glm::vec3 upDir{0.f, -1.f, 0.f};

        glm::vec3 moveDir{0.f};
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) moveDir += forwardDir;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) moveDir -= forwardDir;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) moveDir += rightDir;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) moveDir -= rightDir;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) moveDir += upDir;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) moveDir -= upDir;

        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
            transform.translation += m_MoveSpeed * deltaTime * glm::normalize(moveDir);
        }
        
    }
}
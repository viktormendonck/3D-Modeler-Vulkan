#include "InputManager.h"
#include <limits>
#include <glm/vec3.hpp>
#include <glm/gtx/norm.hpp>

namespace VE
{
    void InputManager::Update(GLFWwindow *window, float deltaTime, TransformComponent& CameraTransform)
    {
        UpdateCameraMovement(window, deltaTime, CameraTransform);
        UpdateVertSelection(window);
        UpdateSelectionMovement(window, deltaTime,CameraTransform);
    }

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
    void InputManager::UpdateVertSelection(GLFWwindow *window)
    {
        if (m_SelectionModel == nullptr || !glfwGetKey(window,GLFW_KEY_LEFT_CONTROL)) return;
        if (m_SelectedVerts.size() != m_SelectionModel->GetVertexCount())
        {
            m_SelectedVerts.resize(m_SelectionModel->GetVertexCount(), false);
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            DeselctAllVerts();
            currentSelectedVert = (currentSelectedVert + 1) % m_SelectedVerts.size();
            m_SelectedVerts[currentSelectedVert] = true;
            SelectNearbyVerts();
            m_SelectionModel->UpdateVertices();
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            DeselctAllVerts();
            currentSelectedVert = (currentSelectedVert - 1) % m_SelectedVerts.size();
            m_SelectedVerts[currentSelectedVert] = true;
            SelectNearbyVerts();
            m_SelectionModel->UpdateVertices();
        }
        
    }

    void InputManager::UpdateSelectionMovement(GLFWwindow *window, float deltaTime,TransformComponent& cameraTransform)
    {
        if (m_SelectionModel == nullptr || glfwGetKey(window,GLFW_KEY_LEFT_CONTROL)) return;
        glm::vec3 forwardDir{};
        glm::vec3 rightDir{};
        glm::vec3 upDir{};
        switch (m_SelectionMode)
        {
        case MoveMode::GLOBAL:
            forwardDir = {0.f, 0.f, 1.f};
            rightDir = {1.f, 0.f, 0.f};
            upDir = {0.f, 1.f, 0.f};
            break;
        case MoveMode::VIEW:
            float yaw = cameraTransform.rotation.y;
            forwardDir = {sin(yaw), 0.f, cos(yaw)};
            rightDir = {forwardDir.z, 0.f, -forwardDir.x};
            upDir = {0.f, -1.f, 0.f};
            break;
        }
        glm::vec3 moveDir{0.f};
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) moveDir += forwardDir;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) moveDir -= forwardDir;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) moveDir += rightDir;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) moveDir -= rightDir;
        if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) moveDir += upDir;
        if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) moveDir -= upDir;  
        
        if (glm::length2(moveDir) > std::numeric_limits<float>::epsilon())
        {
            auto& verts = m_SelectionModel->GetVertices();
            for (int i = 0; i < m_SelectedVerts.size(); i++)
            {
                if (m_SelectedVerts[i])
                {
                    verts[i].position += (deltaTime * glm::normalize(moveDir));
                }
            }
            m_SelectionModel->UpdateVertices();
        }
    }

    void InputManager::SelectNearbyVerts()
    {
        auto& verts = m_SelectionModel->GetVertices();
        auto selectedVert = verts[currentSelectedVert].position;
        for (int i = 0; i < verts.size(); i++)
        {
            if (glm::distance(verts[i].position, selectedVert) < std::numeric_limits<float>::epsilon())
            {
                m_SelectedVerts[i] = true;
                verts[i].color = m_SelectedColor;
            }
        }
        
    }
    void InputManager::DeselctAllVerts()
    {
        auto& verts = m_SelectionModel->GetVertices();
        for (int i = 0; i < verts.size(); i++)
        {
            m_SelectedVerts[i] = false;
            verts[i].color = m_BaseColor;
        }
    }
}
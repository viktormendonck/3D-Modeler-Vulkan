#include "InputManager.h"
#include <limits>
#include <glm/vec3.hpp>
#include <glm/gtx/norm.hpp>
#include <iostream>

namespace VE
{
    void InputManager::Update(VEWindow& window, float deltaTime, TransformComponent& CameraTransform, const GlobalUbo& ubo)
    {
        UpdateVertSelection(window,ubo);
        UpdateSelectionMovement(window.GetWindow(), deltaTime,CameraTransform);
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
    void InputManager::UpdateVertSelection(VEWindow& window, const GlobalUbo& ubo)
    {
        GLFWwindow* glfWindow = window.GetWindow();
        if (m_SelectionModel == nullptr || glfwGetKey(glfWindow,GLFW_KEY_LEFT_CONTROL) != GLFW_PRESS) return;
        
        if (glfwGetMouseButton(glfWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            double mouseX, mouseY;
            glfwGetCursorPos(glfWindow, &mouseX, &mouseY);

            if (glfwGetKey(glfWindow,GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS) {
                DeselctAllVerts();
            }
            
            auto screenSpaceVerts = GetScreenSpaceVerts(window,ubo);
            for (int i = 0; i < screenSpaceVerts.size(); i++)
            {
                if (glm::distance(screenSpaceVerts[i], glm::vec2(mouseX, mouseY)) < 10.0f)
                {
                    m_CurrentSelectedVert = i;
                    m_SelectionModel->GetModel()->GetVertices()[i].selected = true;
                }
            }
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
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) moveDir -= upDir;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) moveDir += upDir;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) moveDir += rightDir;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) moveDir -= rightDir;
        if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) moveDir -= forwardDir;
        if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) moveDir += forwardDir;  
        
        if (glm::length2(moveDir) > std::numeric_limits<float>::epsilon())
        {
            auto& verts = m_SelectionModel->GetModel()->GetVertices();
            for (int i = 0; i < m_VertSize; i++)
            {
                if (verts[i].selected)
                {
                    verts[i].position += (deltaTime * glm::normalize(moveDir));
                }
            }
            m_SelectionModel->GetModel()->UpdateVertices();
        }
    }
     
    void InputManager::SelectNearbyVerts()
    {
        auto& verts = m_SelectionModel->GetModel()->GetVertices();
        auto selectedVert = verts[m_CurrentSelectedVert].position;
        for (int i = 0; i < verts.size(); i++)
        {
            if (glm::distance(verts[i].position, selectedVert) < std::numeric_limits<float>::epsilon())
            {
                verts[i].selected = true;
            }
        }
        
    }
    void InputManager::DeselctAllVerts()
    {
        auto& verts = m_SelectionModel->GetModel()->GetVertices();
        for (int i = 0; i < verts.size(); i++)
        {
            verts[i].selected = false;
        }
    }
    std::vector<glm::vec2> InputManager::GetScreenSpaceVerts(VEWindow& window, const GlobalUbo& ubo)
    {
        std::vector<glm::vec2> screenSpaceVerts{};
        //return all vertices in screen space to compare clicks with :D
        std::vector<VEModel::Vertex>& verts = m_SelectionModel->GetModel()->GetVertices();
        for (VEModel::Vertex& vert: verts){
            
            glm::vec4 vertPos = glm::vec4(vert.position, 1.0f) * m_SelectionModel->GetTransform().GetTransformationMatrix();
            glm::vec4 cameraSpaceVertex = ubo.viewMatrix * vertPos;
            glm::vec4 clipSpaceVertex = ubo.ProjectionMatrix * cameraSpaceVertex;
            glm::vec3 ndcVertex = glm::vec3(clipSpaceVertex) / clipSpaceVertex.w;
            
            glm::vec4 viewport = glm::vec4(0,0,window.GetExtent().width,window.GetExtent().height);

            glm::vec2 screenSpaceVertex = glm::vec2(viewport.z * (ndcVertex.x + 1) / 2 + viewport.x, viewport.w * (ndcVertex.y + 1) / 2 + viewport.y);

            screenSpaceVerts.push_back(screenSpaceVertex);
        }


        return screenSpaceVerts;
    }
}
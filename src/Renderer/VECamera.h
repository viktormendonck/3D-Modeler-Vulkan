#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include "Game/GameObject.h"

namespace VE
{

    class VECamera 
    {
    public:
        void SetOrthographicProjection(float left, float right, float bottom, float top, float near, float far);
        void SetPerspectiveProjection(float fov, float aspectRatio, float near, float far);

        void SetViewMatrix(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});
        void SetViewYXZ(glm::vec3 position,glm::vec3 rotation);

        void CalculateViewMatrix();

        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        TransformComponent& GetTransform() { return m_Transform; }
    private:
        TransformComponent m_Transform;
        glm::mat4 m_ProjectionMatrix{1.f};
        glm::mat4 m_ViewMatrix{1.f};
    };
} // namespace VE

#pragma once
#include "GameObject.h"

namespace VE
{
    class PointLightObject : public GameObject
    {
    public:
        PointLightObject(glm::vec3 pos,glm::vec3 color, float intensity, float radius)
            : m_Color(color), m_Intensity(intensity) 
            {
                GetTransform().pos = pos;
                GetTransform().scale.x = radius;
            }
        
        ~PointLightObject() = default;

        glm::vec3 GetColor() const { return m_Color; }
        void SetColor(const glm::vec3& color) { m_Color = color; }
        float GetIntensity() const { return m_Intensity; }
        void SetIntensity(float intensity) { m_Intensity = intensity; }
        float GetRadius() { return GetTransform().scale.x; }
    private:
        glm::vec3 m_Color{1.0f};
        float m_Intensity{1.0f};
    };

} // namespace VE

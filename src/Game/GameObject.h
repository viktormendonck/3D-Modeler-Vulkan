#pragma once 
#include "../Renderer/VEModel.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>


namespace VE{

    struct TransformComponent{
        glm::vec3 pos;
        glm::vec3 scale{1.0f,1.0f,1.0f};
        glm::vec3 rotation{}; // in radians

        glm::mat4 GetTransformationMatrix();
        glm::mat3 GetNormalMatrix() const ;
    };

    class GameObject{
    public:
        TransformComponent& GetTransform() { return m_Transform; }
    protected:
        GameObject() = default;
    private:
        TransformComponent m_Transform{};
    };
    class ModelObject : public GameObject{
    public:
        ModelObject(const std::shared_ptr<VEModel>& model, const glm::vec3& color = glm::vec3{1.0f}) : m_Model(model), m_Color(color){}

        glm::vec3 GetColor() const { return m_Color; }
        void SetColor(const glm::vec3& color) { m_Color = color; }
        std::shared_ptr<VEModel> GetModel() const { return m_Model; }
        void SetModel(const std::shared_ptr<VEModel>& model) { m_Model = model; }
    private:
        std::shared_ptr<VEModel> m_Model;
        glm::vec3 m_Color{1.0f};
    };
}

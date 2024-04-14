#pragma once 
#include "../Renderer/VEModel.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>


namespace VE{

    struct TransformComponent{
        glm::vec3 translation;
        glm::vec3 scale{1.0f,1.0f,1.0f};
        glm::vec3 rotation{}; // in radians

        glm::mat4 GetTransformationMatrix(){
            const float c3 = glm::cos(rotation.z);
            const float s3 = glm::sin(rotation.z);
            const float c2 = glm::cos(rotation.x);
            const float s2 = glm::sin(rotation.x);
            const float c1 = glm::cos(rotation.y);
            const float s1 = glm::sin(rotation.y);
            return glm::mat4{
                {
                    scale.x * (c1 * c3 + s1 * s2 * s3),
                    scale.x * (c2 * s3),
                    scale.x * (c1 * s2 * s3 - c3 * s1),
                    0.0f,
                },
                {
                    scale.y * (c3 * s1 * s2 - c1 * s3),
                    scale.y * (c2 * c3),
                    scale.y * (c1 * c3 * s2 + s1 * s3),
                    0.0f,
                },
                {
                    scale.z * (c2 * s1),
                    scale.z * (-s2),
                    scale.z * (c1 * c2),
                    0.0f,
                },
                {translation.x, translation.y, translation.z, 1.0f}
            };
        }
    };

    class GameObject{
    public:
        using id_t = unsigned int;

        static GameObject CreateGameObject();
        static GameObject CreateGameObject(const std::shared_ptr<VEModel>& model, const glm::vec3& color = glm::vec3{1.0f});

        GameObject(const GameObject&) = delete;
        GameObject& operator=(const GameObject&) = delete;
        GameObject(GameObject&&) = default;
        GameObject& operator=(GameObject&&) = default;


        id_t GetId() const { return m_Id; }
        glm::vec3 GetColor() const { return m_Color; }
        void SetColor(const glm::vec3& color) { m_Color = color; }
        std::shared_ptr<VEModel> GetModel() const { return m_Model; }
        void SetModel(const std::shared_ptr<VEModel>& model) { m_Model = model; }
        TransformComponent& GetTransform() { return m_Transform; }
        private:

        GameObject(id_t id) : m_Id(id){}
        GameObject(id_t id, const std::shared_ptr<VEModel>& model, const glm::vec3& color) : m_Id(id), m_Model(model), m_Color(color){}
        id_t m_Id;

        std::shared_ptr<VEModel> m_Model;
        glm::vec3 m_Color{};
        TransformComponent m_Transform{};
    };
}

#include "GameObject.h"
namespace VE
{
    GameObject GameObject::CreateGameObject()
    {
        static id_t id = 0;
        return GameObject{id++};
    }

    GameObject GameObject::CreateGameObject(const std::shared_ptr<VEModel> &model, const glm::vec3 &color)
    {
        static id_t id = 0;
        return GameObject{id++, model, color};
    }
}
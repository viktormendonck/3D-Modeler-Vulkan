#pragma once
#include "VEDevice.hpp"

#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>


namespace VE
{
    class VEModel{
      public:

        struct Vertex{
            glm::vec3 position;
            glm::vec3 color;
            //add later
            //glm::vec2 texCoord;

            static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
        };

        VEModel(VEDevice& device, const std::vector<Vertex>& vertices);
        ~VEModel();

        VEModel(const VEModel&) = delete;
        VEModel& operator=(const VEModel&) = delete;

        void Bind(VkCommandBuffer commandBuffer);
        void Draw(VkCommandBuffer commandBuffer);


      private:
        void CreateVertexBuffer(const std::vector<Vertex>& vertices);
        VEDevice& m_Device;
        VkBuffer m_VertexBuffer;
        VkDeviceMemory m_VertexBufferMemory;
        uint32_t m_VertexCount;
    };
} // namespace VE

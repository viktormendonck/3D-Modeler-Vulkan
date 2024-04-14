#pragma once
#include "VEDevice.hpp"

#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>


namespace VE
{
    class 
    VEModel{
      public:

        struct Vertex{
            glm::vec3 position;
            glm::vec3 color;
            //add later
            //glm::vec2 texCoord;

            static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
        };

        struct ModelBuilder{
          std::vector<Vertex> vertices;
          std::vector<uint32_t> indices;
        };

        VEModel(VEDevice& device, const VEModel::ModelBuilder& builder);
        ~VEModel();

        VEModel(const VEModel&) = delete;
        VEModel& operator=(const VEModel&) = delete;

        void Bind(VkCommandBuffer commandBuffer);
        void Draw(VkCommandBuffer commandBuffer);


      private:
        void CreateVertexBuffer(const std::vector<Vertex>& vertices);
        void CreateIndexBuffer(const std::vector<uint32_t>& indices);
        VEDevice& m_Device;
        VkBuffer m_VertexBuffer;
        VkDeviceMemory m_VertexBufferMemory;
        uint32_t m_VertexCount;

        VkBuffer m_IndexBuffer;
        VkDeviceMemory m_IndexBufferMemory;
        uint32_t m_IndexCount;
        bool m_HasIndexBuffer;
    };
} // namespace VE

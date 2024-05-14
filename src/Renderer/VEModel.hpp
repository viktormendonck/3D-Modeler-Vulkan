#pragma once
#include "VEDevice.hpp"

#include "VEBuffer.h"
#include <vector>
#include <memory>
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
            glm::vec3 normal;
            glm::vec2 uv;
            bool selected{false};

            static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
        
          bool operator==(const Vertex& other) const{
            return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
          }

        };

        struct ModelBuilder{
          std::vector<Vertex> vertices;
          std::vector<uint32_t> indices;
          void LoadModel(const std::string& filename);
      
        };

        VEModel(VEDevice& device, const VEModel::ModelBuilder& builder);
        ~VEModel();

        static std::unique_ptr<VEModel> CreateRect(VEDevice &device,const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color);
        static std::unique_ptr<VEModel> CreateElipse(VEDevice& device, int subdevisions, float scale, float widthscale, glm::vec4 color);
        static std::unique_ptr<VEModel> CreateModelFromFile(VEDevice& device, const std::string& filename);

        void Bind(VkCommandBuffer commandBuffer);
        void Draw(VkCommandBuffer commandBuffer);

        std::vector<Vertex>& GetVertices(){ return m_CurrentVertexInfo; }
        uint32_t GetVertexCount() const { return m_VertexCount; }
        void UpdateVertices();
        
      private:
        void CreateVertexBuffer(const std::vector<Vertex>& vertices);
        void CreateIndexBuffer(const std::vector<uint32_t>& indices);

        VEDevice& m_Device;
        std::unique_ptr<VEBuffer> m_VertexBuffer;
        uint32_t m_VertexCount;
        std::unique_ptr<VEBuffer> m_IndexBuffer;

        std::vector<Vertex> m_CurrentVertexInfo;

        uint32_t m_IndexCount;
        bool m_HasIndexBuffer;
    };
} // namespace VE

#include "VEModel.hpp"
#include "VEUtils.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <..\libs\tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <cassert>
#include <stdexcept>
#include <iostream>
#include <unordered_map>

namespace std{
    template<> 
    struct hash<VE::VEModel::Vertex>{
        size_t operator()(VE::VEModel::Vertex const& vertex) const{
            size_t seed = 0;
            VE::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
            return seed;
        }
    };

}

namespace VE{

    VEModel::VEModel(VEDevice &device, const VEModel::ModelBuilder &builder)
        : m_Device(device)
    {
        CreateVertexBuffer(builder.vertices);
        CreateIndexBuffer(builder.indices);
    }

    VEModel::~VEModel()
    {
        vkDestroyBuffer(m_Device.device(), m_VertexBuffer, nullptr);
        vkFreeMemory(m_Device.device(), m_VertexBufferMemory, nullptr);
        if (m_HasIndexBuffer)
        {
        vkDestroyBuffer(m_Device.device(), m_IndexBuffer, nullptr);
        vkFreeMemory(m_Device.device(), m_IndexBufferMemory, nullptr);
        }
    }
    void VEModel::CreateVertexBuffer(const std::vector<Vertex> &vertices)
    {
        m_VertexCount = static_cast<uint32_t>(vertices.size());
        assert(m_VertexCount >= 3 && "Vertex count must be at least 3");

        VkDeviceSize bufferSize = sizeof(vertices[0]) * m_VertexCount;
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        m_Device.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
        
        void* data;
        vkMapMemory(m_Device.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(m_Device.device(), stagingBufferMemory);
        m_Device.createBuffer(bufferSize, 
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
        m_VertexBuffer, m_VertexBufferMemory);

        m_Device.copyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);
        vkDestroyBuffer(m_Device.device(), stagingBuffer, nullptr);
        vkFreeMemory(m_Device.device(), stagingBufferMemory, nullptr);
    }
    void VEModel::CreateIndexBuffer(const std::vector<uint32_t> &indices)
    {
        m_IndexCount = static_cast<uint32_t>(indices.size());
        //prefer runtime errors
        m_HasIndexBuffer = m_IndexCount > 0;
        
        if (!m_HasIndexBuffer)
        {
            return;
        }
        if (m_IndexCount < 3)
        {
            throw std::runtime_error("Index count must be at least 3");
        }
        if (m_IndexCount % 3 != 0)
        {
            throw std::runtime_error("Index count must be a multiple of 3");
        }
    

        VkDeviceSize bufferSize = sizeof(indices[0]) * m_IndexCount;
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        m_Device.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
        
        void* data;
        vkMapMemory(m_Device.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(m_Device.device(), stagingBufferMemory);
        m_Device.createBuffer(bufferSize, 
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
        m_IndexBuffer, m_IndexBufferMemory);

        m_Device.copyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);
        vkDestroyBuffer(m_Device.device(), stagingBuffer, nullptr);
        vkFreeMemory(m_Device.device(), stagingBufferMemory, nullptr);
    }

    std::unique_ptr<VEModel> VEModel::CreateModelFromFile(VEDevice &device, const std::string &filename)
    {
        ModelBuilder builder{};
        builder.LoadModel(filename);
        std::cout<< "vertex cout:" <<builder.vertices.size() << std::endl;
        return std::make_unique<VEModel>(device, builder);
    }

    void VEModel::Bind(VkCommandBuffer commandBuffer)
    {
        VkBuffer buffers[] = {m_VertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
        if (m_HasIndexBuffer)
        {
            vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
        }
    }

    void VEModel::Draw(VkCommandBuffer commandBuffer)
    {
        if (m_HasIndexBuffer)
        {
            vkCmdDrawIndexed(commandBuffer, m_IndexCount, 1, 0, 0, 0);
        }
        else
        {
            vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0);
        }
    }

    std::vector<VkVertexInputBindingDescription> VEModel::Vertex::GetBindingDescriptions()
    {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> VEModel::Vertex::GetAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        attributeDescriptions.push_back({0,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(Vertex, position)});
        attributeDescriptions.push_back({1,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(Vertex, color)});
        attributeDescriptions.push_back({2,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(Vertex, normal)});
        attributeDescriptions.push_back({3,0,VK_FORMAT_R32G32_SFLOAT,offsetof(Vertex, uv)});
        return attributeDescriptions;
    }

    void VEModel::ModelBuilder::LoadModel(const std::string &filename)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warning;
        std::string error;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warning, &error, filename.c_str()))
        {
            throw std::runtime_error(warning + error);
        }
        vertices.clear();
        indices.clear();

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};
        for (const auto &shape : shapes)
        {
            for (const auto &index : shape.mesh.indices)
            {
                Vertex vertex{};
                if (index.vertex_index >= 0)
                {
                    vertex.position = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]};
                }
                if (index.vertex_index >= 0)
                {
                    vertex.color = {
                        attrib.colors[3 * index.vertex_index + 0],
                        attrib.colors[3 * index.vertex_index + 1],
                        attrib.colors[3 * index.vertex_index + 2]};
                }
                
                
                if (index.normal_index >= 0)
                {
                    vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]};
                }
                if (index.texcoord_index >= 0)
                {
                    vertex.uv = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        attrib.texcoords[2 * index.texcoord_index + 1]
                    };
                }
                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }
                indices.push_back(uniqueVertices[vertex]);
            }
        }
    }

}

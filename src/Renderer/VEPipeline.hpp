#pragma once

#include "VEDevice.hpp"

#include <vector>
#include <string>
namespace VE
{
  struct PipelineConfigInfo{
    PipelineConfigInfo() = default;
    PipelineConfigInfo(const PipelineConfigInfo&) = delete;
    PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

    std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

    VkPipelineViewportStateCreateInfo viewportInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    std::vector<VkDynamicState> dynamicStateEnables;
    VkPipelineDynamicStateCreateInfo dynamicStateInfo;
    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;
  };

  class VEPipeline
  {
    public:
      VEPipeline(VEDevice& device ,const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo);
      ~VEPipeline();


      void Bind(VkCommandBuffer commandBuffer);
      static void DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
      

    private:
      static std::vector<char> ReadFile(const std::string& filePath);

      void CreateGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo & configInfo);

      void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

      VEDevice& m_Device;
      VkPipeline m_GraphicsPipeline;
      VkShaderModule m_VertShaderModule{};
      VkShaderModule m_FragShaderModule{};
  };

} 

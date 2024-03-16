#pragma once

#include "VEDevice.hpp"

#include <vector>
#include <string>
namespace VE
{
  struct PipelineConfigInfo{
    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;
  };

  class VEPipeline
  {
    public:
      VEPipeline(VEDevice& device ,const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo);
      ~VEPipeline();

      VEPipeline(const VEPipeline&) = delete;
      VEPipeline& operator=(const VEPipeline&) = delete;

      void Bind(VkCommandBuffer commandBuffer);
      static PipelineConfigInfo DefaultPipelineConfigInfo(uint32_t width, uint32_t height);
      

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

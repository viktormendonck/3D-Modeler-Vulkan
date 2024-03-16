#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
namespace VE {

    class VEWindow {
    public:
        VEWindow(int width, int height, std::string title);
        ~VEWindow();

        VEWindow(const VEWindow&) = delete;
        VEWindow& operator=(const VEWindow&) = delete;

        bool ShouldClose() { return glfwWindowShouldClose(m_pWindow); }

        void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

        VkExtent2D GetExtent() { return {static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height)}; }
        
        bool WasWindowResized() { return m_resized; }
        void ResetWindowResizedFlag() { m_resized = false; }

    private:
        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

        void InitWindow();

        int m_width;
        int m_height;

        bool m_resized{false};

        std::string m_title;
        GLFWwindow* m_pWindow;
    };
}
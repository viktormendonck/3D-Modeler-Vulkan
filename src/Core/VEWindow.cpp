#include "VEWindow.hpp"

#include <stdexcept>

namespace VE{

    VEWindow::VEWindow(int width, int height, std::string title)
        : m_width(width), m_height(height), m_title(title)
    {
        InitWindow();
    }

    VEWindow::~VEWindow()
    {
        glfwDestroyWindow(m_pWindow);
        glfwTerminate();
    }

    void VEWindow::CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
    {
        if (glfwCreateWindowSurface(instance, m_pWindow, nullptr, surface) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create window surface");
        }
    }

    void VEWindow::framebufferResizeCallback(GLFWwindow *window, int width, int height)
    {
        auto pWindow = reinterpret_cast<VEWindow*>(glfwGetWindowUserPointer(window));
        pWindow->m_resized = true;
        pWindow->m_width = width;
        pWindow->m_height = height;
    }

    void VEWindow::InitWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Don't create an OpenGL context since this is vulkan, duh
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); 
        m_pWindow = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);    
        glfwSetWindowUserPointer(m_pWindow, this);
        glfwSetFramebufferSizeCallback(m_pWindow, framebufferResizeCallback);
    }
}
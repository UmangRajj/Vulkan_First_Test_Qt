#ifndef VULKANWIDGET_H
#define VULKANWIDGET_H

#include <QWidget>
#include <QtConcurrent/QtConcurrentRun>
#include <vulkan/vulkan_raii.hpp>

QT_BEGIN_NAMESPACE
namespace Ui {
class VulkanWidget;
}
QT_END_NAMESPACE

class VulkanWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VulkanWidget(QWidget *parent = nullptr);
    ~VulkanWidget() override;

    // Getters
    Ui::VulkanWidget *getUi() const { return ui; }

    // Vulkan Getters
    vk::raii::Context &vkContext() { return m_vkContext; }
    vk::raii::Instance &vkInstance() { return m_vkInstance; }
    vk::raii::DebugUtilsMessengerEXT &vkDebugMessenger() { return m_vkDebugMessenger; }
    vk::raii::PhysicalDevice &vkPhysicalDevice() { return m_vkPhysicalDevice; }
    vk::raii::Device &vkDevice() { return m_vkDevice; }
    /*
    vk::raii::Queue &graphicsQueue() { return m_graphicsQueue; }
    vk::raii::Queue &presentQueue() { return m_presentQueue; }
    vk::raii::SurfaceKHR &windowSurface() { return m_windowSurface; }
    vk::raii::SwapchainKHR &swapChain() { return m_swapChain; }
    QList<VkImage> &swapChainImages() { return m_swapChainImages; }
    VkSurfaceFormatKHR &swapChainFormat() { return m_swapChainFormat; }
    VkExtent2D &swapChainExtent() { return m_swapChainExtent; }
    QList<VkImageView> &swapChainImageViews() { return m_swapChainImageViews; }
    VkPipelineLayout &pipelineLayout() { return m_pipelineLayout; }
    VkRenderPass &renderPass() { return m_renderPass; }
    VkPipeline &graphicsPipeline() { return m_graphicsPipeline; }
    QList<VkFramebuffer> &framebuffers() { return m_framebuffers; }
    VkCommandPool &commandPool() { return m_commandPool; }
    VkCommandBuffer &commandBuffer() { return m_commandBuffer; }
    VkSemaphore &imageReadySphore() { return m_imageReadySphore; }
    VkSemaphore &renderFinishedSphore() { return m_renderFinishedSphore; }
    VkFence &prevFrameWaitFence() { return m_prevFrameWaitFence; }
    */

public:
    // void startMainLoop();

private:
    Ui::VulkanWidget *ui;

    QFuture<void> m_mainLoopFuture; // optional data jo ki parrelel function running se milta hai
                                    // status check karne ke liye
    bool m_mainLoopStop;

    vk::raii::Context m_vkContext;
    vk::raii::Instance m_vkInstance = nullptr;
    vk::raii::DebugUtilsMessengerEXT m_vkDebugMessenger = nullptr; // validation-layers ke messeges khud handle karne ke liye

    vk::raii::PhysicalDevice m_vkPhysicalDevice = nullptr; // computing ke liye physical gpu device
    vk::raii::Device m_vkDevice = nullptr; // logical device
    /*
    vk::raii::Queue m_graphicsQueue; // rendering ke liye graphics work queue!
    vk::raii::Queue m_presentQueue; // Surface (widget) me images present karne ke liye queue
    vk::raii::SurfaceKHR m_windowSurface; // render karne ke liye window surface
    vk::raii::SwapchainKHR m_swapChain; // surface ke liye render swap chain
    QList<VkImage> m_swapChainImages; // swapchain ke images render karne ke liye
    VkSurfaceFormatKHR m_swapChainFormat;
    VkExtent2D m_swapChainExtent;
    QList<VkImageView> m_swapChainImageViews; // swapchain images me render karne ke liye view
    VkRenderPass m_renderPass; // render pass, subpasses ka holder
    VkPipelineLayout m_pipelineLayout; // render pipeline ke liye uniform shader data layout
    VkPipeline m_graphicsPipeline; // main graphics pipeline
    QList<VkFramebuffer> m_framebuffers; // swapchain images ke liye framebuffer
    VkCommandPool m_commandPool; // command pool, command buffer ke memory ko manage karne ke liye
    VkCommandBuffer m_commandBuffer; // commands record kar ke queue me submit karne ke liye buffer
    // semaphore aur fences order lane ke liye //
    VkSemaphore m_imageReadySphore;
    VkSemaphore m_renderFinishedSphore;
    VkFence m_prevFrameWaitFence;
    */
};

#endif // VULKANWIDGET_H

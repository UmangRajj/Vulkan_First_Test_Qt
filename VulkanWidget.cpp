#include "VulkanWidget.h"
#include "ui_VulkanWidget.h"
#include "VulkanSetup.h" // Vulkan Setup funcs, jinhe hum constructor me use karenge

VulkanWidget::VulkanWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VulkanWidget)
{
    ui->setupUi(this);

    connect(ui->startRenderingBtn, &QPushButton::clicked, [this] (const bool& checked) {
        if (checked == true) {
            m_mainLoopStop = false;
            startMainLoop();

            ui->startRenderingBtn->setText("Stop Rendering!");
        } else {
            m_mainLoopStop = true;
            ui->startRenderingBtn->setText("Start Rendering!");
        }
    });

    // Vulkan setup
    initVulkan(this);
    setupWindowSurface(this);
    setupPhysicalDevice(this);
    setupLogicalDevice(this);
    setupSwapchain(this);
    setupImageViews(this);
    setupRenderPass(this);
    setupGraphicsPipeline(this);
    setupFramebuffer(this);
    setupCommandPool(this);
    setupCommandBuffer(this);
    setupSyncObjects(this);

    // shuru se frame drawing (main loop ke andar) shuru hi rakhna hai
    // fir button ise modify karega
    m_mainLoopStop = false;
    startMainLoop();
}

VulkanWidget::~VulkanWidget()
{
    delete ui;
    /*
    //--vulkan handle destructions
    vkDestroySemaphore(m_vkDevice, m_imageReadySphore, nullptr);
    vkDestroySemaphore(m_vkDevice, m_renderFinishedSphore, nullptr);
    vkDestroyFence(m_vkDevice, m_prevFrameWaitFence, nullptr);

    vkDestroyCommandPool(m_vkDevice, m_commandPool, nullptr);
    for (auto &framebuffer : m_framebuffers) {
        vkDestroyFramebuffer(m_vkDevice, framebuffer, nullptr);
    }

    vkDestroyPipeline(m_vkDevice, m_graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(m_vkDevice, m_pipelineLayout, nullptr);
    vkDestroyRenderPass(m_vkDevice, m_renderPass, nullptr);

    for (VkImageView &view : m_swapChainImageViews) {
        vkDestroyImageView(m_vkDevice, view, nullptr);
    }
    vkDestroySwapchainKHR(m_vkDevice, m_swapChain, nullptr);
    vkDestroySurfaceKHR(m_vkInstance, m_windowSurface, nullptr);
    vkDestroyDevice(m_vkDevice, nullptr);

#ifndef NDEBUG
    vkDestroyDebugUtilsMessengerEXT(m_vkInstance, m_vkDebugMessenger, nullptr);
#endif
    vkDestroyInstance(m_vkInstance, nullptr);
    */
}

void VulkanWidget::startMainLoop()
{
    m_mainLoopFuture = QtConcurrent::run([this] {
        while (!m_mainLoopStop) {
            drawFrame(this);
        }
    });
}





























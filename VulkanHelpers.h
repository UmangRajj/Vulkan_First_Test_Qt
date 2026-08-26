#ifndef VULKANHELPERS_H
#define VULKANHELPERS_H

#include "VulkanSetup.h"
#include "VulkanWidget.h"
#include "ui_VulkanWidget.h" // swapchain surface extent resolution ke liye
#include <QDebug>
#include <QList>
#include <QFile>

#include <optional>

class VulkanWidget;

// --vulkan validation layer debug callback
static VkBool32 VKAPI_PTR debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void*                                       pUserData)
{
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        // laal output warning ya error ke liye
        qDebug() << "\033[31m   --Validation Layers warning--\n"<< pCallbackData->pMessage << "\033[0m";
    } else {
        qDebug() << "Validation Layers said- "<< pCallbackData->pMessage;
    }

    return VK_FALSE;
}

// validation layer debug messege printer ke liye
void debugMessengerParmsFill(VkDebugUtilsMessengerCreateInfoEXT &objectToFill, VulkanWidget *data)
{
    objectToFill.sType= VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    objectToFill.pNext = nullptr;
    objectToFill.flags = 0;
    objectToFill.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    objectToFill.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
    objectToFill.pfnUserCallback = debugCallback;
    objectToFill.pUserData = data;
}

// queue families ki avaliblity aur index
struct QueueFamilyIndices {
    std::optional<uint> graphicsFamily;
    std::optional<uint> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

QueueFamilyIndices queryQueueFamilyAvailiblity(VulkanWidget *widget, const VkPhysicalDevice &device)
{
    uint queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    if (queueFamilyCount == 0)
        return QueueFamilyIndices();

    QList<VkQueueFamilyProperties> queueFamilyPropertiesList;
    queueFamilyPropertiesList.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyPropertiesList.data());


    QueueFamilyIndices indices;
    uint it {};
    for (VkQueueFamilyProperties &property : queueFamilyPropertiesList) {
        if (property.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = it;
        }

        VkBool32 surfaceSupport;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, it, widget->windowSurface(), &surfaceSupport);

        if (surfaceSupport)
            indices.presentFamily = it;

        if (indices.isComplete())
            break;

        ++it;
    }

    return indices;
}

struct SwapChainCapablityDetail
{
    VkSurfaceCapabilitiesKHR surfaceCaps;
    QList<VkSurfaceFormatKHR> surfaceFormats;
    QList<VkPresentModeKHR> surfacePresentModes;
};

SwapChainCapablityDetail querySwapChainSupport(VulkanWidget &widget, VkPhysicalDevice &physicalDevice)
{
    SwapChainCapablityDetail swapChainCaps;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, widget.windowSurface(), &swapChainCaps.surfaceCaps);

    // surface ke supported formats fetching
    uint surfaceFormatsCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, widget.windowSurface(), &surfaceFormatsCount, nullptr);
    swapChainCaps.surfaceFormats.resize(surfaceFormatsCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, widget.windowSurface(), &surfaceFormatsCount, swapChainCaps.surfaceFormats.data());

    // surface ke supported formats fetching
    uint surfacePresentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, widget.windowSurface(), &surfacePresentModeCount, nullptr);
    swapChainCaps.surfacePresentModes.resize(surfacePresentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, widget.windowSurface(), &surfaceFormatsCount, swapChainCaps.surfacePresentModes.data());

    return swapChainCaps;
}

void chooseSwapchainSettings(VulkanWidget &widget,
                             const SwapChainCapablityDetail &capablityDetail,
                             VkSurfaceFormatKHR &surfaceFormat,
                             VkPresentModeKHR &presentMode,
                             VkExtent2D &surfaceExtent)
{
    // format choosing
    VkSurfaceFormatKHR preferredFormat;
    preferredFormat.format = VkFormat::VK_FORMAT_B8G8R8A8_SRGB;
    preferredFormat.colorSpace = VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

    for (const VkSurfaceFormatKHR &format : capablityDetail.surfaceFormats) {
        if (format.format == preferredFormat.format
            && format.colorSpace == preferredFormat.colorSpace) {
            surfaceFormat = format;
        }
    }

    if (surfaceFormat.format != preferredFormat.format
        || surfaceFormat.colorSpace != preferredFormat.colorSpace)
    {
        surfaceFormat = capablityDetail.surfaceFormats[0];
    }

    // present mode choosing
    // FIFO mode ki availiblity har vulkan compatible device ke liye sure hoti hai
    // abhi mujhe yahi achha laga to me yahi use karta hu!
    // TODO - baad me dusre present mode try karo!
    VkPresentModeKHR preferredPresentMode;
    preferredPresentMode = VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;

    presentMode = preferredPresentMode;

    // surface extent (resolution) choosing
    if (capablityDetail.surfaceCaps.currentExtent.height != std::numeric_limits<uint32_t>::max()) {
        surfaceExtent = capablityDetail.surfaceCaps.currentExtent;
    } else {
        surfaceExtent.height = widget.getUi()->rendererFrame->height();
        surfaceExtent.width = widget.getUi()->rendererFrame->width();
    }
}

// saadharan sa file reader, shaders ke liye
QByteArray readShader(const QString &shader_location)
{
    QFile shaderFile {shader_location};

    if (!shaderFile.open(QIODevice::OpenModeFlag::ReadOnly))
        throw std::runtime_error("Couldn't parse shader file!");

    QByteArray shaderBytes = shaderFile.readAll();
    shaderFile.close();

    return shaderBytes;
}

// shader file binary se shader module banta hai (aur shaderBinary ko free bhi karta hai)
VkShaderModule createShaderModule(QByteArray &shaderBinary, const VkDevice &device)
{
    if ((shaderBinary.size() % 4) != 0)
    {
        // uint32* me conversion nahi ho sakta!
        throw std::runtime_error("Couldn't use shader file, can't be converted to 32 bit pointer!");
    }

    VkShaderModuleCreateInfo createInfo {
        VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        nullptr,
        0,
        (size_t)shaderBinary.size(),
        reinterpret_cast<const uint32_t *>(shaderBinary.data())
    };

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        throw std::runtime_error("Failed to create shader module");

    shaderBinary.clear();

    return shaderModule;
}

// command buffer ko rendering commands se fill karega,
// "imageIndex" swapchain image ke liye
void recordCommandBuffer(VulkanWidget *widget, VkCommandBuffer &buffer, const uint32_t &imageIndex)
{
    // rendering begin info
    VkCommandBufferBeginInfo beginInfo {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        nullptr,
        0, // commmand buffer record flags
        nullptr // secondary command buffer ke liye caller primary command buffer
                // ka konsa state inherit karna hai
    };

    // command buffer recording shuru karo
    if (vkBeginCommandBuffer(buffer, &beginInfo) != VK_SUCCESS)
        throw std::runtime_error("Failed to create shader module");

    //--sabse pehle render pass shuru karne ka command denge
    VkClearValue clearColor {1.f, 0.f, 0.f, 1.f}; // laal (red)
    VkRenderPassBeginInfo rpassBeginInfo {
        VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        nullptr,
        widget->renderPass(),
        widget->framebuffers()[imageIndex],
        {{0,0}, widget->swapChainExtent()},
        1,
        &clearColor // attachment ko clear karte waqt reset color!
    };

    vkCmdBeginRenderPass(buffer, &rpassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, widget->graphicsPipeline());

    //--dynamic states setup
    VkViewport vport {
        0.f,
        0.f,
        (float)widget->swapChainExtent().width,
        (float)widget->swapChainExtent().height,
        0.f,
        1.f
    };
    vkCmdSetViewport(buffer, 0, 1, &vport);

    VkRect2D scissor {
        {0,0},
        widget->swapChainExtent()
    };
    vkCmdSetScissor(buffer, 0, 1, &scissor);

    //-------DRAW-COMMAND--------
    vkCmdDraw(buffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(buffer);

    if (vkEndCommandBuffer(buffer) != VK_SUCCESS)
        throw std::runtime_error("Couldn't record command buffer!");
}

#endif // VULKANHELPERS_H
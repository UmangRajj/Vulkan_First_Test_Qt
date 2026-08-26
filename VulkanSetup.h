#ifndef VULKANSETUP_H
#define VULKANSETUP_H

#include "VulkanWidget.h"

// sare vulkan objects widget ke getter se liye gaye hai

// Vulkan Instance create kar ke, validation layers
// setup karne ke liye
void initVulkan(VulkanWidget *widget);

// Window surface creation viewer widget ke liye
void setupWindowSurface(VulkanWidget *widget);

// physical device choose aur setup karne ke liye
void setupPhysicalDevice(VulkanWidget *widget);

// Logical device (VkDevice) setup karta hai
void setupLogicalDevice(VulkanWidget *widget);

// Surface ke liye Swapchain setup
void setupSwapchain(VulkanWidget *widget);

// Swapchain image view setup
void setupImageViews(VulkanWidget *widget);

// Render Pass Setup
void setupRenderPass(VulkanWidget *widget);

// Render pipeline setup (Vertex shading, rasterization, fragment shading, color blending)
void setupGraphicsPipeline(VulkanWidget *widget);

// Framebuffer setup, render pass ke liye
void setupFramebuffer(VulkanWidget *widget);

// Commandpool setup, commandpool commandbuffer ka buffer memory manage karta hai
// aur commandpool se hi hum commandbuffer allocate karte hai
void setupCommandPool(VulkanWidget *widget);

// Commandbuffer me hum device queue ke liye render command record karte hai
// fir unhe ek sath submit karte hai, speed ke liye ek sath record karte hai
void setupCommandBuffer(VulkanWidget *widget);

// Sync object, jaise semaphores aur fences, gpu/cpu work
// me order lane wale objects ka setup
void setupSyncObjects(VulkanWidget *widget);

// Final Function image ko swapchain framebuffer me
// render karne ke liye, Main Loop me call hoga
void drawFrame(VulkanWidget *widget);

#endif // VULKANSETUP_H

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "VulkanSetup.h"

#include "VulkanHelpers.h" // sare helpers jinhe hum vulkan setup funcs me use karte hai

#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vk_enum_string_helper.h>
#include <windows.h>

#include <QMessageBox>
#include <QTimer>
#include <QApplication>
#include <QSet>
#include <algorithm>

void initVulkan(VulkanWidget *widget)
{
    // --Vulkan Instance Banao
    constexpr vk::ApplicationInfo vkAppInfo {
        .pNext = nullptr,
        .pApplicationName = "Vulkan First Test!",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "No Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = vk::ApiVersion14,
    };

    const char *extsList[] {
        vk::KHRSurfaceExtensionName,
        vk::KHRWin32SurfaceExtensionName,
        vk::EXTDebugUtilsExtensionName
    };
    const char *validationLayers[] {"VK_LAYER_KHRONOS_validation"};

#ifndef NDEBUG
    // instance creation debug karne ke liye
    vk::DebugUtilsMessengerCreateInfoEXT debugMessengerInstCreateInfo;
    // VulkanHelpers.h se, callback aur parameters wagerah wahi handle hote hai
    debugMessengerParmsFill(debugMessengerInstCreateInfo, widget);
#endif

    vk::InstanceCreateInfo instanceCreateInfo {
#ifdef NDEBUG
        .pNext = nullptr,
#else
        .pNext = &debugMessengerInstCreateInfo, // pnext me ye dena hota hai taki
                                                // instance creation/destruction debug ho sake
#endif
        .pApplicationInfo = &vkAppInfo,
#ifdef NDEBUG
        .enabledLayerCount = 0,
        .ppEnabledExtensionNames = nullptr,
#else
        .enabledLayerCount = std::size(validationLayers),
        .ppEnabledLayerNames = validationLayers,
#endif
        .enabledExtensionCount = std::size(extsList),
        .ppEnabledExtensionNames = extsList
    };

    widget->vkInstance() = vk::raii::Instance(widget->vkContext(), instanceCreateInfo);


#ifndef NDEBUG
    // --Debug Messanger Setup karo baki kaamo ke liye
    vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo;
    debugMessengerParmsFill(debugMessengerCreateInfo, widget);

    widget->vkDebugMessenger() = widget->vkInstance().createDebugUtilsMessengerEXT(debugMessengerCreateInfo, nullptr);
#endif
}

/*
void setupWindowSurface(VulkanWidget *widget)
{
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo {
        VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        nullptr,
        0,
        GetModuleHandle(nullptr),
        reinterpret_cast<HWND>(widget->winId())
    };

    PFN_vkCreateWin32SurfaceKHR createSurface = (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(widget->vkInstance(), "vkCreateWin32SurfaceKHR");

    VkResult result = createSurface(widget->vkInstance(), &surfaceCreateInfo, nullptr, &widget->windowSurface());
    if (result != VK_SUCCESS) {
        QMessageBox errorCritical;
        errorCritical.critical((QWidget*)widget, "Couldn't Create Render Surface!",
                               "Failed to create Vulkan Renderable window, reason- " + QString(string_VkResult(result)));
        errorCritical.open();

        // qapp event loop shuru hone ke baad app band karo
        QTimer::singleShot(0, [widget] () {
            QApplication::quit();
        });
        return;
    }
}
*/

void setupPhysicalDevice(VulkanWidget *widget)
{
    auto devices = widget->vkInstance().enumeratePhysicalDevices();

    qDebug() << "Vulkan supported physical device (GPU) count- " << devices.size();

    if (devices.empty()) {
        QMessageBox errorCritical;
        errorCritical.critical((QWidget*)widget, "No Vulkan Device!",
                               "Vulkan supported device (GPU) not avaiible!");
        errorCritical.open();

        // qapp event loop shuru hone ke baad app band karo
        QTimer::singleShot(0, [widget] () {
            QApplication::quit();
        });
        return;
    }

    for (uint i {}; i < devices.size(); ) {
        vk::PhysicalDeviceProperties deviceProperties = devices[i].getProperties();
        vk::PhysicalDeviceFeatures deviceFeatures = devices[i].getFeatures();

        bool graphicsQueueSupported = std::ranges::

        // swap chain supported hai ya nahi, iska query func helpers header me hai
        bool swapChainSupported;
        SwapChainCapablityDetail capDetail = querySwapChainSupport(*widget, devices[i]);
        swapChainSupported = !capDetail.surfaceFormats.empty() && !capDetail.surfacePresentModes.empty();

        if (deviceProperties.apiVersion >= vk::ApiVersion13
            && queryQueueFamilyAvailiblity(widget, devices[i]).isComplete()
            && swapChainSupported) {
            qDebug() << "Device " << deviceProperties.deviceName << "is choosed!";
            ++i;
        } else {
            qDebug() << "Device" << deviceProperties.deviceName << "isn't choosed!";
            devices.erase(devices.begin() + i);
        }
    }

    // ab hum pehla device list se choose kar lenge!
    if (devices.size()) {
        widget->vkPhysicalDevice() = devices[0];
    } else {
        QMessageBox errorCritical;
        errorCritical.critical((QWidget*)widget, "No Vulkan Device!",
                               "Failed to find a vulkan compatible device!");
        errorCritical.open();

        // qapp event loop shuru hone ke baad app band karo
        QTimer::singleShot(0, [widget] () {
            QApplication::quit();
        });
        return;
    }
}

/*
void setupLogicalDevice(VulkanWidget *widget)
{
    QueueFamilyIndices queueFamilies = queryQueueFamilyAvailiblity(widget, widget->vkPhysicalDevice());
    float queuePriority = 0.5f;

    QList<VkDeviceQueueCreateInfo> queueCreateInfos;
    QSet<uint> uniqueQueueFamilies;
    // ye shayad 2 ho, ya 1, mere liye ye 1 hi hoga, fir bhi..
    uniqueQueueFamilies << queueFamilies.graphicsFamily.value() << queueFamilies.presentFamily.value();

    for (const uint &queueFamiliy : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo curQueueCreateInfo {
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            nullptr,
            0,
            queueFamiliy,
            1,
            &queuePriority
        };
        queueCreateInfos.push_back(curQueueCreateInfo);
    }

    VkPhysicalDeviceFeatures featuresToUse {};
    const char *validationLayers[] {"VK_LAYER_KHRONOS_validation"};
    const char *deviceExtsList[] {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VkDeviceCreateInfo deviceCreateInfo {
        VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        nullptr,
        0,
        1,
        queueCreateInfos.data(),
        (uint32_t)queueCreateInfos.size(),
#ifndef NDEBUG
        validationLayers,
#else
            nullptr,
#endif
        std::size(deviceExtsList),
        deviceExtsList,
        &featuresToUse
    };

    VkResult result = vkCreateDevice(widget->vkPhysicalDevice(), &deviceCreateInfo, nullptr, &widget->vkDevice());
    if (result != VK_SUCCESS) {
        QMessageBox errorCritical;
        errorCritical.critical((QWidget*)widget, "Couldn't Create Vulkan Device!",
                               "Failed to create Vulkan Device, reason- " + QString(string_VkResult(result)));
        errorCritical.open();

        // qapp event loop shuru hone ke baad app band karo
        QTimer::singleShot(0, [widget] () {
            QApplication::quit();
        });
        return;
    }

    vkGetDeviceQueue(widget->vkDevice(), queueFamilies.graphicsFamily.value(), 0, &widget->graphicsQueue());
    vkGetDeviceQueue(widget->vkDevice(), queueFamilies.presentFamily.value(), 0, &widget->presentQueue());
}

void setupSwapchain(VulkanWidget *widget)
{
    //--setting choosing
    // yaha hum resolution (extent), format, aur aisi chije,
    // surface ke capapblities (caps) ke hisab se select karenge
    SwapChainCapablityDetail capsDetail = querySwapChainSupport(*widget, widget->vkPhysicalDevice());
    VkSurfaceFormatKHR choosedFormat;
    VkPresentModeKHR choosedPresentMode;
    VkExtent2D choosedExtent;

    chooseSwapchainSettings(*widget, capsDetail, choosedFormat, choosedPresentMode, choosedExtent);

    uint imageCount = capsDetail.surfaceCaps.minImageCount + 1;

    if (capsDetail.surfaceCaps.maxImageCount > 0
        && imageCount > capsDetail.surfaceCaps.maxImageCount) {
        imageCount = capsDetail.surfaceCaps.maxImageCount;
    }
    qDebug() << "Choosed swapchain image count- " << imageCount;

    VkSwapchainCreateInfoKHR swapCreateInfo {
        VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        nullptr,
        0,
        widget->windowSurface(),
        imageCount,
        choosedFormat.format,
        choosedFormat.colorSpace,
        choosedExtent,
        1, // image layer amount, streoscopic rendering jaise chijo ke liye 1 se jyada
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, // swap chain ka usage kya hai
        VK_SHARING_MODE_EXCLUSIVE, // kya alag alag queue family ise use kar sakte hai?
        0,
        nullptr,
        capsDetail.surfaceCaps.currentTransform,
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, // TODO - ALPHA ON KAR KE DEKHO!
        choosedPresentMode,
        VK_TRUE,
        VK_NULL_HANDLE
    };

    QueueFamilyIndices indices = queryQueueFamilyAvailiblity(widget, widget->vkPhysicalDevice());
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        swapCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapCreateInfo.queueFamilyIndexCount = 2;
        swapCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        swapCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapCreateInfo.queueFamilyIndexCount = 0; // Optional
        swapCreateInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    VkResult result = vkCreateSwapchainKHR(widget->vkDevice(), &swapCreateInfo, nullptr, &widget->swapChain());

    if (result != VK_SUCCESS) {
        QMessageBox errorCritical;
        errorCritical.critical((QWidget*)widget, "Couldn't Create Swapchain!",
                               "Failed to create swapchain to render to, reason- " + QString(string_VkResult(result)));
        errorCritical.open();

        // qapp event loop shuru hone ke baad app band karo
        QTimer::singleShot(0, [widget] () {
            QApplication::quit();
        });
        return;
    }

    //--Image acquiring aur setting storing
    uint swapImageCount;
    vkGetSwapchainImagesKHR(widget->vkDevice(), widget->swapChain(), &swapImageCount, nullptr);
    widget->swapChainImages().resize(swapImageCount);
    vkGetSwapchainImagesKHR(widget->vkDevice(), widget->swapChain(), &swapImageCount, widget->swapChainImages().data());

    widget->swapChainFormat() = choosedFormat;
    widget->swapChainExtent() = choosedExtent;
}

void setupImageViews(VulkanWidget *widget)
{
    widget->swapChainImageViews().resize(widget->swapChainImages().size());

    for (size_t i {}; i < widget->swapChainImages().size(); ++i) {
        // ek hi image channel component ko dusre channel me use karo
        VkComponentMapping componentMap {
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY
        };
        // image view type (aspect), mip-map aur image layers count
        VkImageSubresourceRange subrsrcRng {
            VK_IMAGE_ASPECT_COLOR_BIT,
            0,
            1,
            0,
            1
        };
        VkImageViewCreateInfo createInfo {
            VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            nullptr,
            0,
            widget->swapChainImages()[i],
            VK_IMAGE_VIEW_TYPE_2D,
            widget->swapChainFormat().format,
            componentMap,
            subrsrcRng
        };
        vkCreateImageView(widget->vkDevice(), &createInfo, nullptr, &widget->swapChainImageViews()[i]);
    }
}

void setupRenderPass(VulkanWidget *widget)
{
    // attachment desicription humare render target framebuffer image desc
    // ko represent karta hai, is waqt ye humare swap chain ke image ka desc set karega
    VkAttachmentDescription colAttachmentDesc {
        0,
        widget->swapChainFormat().format,
        VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR,
        VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };

    // pehle arg hamare attachment ke arr ka index hai, jo hum baad me jodenge
    VkAttachmentReference colAttachmentRef {
        0,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    // hume sirf ek subpass me sirf ek color output hi karna hai
    // shader me hum ye is tarah se karenge-
    // layout(location = 0) out vec4 outColor
    // jaha location attachment arr ka index hai aur
    // data type format attachment ke format se match karna chahiye!
    VkSubpassDescription subpDesc {
        0,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        0,
        nullptr, // input attachment shader me texture/frame input dene ke liye
        1,
        &colAttachmentRef, // color attachment, color output karne ke liye
        nullptr, // resolve attachment, multisampling (MSAA) ke liye
        nullptr, // depth/stencil attachment
        0,
        nullptr // preserver attachment, desc-
                // vulkan unused attachments ko discard/overwrite kar sakta hai
                // yaha par aap inhe dekar aap unke values preserve kar sakte hai
    };

    VkRenderPassCreateInfo renderPassInfo {
        VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        nullptr,
        0,
        1,
        &colAttachmentDesc,
        1,
        &subpDesc,
        0,
        nullptr
    };

    VkResult result = vkCreateRenderPass(widget->vkDevice(), &renderPassInfo, nullptr, &widget->renderPass());
    if (result != VK_SUCCESS) {
        QMessageBox errorCritical;
        errorCritical.critical((QWidget*)widget, "Couldn't Create Renderpass!",
                               "Failed to create vulkan renderpass, reason- " + QString(string_VkResult(result)));
        errorCritical.open();

        // qapp event loop shuru hone ke baad app band karo
        QTimer::singleShot(0, [widget] () {
            QApplication::quit();
        });
        return;
    }
}

void setupGraphicsPipeline(VulkanWidget *widget)
{
    // --vertex input state setup, chuki hum vertex attribs ko shader me hardcode karenge
    // --isiliye bindings(sare data ka info) aur attributes (data specefic info) default hai
    VkPipelineVertexInputStateCreateInfo vertInputStateInfo {
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        nullptr,
        0,
        0,
        nullptr,
        0,
        nullptr
    };

    // --vertex input assembly setup, kaise hum input vertex buffer data ko handle karenge
    // --aur unse sare trikon banyenge
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo {
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        nullptr,
        0,
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        VK_FALSE // strip topology types ke liye, ki hum lines/triangles ko tod sakte hai ya nahi
    };

    // --shader loading, shader module aur shader pipeline stage creation
    QByteArray fragShader = readShader("shaders/frag_shader.spv");
    QByteArray vertShader = readShader("shaders/vert_shader.spv");

    VkShaderModule fragModule = createShaderModule(fragShader, widget->vkDevice());
    VkShaderModule vertModule = createShaderModule(vertShader, widget->vkDevice());

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        nullptr,
        0,
        VK_SHADER_STAGE_FRAGMENT_BIT,
        fragModule,
        "main",
        nullptr
    };
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        nullptr,
        0,
        VK_SHADER_STAGE_VERTEX_BIT,
        vertModule,
        "main",
        nullptr
    };

    VkPipelineShaderStageCreateInfo shaderStageCreateInfos[] {fragShaderStageInfo, vertShaderStageInfo};

    // --dynamic states setup
    // --wese states jinhe hum baad me change kar sakte bina pipline recreation ke
    VkDynamicState dynStates[] {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynStateInfo {
        VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        nullptr,
        0,
        std::size(dynStates),
        dynStates
    };

    //--viewport aur scissor setup
    // viweport framebuffer ka wo jagah hai jisme hum render karenge
    VkViewport viewport {
        0.f,
        0.f,
        (float)widget->swapChainExtent().width,
        (float)widget->swapChainExtent().height,
        0.f,
        1.f
    };

    // scissor me aap us jagah ko specify karte hai jise aap rendering se kaatna chahenge
    // ise aap dynamic bana sakte hai, isme jo area define hoga sirf wahi render hoga
    VkRect2D scissor;
    scissor.offset = {0, 0};
    scissor.extent = widget->swapChainExtent();

    VkPipelineViewportStateCreateInfo viewportStateInfo {
        VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        nullptr,
        0,
        1, // viewport count
        nullptr,
        1, // scissor count
        nullptr
    };

    //--rasterization aur multisampling aa (msaa) setup
    VkPipelineRasterizationStateCreateInfo rasterizationStateInfo {
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        nullptr,
        0,
        VK_FALSE, // depth clamp, near/far border ke bahar wale pixels ko discard mat karo clamp karo!
        VK_FALSE, // rasterization step discard
        VK_POLYGON_MODE_FILL,
        VK_CULL_MODE_BACK_BIT,
        VK_FRONT_FACE_CLOCKWISE,
        VK_FALSE, // depth bias, niche 3 depth bias arguements
        0.f,
        0.f,
        0.f,
        1.f // line width
    };

    // MSAA ka setup, abhi off hai
    // TODO - MSAA ON KARKE DEKHO!
    VkPipelineMultisampleStateCreateInfo multisamplingStateInfo {
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        nullptr,
        0,
        VK_SAMPLE_COUNT_1_BIT,
        VK_FALSE,
        1.f,
        nullptr,
        VK_FALSE,
        VK_FALSE
    };

    // TODO - DEPTH/STENCIL TEST SETUP KARO!

    //--Color blending setup
    // per framebuffer color blend config attachment
    VkPipelineColorBlendAttachmentState colorBlendAttachment {
        VK_FALSE,
        VK_BLEND_FACTOR_ONE,
        VK_BLEND_FACTOR_ZERO,
        VK_BLEND_OP_ADD,
        VK_BLEND_FACTOR_ONE,
        VK_BLEND_FACTOR_ZERO,
        VK_BLEND_OP_ADD,
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
        | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
    };

    VkPipelineColorBlendStateCreateInfo colorBlendStateInfo {
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        nullptr,
        0,
        VK_FALSE, // logic op usage, agar on hua to per framebuffer attachment ko disable kar dega
        VK_LOGIC_OP_COPY,
        1,
        &colorBlendAttachment,
        {0.f, 0.f, 0.f, 0.f}
    };

    //--layout setup
    // shader me uniform variables ka definition layouts se hota hai
    // inhe layout isliye kehte hai kyoki ye data ka layout tay karta hai
    // ye hum program ke lifetime tak rakhenge (layout ko)
    VkPipelineLayoutCreateInfo layoutInfo {
        VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        nullptr,
        0,
        0,
        nullptr,
        0,
        nullptr
    };

    VkResult layoutResult = vkCreatePipelineLayout(widget->vkDevice(), &layoutInfo, nullptr, &widget->pipelineLayout());

    if (layoutResult != VK_SUCCESS) {
        throw std::runtime_error("Couldn't create vulkan pipeline layout!");
        return;
    }

    VkGraphicsPipelineCreateInfo pipelineInfo {
        VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        nullptr,
        0,
        2,
        shaderStageCreateInfos,
        &vertInputStateInfo,
        &inputAssemblyInfo,
        nullptr,
        &viewportStateInfo,
        &rasterizationStateInfo,
        &multisamplingStateInfo,
        nullptr,
        &colorBlendStateInfo,
        &dynStateInfo,
        widget->pipelineLayout(),
        widget->renderPass(),
        0,
        VK_NULL_HANDLE,
        -1
    };

    VkResult result = vkCreateGraphicsPipelines(widget->vkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &widget->graphicsPipeline());
    if (result != VK_SUCCESS) {
        QMessageBox errorCritical;
        errorCritical.critical((QWidget*)widget, "Couldn't Create Graphics Pipeline!",
                               "Failed to create vulkan graphics pipeline, reason- " + QString(string_VkResult(result)));
        errorCritical.open();

        // qapp event loop shuru hone ke baad app band karo
        QTimer::singleShot(0, [widget] () {
            QApplication::quit();
        });
        return;
    }
    vkDestroyShaderModule(widget->vkDevice(), fragModule, nullptr);
    vkDestroyShaderModule(widget->vkDevice(), vertModule, nullptr);
}


void setupFramebuffer(VulkanWidget *widget)
{
    widget->framebuffers().resize(widget->swapChainImageViews().size());
    for (size_t i {}; i < widget->framebuffers().size(); ++i) {
        VkImageView attachments[] {
            widget->swapChainImageViews()[i]
        };

        VkFramebufferCreateInfo framebufferInfo {
            VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            nullptr,
            0,
            widget->renderPass(),
            std::size(attachments),
            attachments,
            widget->swapChainExtent().width,
            widget->swapChainExtent().height,
            1
        };

        if (vkCreateFramebuffer(widget->vkDevice(), &framebufferInfo, nullptr, &widget->framebuffers()[i]) != VK_SUCCESS)
            throw std::runtime_error("Failed to create framebuffer for surface swapchain images!");
    }
}

void setupCommandPool(VulkanWidget *widget)
{
    VkCommandPoolCreateInfo info {
        VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        nullptr,
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        queryQueueFamilyAvailiblity(widget, widget->vkPhysicalDevice()).graphicsFamily.value()
    };

    if (vkCreateCommandPool(widget->vkDevice(), &info, nullptr, &widget->commandPool()) != VK_SUCCESS)
        throw std::runtime_error("Failed to create command pool for command buffer (used in recording graphics draw commands)!");
}

void setupCommandBuffer(VulkanWidget *widget)
{
    // command buffer allocate info, isme  hum command pool aur usme banaye gaye
    // command buffers ka count aur command buffer type set karenge
    VkCommandBufferAllocateInfo allocInfo {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        nullptr,
        widget->commandPool(),
        VK_COMMAND_BUFFER_LEVEL_PRIMARY, // secondary level ko hum queue me submit nahi kar sakte
                                         // lekin, use dusre command buffer me call kar sakte
        1
    };
    if (vkAllocateCommandBuffers(widget->vkDevice(), &allocInfo, &widget->commandBuffer()) != VK_SUCCESS)
        throw std::runtime_error("Failed to create vulkan command buffers (used in recording graphics draw commands)!");
}

void setupSyncObjects(VulkanWidget *widget)
{
    VkSemaphoreCreateInfo sphoreInfo {
        VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };
    VkFenceCreateInfo fenceInfo {
        VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        nullptr,
        VK_FENCE_CREATE_SIGNALED_BIT
    };

    if (vkCreateSemaphore(widget->vkDevice(), &sphoreInfo, nullptr, &widget->imageReadySphore()) != VK_SUCCESS
        || vkCreateSemaphore(widget->vkDevice(), &sphoreInfo, nullptr, &widget->imageReadySphore()) != VK_SUCCESS
        || vkCreateFence(widget->vkDevice(), &fenceInfo, nullptr, &widget->prevFrameWaitFence()) != VK_SUCCESS)
        throw std::runtime_error("Couldn't create vulkan synchronization objects (Semaphores and Fences)!");

}

void drawFrame(VulkanWidget *widget)
{
    vkWaitForFences(widget->vkDevice(), 1, &widget->prevFrameWaitFence(), VK_TRUE, UINT64_MAX);
    vkResetFences(widget->vkDevice(), 1, &widget->prevFrameWaitFence());


}
*/

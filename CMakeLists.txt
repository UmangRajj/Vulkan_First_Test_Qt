cmake_minimum_required(VERSION 3.7)
project(Vulkan_First_Test LANGUAGES CXX)

# VOLK KE LIYE, KYOKI VULKAN ME EXT FUNCTIONS KE PROTOTYPE HOTE HAI
add_compile_definitions(VK_NO_PROTOTYPES VK_USE_PLATFORM_WIN32_KHR)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
find_package(Vulkan REQUIRED)

qt_standard_project_setup()

qt_add_executable(Vulkan_First_Test
    WIN32 MACOSX_BUNDLE
    main.cpp
    VulkanWidget.cpp
    VulkanWidget.h
    VulkanWidget.ui
    VulkanHelpers.h
    VulkanSetup.h
    VulkanSetup.cpp
    vert_shader.vert
    frag_shader.frag

)

target_include_directories(Vulkan_First_Test PRIVATE ${Vulkan_INCLUDE_DIR})

target_link_libraries(Vulkan_First_Test
    PRIVATE
        Qt::Core
        Qt::Widgets
        # Volk se link karne ke baad vulkan se karne ki jarurat nahi
        # SIRF MINGW KE LIYE, MSVC KE LIYE HUM DIRECT LINK KAR SAKTE HAI
        # TAB UPAR ME VULKAN PACKAGE KA "volk" COMPONENT BHI LENA PADEGA
        "D:/Packages/VulkanSDK/1.4.350.0/Lib/libvolk.a"
)

install(TARGETS Vulkan_First_Test
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Vulkan_First_Test
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

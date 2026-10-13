// vk_base.h — ready-made Vulkan bring-up for the "no Mac" track.
//
// Helper code, not part of the task. It plays the role MTKView plays on the
// Mac track: instance + validation messenger, surface, physical device and
// one graphics+present queue, logical device (Vulkan 1.3 with dynamic
// rendering and synchronization2), swapchain with image views, depth image,
// and swapchain recreation on resize. You do not need to change it, but you
// may (or replace it with vk-bootstrap) — say so in README.
//
// What it does NOT do (that is your part, see main.cpp): pipeline, buffers,
// texture upload with layout transitions, descriptors, per-frame command
// buffers, fences and semaphores, the frame loop.
#pragma once

#include <GLFW/glfw3.h>  // GLFW_INCLUDE_VULKAN is defined in CMakeLists.txt

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <vector>

#define VK_CHECK(expr)                                                              \
    do {                                                                            \
        VkResult _r = (expr);                                                       \
        if (_r != VK_SUCCESS) {                                                     \
            std::fprintf(stderr, "%s failed: VkResult %d (%s:%d)\n", #expr, (int)_r, \
                         __FILE__, __LINE__);                                       \
            std::abort();                                                           \
        }                                                                           \
    } while (0)

namespace bhs {

class VulkanBase {
public:
    // Creates everything listed above. Validation layers are enabled when
    // `enableValidation` is true and VK_LAYER_KHRONOS_validation is installed.
    void Init(GLFWwindow* window, bool enableValidation);

    // Recreates the swapchain, its image views, the depth image and the
    // per-image renderFinished semaphores for the current framebuffer size.
    // Call it after VK_ERROR_OUT_OF_DATE_KHR / VK_SUBOPTIMAL_KHR or a resize.
    // Waits while the window is minimized (framebuffer 0x0) and calls
    // vkDeviceWaitIdle, which is fine here: it is not done every frame.
    void RecreateSwapchain();

    // Destroys everything created by Init. Destroy your own objects first.
    void Destroy();

    // --- Helpers -----------------------------------------------------------
    uint32_t FindMemoryType(uint32_t typeBits, VkMemoryPropertyFlags props) const;
    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags props,
                      VkBuffer& buffer, VkDeviceMemory& memory) const;
    // 2D image, 1 mip level, 1 layer, optimal tiling, device-local memory.
    // Initial layout is VK_IMAGE_LAYOUT_UNDEFINED: transitions are up to you.
    void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage,
                     VkImage& image, VkDeviceMemory& memory) const;
    VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspect) const;
    // Reads a SPIR-V file (e.g. BHS_SHADER_DIR "/cube.vert.spv").
    VkShaderModule LoadShaderModule(const char* path) const;
    // Records commands into a temporary command buffer, submits them and
    // waits for completion. For one-time uploads at startup, not per frame.
    void SubmitImmediate(const std::function<void(VkCommandBuffer)>& record) const;

    // --- Created objects (read them, do not destroy them) ------------------
    GLFWwindow* window = nullptr;
    VkInstance instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    uint32_t queueFamily = 0;  // supports graphics and present
    VkQueue queue = VK_NULL_HANDLE;

    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkFormat swapchainFormat = VK_FORMAT_UNDEFINED;  // an *_SRGB format if available
    VkExtent2D swapchainExtent{};                    // in pixels
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainViews;
    // One semaphore per swapchain image: signal renderFinished[imageIndex] in
    // vkQueueSubmit and wait on it in vkQueuePresentKHR. A semaphore per frame
    // slot would be reused while the presentation engine may still hold it.
    std::vector<VkSemaphore> renderFinished;

    VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;
    VkImage depthImage = VK_NULL_HANDLE;  // same size as the swapchain, layout UNDEFINED
    VkDeviceMemory depthMemory = VK_NULL_HANDLE;
    VkImageView depthView = VK_NULL_HANDLE;

private:
    void CreateInstance(bool enableValidation);
    void PickPhysicalDevice();
    void CreateDevice();
    void CreateSwapchain(VkSwapchainKHR oldSwapchain);
    void DestroySwapchainResources();

    VkCommandPool immediatePool_ = VK_NULL_HANDLE;
    bool portabilitySubset_ = false;
};

}  // namespace bhs

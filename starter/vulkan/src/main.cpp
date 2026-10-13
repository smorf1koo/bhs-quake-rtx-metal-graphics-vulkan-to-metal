// main.cpp — starter skeleton for the "no Mac" track (Vulkan 1.3 + GLFW).
//
// Already done: GLFW window, the main loop, and the Vulkan bring-up in
// vk_base.h / vk_base.cpp: instance + validation, device and queue,
// swapchain, depth image, swapchain recreation (including a minimized
// window) and small helpers (buffers, images, shader modules, one-time
// submits). This is roughly what MTKView gives the Mac track.
// checker.png is decoded into RGBA8 pixels in CPU memory by
// bhs::LoadRGBA8 (image_io.h, stb_image) — the file-reading half of
// MTKTextureLoader.
//
// Your part:
//   - textured cube: vertex/index buffers (assets/cube.obj, common/obj_loader.h),
//        graphics pipeline with dynamic rendering (cube.vert / cube.frag
//        compiled by glslc into BHS_SHADER_DIR);
//   - texture upload: the decoded checker.png pixels -> staging buffer ->
//        sRGB VkImage, layout transitions, vkCmdCopyBufferToImage, image
//        view and sampler (see CreateTexture);
//   - depth test and back-face culling (the depth image is vk_.depthImage);
//   - time-based rotation and a Vulkan projection (Y down, z in [0, 1]),
//        see bhs::Perspective in common/math3d.h;
//   - uniform buffers triple-buffered (kMaxFramesInFlight = 3);
//   - synchronization: a fence per frame in flight + acquire/present
//        semaphores, no vkDeviceWaitIdle / vkQueueWaitIdle in the frame loop,
//        clean VK_LAYER_KHRONOS_validation output;
//   - window resize: call vk_.RecreateSwapchain() when needed.
//
// vk-bootstrap / VMA / volk are allowed: mention them in README. You may
// rewrite the skeleton completely.

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <string>

#include "../../common/math3d.h"
#include "../../common/obj_loader.h"
#include "image_io.h"
#include "vk_base.h"

#ifndef BHS_ASSET_DIR
#define BHS_ASSET_DIR "assets"
#endif
#ifndef BHS_SHADER_DIR
#define BHS_SHADER_DIR "shaders"
#endif

namespace {

constexpr uint32_t kWidth = 1280;
constexpr uint32_t kHeight = 720;
constexpr uint32_t kMaxFramesInFlight = 3;  // triple buffering

#ifdef NDEBUG
constexpr bool kEnableValidation = false;
#else
constexpr bool kEnableValidation = true;
#endif

// Per-frame-in-flight data. The "render finished" semaphore used by present
// belongs to the swapchain image, not to the frame slot: see
// vk_.renderFinished.
struct FrameData {
    VkCommandPool commandPool = VK_NULL_HANDLE;
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    VkFence inFlight = VK_NULL_HANDLE;               // TODO(sync)
    VkSemaphore imageAvailable = VK_NULL_HANDLE;     // TODO(sync)
    VkBuffer uniformBuffer = VK_NULL_HANDLE;         // TODO(triple buffering)
    VkDeviceMemory uniformMemory = VK_NULL_HANDLE;   // TODO(triple buffering)
    void* uniformMapped = nullptr;                   // TODO(triple buffering)
    VkDescriptorSet descriptorSet = VK_NULL_HANDLE;  // TODO(triple buffering)
};

class CubeApp {
public:
    void Run() {
        InitWindow();
        vk_.Init(window_, kEnableValidation);
        InitScene();
        MainLoop();
        Cleanup();
    }

private:
    GLFWwindow* window_ = nullptr;
    bool framebufferResized_ = false;

    bhs::VulkanBase vk_;
    FrameData frames_[kMaxFramesInFlight];
    uint32_t frameIndex_ = 0;

    void InitWindow() {
        if (!glfwInit()) throw std::runtime_error("glfwInit failed");
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window_ = glfwCreateWindow((int)kWidth, (int)kHeight, "BHS · Vulkan cube", nullptr, nullptr);
        if (!window_) throw std::runtime_error("glfwCreateWindow failed");
        glfwSetWindowUserPointer(window_, this);
        glfwSetFramebufferSizeCallback(window_, [](GLFWwindow* w, int, int) {
            static_cast<CubeApp*>(glfwGetWindowUserPointer(w))->framebufferResized_ = true;
        });
    }

    void InitScene() {
        // Already done: checker.png decoded to RGBA8, rows top to bottom.
        const bhs::ImageRGBA8 checker = bhs::LoadRGBA8(BHS_ASSET_DIR "/checker.png");
        CreateTexture(checker);  // TODO(texture): staging buffer -> sRGB image, layout transitions,
                                 //            copy, view, sampler (vk_.CreateBuffer, vk_.CreateImage,
                                 //            vk_.SubmitImmediate, vk_.CreateImageView)
        CreateGeometry();     // TODO(cube): bhs::LoadObj(BHS_ASSET_DIR "/cube.obj", ...)
        CreateDescriptors();  // TODO(triple buffering): one descriptor set per frame slot
        CreatePipeline();     // TODO(cube): vk_.LoadShaderModule(BHS_SHADER_DIR "/cube.vert.spv"),
                              //            VkPipelineRenderingCreateInfo with vk_.swapchainFormat
                              //            and vk_.depthFormat
        CreateFrames();       // TODO(triple buffering, sync): command pools/buffers, fences,
                              //            semaphores, uniform buffers
    }

    void CreateTexture(const bhs::ImageRGBA8& image) {
        (void)image;
        // TODO(texture):
        //   1. Staging buffer of image.pixels.size() bytes (TRANSFER_SRC, HOST_VISIBLE |
        //      HOST_COHERENT); copy image.pixels into it.
        //   2. vk_.CreateImage(image.width, image.height, VK_FORMAT_R8G8B8A8_SRGB,
        //      TRANSFER_DST | SAMPLED, ...); the image starts in layout UNDEFINED.
        //   3. In vk_.SubmitImmediate: barrier UNDEFINED -> TRANSFER_DST_OPTIMAL,
        //      vkCmdCopyBufferToImage, barrier TRANSFER_DST_OPTIMAL ->
        //      SHADER_READ_ONLY_OPTIMAL. Pick stage and access masks for each barrier.
        //   4. Destroy the staging buffer; create the image view and a VkSampler.
    }
    void CreateGeometry() { /* TODO */ }
    void CreateDescriptors() { /* TODO */ }
    void CreatePipeline() { /* TODO */ }
    void CreateFrames() { /* TODO */ }

    void DrawFrame(double timeSeconds) {
        FrameData& frame = frames_[frameIndex_];
        (void)frame;
        (void)timeSeconds;
        // TODO(sync): vkWaitForFences(frame.inFlight) — wait until the GPU releases THIS slot.
        // TODO(sync, resize): vkAcquireNextImageKHR(..., frame.imageAvailable, ...);
        //           on VK_ERROR_OUT_OF_DATE_KHR call vk_.RecreateSwapchain() and return.
        // TODO(projection, triple buffering): write MVP into frame.uniformMapped
        //           (only after the fence wait!). Aspect from vk_.swapchainExtent.
        // TODO(sync): vkResetFences only when a vkQueueSubmit is guaranteed to follow.
        // TODO(cube): record the command buffer: image barriers for the swapchain image
        //           and vk_.depthImage, vkCmdBeginRendering, viewport/scissor,
        //           pipeline, descriptor set, vertex/index buffers, vkCmdDrawIndexed,
        //           vkCmdEndRendering, barrier to PRESENT_SRC_KHR.
        // TODO(sync): vkQueueSubmit (wait frame.imageAvailable, signal
        //           vk_.renderFinished[imageIndex], fence frame.inFlight), vkQueuePresentKHR.
        // TODO(resize): on VK_ERROR_OUT_OF_DATE_KHR / VK_SUBOPTIMAL_KHR from present or
        //           framebufferResized_ — reset the flag and call vk_.RecreateSwapchain().
        frameIndex_ = (frameIndex_ + 1) % kMaxFramesInFlight;
    }

    void MainLoop() {
        while (!glfwWindowShouldClose(window_)) {
            glfwPollEvents();
            int w = 0, h = 0;
            glfwGetFramebufferSize(window_, &w, &h);
            if (w == 0 || h == 0) {  // minimized: nothing to draw
                glfwWaitEvents();
                continue;
            }
            DrawFrame(glfwGetTime());
        }
        // vkDeviceWaitIdle is fine here (shutdown), not inside the frame loop.
        VK_CHECK(vkDeviceWaitIdle(vk_.device));
    }

    void Cleanup() {
        // TODO: destroy the objects you created (pipeline, buffers, texture,
        //       descriptors, fences, semaphores, command pools) before
        //       vk_.Destroy(); validation must report nothing on exit.
        vk_.Destroy();
        glfwDestroyWindow(window_);
        glfwTerminate();
    }
};

}  // namespace

int main() {
    try {
        CubeApp app;
        app.Run();
    } catch (const std::exception& e) {
        std::fprintf(stderr, "Fatal: %s\n", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

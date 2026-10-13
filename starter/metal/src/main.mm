// main.mm — starter skeleton for the "Mac" track (Metal + MetalKit/AppKit).
//
// Already done: window, MTKView, MTLDevice, MTLCommandQueue, a draw loop that
// clears the screen, and runtime compilation of shaders.metal (works with the
// Command Line Tools only, no full Xcode needed).
//
// Your part:
//   - textured cube: vertex/index buffers from assets/cube.obj
//        (loader: common/obj_loader.h), render pipeline, checker.png + sampler;
//   - depth buffer, depth test, back-face culling;
//   - time-based rotation and a Metal projection (NDC z in [0, 1]),
//        see bhs::Perspective in common/math3d.h;
//   - uniform data triple-buffered (kMaxFramesInFlight copies);
//   - CPU<->GPU synchronization with dispatch_semaphore;
//   - window resize (aspect ratio and size-dependent resources).
//
// You may rewrite the skeleton completely (metal-cpp, your own AppKit code,
// GLFW + CAMetalLayer) — describe the decision in README.

#import <AppKit/AppKit.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <QuartzCore/QuartzCore.h>
#include <simd/simd.h>

#include "../../common/math3d.h"
#include "../../common/obj_loader.h"

#ifndef BHS_ASSET_DIR
#define BHS_ASSET_DIR "assets"
#endif
#ifndef BHS_SHADER_PATH
#define BHS_SHADER_PATH "shaders.metal"
#endif

// Maximum number of frames in flight. The task asks for 3 (triple buffering).
[[maybe_unused]] static const NSUInteger kMaxFramesInFlight = 3;

// ---------------------------------------------------------------------------
// Renderer
// ---------------------------------------------------------------------------
@interface Renderer : NSObject <MTKViewDelegate>
- (nullable instancetype)initWithView:(nonnull MTKView*)view;
@end

@implementation Renderer {
    id<MTLDevice> _device;
    id<MTLCommandQueue> _queue;
    id<MTLLibrary> _library;

    // TODO(cube): id<MTLRenderPipelineState> _pipeline;
    // TODO(cube): id<MTLBuffer> _vertexBuffer, _indexBuffer; NSUInteger _indexCount;
    // TODO(cube): id<MTLTexture> _texture; id<MTLSamplerState> _sampler;
    // TODO(depth): id<MTLDepthStencilState> _depthState;
    // TODO(triple buffering): id<MTLBuffer> _uniformBuffers[kMaxFramesInFlight]; NSUInteger _frameIndex;
    // TODO(sync): dispatch_semaphore_t _inFlightSemaphore;

    CFTimeInterval _startTime;
}

- (nullable instancetype)initWithView:(nonnull MTKView*)view {
    if (!(self = [super init])) return nil;
    _device = view.device;
    _queue = [_device newCommandQueue];
    _startTime = CACurrentMediaTime();

    // TODO(depth): pick a depth format (e.g. MTLPixelFormatDepth32Float) and set
    //           view.depthStencilPixelFormat / view.clearDepth. MTKView then
    //           creates and resizes the depth texture for you.
    view.colorPixelFormat = MTLPixelFormatBGRA8Unorm_sRGB;
    view.clearColor = MTLClearColorMake(0.10, 0.12, 0.16, 1.0);

    // Runtime MSL compilation. With full Xcode you could build a .metallib
    // with `xcrun metal` instead and load it via newLibraryWithURL:.
    NSError* error = nil;
    NSString* path = @BHS_SHADER_PATH;
    NSString* source = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:&error];
    if (!source) {
        NSLog(@"Cannot read shader source %@: %@", path, error);
        return nil;
    }
    _library = [_device newLibraryWithSource:source options:[MTLCompileOptions new] error:&error];
    if (!_library) {
        NSLog(@"Shader compilation failed: %@", error);
        return nil;
    }

    // TODO(cube): load the cube:
    //   bhs::ObjMesh mesh; bhs::LoadObj(std::string(BHS_ASSET_DIR) + "/cube.obj", mesh);
    //   then create MTLBuffers. Does the ObjVertex layout match your
    //   MTLVertexDescriptor / the vertex struct in MSL?

    // TODO(cube): load assets/checker.png (MTKTextureLoader is fine) and create an
    //           MTLSamplerState. Mind sRGB and the direction of the V axis.

    // TODO(cube): MTLRenderPipelineDescriptor with functions from _library, an
    //           MTLVertexDescriptor and attachment formats matching the view.

    // TODO(triple buffering, sync): allocate kMaxFramesInFlight uniform buffers and the semaphore
    //               dispatch_semaphore_create(kMaxFramesInFlight).

    return self;
}

- (void)mtkView:(nonnull MTKView*)view drawableSizeWillChange:(CGSize)size {
    // TODO(resize): the window was resized. `size` is in pixels (not points):
    //           update the aspect ratio used by the projection.
    (void)view; (void)size;
}

- (void)drawInMTKView:(nonnull MTKView*)view {
    // TODO(sync): wait for a free slot: dispatch_semaphore_wait(...).
    //           Where exactly to wait and where to signal is the key question.

    // TODO(projection, triple buffering): write MVP into the uniform buffer of the CURRENT frame
    //               (rotation angle from time, not from a frame counter).
    float t = (float)(CACurrentMediaTime() - _startTime);
    (void)t;

    id<MTLCommandBuffer> cmd = [_queue commandBuffer];
    cmd.label = @"Frame";

    // TODO(sync): [cmd addCompletedHandler:...] — signal the semaphore when the GPU
    //           has finished this frame.

    MTLRenderPassDescriptor* pass = view.currentRenderPassDescriptor;
    if (pass) {
        id<MTLRenderCommandEncoder> enc = [cmd renderCommandEncoderWithDescriptor:pass];
        enc.label = @"Cube pass";
        // TODO(draw): setRenderPipelineState, setDepthStencilState, setCullMode +
        //              setFrontFacingWinding (Metal's default front face is clockwise,
        //              the cube is counter-clockwise),
        //              setVertexBuffer (vertices + uniforms of the current frame),
        //              setFragmentTexture / setFragmentSamplerState,
        //              drawIndexedPrimitives.
        [enc endEncoding];
        [cmd presentDrawable:view.currentDrawable];
    }
    [cmd commit];
}

@end

// ---------------------------------------------------------------------------
// Application and window
// ---------------------------------------------------------------------------
@interface AppDelegate : NSObject <NSApplicationDelegate>
@end

@implementation AppDelegate {
    NSWindow* _window;
    MTKView* _view;
    Renderer* _renderer;
}

- (void)applicationDidFinishLaunching:(NSNotification*)note {
    (void)note;
    NSRect frame = NSMakeRect(0, 0, 1280, 720);
    _window = [[NSWindow alloc] initWithContentRect:frame
                                          styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                                                    NSWindowStyleMaskResizable | NSWindowStyleMaskMiniaturizable
                                            backing:NSBackingStoreBuffered
                                              defer:NO];
    _window.title = @"BHS · Metal cube";
    [_window center];

    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    if (!device) {
        NSLog(@"Metal is not supported on this machine");
        [NSApp terminate:nil];
        return;
    }
    _view = [[MTKView alloc] initWithFrame:frame device:device];
    _view.preferredFramesPerSecond = 60;
    _renderer = [[Renderer alloc] initWithView:_view];
    if (!_renderer) {
        [NSApp terminate:nil];
        return;
    }
    _view.delegate = _renderer;
    _window.contentView = _view;
    [_window makeKeyAndOrderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)app {
    (void)app;
    return YES;
}
@end

int main(int argc, const char* argv[]) {
    (void)argc; (void)argv;
    @autoreleasepool {
        NSApplication* app = [NSApplication sharedApplication];
        [app setActivationPolicy:NSApplicationActivationPolicyRegular];
        AppDelegate* delegate = [AppDelegate new];
        app.delegate = delegate;
        [app run];
    }
    return 0;
}

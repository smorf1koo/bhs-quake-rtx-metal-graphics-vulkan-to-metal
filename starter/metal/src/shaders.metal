// shaders.metal — shader stubs for the "Mac" track.
// Compiled at runtime (newLibraryWithSource:), so compilation errors are
// printed to the console when the app starts.
//
// TODO(cube): vertex input struct with [[attribute(n)]] matching the
//           MTLVertexDescriptor on the CPU side; sample the texture in the
//           fragment shader.
// TODO(projection, triple buffering): uniforms struct (MVP etc.). Mind the alignment: float3 in MSL
//           takes 16 bytes, same as simd_float3 on the CPU.

#include <metal_stdlib>
using namespace metal;

struct VertexOut {
    float4 position [[position]];
    float2 uv;
};

vertex VertexOut cube_vertex(uint vid [[vertex_id]])
{
    // TODO(cube, projection): replace with the real vertex transform.
    VertexOut out;
    out.position = float4(0.0, 0.0, 0.0, 1.0);
    out.uv = float2(0.0);
    (void)vid;
    return out;
}

fragment float4 cube_fragment(VertexOut in [[stage_in]])
{
    // TODO(cube): return the texture color.
    return float4(in.uv, 1.0, 1.0);
}

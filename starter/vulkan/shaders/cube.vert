#version 450
// cube.vert — vertex shader stub ("no Mac" track).
// TODO(cube): input attributes (location = 0, 1, 2) matching
//           VkVertexInputAttributeDescription on the CPU side.
// TODO(triple buffering): uniform block with MVP (set/binding per your descriptor layout).
// TODO(projection): in Vulkan clip-space Y points down and NDC z is in [0, 1].

layout(location = 0) out vec2 vUV;

void main()
{
    vUV = vec2(0.0);
    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
}

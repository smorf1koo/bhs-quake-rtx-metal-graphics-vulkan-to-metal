#version 450
// cube.frag — fragment shader stub ("no Mac" track).
// TODO(cube): combined image sampler (or separate texture + sampler) with checker.png.

layout(location = 0) in vec2 vUV;
layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(vUV, 1.0, 1.0);
}

#version 440
layout(location = 0) in vec3 color;

// framebuffer ka index de kar color output karo!
layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(color, 1.f);
}

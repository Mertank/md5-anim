#version 330

// input from rasterizer
smooth in vec3 interpColor;
smooth in vec2 outUV;

uniform sampler2D tex;

// output to framebuffer
out vec4 fragColor;

void main()
{
    fragColor = texture(tex, outUV) * vec4(interpColor, 1.0);  // passthrough    
}

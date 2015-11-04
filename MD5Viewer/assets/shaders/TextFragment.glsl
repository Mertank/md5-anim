#version 330

// input from rasterizer
in vec2 var_TexCoord;

// input from application
uniform sampler2D u_TexSampler;
uniform vec4 u_Tint;

// output to framebuffer
out vec4 out_Color;

void main()
{
	// multiply texel color by tint
    out_Color = u_Tint * texture2D(u_TexSampler, var_TexCoord);
}

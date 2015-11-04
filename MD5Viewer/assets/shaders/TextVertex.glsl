#version 330

// vertex attributes
layout(location = 0) in vec4 in_Vertex;
layout(location = 3) in vec2 in_TexCoord;

// transformations
uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ModelMatrix;

// outputs to rasterizer
out vec2 var_TexCoord;

void main()
{
    gl_Position = u_ProjectionMatrix * u_ModelMatrix * in_Vertex;
	var_TexCoord = in_TexCoord;
}

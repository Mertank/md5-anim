#version 330

// vertex attributes
layout(location = 0) in vec4 inVertex;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

// transformations
uniform mat4 uProjectionMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat3 uNormalMatrix;

// light info
uniform vec3 uLightColor;
uniform vec3 uLightDir;

// material color
uniform vec3 uMatColor;

// outputs to rasterizer 
smooth out vec3 interpColor;
smooth out vec2 outUV;

void main()
{
    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * inVertex;

	// can remove these normalizations if we're absolutely sure that normals and light directions are unit vectors
	vec3 N = normalize(uNormalMatrix * inNormal);	// transform surface normal
	vec3 L = normalize(-uLightDir);					// compute direction to light

	// compute diffuse lighting intensity
	float NdotL = max(dot(N, L), 0.5);	// assumes N and L are unit vectors; clamps negative values to 0

	interpColor = NdotL * uLightColor * uMatColor;
	outUV = inUV;
}

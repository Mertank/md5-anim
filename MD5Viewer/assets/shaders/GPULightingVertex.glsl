#version 330

// vertex attributes
layout(location = 0) in vec4 inVertex;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec4 inWeight;
layout(location = 4) in vec4 inMatrixIndex;

uniform samplerBuffer uMatriciesBuffer; 

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
    mat4 boneMatricies[4];
    boneMatricies[0] = mat4( texelFetch( uMatriciesBuffer, int(inMatrixIndex.x) * 4 + 0 ),
                             texelFetch( uMatriciesBuffer, int(inMatrixIndex.x) * 4 + 1 ),
                             texelFetch( uMatriciesBuffer, int(inMatrixIndex.x) * 4 + 2 ),
                             texelFetch( uMatriciesBuffer, int(inMatrixIndex.x) * 4 + 3 ) );
    boneMatricies[1] = mat4( texelFetch( uMatriciesBuffer, int(inMatrixIndex.y) * 4 + 0 ),
                             texelFetch( uMatriciesBuffer, int(inMatrixIndex.y) * 4 + 1 ),
                             texelFetch( uMatriciesBuffer, int(inMatrixIndex.y) * 4 + 2 ),
                             texelFetch( uMatriciesBuffer, int(inMatrixIndex.y) * 4 + 3 ) );
    boneMatricies[2] = mat4( texelFetch( uMatriciesBuffer, int(inMatrixIndex.z) * 4 + 0 ),
                             texelFetch( uMatriciesBuffer, int(inMatrixIndex.z) * 4 + 1 ),
                             texelFetch( uMatriciesBuffer, int(inMatrixIndex.z) * 4 + 2 ),
                             texelFetch( uMatriciesBuffer, int(inMatrixIndex.z) * 4 + 3 ) );
    boneMatricies[3] = mat4( texelFetch( uMatriciesBuffer, int(inMatrixIndex.w) * 4 + 0 ),
                             texelFetch( uMatriciesBuffer, int(inMatrixIndex.w) * 4 + 1 ),
                             texelFetch( uMatriciesBuffer, int(inMatrixIndex.w) * 4 + 2 ),
                             texelFetch( uMatriciesBuffer, int(inMatrixIndex.w) * 4 + 3 ) );


    mat4 transformMat = boneMatricies[0] * inWeight.x;
    transformMat     += boneMatricies[1] * inWeight.y;
    transformMat     += boneMatricies[2] * inWeight.z;
    transformMat     += boneMatricies[3] * inWeight.w;
    
    vec4 vertexNormal = transformMat * vec4( inNormal, 0.0 );
   
    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * transformMat * inVertex;

	// can remove these normalizations if we're absolutely sure that normals and light directions are unit vectors
	vec3 N = normalize(uNormalMatrix * vec3(vertexNormal));	// transform surface normal
	vec3 L = normalize(-uLightDir);					// compute direction to light

	// compute diffuse lighting intensity
	float NdotL = max(dot(N, L), 0.5);	// assumes N and L are unit vectors; clamps negative values to 0

	interpColor = NdotL * uLightColor * uMatColor; 
	outUV = inUV;
}


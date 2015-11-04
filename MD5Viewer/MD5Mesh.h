#ifndef __MD5MESH_H__
#define __MD5MESH_H__

#define STRINGS_ARE_EQUAL( x, y ) ( strcmp( x, y ) == 0 )

#include <string>
#include <GL\glew.h>
#include "MD5ModelStructs.h"
#include "MD5AnimationStructs.h"

/*
========================

	MD5Mesh

		An MD5Mesh.

========================
*/
class MD5Mesh {
public:
					~MD5Mesh( void );

	static MD5Mesh*	CreateMeshFromData( char* data, const Joints& jointData, char** endPosition );
	bool			InitWithData( char* data, const Joints& jointData, char** endPosition );
	
	void			ApplySkeleton( const Skeleton& skeleton );
	void			SetVertexBufferToBindPose( void );

	void			Render( ModelSkinningType skinningType );

private:
public:
	std::string     shaderName;
    
	GLuint          indexCount;
	GLuint          triangleCount;
	GLuint          vboName;
	GLuint          iboName;
	GLuint          cpuVaoName;
    GLuint          gpuVaoName;
	
	float*          vertexData;
	GLuint*         indexBuffer;

	Verticies       verticies;
	Triangles       triangles;
	Weights         weights;

	Texture*        diffuseTexture;

					MD5Mesh( void );

	bool			Upload( void );
    bool            SetupOpenGLBuffers( void );

	void			BuildBindPose( const Joints& joints );

	void			ComputeVerticies( const Joints& joints );
	void			ComputeNormals( const Joints& joints );
	void			ComputeIndicies( void );

	void			ReadVertex( char* startingPosition );
	void			ReadWeight( char* startingPostion );
	void			ReadTriangle( char* startingPosition );

    void            RenderCPUSkinning( void );
    void            RenderGPUSkinning( void );

	static bool		ValidMD5MeshExtension( const char* path );
};
typedef std::vector<MD5Mesh*> MD5Meshes;

#endif //__MD5MESH_H__
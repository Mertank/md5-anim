#ifndef __MD5MODEL_H__
#define __MD5MODEL_H__

#include "MD5ModelStructs.h"
#include "Program.h"
#include "MD5Mesh.h"
#include "MD5Animation.h"

/*
========================

	MD5Model

		An MD5Model.
		Contains multiple meshes.
		Animations can be added and played.

========================
*/
class MD5Model {
public:
    							~MD5Model( void );
	
	static MD5Model*			CreateMD5ModelWithMesh( const char* path );
	static MD5Model*			CreateMD5Model( void );

	bool						InitMD5ModelWithMesh( const char* path );
	
	bool						AddAnimation( const char* path );

	void						Update( float dt );
	void						Render( Program* program );

	void						SetRotation( float angle, glm::vec3 axis );
	void						RotateAround( float angle, glm::vec3 axis );

	inline const glm::mat4&		GetModelMatrix( void ) const { return modelMatrix; }    
	
    inline void					SetMaterial( glm::vec3 newMaterial ) { materialColor = newMaterial; }
	inline const glm::vec3&		GetMaterial( void ) const { return materialColor; }
	
    inline const std::string&   GetModelName( void ) const { return modelName; }
	
    inline float				GetBlendFactor( void ) const { return blendAmount; }
	void						SetBlendFactor( float newFactor );
    
	void						SetSkinningType( ModelSkinningType skType );
    inline ModelSkinningType    GetSkinningType( void ) const { return skinningType; }

    inline unsigned				GetAnimationCount( void ) const { return animations.size(); }	
    std::vector<std::string>	GetPlayingAnimationNames( void ) const;
	std::vector<int>			GetPlayingAnimationIndicies( void ) const;

	void						PlaySingleAnimation( int animation1Index );
	void						PlayBlendedAnimation( int animation1Index, int animation2Index, float blendAmount );

private:
								MD5Model( void );	
	
	static bool					ValidMD5MeshExtension( const char* path );

    bool                        SetupMatrixTextureBuffer( void );
    void                        UpdateMatrixTextureBuffer( void );

	char*						ReadJoints( char* startingPosition );
	char*						ReadMesh( char* startingPosition );
	void						ReadJoint( char* startingPosition, Joint& dest );    
    void                        GenerateBindPoseMatricies( void );

    std::string                 modelName;

	Joints						joints;
    MD5Meshes					meshes;
    std::vector<glm::mat4>      inverseBoneMatricies;

	bool						animate;
	int							animation1Index;
	int							animation2Index;
	float						blendAmount;
    GLuint                      matrixBufferName;
    GLuint                      matrixTextureName;

	glm::mat4					modelMatrix;
	glm::vec3					materialColor;

	MD5Animations				animations;
	Skeleton					blendSkeleton;
	
	MD5Animation*				animation1;
	MD5Animation*				animation2;

    ModelSkinningType           skinningType;
};

#endif //__MD5MODEL_H__
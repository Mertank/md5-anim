#include "MD5Model.h"
#include "MD5FileOperations.h"
#include <fstream>
/*
=============
MD5Model::MD5Model

	MD5Model Constructor.
=============
*/
MD5Model::MD5Model( void ) :
	animate( false ),
	animation1Index( 0 ),
	animation2Index( -1 ),
	blendAmount( 0.0f ),
    matrixBufferName( 0 ),
    matrixTextureName( 0 ),
	animation1( NULL ),
	animation2( NULL ),
    modelName( "NULL" ),
    skinningType( CPU_SKINNING )
{}
/*
=============
MD5Model::~MD5Model

	MD5Model Destructor.
=============
*/
MD5Model::~MD5Model( void ) {
	for ( MD5Animations::iterator currentAnim = animations.begin();
		  currentAnim != animations.end(); ++currentAnim ) {
		delete *currentAnim;
	}
	animations.clear();

	for ( MD5Meshes::iterator currentMesh = meshes.begin();
		  currentMesh != meshes.end(); ++currentMesh ) {
		delete *currentMesh;
	}
	meshes.clear();
}
/*
=============
MD5Model::CreateMD5Model

	Creates an empty MD5Model
=============
*/
MD5Model* MD5Model::CreateMD5Model( void ) {
	return new MD5Model();
}
/*
=============
MD5Model::CreateMD5ModelWithMesh

	Loads an MD5 Model with a mesh at a path.
=============
*/
MD5Model* MD5Model::CreateMD5ModelWithMesh( const char* path ) {
	MD5Model* model = new MD5Model();
	if ( model == NULL || !model->InitMD5ModelWithMesh( path ) ) {
		//Something went wrong
		delete model;
		model = NULL;
	}
	return model;
}
/*
=============
MD5Model::InitMD5ModelWithMesh

	Initializes MD5Model with the mesh at the path.
=============
*/
bool MD5Model::InitMD5ModelWithMesh( const char* path ) {

	if ( !ValidMD5MeshExtension( path ) ) {
		printf( "Not a valid MD5Mesh extension\n" );
		return false;
	}

	char* fileData = FileOperations::ReadFileToCharBuffer( path );

	if ( fileData == NULL ) { //File wasn't opened
		printf( "Mesh at path '%s' could not be opened\n", path );
		return false;
	} else {
		printf( "Beginning load of: %s\n", path );

        modelName       = path;
        unsigned slash  = modelName.find_last_of( "/" ) + 1;
        unsigned dot    = modelName.find_last_of( "." );
        modelName		= modelName.substr( slash, dot - slash );

		char* nextLineToken	= NULL;
		char* currentLine   = strtok_s( fileData, "\n", &nextLineToken );

		while ( currentLine != NULL ) {
			char* nextParam     = NULL;
			char* currentParam  = strtok_s( currentLine, " ", &nextParam );

			if ( STRINGS_ARE_EQUAL( currentParam, "MD5Version" ) ) { //Read the version
				nextParam[2] = '\0'; //Truncate to 2 chars
				 if ( !STRINGS_ARE_EQUAL( nextParam, "10" ) ) {
					printf( "Only MD5Version 10 is supported\n" );
					delete[] fileData;
					return false;                
				 }
			} else if ( STRINGS_ARE_EQUAL( currentParam, "numJoints" ) ) { //Read numjoints
				int numJoints = std::atoi( nextParam );
				joints.resize( numJoints );
				blendSkeleton.joints.resize( numJoints );
                blendSkeleton.jointMatricies.resize( numJoints );                
			} else if ( STRINGS_ARE_EQUAL( currentParam, "numMeshes" ) ) { //Read nummeshes
				int numMeshes = std::atoi( nextParam );
				meshes.reserve( numMeshes );
			} else if ( STRINGS_ARE_EQUAL( currentParam, "joints" ) ) { //Read joints
				if ( joints.size() == 0 ) {
					printf( "numJoints was not specified\n" );
					delete[] fileData;
					return false;    
				}
				nextLineToken = ReadJoints( nextLineToken );
			} else if ( STRINGS_ARE_EQUAL( currentParam, "mesh" ) ) { //Read a mesh
				nextLineToken = ReadMesh( nextLineToken );
			}

			currentLine = strtok_s( NULL, "\n", &nextLineToken );
		}

		GenerateBindPoseMatricies();

		printf( "   MD5Mesh file parsed\n" );
		printf( "      Joint count:\t%i\n", joints.size() );
		printf( "      Mesh count:\t%i\n", meshes.size() );        
		printf( "Successfully Loaded MD5Mesh: %s\n", path );        

	}
    
    delete[] fileData;

	return SetupMatrixTextureBuffer();
}
/*
=============
MD5Model::SetupMatrixTextureBuffer

	Sets up the texture buffer for GPU skinning
=============
*/
bool MD5Model::SetupMatrixTextureBuffer( void ) {
    glGenBuffers( 1, &matrixBufferName );
    if ( matrixBufferName == 0 ) {
        printf( "Could not create matrix buffer" );
        return false;
    }

    glGenTextures( 1, &matrixTextureName );
    if ( matrixTextureName == 0 ) {
        glDeleteBuffers( 1, &matrixBufferName );
        printf( "Could not create matrix texture" );
        return false;
    }

    glm::mat4 matrix(1.0); //Init to bind pose
    float* matrixBuffer = new float[joints.size() * 4 * 4];
    for ( unsigned i = 0; i < joints.size() * 4 * 4; i += 16 ) {
        memcpy( &matrixBuffer[i], &matrix[0], sizeof( float ) * 16 );
    }

    glBindBuffer( GL_TEXTURE_BUFFER, matrixBufferName );
    glBufferData( GL_TEXTURE_BUFFER, sizeof( float ) * joints.size() * 4 * 4, matrixBuffer, GL_DYNAMIC_DRAW );
    glBindBuffer( GL_TEXTURE_BUFFER, 0 );

    delete[] matrixBuffer;

    return true;
}
/*
=============
MD5Model::ValidMD5MeshExtension

	Checks if the path ends with md5mesh
=============
*/
bool MD5Model::ValidMD5MeshExtension( const char* path ) {
	std::string pathStr( path );
	return ( pathStr.length() > 7 &&
			 pathStr.substr( pathStr.length() - 7, 7 ).compare( "md5mesh" ) == 0 );
}
/*
=============
MD5Model::AddAnimation

	Add an .md5anim to this model.
=============
*/
bool MD5Model::AddAnimation( const char* path ) {
	MD5Animation* newAnimation = MD5Animation::CreateAnimationFromFile( path );
	
	if ( newAnimation != NULL ) {
		animations.push_back( newAnimation );  
		if ( animations.size() == 1 ) { //First anim added
			animate = true;
			animation1Index = 0;
			animation1 = animations[0];
			animation2Index = -1;
		}
		return true;
	}

	return false;
}
/*
=============
MD5Model::ReadJoints

	Reads all joints from a char buffer.
	Stores them in joints member variable.
	Returns a pointer to where the buffer should continue reading.
=============
*/
char* MD5Model::ReadJoints( char* startingPosition ) {
	char* nextLine      = NULL;
	char* currentLine   = strtok_s( startingPosition, "\n", &nextLine );
	char* nextToken     = NULL;
	int   jointIndex    = 0;    

	while ( currentLine[0] != '}' ) {     
		ReadJoint( currentLine, joints[jointIndex] );

		++jointIndex;
		currentLine = strtok_s( NULL, "\n", &nextLine );
	}

	return nextLine;
}
/*
=============
MD5Model::ReadJoint

	Reads a joints from a char buffer.
	Stores it in destination joint
=============
*/
void MD5Model::ReadJoint( char* startPosition, Joint& dest ) {
	char* nextToken = NULL;

	dest.name = strtok_s( startPosition, "\t\"", &nextToken );							//Read the joint name	
	dest.parentID = std::atoi( strtok_s( NULL, "\t ", &nextToken ) );					//Joint's parent id
	FileOperations::ReadVec3( strtok_s( NULL, "()", &nextToken ), dest.position );	    //The joint's position	
	++nextToken;																		//Skip the space between the position and orientation		
	FileOperations::ReadQuat( strtok_s( NULL, "()", &nextToken ), dest.orientation );   //The joint's orientation
}
/*
=============
MD5Model::ReadMesh

	Reads all meshes from a char buffer.
	Stores them in meshes.
	Returns a pointer to where the buffer should continue reading.
=============
*/
char* MD5Model::ReadMesh( char* startingPosition ) {	
	char* endPosition;
	meshes.push_back( MD5Mesh::CreateMeshFromData( startingPosition, joints, &endPosition ) );	
	return endPosition;
}
/*
=============
MD5Model::GenerateBindPoseMatricies

	Creates the inverse matrix for all joints.
=============
*/
void MD5Model::GenerateBindPoseMatricies( void ) {
    unsigned matrixIndex = 0;
	inverseBoneMatricies.resize( joints.size() );
    for ( Joints::iterator joint = joints.begin();
          joint != joints.end(); ++joint, ++matrixIndex ) {
        glm::mat4 translation   = glm::translate( glm::mat4( 1.0 ), joint->position );
        glm::mat4 rotation      = glm::toMat4( joint->orientation );

        glm::mat4 finalMatrix   = translation * rotation;

        inverseBoneMatricies[matrixIndex] = glm::inverse( finalMatrix );    
    }
}
/*
=============
MD5Model::PlayAnimation

	Plays the animation at the index.
	Resets the blend animation.
	Does bound checking.
=============
*/
void MD5Model::PlaySingleAnimation( int index ) {
	if ( index < ( int )animations.size() ) {
		animation1Index = index;		
		if ( index < 0 ) {
			animation1 = NULL;			
		} else {
			animation1 = animations[index];
			animation1->SetCurrentFrame( 0 );
		}
	}

	animation2		= NULL;
	animation2Index = -1;
	blendAmount		= 0.0f;
}
/*
=============
MD5Model::PlayBlendedAnimation

	Plays a blended animation.	
	Does bound checking.
=============
*/
void MD5Model::PlayBlendedAnimation( int index1, int index2, float blend ) {
	if ( index1 < ( int )animations.size() && index2 < ( int )animations.size() &&
		 index1 > -1 && index2 > -1 ) {
		animation1Index = index1;		
		animation2Index = index2;
		animation1		= animations[animation1Index];
		animation2		= animations[animation2Index];
		blendAmount		= blend;

		animation1->SetCurrentFrame( 0 );
		animation2->SetCurrentFrame( 0 );
	}
}
/*
=============
MD5Model::Update

	Update the animation
=============
*/
void MD5Model::Update( float dt ) {
	if ( animate && animation1 ) {

        animation1->Update( dt );
		
        if ( animation2 && animation1 != animation2 ) { //If animation1 & animation2 are the same don't update twice.
			animation2->Update( dt );
			MD5Animation::InterpolateSkeletonFrames( animation1->GetCurrentSkeleton(), animation2->GetCurrentSkeleton(), blendSkeleton, blendAmount );
		}

		if ( skinningType == CPU_SKINNING ) { 
			const Skeleton* currentSkeleton = &animation1->GetCurrentSkeleton();
			if ( animation2 ) {
				if ( animation1 != animation2 && blendAmount != 0.0f ) {
					if ( blendAmount > 0.99f ) {
						currentSkeleton = &animation2->GetCurrentSkeleton();
					} else {
						currentSkeleton = &blendSkeleton;
					}
				}
			}
            for ( MD5Meshes::iterator currentMesh = meshes.begin();
				    currentMesh != meshes.end(); ++currentMesh ) {
			    ( *currentMesh )->ApplySkeleton( *currentSkeleton );
		    }
        } 
	}
}
/*
=============
MD5Model::Render

	Render all the model's meshes
=============
*/
void MD5Model::Render( Program* program ) {

    if ( skinningType == GPU_SKINNING ) {
        UpdateMatrixTextureBuffer();
    }

	program->SetUniform( "uModelMatrix", &modelMatrix[0][0], 16 );
	program->SetUniform( "uMatColor", &materialColor[0], 3 );

    for ( MD5Meshes::iterator currentMesh = meshes.begin();
		  currentMesh != meshes.end(); ++currentMesh++ ) {
		( *currentMesh )->Render( skinningType );
	}    
}
/*
=============
MD5Model::UpdateMatrixTextureBuffer

	Updates the matrix texture buffer on the GPU.
=============
*/
void MD5Model::UpdateMatrixTextureBuffer( void ) {

    const Skeleton* currentSkeleton = &animation1->GetCurrentSkeleton();
	if ( animation2 ) {
		if ( animation1 != animation2 && blendAmount != 0.0f ) {
			if ( blendAmount > 0.99f ) {
				currentSkeleton = &animation2->GetCurrentSkeleton();
			} else {
				currentSkeleton = &blendSkeleton;
			}
		}
	}

    glBindBuffer( GL_TEXTURE_BUFFER, matrixBufferName );
    unsigned index = 0;
    for ( SkeletonMatricies::const_iterator currentMatrix = currentSkeleton->jointMatricies.begin();
          currentMatrix != currentSkeleton->jointMatricies.end(); ++currentMatrix, ++index ) {
        glBufferSubData( GL_TEXTURE_BUFFER, index * 16 * sizeof( float ), sizeof( float ) * 16, &( *currentMatrix * inverseBoneMatricies[index] )[0] );        
    }

    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, matrixTextureName );
    glTexBuffer( GL_TEXTURE_BUFFER, GL_RGBA32F, matrixBufferName );
}
/*
=============
MD5Model::SetSkinningType

	Sets the skinning type.
=============
*/
void MD5Model::SetSkinningType( ModelSkinningType type ) {
	skinningType = type;
	if ( skinningType == GPU_SKINNING ) {
		for ( MD5Meshes::iterator currentMesh = meshes.begin();
			  currentMesh != meshes.end(); ++currentMesh++ ) {
			( *currentMesh )->SetVertexBufferToBindPose(); //Do this or it looks crazy.
		}    
	}
}
/*
=============
MD5Model::SetRotation

	Sets the model's rotation.
=============
*/
void MD5Model::SetRotation( float angle, glm::vec3 axis ) {
	modelMatrix = glm::rotate( glm::mat4( 1.0 ), angle, axis );
}
/*
=============
MD5Model::Rotate

	Rotates the model around an axis
=============
*/
void MD5Model::RotateAround( float angle, glm::vec3 axis ) {
	modelMatrix = glm::rotate( modelMatrix, angle, axis );
}
/*
=============
MD5Model::GetPlayingAnimationNames

	Returns a vector of the currently playing animations.
=============
*/
std::vector<std::string> MD5Model::GetPlayingAnimationNames( void ) const {
	std::vector<std::string> animNames;
	if ( animation1Index > -1 && animation1Index < ( int )animations.size() ) {
		animNames.push_back( animations[animation1Index]->GetAnimationName() );
	} else {
		animNames.push_back( MD5Animation::DefaultAnimationName );
	}

	if ( animation2Index > -1 && animation2Index < ( int )animations.size() ) {
		animNames.push_back( animations[animation2Index]->GetAnimationName() );
	} else {
		animNames.push_back( MD5Animation::DefaultAnimationName );
	}

	return animNames;
}
/*
=============
MD5Model::GetPlayingAnimationIndicies

	Returns a vector of the currently playing animations indicies.
=============
*/
std::vector<int> MD5Model::GetPlayingAnimationIndicies( void ) const {
	std::vector<int> animIndicies;
	animIndicies.push_back( animation1Index );
	animIndicies.push_back( animation2Index );
	return animIndicies;
}
/*
=============
MD5Model::SetBlendFactor

	Sets the amount to blend the two playing animations
=============
*/
void MD5Model::SetBlendFactor( float newFactor ) {
	if ( animation1 && animation2 ) {
		blendAmount = std::min( std::max( newFactor, 0.0f ), 1.0f );
	}
}
#include "MD5Mesh.h"
#include "MD5FileOperations.h"
#include <algorithm>
/*
=============
MD5Mesh::MD5Mesh

	MD5Mesh Default Constructor.
=============
*/
MD5Mesh::MD5Mesh( void ) :
	shaderName( "NULL" ),
	vertexData( NULL ),
	indexBuffer( NULL ),
	diffuseTexture( NULL ),
    vboName( 0 ),
    iboName( 0 ),
    cpuVaoName( 0 ),
    gpuVaoName( 0 ),
	indexCount( 0 ),
    triangleCount( 0 )
{}
/*
=============
MD5Mesh::~MD5Mesh

	MD5Mesh Destructor.
=============
*/
MD5Mesh::~MD5Mesh( void ) {    
    glDeleteBuffers( 1, &vboName );
    glDeleteBuffers( 1, &iboName );
    glDeleteVertexArrays( 1, &cpuVaoName );
    glDeleteVertexArrays( 1, &gpuVaoName );

	delete diffuseTexture;
	delete vertexData;
	delete indexBuffer;
}
/*
=============
MD5Mesh::CreateMeshFromData

	Return an MD5Mesh from a byte stream if successful.
	NULL if not successful.	
	Fills end position with the address of where the buffer should continue reading.
=============
*/
MD5Mesh* MD5Mesh::CreateMeshFromData( char* data, const Joints& jointData, char** endPosition ) {
	MD5Mesh* mesh = new MD5Mesh();
	
	if ( mesh == NULL ||
		 !mesh->InitWithData( data, jointData, endPosition ) ) {
		delete mesh;
		return NULL;
	}

	return mesh;
}
/*
=============
MD5Mesh::LoadFromData

	Render the mesh with the appropriate diffuse map.
=============
*/
bool MD5Mesh::InitWithData( char* startingPosition, const Joints& jointInfo, char** endPosition ) {
	char* nextLine      = NULL;
	char* currentLine   = strtok_s( startingPosition, "\n", &nextLine );
	char* junk          = NULL;
	char* currentToken  = NULL;
	char* nextToken     = NULL;
	
	while ( currentLine[0] != '}' ) {

		currentToken    = strtok_s( currentLine, "\t", &nextToken );
		currentToken    = strtok_s( currentToken, " ", &nextToken );
		
		if ( STRINGS_ARE_EQUAL( currentToken, "shader" ) ) {
			shaderName = strtok_s( NULL, "\"", &nextToken );
			shaderName.append( ".tga" );
		} else if ( STRINGS_ARE_EQUAL( currentToken, "numverts" ) ) {
			int numVerticies = std::atoi( strtok_s( NULL, " ", &nextToken ) );
			verticies.resize( numVerticies );
		} else if ( STRINGS_ARE_EQUAL( currentToken, "vert" ) ) {
			if ( verticies.size() > 0 ) {
				ReadVertex( nextToken );
			} else {
				printf( "Can't load verticies. No numverts was specified\n" );
				return false;
			}
		} else if ( STRINGS_ARE_EQUAL( currentToken, "numtris" ) ) {
			int numTris = std::atoi( strtok_s( NULL, " ", &nextToken ) );
			triangles.resize( numTris );
		} else if ( STRINGS_ARE_EQUAL( currentToken, "tri" ) ) {
			if ( triangles.size() > 0 ) {
				ReadTriangle( nextToken );
			} else {
				printf( "Can't load triangles. No numtris was specified\n" );
				return false;
			}
		} else if ( STRINGS_ARE_EQUAL( currentToken, "numweights" ) ) {
			int numWeights = std::atoi( strtok_s( NULL, " ", &nextToken ) );
			weights.resize( numWeights );
		} else if ( STRINGS_ARE_EQUAL( currentToken, "weight" ) ) {
			if (weights.size() > 0 ) {
				ReadWeight( nextToken );
			} else {
				printf( "Can't load weights. No numweights was specified\n" );
				return false;
			}
		}

		currentLine = strtok_s( NULL, "\n", &nextLine );
	}

	BuildBindPose( jointInfo );    

	printf( "   Loaded mesh component\n" );
	printf( "      Vertex count:\t%i\n", verticies.size() );
	printf( "      Triangle count:\t%i\n", triangles.size() );
	printf( "      Weights count:\t%i\n", weights.size() );

	*endPosition = nextLine;
	return true;
}
/*
=============
MD5Mesh::ApplySkeleton

	Applys a skeleton to the mesh.
	Updates the VBO position and normal values.
=============
*/
void MD5Mesh::ApplySkeleton( const Skeleton& skeleton ) {
	unsigned	vertexIndex = 0;
	float		newValues[6];

    glBindBuffer( GL_ARRAY_BUFFER, vboName );

	for ( Verticies::iterator vertex = verticies.begin();
		  vertex != verticies.end(); ++vertex ) {
 
		glm::vec3 vertexPosition	= glm::vec3( 0.0f );
		glm::vec3 vertexNormal		= glm::vec3( 0.0f );

		unsigned weightStart = vertex->startWeight;
		unsigned weightCount = vertex->countWeight;
		
		//Calculate position using all weights
		for ( unsigned i = 0; i < weightCount; i++ ) {            
			const Weight&			currentWeight	= weights[weightStart + i];
			const SkeletonJoint&	currentJoint	= skeleton.joints[currentWeight.joint];

			glm::vec3 weightedVertex = currentJoint.orientation * currentWeight.position;
			vertexPosition += ( ( currentJoint.position + weightedVertex ) * currentWeight.bias );
			vertexNormal += ( ( currentJoint.orientation * vertex->jointNormal ) * currentWeight.bias );
		}

		memcpy( &newValues[0], &vertexPosition[0], sizeof( float ) * 3 ); //Vertex Position
		memcpy( &newValues[3], &vertexNormal[0]  , sizeof( float ) * 3 ); //Vertex Normal

        glBufferSubData( GL_ARRAY_BUFFER, sizeof( float ) * 16 * vertexIndex, sizeof( float ) * 6, &newValues[0] );

        ++vertexIndex;
	}

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}
/*
=============
MD5Mesh::SetVertexBufferToBindPose

	Resets the vertex buffer to the bind pose.
	This is needed for GPU skinning.
=============
*/
void MD5Mesh::SetVertexBufferToBindPose( void ) {

    glBindBuffer( GL_ARRAY_BUFFER, vboName );

	float newValues[6];
	unsigned vertexIndex = 0;
	for ( Verticies::iterator vertex = verticies.begin();
		  vertex != verticies.end(); ++vertex, ++vertexIndex ) {
 
		memcpy( &newValues[0], &vertex->bindPosition[0], sizeof( float ) * 3 ); //Vertex Position
		memcpy( &newValues[3], &vertex->bindNormal[0]  , sizeof( float ) * 3 ); //Vertex Normal

		glBufferSubData( GL_ARRAY_BUFFER, sizeof( float ) * 16 * vertexIndex, sizeof( float ) * 6, &newValues[0] );
	}

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}
/*
=============
MD5Mesh::ReadVertex

	Reads a vertex from a char buffer.
=============
*/
void MD5Mesh::ReadVertex( char* startPosition ) {

	char* nextToken = NULL;

	int vertexIndex = std::atoi( strtok_s( startPosition, " ", &nextToken ) );						 //Read the vertex index

	Vertex& currentVertex = verticies[vertexIndex];
	currentVertex.vertexIndex = vertexIndex;

	FileOperations::ReadVec2( strtok_s( NULL, "()", &nextToken ), currentVertex.textureCoordinate ); //Read texture coordinates
	currentVertex.startWeight = std::atoi( strtok_s( NULL, " ", &nextToken ) );						 //Read start weight
	currentVertex.countWeight = std::atoi( strtok_s( NULL, " ", &nextToken ) );						 //Read weight count
}
/*
=============
MD5Mesh::ReadWeight

	Reads a weight from a char buffer.
=============
*/
void MD5Mesh::ReadWeight( char* startPosition ) {
	char* nextToken = NULL;

	int weightIndex = std::atoi( strtok_s( startPosition, " ", &nextToken ) );					//Read the weight index

	Weight& currentWeight = weights[weightIndex];
	currentWeight.weightIndex = weightIndex;

	currentWeight.joint = std::atoi( strtok_s( NULL, " ", &nextToken ) );						//Read joint
	currentWeight.bias  = ( float )std::atof( strtok_s( NULL, " ", &nextToken ) );				//Read bias
	FileOperations::ReadVec3( strtok_s( NULL, "()", &nextToken ), currentWeight.position );		//Read position
}
/*
=============
MD5Mesh::ReadTriangle

	Reads an index from a char buffer.
=============
*/
void MD5Mesh::ReadTriangle( char* startPosition ) {

	char* nextToken = NULL;

	int triIndex = std::atoi( strtok_s( startPosition, " ", &nextToken ) );	

	Triangle& currentTri = triangles[triIndex];
	currentTri.triIndex = triIndex;
	
	currentTri.indices[0] = std::atoi( strtok_s( NULL, " ", &nextToken ) );
	currentTri.indices[1] = std::atoi( strtok_s( NULL, " ", &nextToken ) );
	currentTri.indices[2] = std::atoi( strtok_s( NULL, " ", &nextToken ) );
}
/*
=============
MD5Mesh::BuildBindPose

	Computes bind pose of the mesh.
	Generates the vertex buffer for the mesh.
=============
*/
void MD5Mesh::BuildBindPose( const Joints& joints ) {
	//TODO: These buffers can probably be blown these away since any future updates are done on the GPU
	vertexData = new float[verticies.size() * 16];   // 3 for position + 3 for normal + 2 for texture + 4 for boneIds + 4 for matrixId
	indexBuffer = new GLuint[triangles.size() * 3]; //3 indicies per tri

	ComputeVerticies( joints );
	ComputeIndicies();
	ComputeNormals( joints );    

	Upload(); //Upload to OpenGL
}
/*
=============
MD5Mesh::ComputeVerticies

	Computes the verticies for the bind pose.
=============
*/
void MD5Mesh::ComputeVerticies( const Joints& joints ) {
	int vertexIndex = 0;
	std::vector<Weight> weightsToSort;
	weightsToSort.reserve( 10 );

	for ( Verticies::iterator currentVertex = verticies.begin();
		  currentVertex != verticies.end(); ++currentVertex, ++vertexIndex ) {
		glm::vec3 vertexPosition    = glm::vec3( 0.0f );

		unsigned weightStart = currentVertex->startWeight;
		unsigned weightCount = currentVertex->countWeight;
		
		//Calculate position using all weights
		for ( unsigned i = 0; i < weightCount; i++ ) {            
			Weight&			currentWeight	= weights[weightStart + i];
			const Joint&	currentJoint	= joints[currentWeight.joint];            
			
			weightsToSort.push_back( currentWeight );

			glm::vec3 weightedVertex = currentJoint.orientation * currentWeight.position;
			vertexPosition += ( ( currentJoint.position + weightedVertex ) * currentWeight.bias );
		}

		float scaleAmount = 1.0f;
		if ( weightsToSort.size() > 4 ) {
			std::sort( weightsToSort.begin(), weightsToSort.end() );
			float currentBiasTotal = 0.0f;
			for ( int i = 0; i < 4; ++i ) {
				currentBiasTotal += weightsToSort[i].bias;
			}
			scaleAmount = 1.0f / currentBiasTotal; //How much the weights should scale to = 1.
		}	
		//Figure out which verticies are most important since they're out of order by default
		for ( unsigned i = 0; i < weightsToSort.size() && i < 4; ++i ) {
			currentVertex->boneWeights[i]	= weightsToSort[i].bias * scaleAmount;
			currentVertex->boneIndicies[i]	= ( float )weightsToSort[i].joint;
		}

		memcpy( &vertexData[vertexIndex * 16]     , &vertexPosition[0]					, sizeof( float ) * 3 ); //Vertex Position
		memcpy( &vertexData[vertexIndex * 16 + 6] , &currentVertex->textureCoordinate[0], sizeof( float ) * 2 ); //Vertex TexCoord
        memcpy( &vertexData[vertexIndex * 16 + 8] , &currentVertex->boneWeights[0]		, sizeof( float ) * 4 ); //Vertex Weights
        memcpy( &vertexData[vertexIndex * 16 + 12], &currentVertex->boneIndicies[0]		, sizeof( float ) * 4 ); //Matrix Indicies

		currentVertex->bindPosition = vertexPosition;
		
		weightsToSort.clear();
	}
}
/*
=============
MD5Mesh::ComputeIndicies

	Computes the indicies for mesh.
=============
*/
void MD5Mesh::ComputeIndicies( void ) {
	int triangleIndex = 0;
	for ( Triangles::iterator currentTri = triangles.begin();
		  currentTri != triangles.end(); ++currentTri ) {
		memcpy( &indexBuffer[triangleIndex * 3], &currentTri->indices[0], sizeof( GLuint ) * 3 );
		++triangleIndex;
	}
	indexCount = triangles.size() * 3;
    triangleCount = triangles.size();
}
/*
=============
MD5Mesh::ComputeNormals

	Computes the normals for the bind pose.
=============
*/
void MD5Mesh::ComputeNormals( const Joints& joints ) {
	//Calculate the average normals for each vertex
	for ( Triangles::iterator currentTriangle = triangles.begin();
		  currentTriangle != triangles.end(); ++currentTriangle ) {
		glm::vec3 faceNormal = glm::cross( verticies[currentTriangle->indices[2]].bindPosition - verticies[currentTriangle->indices[0]].bindPosition,
										   verticies[currentTriangle->indices[1]].bindPosition - verticies[currentTriangle->indices[0]].bindPosition );
		
		verticies[currentTriangle->indices[0]].bindNormal += faceNormal;
		verticies[currentTriangle->indices[1]].bindNormal += faceNormal;
		verticies[currentTriangle->indices[2]].bindNormal += faceNormal;
	}

	int vertexIndex = 0;
	for ( Verticies::iterator currentVertex = verticies.begin();
		  currentVertex != verticies.end(); ++currentVertex ) {
		currentVertex->bindNormal = glm::normalize( currentVertex->bindNormal );
		memcpy( &vertexData[vertexIndex * 16 + 3], &currentVertex->bindNormal[0], sizeof( float ) * 3 );

		int weightCount = currentVertex->countWeight;
		int weightStart = currentVertex->startWeight;

		//Calculate normal to joint local space
		//This is apparently faster for calculating the normal later
		for ( int i = 0; i < weightCount; i++ ) {            
			Weight& currentWeight		= weights[weightStart + i];
			const Joint&  currentJoint  = joints[currentWeight.joint];

		   currentVertex->jointNormal += ( ( currentVertex->bindNormal * currentJoint.orientation ) * currentWeight.bias );
		}

		vertexIndex++;
	}
}
/*
=============
MD5Mesh::Render

	Render the mesh with the appropriate diffuse map.
=============
*/
void MD5Mesh::Render( ModelSkinningType skinningType ) {
    glActiveTexture( GL_TEXTURE0 );
    if ( diffuseTexture != NULL ) {
        glBindTexture( GL_TEXTURE_2D, diffuseTexture->textureName );
	} else {
        glBindTexture( GL_TEXTURE_2D, 0 );
    }
	
    if ( skinningType == CPU_SKINNING ) {
        RenderCPUSkinning();
    } else if ( skinningType == GPU_SKINNING ) {
        RenderGPUSkinning();
    }

	glBindTexture( GL_TEXTURE_2D, 0 );
}
/*
=============
MD5Mesh::RenderCPUSkinning

	Render the mesh using CPU Skinning
=============
*/
void MD5Mesh::RenderCPUSkinning( void ) {
    glBindVertexArray( cpuVaoName );
    glDrawElements( GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, ( void* )( 0 ) );
    glBindVertexArray( 0 );		
}
/*
=============
MD5Mesh::RenderGPUSkinning

	Render the mesh using GPU Skinning
=============
*/
void MD5Mesh::RenderGPUSkinning( void ) {
    glBindVertexArray( gpuVaoName );
    glDrawElements( GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, ( void* )( 0 ) );
    glBindVertexArray( 0 );		
}
/*
=============
MD5Mesh::Upload

	Uploads the mesh to OpenGL Server.
	Returns whether or not it was successful
=============
*/
bool MD5Mesh::Upload( void ) {
    if ( !SetupOpenGLBuffers() ) {
        return false;
    }

    glBindVertexArray( cpuVaoName );
    glBindBuffer( GL_ARRAY_BUFFER, vboName );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, iboName );

    glEnableVertexAttribArray( 0 );
    glEnableVertexAttribArray( 1 );
    glEnableVertexAttribArray( 2 );
    glDisableVertexAttribArray( 3 ); //Don't need these for CPU Skinning
    glDisableVertexAttribArray( 4 ); //Don't need these for CPU Skinning

    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( float ) * 16, ( void* )( 0 ) );
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( float ) * 16, ( void* )( 3 * sizeof( float ) ) );
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof( float ) * 16, ( void* )( 6 * sizeof( float ) ) );

    glBufferData( GL_ARRAY_BUFFER, ( sizeof( float ) * 16 ) * verticies.size(), vertexData, GL_DYNAMIC_DRAW );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( GLuint ) * indexCount, indexBuffer, GL_STATIC_DRAW );

    glBindVertexArray( 0 );

    glBindVertexArray( gpuVaoName );
    glBindBuffer( GL_ARRAY_BUFFER, vboName );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, iboName );

    glEnableVertexAttribArray( 0 );
    glEnableVertexAttribArray( 1 );
    glEnableVertexAttribArray( 2 );
    glEnableVertexAttribArray( 3 );
    glEnableVertexAttribArray( 4 );

    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( float ) * 16, ( void* )( 0 ) );
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( float ) * 16, ( void* )( 3 * sizeof( float ) ) );
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof( float ) * 16, ( void* )( 6 * sizeof( float ) ) );
    glVertexAttribPointer( 3, 4, GL_FLOAT, GL_FALSE, sizeof( float ) * 16, ( void* )( 8 * sizeof( float ) ) );
    glVertexAttribPointer( 4, 4, GL_FLOAT, GL_FALSE, sizeof( float ) * 16, ( void* )( 12 * sizeof( float ) ) );

    glBindVertexArray( 0 );

	unsigned shaderNameLength = shaderName.size();
	if ( shaderNameLength > 3 && 
			shaderName.substr( shaderNameLength - 3, shaderNameLength ).compare( "tga" ) == 0 ) {
		diffuseTexture = TextureLoader::LoadTexture( shaderName.c_str() );
	}

	return true;
}
/*
=============
MD5Mesh::SetupOpenGLBuffers

	Sets up the required buffers on the GPU.
=============
*/
bool MD5Mesh::SetupOpenGLBuffers( void ) {
    glGenVertexArrays( 1, &cpuVaoName );
    if ( cpuVaoName == 0 ) {
        printf( "Error generating CPU VAO\n" );
        return false;
    }

    glGenVertexArrays( 1, &gpuVaoName );
    if ( gpuVaoName == 0 ) {
        printf( "Error generating GPU VAO\n" );
        glDeleteVertexArrays( 1, &cpuVaoName );
        return false;
    }

    glGenBuffers( 1, &vboName );
    if ( vboName == 0 ) {
        printf( "Error generating VBO\n" );
        glDeleteVertexArrays( 1, &cpuVaoName );
        glDeleteVertexArrays( 1, &gpuVaoName );
        return false;
    }
	
    glGenBuffers( 1, &iboName );
    if ( iboName == 0 ) {
        printf( "Error generating IBO\n" );
        glDeleteVertexArrays( 1, &cpuVaoName );
        glDeleteVertexArrays( 1, &gpuVaoName );
        glDeleteBuffers( 1, &vboName );
        return false;
    }

    return true;
}
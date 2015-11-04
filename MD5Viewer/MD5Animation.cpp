#include "MD5Animation.h"
#include "MD5FileOperations.h"
#include <glm\gtc\matrix_transform.hpp>

const std::string MD5Animation::DefaultAnimationName = "None";
/*
=============
MD5Animation::MD5Animation

	MD5Animation Default constructor.
=============
*/
MD5Animation::MD5Animation( void ) :
    numberOfFrames( 0 ),
    numberOfJoints( 0 ),
    numberOfAnimatedComponents( 0 ),
    frameRate( 0 ),
	frameDuration( 0.0f ),
	animationDuration( 0.0f ),
	animTime( 0.0f ),
	currentFrame( 0 ),
	animationName( "NULL" )
{}
/*
=============
MD5Animation::~MD5Animation

	MD5Animation Destructor.
=============
*/
MD5Animation::~MD5Animation( void )
{}
/*
=============
MD5Animation::CreateAnimationFromFile

	Create an animation using the .md5anim at path.
    Returns an animation if successful, NULL if not.
=============
*/
MD5Animation* MD5Animation::CreateAnimationFromFile( const char* path ) {
    MD5Animation* animation = new MD5Animation();
    
    if ( animation == NULL || 
		 !animation->InitWithAnimationFromFile( path ) ) {
        delete animation;
        return NULL;
    }

    return animation;
}
/*
=============
MD5Animation::InitWithAnimationFromFile

	Initializes the Animation
=============
*/
bool MD5Animation::InitWithAnimationFromFile( const char* path ) {

    if ( !ValidMD5AnimationExtension( path ) ) {
        return false;
    }

	char* fileData = FileOperations::ReadFileToCharBuffer( path );

	if ( fileData == NULL ) { //File wasn't opened
		printf( "Anim at path '%s' could not be opened\n", path );
		return false;
	} else {
		printf( "Beginning load of: %s\n", path );

		animationName   = path;
        unsigned slash  = animationName.find_last_of( "/" ) + 1;
        unsigned dot    = animationName.find_last_of( "." );
        animationName	= animationName.substr( slash, dot - slash );

		char* nextLineToken	= NULL;
		char* currentLine   = strtok_s( fileData, "\n", &nextLineToken );

		while ( currentLine != NULL ) {
            char* nextParam     = NULL;
            char* currentParam  = strtok_s( currentLine, " ", &nextParam );

            if ( STRINGS_ARE_EQUAL( currentParam, "MD5Version" ) ) { //Read the version
				nextParam[2] = '\0'; //Truncate to 2 chars.
                 if ( !STRINGS_ARE_EQUAL( nextParam, "10" ) ) {
					printf( "Only MD5Version 10 is supported\n" );
					delete[] fileData;
					return false;                
				 }
            } else if ( STRINGS_ARE_EQUAL( currentParam, "numFrames" ) ) { //Read numFrames
                numberOfFrames = std::atoi( nextParam );
                frameData.resize( numberOfFrames );
				frameBounds.resize( numberOfFrames );
				skeletonList.resize( numberOfFrames );
            } else if ( STRINGS_ARE_EQUAL( currentParam, "numJoints" ) ) { //Read numJoints
                numberOfJoints = std::atoi( nextParam );
				jointInfo.resize( numberOfJoints );
				baseFrameJoints.resize( numberOfJoints );
			} else if ( STRINGS_ARE_EQUAL( currentParam, "frameRate" ) ) { //Read frameRate
                frameRate = std::atoi( nextParam );
			} else if ( STRINGS_ARE_EQUAL( currentParam, "numAnimatedComponents" ) ) { //Read numAnimatedComponents
                numberOfAnimatedComponents = std::atoi( nextParam );
				SetFrameDataSize( numberOfAnimatedComponents );					  
            } else if ( STRINGS_ARE_EQUAL( currentParam, "hierarchy" ) ) { //Read hierarchy
				if ( jointInfo.size() == 0 ) {
                    printf( "numJoints was not specified\n" );
	                delete[] fileData;
                    return false;
                }
                nextLineToken = ReadHierarchy( nextLineToken );
            } else if ( STRINGS_ARE_EQUAL( currentParam, "bounds" ) ) { //Read the bounds
               if ( frameBounds.size() == 0 ) {
                    printf( "numFrames was not specified\n" );
	                delete[] fileData;
                    return false;
                }
                nextLineToken = ReadBounds( nextLineToken );
            } else if ( STRINGS_ARE_EQUAL( currentParam, "baseframe" ) ) { //Read the baseframe
               if ( frameBounds.size() == 0 ) {
                    printf( "numJoints was not specified\n" );
	                delete[] fileData;
                    return false;
                }
                nextLineToken = ReadBaseFrame( nextLineToken );
            } else if ( STRINGS_ARE_EQUAL( currentParam, "frame" ) ) { //Read a frame
               if ( frameData.size() == 0 ) {
                    printf( "numFrames was not specified\n" );
	                delete[] fileData;
                    return false;
                }
				unsigned frameIndex = std::atoi( strtok_s( NULL, " ", &nextParam ) );
                nextLineToken = ReadFrame( nextLineToken, frameIndex );
            }

			currentLine = strtok_s( NULL, "\n", &nextLineToken );
		}

		BuildSkeletonFrames();

		frameDuration		= 1.0f / frameRate;
		animationDuration	= frameDuration * numberOfFrames;

        printf( "   MD5Anim file parsed\n" );
		printf( "	   Number of frames:\t%i\n", numberOfFrames );
		printf( "	   Number of joints:\t%i\n", numberOfJoints );
		printf( "	   Animated components:\t%i\n", numberOfAnimatedComponents );
		printf( "	   Frame Rate:    \t%i\n", frameRate );
		printf( "	   Animation Time:\t%f\n", animationDuration );
        printf( "Successfully Loaded MD5Anim: %s\n", path );       
	}

	delete[] fileData;
    return true;
}
/*
=============
MD5Animation::Update

	MD5Animation Update.
    Updates the skeleton frame.
=============
*/
void MD5Animation::Update( float deltaTime ) {
	if ( numberOfFrames < 2 ) {
		return;
	}

	animTime += deltaTime;
	if ( animTime >= frameDuration ) {		
        while ( animTime > frameDuration ) {
            ++currentFrame;        
		    animTime -= frameDuration;
        
		    if ( currentFrame >= numberOfFrames ) {
			    currentFrame = 0;
		    }
        }
	}

	unsigned frame0 = currentFrame;
	unsigned frame1 = currentFrame + 1;
	if ( frame1 >= numberOfFrames ) {
		frame1 = 0;
	}
	float interpolateAmount = std::max( std::min( animTime / frameDuration, 1.0f ), 0.0f );
	InterpolateSkeletonFrames( skeletonList[frame0], skeletonList[frame1], currentSkeleton, interpolateAmount );
}
/*
=============
MD5Animation::SetFrameDataSize

	Sets the float array size in the frameData
=============
*/
void MD5Animation::SetFrameDataSize( unsigned size ) {
	for ( FrameDataList::iterator currentFrame = frameData.begin();
		  currentFrame != frameData.end(); ++currentFrame ) {
		delete[] currentFrame->data;
		currentFrame->data = new float[size];
	}
}
/*
=============
MD5Animation::ReadHierarchy

	Reads in the hierarchy information.
=============
*/
char* MD5Animation::ReadHierarchy( char* startingPosition ) {
	char* nextLine      = NULL;
	char* currentLine   = strtok_s( startingPosition, "\n", &nextLine );
	char* nextToken     = NULL;
	int   jointIndex    = 0;    

	while ( currentLine[0] != '}' ) {     
		ReadJointInfo( currentLine, jointInfo[jointIndex] );

		++jointIndex;
		currentLine = strtok_s( NULL, "\n", &nextLine );
	}

	return nextLine;
}
/*
=============
MD5Animation::ReadJointInfo

	Reads the current line into a JointInfo struct
=============
*/
void MD5Animation::ReadJointInfo( char* startPosition, JointInfo& dest ) {
	char* nextToken = NULL;

	dest.name		= strtok_s( startPosition, "\t\"", &nextToken );	 //Read the joint name	
	dest.parentID	= std::atoi( strtok_s( NULL, "\t ", &nextToken ) );	 //Joint's parent id
	dest.flags		= std::atoi( strtok_s( NULL, "\t ", &nextToken ) );	 //Joint's flags
	dest.startIndex = std::atoi( strtok_s( NULL, "\t ", &nextToken ) );	 //Joint's startIndex
}
/*
=============
MD5Animation::ReadBounds

	Reads in the bounds information
=============
*/
char* MD5Animation::ReadBounds( char* startingPosition ) {
	char* nextLine      = NULL;
	char* currentLine   = strtok_s( startingPosition, "\n", &nextLine );
	char* nextToken     = NULL;
	int   boundsIndex   = 0;    

	while ( currentLine[0] != '}' ) {     
		ReadFrameBound( currentLine, frameBounds[boundsIndex] );
		++boundsIndex;
		currentLine = strtok_s( NULL, "\n", &nextLine );
	}

	return nextLine;
}
/*
=============
MD5Animation::ReadFrameBound

	Reads in a frame bound.
=============
*/
void MD5Animation::ReadFrameBound( char* startPosition, Bound& dest ) {
	char* nextToken = NULL;
	nextToken = strtok_s( startPosition, "\t", &nextToken );
	
	FileOperations::ReadVec3( strtok_s( NULL, "()", &nextToken ), dest.minBounds ); //Read min bounds
	++nextToken;																	//Skip Space
	FileOperations::ReadVec3( strtok_s( NULL, "()", &nextToken ), dest.maxBounds ); //Read max bounds
}
/*
=============
MD5Animation::ReadBaseFrame

	Reads in the base frame information
=============
*/
char* MD5Animation::ReadBaseFrame( char* startingPosition ) {
	char* nextLine      = NULL;
	char* currentLine   = strtok_s( startingPosition, "\n", &nextLine );
	char* nextToken     = NULL;
	int   jointIndex	= 0;    

	while ( currentLine[0] != '}' ) {     
		ReadBaseFrameJoint( currentLine, baseFrameJoints[jointIndex] );
		++jointIndex;
		currentLine = strtok_s( NULL, "\n", &nextLine );
	}

	return nextLine;
}
/*
=============
MD5Animation::ReadBaseFrameJoint

	Reads in a base frame joint.
=============
*/
void MD5Animation::ReadBaseFrameJoint( char* startPosition, BaseFrameJoint& dest ) {
	char* nextToken = NULL;
	nextToken = strtok_s( startPosition, "\t", &nextToken );
	
	FileOperations::ReadVec3( strtok_s( NULL, "()", &nextToken ), dest.position );	  //Read joint position
	++nextToken;																	  //Skip Space
	FileOperations::ReadQuat( strtok_s( NULL, "()", &nextToken ), dest.orientation ); //Read joint orientation
}
/*
=============
MD5Animation::ReadFrame

	Reads in the data for a frame
=============
*/
char* MD5Animation::ReadFrame( char* startingPosition, unsigned frameIndex ) {
	FrameData&	currentFrame	= frameData[frameIndex];
	char*		nextLine		= NULL;
	char*		currentLine		= strtok_s( startingPosition, "\n", &nextLine );
	char*		currentToken	= NULL;	
	char*		nextToken		= NULL;
	unsigned	dataIndex		= 0;

	while ( currentLine[0] != '}' ) {     
		currentToken = strtok_s( currentLine, "\t", &nextToken );		
		currentToken = strtok_s( currentToken, " ", &nextToken );
		while ( currentToken != NULL ) {
			currentFrame.data[dataIndex++] = ( float )std::atof( currentToken );
			currentToken = strtok_s( NULL, " ", &nextToken );
		}
		currentLine = strtok_s( NULL, "\n", &nextLine );
	}

	return nextLine;
}
/*
=============
MD5Animation::BuildSkeletonFrames

	Builds the skeletons for all the frames.
=============
*/
void MD5Animation::BuildSkeletonFrames( void ) {
	BaseFrameJoint*	baseJoint			= NULL;
	Skeleton*		skeletonFrame		= NULL;
	float*			curFrameData		= NULL;
	unsigned		currentJointIndex	= 0;
	glm::vec3		rotatedPosition;

	for ( unsigned currentFrame = 0; currentFrame < numberOfFrames; ++currentFrame ) { //Each frame
		curFrameData	= frameData[currentFrame].data; 
		skeletonFrame	= &skeletonList[currentFrame];
		
        skeletonFrame->joints.resize( numberOfJoints );

		for ( JointInfoList::iterator currentJointInfo = jointInfo.begin();
			  currentJointInfo != jointInfo.end(); ++currentJointInfo ) { //Each joint
			baseJoint = &baseFrameJoints[currentJointIndex];
			unsigned dataOffset = 0;
			
			SkeletonJoint& currentSkeletonJoint = skeletonFrame->joints[currentJointIndex];
			
			currentSkeletonJoint.parentID		= currentJointInfo->parentID;
			currentSkeletonJoint.position		= baseJoint->position;
			currentSkeletonJoint.orientation	= baseJoint->orientation;

			if ( currentJointInfo->flags & TRANSLATE_X ) {
				currentSkeletonJoint.position.x = curFrameData[currentJointInfo->startIndex + dataOffset++];
			}
			if ( currentJointInfo->flags & TRANSLATE_Y ) {
				currentSkeletonJoint.position.y = curFrameData[currentJointInfo->startIndex + dataOffset++];
			}
			if ( currentJointInfo->flags & TRANSLATE_Z ) {
				currentSkeletonJoint.position.z = curFrameData[currentJointInfo->startIndex + dataOffset++];
			}
			if ( currentJointInfo->flags & QUATERNION_X ) {
				currentSkeletonJoint.orientation.x = curFrameData[currentJointInfo->startIndex + dataOffset++];
			}
			if ( currentJointInfo->flags & QUATERNION_Y ) {
				currentSkeletonJoint.orientation.y = curFrameData[currentJointInfo->startIndex + dataOffset++];
			}
			if ( currentJointInfo->flags & QUATERNION_Z ) {
				currentSkeletonJoint.orientation.z = curFrameData[currentJointInfo->startIndex + dataOffset++];
			}

			ComputeQuaternionW( currentSkeletonJoint.orientation );

			//Has a parent
			if ( currentSkeletonJoint.parentID > -1 ) {		
				SkeletonJoint& parent				= skeletonFrame->joints[currentSkeletonJoint.parentID];
				rotatedPosition						= parent.orientation * currentSkeletonJoint.position; //Rotate position				
				//Inherit parent transforms
				currentSkeletonJoint.position		= parent.position + rotatedPosition;
                currentSkeletonJoint.orientation	= glm::normalize( parent.orientation * currentSkeletonJoint.orientation );	
			}
			
            ++currentJointIndex;			
		}		
		currentJointIndex	= 0;		
	}

	currentSkeleton.joints.resize( numberOfJoints );		 //Enough joints for the skeleton used in the animation 
	currentSkeleton.jointMatricies.resize( numberOfJoints ); //Enough joints for the skeleton used in the animation 

    //Initialize the current skeleton
	for ( unsigned i = 0; i < numberOfJoints; ++i ) {
		currentSkeleton.joints[i] = skeletonList[0].joints[i];
	}
}
/*
=============
MD5Animation::SetCurrentFrame

	Sets currentFrame.
	Does bound checking.
=============
*/
void MD5Animation::SetCurrentFrame( unsigned newFrame ) {
	if ( newFrame < numberOfFrames ) {
		currentFrame = newFrame;
	}
}
/*
=============
MD5Animation::InterpolateSkeletonFrames

	Blends between 2 skeleton frames.
	Store result in destination skeleton.
=============
*/
void MD5Animation::InterpolateSkeletonFrames( const Skeleton& s1, const Skeleton& s2, Skeleton& destination, float amount ) {
	unsigned numberOfJoints         = s1.joints.size();
    glm::mat4 boneTranslationMatrix = glm::mat4( 1.0 );

	for ( unsigned i = 0; i < numberOfJoints; i++ ) {
		SkeletonJoint&  goalJoint	= destination.joints[i];
		glm::mat4&      goalMatrix  = destination.jointMatricies[i];

        const SkeletonJoint& joint1	= s1.joints[i];
		const SkeletonJoint& joint2	= s2.joints[i];

		goalJoint.parentID			= joint1.parentID;
		goalJoint.position			= joint1.position + ( amount * ( joint2.position - joint1.position ) );
        goalJoint.orientation       = glm::slerp( joint1.orientation, joint2.orientation, amount );
	
        boneTranslationMatrix[3][0] = goalJoint.position.x;
        boneTranslationMatrix[3][1] = goalJoint.position.y;
        boneTranslationMatrix[3][2] = goalJoint.position.z;

        goalMatrix = boneTranslationMatrix * glm::toMat4( goalJoint.orientation );
	}
}
/*
=============
MD5Animation::ComputeQuaternionW
 
	Computes the W value for a quaternion
=============
*/
void MD5Animation::ComputeQuaternionW( glm::quat& quaternion ) {
	float w = 1.0f - ( quaternion.x * quaternion.x ) - ( quaternion.y * quaternion.y ) - ( quaternion.z * quaternion.z );
	if ( w < 0.0f ) {
		quaternion.w = 0.0f;
	} else {
		quaternion.w = -sqrtf( w );
	}
}
/*
=============
MD5Animation::ValidMD5PathExtension
 
	Checks if the extension is .md5anim.
=============
*/
bool MD5Animation::ValidMD5AnimationExtension( const char* path ) {
    std::string pathStr( path );

    return ( pathStr.length() > 7 &&
             pathStr.substr( pathStr.length() - 7, 7 ).compare( "md5anim" ) == 0 );
}
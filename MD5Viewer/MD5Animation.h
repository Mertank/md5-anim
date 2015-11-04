#ifndef __MD5ANIMATION_H__
#define __MD5ANIMATION_H__

#define STRINGS_ARE_EQUAL( x, y ) ( strcmp( x, y ) == 0 )

#define TRANSLATE_X		0x01
#define TRANSLATE_Y		0x02
#define TRANSLATE_Z		0x04
#define QUATERNION_X	0x08
#define QUATERNION_Y	0x10
#define QUATERNION_Z	0x20

#include <GL\glew.h>
#include <fstream>

#include "MD5AnimationStructs.h"

/*
========================

	MD5Animation

		An MD5Animation.

========================
*/
class MD5Animation {
public:
	static const std::string	DefaultAnimationName;

								~MD5Animation( void );

    static MD5Animation*		CreateAnimationFromFile( const char* path );

    void						Update( float delta );
	void						BuildSkeletonFrames( void );
	static void					InterpolateSkeletonFrames( const Skeleton& skeleton1, const Skeleton& skeleton2, Skeleton& destination, float amount );

	inline const Skeleton&		GetCurrentSkeleton( void ) { return currentSkeleton; }
	
	void						SetCurrentFrame( unsigned newFrame );
	const std::string&			GetAnimationName( void ) const { return animationName; }

private:
								MD5Animation( void );

    bool						InitWithAnimationFromFile( const char* path );

    static bool					ValidMD5AnimationExtension( const char* path );

	void						SetFrameDataSize( unsigned size );
	
	char*						ReadHierarchy( char* startingPosition );
	char*						ReadBounds( char* startingPosition );
    char*						ReadBaseFrame( char* startingPosition );
	char*						ReadFrame( char* startingPosition, unsigned frameIndex );

	void						ReadJointInfo( char* startPosition, JointInfo& dest );
	void						ReadFrameBound( char* startPosition, Bound& dest );
	void						ReadBaseFrameJoint( char* startPosition, BaseFrameJoint& dest );
	
	void						ComputeQuaternionW( glm::quat& quaternion );

	std::string					animationName;

    unsigned int				numberOfFrames;
    unsigned int				numberOfJoints;
    unsigned int				frameRate;
    unsigned int				numberOfAnimatedComponents;
	unsigned int				currentFrame;

	float						frameDuration;
	float						animationDuration;
	float						animTime;

	FrameDataList				frameData;
	JointInfoList				jointInfo;
	Bounds						frameBounds;
	BaseFrameJoints				baseFrameJoints;
	SkeletonList				skeletonList;
	Skeleton					currentSkeleton;
};
typedef std::vector<MD5Animation*> MD5Animations;

#endif //__MD5ANIMATION_H__
#ifndef __MD5ANIMATIONSTRUCTS_H__
#define __MD5ANIMATIONSTRUCTS_H__

#include <string>
#include <vector>
#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtx\quaternion.hpp>

/*
========================

	JointInfo

		Info for a single joint.

========================
*/
struct JointInfo {
	std::string     name;
	int             parentID;
	int             flags;
	unsigned        startIndex;

	JointInfo( void ) :
		name( "NULL" ),
		parentID( -1 ),
		flags( 0 ),
		startIndex( 0 )
	{}
};
typedef std::vector<JointInfo> JointInfoList;
/*
========================

	Bound

		A bounding box for an animation frame.

========================
*/
struct Bound {
	glm::vec3   minBounds;
	glm::vec3   maxBounds;

	Bound( void ) :
		minBounds( 0.0f ),
		maxBounds( 0.0f )
	{}
};
typedef std::vector<Bound> Bounds;
/*
========================

	BaseFrameJoint

		A joint in its base position.

========================
*/
struct BaseFrameJoint {
	glm::vec3   position;
	glm::quat   orientation;

	BaseFrameJoint( void ) :
		position( 0.0f ),
		orientation()
	{}
};
typedef std::vector<BaseFrameJoint> BaseFrameJoints;
/*
========================

	FrameData

		Data for a single frame.

========================
*/
struct FrameData {
	unsigned    index;
	float*      data;

	FrameData( void ) :
		index( 0 ),
		data( NULL )
	{}

	~FrameData( void ) {
		delete[] data;
	}
};
typedef std::vector<FrameData> FrameDataList;
/*
========================

	Skeleton Joint

		Information about a single joint.

========================
*/
struct SkeletonJoint {
	int			parentID;
	glm::vec3	position;
	glm::quat	orientation;

	SkeletonJoint( void ) :
		parentID( -1 ),
		position( 0.0f ),
		orientation()
	{}
};
typedef std::vector<SkeletonJoint>  SkeletonJoints;
typedef std::vector<glm::mat4>      SkeletonMatricies;
/*
========================

	Skeleton 

		A skeleton representation of
		a single animation frame.

========================
*/
struct Skeleton {
	SkeletonJoints	    joints;
    SkeletonMatricies   jointMatricies;
};
typedef std::vector<Skeleton> SkeletonList;


#endif //__MD5ANIMATIONSTRUCTS_H__
#ifndef __FILEOPERATIONS_H__
#define __FILEOPERATIONS_H__

#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>
#include <string>

class FileOperations {
public:
	/*
	=============
	FileOperations::ReadFileToCharBuffer

		Reads a file at a specified path into a char buffer.
		You are responsible for freeing the char buffer.
		Returns NULL if there was a problem opening the file.
	=============
	*/
	static char* ReadFileToCharBuffer( const char* path ) {
		std::ifstream file;
		file.open( path, std::ios_base::in | std::ios_base::binary );
		char* fileData = NULL;
		if ( file.good() ) {
			//Read the whole file at once!
			//Like a boss
			file.seekg( 0, std::ios::end );
			unsigned fileSize = ( unsigned )file.tellg();
			fileData = new char[fileSize];
			file.seekg( 0, std::ios::beg );
			file.read( fileData, fileSize );
		} else {
			printf( "Could not open file: %s", path );
		}
		file.close();

		return fileData;
	}
	/*
	=============
	FileOperations::ReadVec2

		Reads a glm::vec2 from a char buffer.
	=============
	*/
	static void ReadVec2( char* src, glm::vec2& dest ) {
		char* nextToken = NULL;

		dest.s = ( float )std::atof( strtok_s( src, " ", &nextToken ) );
		dest.t = ( float )std::atof( strtok_s( NULL, " ", &nextToken ) );
	}
	/*
	=============
	FileOperations::ReadVec3

		Reads a glm::vec3 from a char buffer.
	=============
	*/
	static void ReadVec3( char* src, glm::vec3& dest ) {
		char* nextToken = NULL;

		dest.x = ( float )std::atof( strtok_s( src, " ", &nextToken ) );
		dest.y = ( float )std::atof( strtok_s( NULL, " ", &nextToken ) );
		dest.z = ( float )std::atof( strtok_s( NULL, " ", &nextToken ) );    
	}
	/*
	=============
	FileOperations::ReadQuat

		Reads a glm::quat from a char buffer.
		Calculates w, if required.
	=============
	*/
	static void ReadQuat( char* src, glm::quat& dest ) {
		char* nextToken = NULL;

		dest.x = ( float )std::atof( strtok_s( src, " ", &nextToken ) );
		dest.y = ( float )std::atof( strtok_s( NULL, " ", &nextToken ) );
		dest.z = ( float )std::atof( strtok_s( NULL, " ", &nextToken ) );    

		float t = 1.0f - ( dest.x * dest.x ) - ( dest.y * dest.y ) - ( dest.z * dest.z );
		if ( t < 0.0f ) {
			dest.w = 0.0f;
		} else {
			dest.w = -sqrtf( t );
		}
	}
};

#endif //__FILEOPERATIONS_H__
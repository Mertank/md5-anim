#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include "GLSH.h"
#include <map>

/*
========================

	Program

		A program with all of it's uniform attributes.
		Keeps track of current values to avoid redundant state changes.

========================
*/
class Program {
public:
									~Program( void );

	static Program*					CreateProgram( const char* vertShaderPath, const char* fragShaderPath );
	GLuint							GetProgramId( void ) { return programID; }
	void							SetUniform( const char* name, const float* value, unsigned size );
	void							SetUniform( const char* name, const int* value, unsigned size );
	void							SetUniform( const char* name, const float value );
	void							SetUniform( const char* name, const int value );
	GLuint							GetAttributeLocation( const char* name );
    void                            Use( void );
									
private:
    /*
    ========================

	    Uniform

		    Helper struct to store current value on CPU side.
            This will avoid redundant state changes

    ========================
    */
    struct Uniform {
    private:
	    void*		currentValues;	

    public:
	    unsigned 	size;
	    GLint		location;
	    std::string name;

	    Uniform( GLuint location, const char* name, unsigned valueSize ) :
		    location( location ),
		    name( name ),
		    size( valueSize )
	    {
		    currentValues = new float[valueSize];
		    memset( currentValues, 0, valueSize * sizeof( float ) );
	    }

		~Uniform( void ) {
			delete[] currentValues;
		}
        /*
        ==============
	        Set

		        Will set float values.
        ==============
        */
	    void Set( const float* newValue ) {
			if ( memcmp( currentValues, newValue, sizeof( float ) * size ) != 0 ) {
				if ( size == 1 ) {
					glUniform1fv( location, 1, &newValue[0] );
				} else if ( size == 2 ) {
					glUniform2fv( location, 1, &newValue[0] );
				} else if ( size == 3 ) {
					glUniform3fv( location, 1, &newValue[0] );
				} else if ( size == 4 ) {
					glUniform4fv( location, 1, &newValue[0] );
				} else if ( size == 12 ) {
					glUniformMatrix3fv( location, 1, GL_FALSE, &newValue[0] );
				} else if ( size == 16 ) {
					glUniformMatrix4fv( location, 1, GL_FALSE, &newValue[0] );
				}

				memcpy( currentValues, newValue, size );
			}
	    }
        /*
        ==============
	        Set

		        Will set int values.
        ==============
        */
        void Set( const int* newValue ) {
		   if ( memcmp( currentValues, newValue, sizeof( int ) * size ) != 0 ) {
			    if ( size == 1 ) {
					glUniform1iv( location, 1, &newValue[0] );
				} else if ( size == 2 ) {
					glUniform2iv( location, 1, &newValue[0] );
				} else if ( size == 3 ) {
					glUniform3iv( location, 1, &newValue[0] );
				} else if ( size == 4 ) {
					glUniform4iv( location, 1, &newValue[0] );
				} 
				memcpy( currentValues, newValue, size );
		    }
	    }
    };

    
									Program( void );
	bool							InitializeWithShaders( const char* vertShaderPath, const char* fragShaderPath );	
    Uniform*                        GetCachedUniform( const char* name );

	GLuint							programID;
	static GLuint					currentProgam;

	typedef std::vector<Uniform*>   Uniforms;
    Uniforms						uniforms;    
};

#endif __PROGRAM_H__
#include "Program.h"

GLuint Program::currentProgam = 0;

/*
=============
Program::Program

	Program Constructor.
=============
*/
Program::Program( void )
	: programID( 0 )
{}
/*
=============
Program::~Program

	Program Destructor.
=============
*/
Program::~Program( void ) {
	for ( Uniforms::iterator uniform = uniforms.begin();
		  uniform != uniforms.end(); ++uniform ) {
		delete *uniform;
	}
	uniforms.clear();

	glDeleteProgram( programID );
    glUseProgram( 0 );
}
/*
=============
Program::Program

	Create a shader program.
=============
*/
Program* Program::CreateProgram( const char* vertShaderPath, const char* fragShaderPath ) {
	Program* program = new Program();
    
	if ( program == NULL ||
		!program->InitializeWithShaders( vertShaderPath, fragShaderPath ) ) {
		delete program;
		
		return NULL;
	}
    printf( "Successfully created program: %i\n", program->programID );
	return program;
}
/*
=============
Program::InitializeWithId

	Program Initialization.
=============
*/
bool Program::InitializeWithShaders( const char* vertShaderPath, const char* fragShaderPath ) {
	
	programID = glsh::BuildShaderProgram( vertShaderPath, fragShaderPath );
	return programID > 0;
}
/*
=============
Program::Use

	Uses the program.
=============
*/
void Program::Use( void ) {
	if ( currentProgam != programID ) {
		glUseProgram( programID );
		currentProgam = programID;
	}
}
/*
=============
Program::SetUniform

	Sets the a uniform float value.
=============
*/
void Program::SetUniform( const char* name, const float* value, unsigned size ) {	
	Uniform* cachedUniform = GetCachedUniform( name );
    if ( cachedUniform ) {
        cachedUniform->Set( value );
    } else {
	    //Not found
        GLint location = glGetUniformLocation( programID, name );
	    if ( location > -1 ) {
		    Uniform* newUniform = new Uniform( location, name, size );
		    newUniform->Set( value );
        
            uniforms.push_back( newUniform );
	    } else {
			printf( "Could not find float uniform: %s\n", name );
		}
    }
}
/*
=============
Program::SetUniform

	Sets a uniform int value.
=============
*/
void Program::SetUniform( const char* name, const int* value, unsigned size ) {	
	Uniform* cachedUniform = GetCachedUniform( name );
    if ( cachedUniform ) {
        cachedUniform->Set( value );
    } else {
	    //Not found
        GLint location = glGetUniformLocation( programID, name );
	    if ( location > -1 ) {
		    Uniform* newUniform = new Uniform( location, name, size );
		    newUniform->Set( value );
        
            uniforms.push_back( newUniform );
	    } else {
			printf( "Could not find int uniform: %s\n", name );
        }
    }
}
/*
=============
Program::SetUniform

	Sets a single int uniform value
=============
*/
void Program::SetUniform( const char* name, const int value ) {	
	SetUniform( name, &value, 1 );
}
/*
=============
Program::SetUniform

	Sets a single float uniform value
=============
*/
void Program::SetUniform( const char* name, const float value ) {	
	SetUniform( name, &value, 1 );
}
/*
=============
Program::GetCachedUniform

	Gets a cached uniform.
    Returns null if it's not cached.
=============
*/
Program::Uniform* Program::GetCachedUniform( const char* name ) {
    Use();
	Uniforms::iterator iter = uniforms.begin();
    while( iter != uniforms.end() ) { //Is location cached?
		if ( ( *iter )->name.compare( name ) == 0 ) {
			return ( *iter );
		}
		++iter;
	}

    return NULL;
}
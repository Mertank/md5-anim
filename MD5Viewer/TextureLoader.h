#ifndef __TEXTURELOADER_H__
#define __TEXTURELOADER_H__

#include <GL\glew.h>
#include <map>
#include <fstream>
/*
=============================

    Texture

        A texture object.
        Stores the OpenGL location.
		Yeah, I wrote my own even though glsh has one.

=============================
*/
struct Texture {
    GLuint      textureName;
    GLenum      type; //GL_RGB or GL_RGBA

    unsigned    width;
    unsigned    height;

    unsigned    bitsPerPixel;
    
    Texture( void ) :
        width( 0 ),
        height( 0 ),
        type( GL_RGBA ),
        textureName( 0 )
    {}

    ~Texture( void ) {
        glDeleteTextures( 1, &textureName );
    }
};
/*
=============================

    TextureLoader

        Loads TGA texture.
        Both compressed and uncompressed versions are supported.

=============================
*/
class TextureLoader {
public:
    static Texture*		LoadTexture( const char* path );
private:
    static Texture*     LoadUncompressedTGA( std::ifstream& fileStream );
    static Texture*     LoadCompressedTGA( std::ifstream& fileStream );

    static GLubyte      UncompressedSignature[];
    static GLubyte      CompressedSignature[];
};

#endif //__TEXTURELOADER_H__
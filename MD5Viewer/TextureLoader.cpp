#include "TextureLoader.h"

GLubyte TextureLoader::UncompressedSignature[12] = { 0, 0,  2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
GLubyte TextureLoader::CompressedSignature  [12] = { 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
/*
=============
TextureLoader::LoadTexture

	Loads a TGA Texture.
    Returns a Texture pointer.
    You are responsible for deleting the pointer.
=============
*/
Texture* TextureLoader::LoadTexture( const char* path ) {
    std::ifstream file( path, std::ios_base::in | std::ios_base::binary );
    Texture* tex = NULL;

    if ( file.good() ) {
        GLubyte signature[12];
        file.read( ( char* )&signature[0], 12 );

        if ( ( memcmp( signature, CompressedSignature, 12 ) == 0 ) ) { //Compressed
            tex = LoadCompressedTGA( file );
        } else if ( ( memcmp( signature, UncompressedSignature, 12 ) == 0 ) ) { //Uncompressed 
            tex = LoadUncompressedTGA( file );
        } else {
            printf( "Not a valid TGA signature\n" );
        }
    } else {
		printf( "Could not open texture\n" );
	}

    file.close();

	if ( tex == NULL ) {
		printf( "Could not load Texture: %s\n", path );
	}

    return tex;
}
/*
=============
TextureLoader::LoadUncompressedTGA

	Loads an Uncompressed TGA Texture from a file stream.
    Returns a texture object.
=============
*/
Texture* TextureLoader::LoadUncompressedTGA( std::ifstream& file ) {
    Texture* tex = new Texture();
    
    GLubyte header[6];
    file.read( ( char* )&header[0], 6 ); //Read header

    tex->width  = header[1] * 256 + header[0];
    tex->height = header[3] * 256 + header[2];

    tex->bitsPerPixel = header[4];

    //That's not right
    if ( tex->width <= 0 || tex->height <= 0 || ( tex->bitsPerPixel != 24 && tex->bitsPerPixel != 32 ) ) {
        delete tex;
		printf( "Invalid TGA Header\n" );
        return NULL;
    }

    tex->type = ( ( tex->bitsPerPixel == 24 ) ? GL_RGB : GL_RGBA );
    unsigned bytesPerPixel = ( tex->bitsPerPixel / 8 );
    
    unsigned imageSize = bytesPerPixel * tex->width * tex->height;
    GLubyte* imageData = new GLubyte[imageSize];
    
    if ( imageData == NULL ) { //WHAT?!?
        delete tex;
		printf( "No texture data read\n" );
        return NULL;
    }

    //Image needs to be flipped on Y because OpenGL
	for ( int i = tex->height - 1; i >= 0; i-- ) {
		file.read( ( char* )&imageData[i * tex->width * bytesPerPixel], tex->width * bytesPerPixel );
	}
    file.close();

    //This is wizardy to change it from BGR to RGB
    for ( GLuint swap = 0; swap < imageSize; swap += bytesPerPixel ) {
        imageData[swap] ^= imageData[swap + 2] ^= 
        imageData[swap] ^= imageData[swap + 2];
    }
    
	glGenTextures( 1, &tex->textureName );
    
	if ( tex->textureName == 0 ) {
        delete tex;
		delete[] imageData;

		printf( "Texture not created\n" );

        return NULL;
    }

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, tex->textureName );
        
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glTexImage2D( GL_TEXTURE_2D, 0, tex->type, tex->width, tex->height, 0, tex->type, GL_UNSIGNED_BYTE, imageData );

    delete[] imageData;

    return tex;
}
/*
=============
TextureLoader::LoadCompressedTGA

	Loads a Compressed TGA Texture from a file stream.
    Returns a texture object.
=============
*/
Texture* TextureLoader::LoadCompressedTGA( std::ifstream& file ) {
    Texture* tex = new Texture();

    GLubyte header[6];
    file.read( ( char* )&header[0], 6 ); //Read header

    tex->width  = header[1] * 256 + header[0];
    tex->height = header[3] * 256 + header[2];

    tex->bitsPerPixel = header[4];

    //That's not right
    if ( tex->width <= 0 || tex->height <= 0 || ( tex->bitsPerPixel != 24 && tex->bitsPerPixel != 32 ) ) { 
        delete tex;		
		printf( "Invalid TGA Header\n" );
        return NULL;
    }

    tex->type = ( ( tex->bitsPerPixel == 24 ) ? GL_RGB : GL_RGBA );
    unsigned bytesPerPixel = ( tex->bitsPerPixel / 8 );
    
    unsigned imageSize = bytesPerPixel * tex->width * tex->height;
    GLubyte* imageData = new GLubyte[imageSize];
    memset( imageData, 0, imageSize );
    
    if ( imageData == NULL ) { //WHAT?!?
        delete tex;
		printf( "No texture data read\n" );
        return NULL;
    }

    GLuint pixelCount       = tex->width * tex->height;
    GLuint currentPixel     = 0;
    GLuint currentRow       = tex->height - 1;
    GLuint currentByte      = currentRow * ( tex->width * bytesPerPixel );    
    GLubyte* color          = new GLubyte[bytesPerPixel];

    do {
        GLubyte chunkHeader = 0;
        file.read( ( char* )&chunkHeader, 1 );

        if ( chunkHeader < 128 ) { //'RAW' chunk
            chunkHeader++; //Total number of pixels
        } else { //RLE Header
            chunkHeader -= 127;
        }

        for ( short i = 0; i < chunkHeader; i++ ) {
            file.read( ( char* )color, bytesPerPixel );

            imageData[currentByte]      = color[2]; //R
            imageData[currentByte + 1]  = color[1]; //G
            imageData[currentByte + 2]  = color[0]; //B
            if ( bytesPerPixel == 4 ) {             
                imageData[currentByte + 3]  = color[3]; //A
            }
            currentByte += bytesPerPixel;
            if ( currentByte > ( tex->width * bytesPerPixel ) ) {
                --currentRow;
                currentByte =  currentRow * ( tex->width * bytesPerPixel );
            }
            ++currentPixel;
        }
        
    } while ( currentPixel < pixelCount );

	delete[] color;

    file.close();    

    glGenTextures( 1, &tex->textureName );
    if ( tex->textureName == 0 ) {
        delete tex;
		delete[] imageData;

		printf( "Texture not created\n" );
        return NULL;
    }
    glBindTexture( GL_TEXTURE_2D, tex->textureName );
    
    glActiveTexture( GL_TEXTURE0 );    
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexImage2D( GL_TEXTURE_2D, 0, tex->type, tex->width, tex->height, 0, tex->type, GL_FLOAT, imageData );

    delete[] imageData;
    return tex;
}
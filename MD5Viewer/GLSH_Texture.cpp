#include "GLSH_Texture.h"
#include "GLSH_Image.h"
#include "GLSH_Util.h"

#include <iostream>

namespace glsh {

GLuint CreateTexture2D(const std::string& path, bool genMipmaps)
{
    Image img;
    if (img.LoadTarga(path)) {
        return CreateTexture2D(img, genMipmaps);
    } else {
        std::cerr << "*** Failed to load texture from " << path << std::endl;
        return 0;
    }
}

GLuint CreateTexture2D(const Image& img, bool genMipmaps)
{
    if (!img.isGood()) {
        std::cout << "*** Can't create texture from image: it ain't no good" << std::endl;
        return 0;
    }

    int bpp = img.getBytesPerPixel();

    // GL texture format lookup table indexed by image color depth in bytes-per-pixel
    static const GLenum bpp2fmt[] = { GL_NONE, GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA };

    // figure out the image format for proper unpacking by glTexImage2D
    GLenum imgFormat = bpp2fmt[img.getBytesPerPixel()];

    // set the texture internal format to match the image format
    GLint texFormat = imgFormat;

    // get source image width, height, and pointer to data
    GLsizei width = img.getWidth();
    GLsizei height = img.getHeight();
    const unsigned char* data = img.getData();

    // create texture object
    GLuint texId = 0;
    glGenTextures(1, &texId);

    // bind it as a 2D texture
    glBindTexture(GL_TEXTURE_2D, texId);

    // set pixel row alignment, needed by glTexImage2D
    int rowlen = img.getWidth() * img.getBytesPerPixel();
    if ((rowlen & 3) == 0) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    } else if ((rowlen & 1) == 0) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
    } else {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }

    // upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, texFormat, width, height,
                                0, imgFormat, GL_UNSIGNED_BYTE, data);

    // generate mipmaps if needed

    bool haveMipmaps = false;

    if (genMipmaps) {
        if (IsPowerOf2(width) && IsPowerOf2(height)) {
            if (width > 1 && height > 1) {
                glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
                glGenerateMipmap(GL_TEXTURE_2D);
                haveMipmaps = true;
            } else {
                std::cerr << "*** Oops, not generating mipmaps: image at minimum resolution already" << std::endl;
            }
        } else {
            std::cerr << "*** Oops, not generating mipmaps: image dimensions not powers of 2" << std::endl;
        }
    }

    if (!haveMipmaps) {
        // tell OpenGL that this texture only has one level (texture completeness)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    }

    // FIXME: check for errors

    return texId;
}

} // end of namespace

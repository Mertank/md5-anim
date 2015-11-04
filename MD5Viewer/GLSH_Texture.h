#ifndef GLSH_TEXTURE_H_
#define GLSH_TEXTURE_H_

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>

namespace glsh {

class Image;

// load texture from file
GLuint CreateTexture2D(const std::string& path, bool genMipmaps);

// create texture from Image data in memory
GLuint CreateTexture2D(const Image& img, bool genMipmaps);


struct TexRect {
    float w, h;             // size in texels/pixels
    float uLeft, uRight;
    float vBottom, vTop;
    TexRect() : w(0), h(0), uLeft(0), uRight(0), vBottom(0), vTop(0) {}
};


class TextureSpace {
    float mTexelWidth, mTexelHeight;
    float mUOffset, mVOffset;
    int mYMax;

public:
    TextureSpace(int w, int h)
        : mTexelWidth(1.0f / w)
        , mTexelHeight(1.0f / h)
        , mUOffset(0.5f * mTexelWidth)
        , mVOffset(0.5f * mTexelHeight)
        , mYMax(h - 1)
    { }

    glm::vec2 getTexCoord(int x, int y) const
    {
        return glm::vec2(x * mTexelWidth + mUOffset, (mYMax - y) * mTexelHeight + mVOffset);
    }

    void getTexRect(int x, int y, int w, int h, TexRect* pRet)
    {
        pRet->w = (float)w;
        pRet->h = (float)h;

        float uLeft = x * mTexelWidth + mUOffset;
        pRet->uLeft = uLeft;
        pRet->uRight = uLeft + w * mTexelWidth;

        float vTop = (mYMax - y) * mTexelHeight + mVOffset;
        pRet->vTop = vTop;
        pRet->vBottom = vTop - h * mTexelHeight;
    }
};

} // end of namespace

#endif

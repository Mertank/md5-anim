// <barf>

#ifndef GLSH_TEXT_H_
#define GLSH_TEXT_H_

#include "GLSH_Texture.h"
#include "GLSH_Vertex.h"
#include <glm\gtc\matrix_transform.hpp>

#include <string>
#include <vector>


namespace glsh {

class Font {

    GLuint                  mTex;

    std::vector<TexRect>    mChars;     // indexed using ASCII codes

    float                   mHeight;    // useful when rendering multiple lines of text
    float                   mWidth;     // useful when rendering fixed-width text

public:
                            Font();
                            ~Font();

    bool                    Load(const std::string& name);
    void                    Unload();

    bool                    IsLoaded() const;

    GLuint                  getTex() const              { return mTex; }

    float                   getHeight() const           { return mHeight; }
    float                   getWidth() const            { return mWidth; }

    const TexRect&          getCharRect(int c) const    { return mChars[c]; }       // UNCHECKED

    bool                    hasChar(int c) const        { return c >= 0 && c < (int)mChars.size() && mChars[c].w != 0.0f && mChars[c].h != 0.0f; }
};


#if _WIN32
// fuck you Win32
#undef CreateFont
#endif

Font* CreateFont(const std::string& name);


class TextBatch {

    const Font*             mFont;
    std::vector<VPT>        mVerts;
    float                   mWidth, mHeight;
    glm::mat4               modelMatrix;

public:

                            TextBatch();

    void                    SetText(const Font* font, const std::string& text, bool fixedWidth = false);
    void                    SetText(const Font* font, const std::vector<std::string>& textLines);
    void                    SetPosition( const glm::vec2& pos );
	void                    SetScale( float scale );
    const glm::mat4&        GetModelMatrix( void ) const { return modelMatrix; }

    void                    Clear();

    const Font*             GetFont() const         { return mFont; }
    float                   GetWidth() const        { return mWidth * modelMatrix[0][0]; }
    float                   GetHeight() const       { return mHeight * modelMatrix[0][0]; }

    void                    DrawGeometry() const;
};

} // end of namespace

#endif

// </barf>

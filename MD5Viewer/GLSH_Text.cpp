#include "GLSH_Text.h"
#include "GLSH_Image.h"
#include "GLSH_Mesh.h"

#include "tinyxml2.h"

#include <iostream>
#include <vector>

namespace glsh {

Font::Font()
    : mTex(0)
    , mHeight(0)
    , mWidth(0)
{
}

Font::~Font()
{
    Unload();
}

void Font::Unload()
{
    if (IsLoaded()) {
        glDeleteTextures(1, &mTex);
        mTex = 0;
        mChars = std::vector<TexRect>();
        mHeight = 0;
        mWidth = 0;
    }

}

bool Font::IsLoaded() const
{
    return mTex != 0;
}

bool Font::Load(const std::string& name)
{
    Unload();

    std::cout << "Loading font " << name << std::endl;

    std::string textureFilename = name + ".tga";
    std::string metricsFilename = name + ".xml";

    using namespace tinyxml2;

    XMLDocument doc;
    if (doc.LoadFile(metricsFilename.c_str()) != XML_NO_ERROR) {
        std::cerr << "*** Failed to load " << metricsFilename << std::endl;
        return false;
    }

    Image img;
    if (!img.LoadTarga(textureFilename)) {
        std::cerr << "*** Failed to load " << textureFilename << std::endl;
        return false;
    }

    TextureSpace texSpace(img.getWidth(), img.getHeight());

    XMLElement* root = doc.FirstChildElement("fontMetrics");
    //std::cout << (void*)root << std::endl;

    mChars.resize(128);  // basic ASCII chars only

    int maxHeight = 0;
    int maxWidth = 0;

    XMLElement* ch = root->FirstChildElement("character");
    for ( ; ch; ch = ch->NextSiblingElement("character")) {
        const char* keystr = ch->Attribute("key");
        //std::cout << "char " << keystr << std::endl;

        int key = std::atoi(keystr);

        if (key < 0 || key >= (int)mChars.size()) {
            std::cerr << "character key out of range: " << key << std::endl;
            continue;
        }

        const char* xstr = ch->FirstChildElement("x")->GetText();
        const char* ystr = ch->FirstChildElement("y")->GetText();
        const char* wstr = ch->FirstChildElement("width")->GetText();
        const char* hstr = ch->FirstChildElement("height")->GetText();

        //std::cout << xstr << std::endl;

        int x = std::atoi(xstr);
        int y = std::atoi(ystr);
        int w = std::atoi(wstr);
        int h = std::atoi(hstr);

        if (h > maxHeight) {
            maxHeight = h;
        }
        if (w > maxWidth) {
            maxWidth = w;
        }

        texSpace.getTexRect(x, y, w, h, &mChars[key]);
    }

    mHeight = (float)maxHeight;
    mWidth = (float)maxWidth;

    // create the texture
    mTex = CreateTexture2D(img, false);

    if (!mTex) {
        std::cerr << "*** Failed to create font texture" << std::endl;
        return false;
    }

    // yay
    return true;
}

Font* CreateFont(const std::string& name)
{
    Font* font = new Font;
    font->Load(name);
    return font;
}


TextBatch::TextBatch()
    : mFont(NULL)
    , mWidth(0)
    , mHeight(0)
	, modelMatrix( 1.0 )
{
}

void TextBatch::Clear()
{
    mFont = NULL;
    mWidth = 0;
    mHeight = 0;
    mVerts.clear();
}

void TextBatch::SetText(const Font* font, const std::string& text, bool fixedWidth)
{
    Clear();

    if (font && font->IsLoaded()) {
        mFont = font;

        float x = 0;
        float y = 0;

        float maxX = 0;  // used to figure out width

        float fontHeight = font->getHeight();
        float fontWidth = font->getWidth();

        // line spacing
        float lineSpacing = 0;
        //float lineSpacing = 0.1f * fontHeight;
        float dy = fontHeight + lineSpacing;

        for (char c : text) {

            if (c == '\n') {

                x = 0;
                y -= dy;

            } else if (font->hasChar(c)) {
                const TexRect& cRect = font->getCharRect(c);

                float xOffset;
                if (fixedWidth) {
                    // center this character
                    xOffset = std::floor(0.5f * (fontWidth - cRect.w));
                } else {
                    xOffset = 0;
                }

                float x1 = x + xOffset;
                float x2 = x1 + cRect.w;
                float y1 = y;
                float y2 = y1 - cRect.h;

                mVerts.push_back(VPT(x1, y1, 0,  cRect.uLeft, cRect.vTop));        // top-left
                mVerts.push_back(VPT(x1, y2, 0,  cRect.uLeft, cRect.vBottom));     // bottom-left
                mVerts.push_back(VPT(x2, y2, 0,  cRect.uRight, cRect.vBottom));    // bottom-right

                mVerts.push_back(VPT(x2, y2, 0,  cRect.uRight, cRect.vBottom));    // bottom-right
                mVerts.push_back(VPT(x2, y1, 0,  cRect.uRight, cRect.vTop));       // top-right
                mVerts.push_back(VPT(x1, y1, 0,  cRect.uLeft, cRect.vTop));        // top-left

                if (fixedWidth) {
                    x += fontWidth;
                } else {
                    x += cRect.w;
                }

                if (x > maxX) {
                    maxX = x;
                }
            }

            mWidth = maxX;
            mHeight = -(y - fontHeight);
        }
    }
}

void TextBatch::SetText(const Font* font, const std::vector<std::string>& textLines)
{
    Clear();

    if (font && font->IsLoaded()) {
        mFont = font;

        float x = 0;
        float y = 0;

        float maxX = 0;  // used to figure out width

        // line spacing
        float fontHeight = font->getHeight();
        //float lineSpacing = 0.1f * fontHeight;
        float lineSpacing = 0;
        float dy = fontHeight + lineSpacing;

        for (unsigned i = 0; i < textLines.size(); i++) {

            if (i > 0) {
                y -= dy;
            }

            const std::string& text = textLines[i];

            for (char c : text) {

                if (c == '\n') {

                    x = 0;
                    y -= dy;

                } else if (font->hasChar(c)) {
                    const TexRect& cRect = font->getCharRect(c);

                    mVerts.push_back(VPT(x, y, 0,                       cRect.uLeft, cRect.vTop));        // top-left
                    mVerts.push_back(VPT(x, y - cRect.h, 0,             cRect.uLeft, cRect.vBottom));     // bottom-left
                    mVerts.push_back(VPT(x + cRect.w, y - cRect.h, 0,   cRect.uRight, cRect.vBottom));    // bottom-right

                    mVerts.push_back(VPT(x + cRect.w, y - cRect.h, 0,   cRect.uRight, cRect.vBottom));    // bottom-right
                    mVerts.push_back(VPT(x + cRect.w, y, 0,             cRect.uRight, cRect.vTop));       // top-right
                    mVerts.push_back(VPT(x, y, 0,                       cRect.uLeft, cRect.vTop));        // top-left

                    x += cRect.w;

                    if (x > maxX) {
                        maxX = x;
                    }
                }

                mWidth = maxX;
                mHeight = -(y - fontHeight);
            }
        }
    }
}

void TextBatch::DrawGeometry() const
{
    glBindTexture( GL_TEXTURE_2D, mFont->getTex() );
    ::glsh::DrawGeometry(GL_TRIANGLES, mVerts);

    // FUTURE: could implement a retained mode
}

void TextBatch::SetPosition( const glm::vec2& pos ) {
	modelMatrix[3][0] = pos.x;
	modelMatrix[3][1] = pos.y;
}

void TextBatch::SetScale( float scale ) {
	modelMatrix[0][0] = modelMatrix[1][1] = modelMatrix[2][2] = scale;
}


} // end of namespace

#include "TextureManager.h"

TextureManager::TextureManager(const std::string& rootDir)
{
    if (rootDir.empty()) {
        mRootDir = "./";
    } else {
        mRootDir = rootDir;
        char last = mRootDir[mRootDir.size() - 1];
        if (last != '/' && last != '\\') {
            mRootDir += '/';
        }
    }
}

TextureManager::~TextureManager()
{
    // FIXME: cleanup!
}

GLuint TextureManager::GetTexture(const std::string& fname)
{
    std::string path = mRootDir + fname;

    std::map<std::string, GLuint>::iterator it = mTextures.find(path);
    if (it != mTextures.end()) {
        return it->second;
    } else {
        GLuint tex = glsh::CreateTexture2D(path, true);
        mTextures[path] = tex;
        return tex;
    }
}

#ifndef TEXTURE_MANAGER_H_
#define TEXTURE_MANAGER_H_

#include "GLSH_Texture.h"

#include <map>
#include <string>

class TextureManager {

    std::string                     mRootDir;
    std::map<std::string, GLuint>   mTextures;

public:

                                    TextureManager(const std::string& rootDir);
                                    ~TextureManager();
                                    
    GLuint                          GetTexture(const std::string& fname);
};

#endif

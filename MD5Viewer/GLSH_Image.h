#ifndef GLSH_IMAGE_H_
#define GLSH_IMAGE_H_

#include <vector>
#include <string>

namespace glsh {

// a forward declaration
struct TargaHeader;


class Image {
private:
    int                     mWidth, mHeight;
    int                     mBytesPerPixel;
    unsigned char*          mData;

public:
                            Image();
                            ~Image();

    bool                    Allocate(int width, int height, int bytesPerPixel);
    void                    Deallocate();

    bool                    isGood() const              { return mData != 0; }
    int                     getWidth() const            { return mWidth; }
    int                     getHeight() const           { return mHeight; }
    int                     getBytesPerPixel() const    { return mBytesPerPixel; }
    int                     getBitsPerPixel() const     { return 8 * mBytesPerPixel; }
    const unsigned char*    getData() const             { return mData; }
    unsigned char*          getData()                   { return mData; }

    bool                    LoadTarga(const std::string& path);

private:

    //
    // helper methods for loading TGA images
    //
    void                    LoadTargaUncompressed(const TargaHeader* hdr, const unsigned char* imgData);
    void                    LoadTargaRLE(const TargaHeader* hdr, const unsigned char* imgData);

    //
    // stuff needed for mipmapping
    //

    struct Mipmap {
        int             width, height;
        unsigned char*  data;

        Mipmap()
            : width(0)
            , height(0)
            , data(NULL)
        { }

        Mipmap(int width, int height, unsigned char* data)
            : width(width)
            , height(height)
            , data(data)
        { }
    };

    std::vector<Mipmap>     mMipmaps;

public:

    bool                    GenerateMipmaps(int minSize);

    int                     numMipmaps() const                  { return (int)mMipmaps.size(); }

    int                     getMipmapWidth(int level) const     { return mMipmaps[level].width; }
    int                     getMipmapHeight(int level) const    { return mMipmaps[level].height; }
    const unsigned char*    getMipmapData(int level) const      { return mMipmaps[level].data; }
    unsigned char*          getMipmapData(int level)            { return mMipmaps[level].data; }
};

} // end of namespace

#endif

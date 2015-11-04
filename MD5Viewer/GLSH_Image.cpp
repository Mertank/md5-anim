#include "GLSH_Image.h"
#include "GLSH_Util.h"

#include <iostream>
#include <fstream>

namespace glsh {

enum TargaFileType {
    TARGA_RGB               = 2,
    TARGA_GRAYSCALE         = 3,
    TARGA_RLE_RGB           = 10,
    TARGA_RLE_GRAYSCALE     = 11
};

struct TargaHeader {
    unsigned char idLength;
    unsigned char colorMapType;
    unsigned char imageTypeCode;
    unsigned char colorMapSpec[5];
    unsigned short xOrigin;
    unsigned short yOrigin;
    unsigned short width;
    unsigned short height;
    unsigned char bpp;
    unsigned char imageDesc;
};


Image::Image()
    : mWidth(0)
    , mHeight(0)
    , mBytesPerPixel(0)
    , mData(NULL)
{
}

Image::~Image()
{
    Deallocate();
}

bool Image::Allocate(int width, int height, int bytesPerPixel)
{
    // make sure to delete old memory if reallocating
    Deallocate();

    // allocate new memory for the image
    mData = new unsigned char[width * height * bytesPerPixel];
    mWidth = width;
    mHeight = height;
    mBytesPerPixel = bytesPerPixel;

    // create level 0 mipmap
    mMipmaps.push_back(Mipmap(mWidth, mHeight, mData));

    return true;
}

void Image::Deallocate()
{
    delete [] mData;
    mData = NULL;
    mWidth = 0;
    mHeight = 0;
    mBytesPerPixel = 0;

    // delete mipmaps
    for (unsigned i = 1; i < mMipmaps.size(); i++) {
        delete mMipmaps[i].data;
    }
    mMipmaps.clear();
}

bool Image::LoadTarga(const std::string& path)
{
    // open the file in binary mode
    std::ifstream file(path.c_str(), std::ios::binary);

    // make sure the file opened correctly
    if (!file.good()) {
        std::cerr << "*** Failed to open file '" << path << "'" << std::endl;
        return false;
    }

    // get the file size
    file.seekg(0, std::ios::end);          // seek to the end of the file
    size_t len = (size_t)file.tellg();     // get the file pointer offset in bytes
    file.seekg(0, std::ios::beg);          // rewind back to the beginning

    // allocate memory for the file contents
    unsigned char* buf = new unsigned char[len];

    // read entire file into memory
    file.read((char*)buf, len);
    if (!file.good()) {
        std::cerr << "*** Failed to read file '" << path << "'" << std::endl;
        delete [] buf;  // avoid a leak
        return false;
    }

    // the header is at the beginning of the file contents; use a cast to reinterpret that chunk of memory
    TargaHeader* hdr = reinterpret_cast<TargaHeader*>(buf);

    //std::cout << "Loading '" << path << "': " << hdr->width << "x" << hdr->height << ", " << (unsigned)hdr->bpp << " bpp" << std::endl;

    // check if the image type is supported
    switch (hdr->imageTypeCode) {
    case TARGA_RGB:
    case TARGA_GRAYSCALE:
    case TARGA_RLE_RGB:
    case TARGA_RLE_GRAYSCALE:
        // ok, these are supported types
        break;
    default:
        // anything else (like indexed formats) is unsupported
        std::cerr << "*** Unsuported TGA format" << std::endl;
        delete [] buf;  // avoid a leak
        return false;
    }

    // bit 4 of image descriptor indicates right-to-left pixel ordering, which we don't support
    if (hdr->imageDesc & 0x10) {
        std::cerr << "*** Oopsy doodle, right-to-left TGA files are not supported" << std::endl;
        delete [] buf;  // avoid a leak
        return false;
    }

    // allocate memory for the image data
    if (!Allocate(hdr->width, hdr->height, hdr->bpp / 8)) {
        std::cerr << "*** Failed to allocate memory for image" << std::endl;
        delete [] buf;  // avoid a leak
        return false;
    }

    // jump to the start of the image data (skip past header and optional variable-length id field)
    unsigned char* imgData = buf + sizeof(TargaHeader) + hdr->idLength;

    // decide how to load the image depending on type
    switch (hdr->imageTypeCode) {
    case TARGA_RGB:
    case TARGA_GRAYSCALE:
        // load an uncompressed image
        LoadTargaUncompressed(hdr, imgData);
        break;
    case TARGA_RLE_RGB:
    case TARGA_RLE_GRAYSCALE:
        // load RLE-compressed image
        LoadTargaRLE(hdr, imgData);
        break;
    default:
        // we should never get here
        std::cerr << "*** Oops, don't know how to load this format: fire the programmer" << std::endl;
        delete [] buf;
        Deallocate();
        return false;
    }

    // done loading, so deallocate the file contents
    delete [] buf;

    // all good, yay
    return true;
}

void Image::LoadTargaUncompressed(const TargaHeader* hdr, const unsigned char* imgData)
{
    int rowlen = (hdr->bpp / 8) * hdr->width;  // bytes per row
    int rowstep;
    unsigned char* dstRow;
    // check bit 5 of image descriptor to determine row ordering
    if (hdr->imageDesc & 0x20) {
        // bottom-to-top
        rowstep = -rowlen;
        dstRow = mData + rowlen * (hdr->height - 1);
    } else {
        // top-to-bottom
        rowstep = rowlen;
        dstRow = mData;
    }

    switch (hdr->bpp) {
    case 24:
        //std::cout << "~~ Loading 24 bpp" << std::endl;
        for (unsigned short j = 0; j < hdr->height; j++) {
            unsigned char* p = dstRow;
            for (unsigned short i = 0; i < hdr->width; i++) {
                char b = *imgData++;
                char g = *imgData++;
                char r = *imgData++;
                *p++ = r;
                *p++ = g;
                *p++ = b;
            }
            dstRow += rowstep;
        }
        break;

    case 32:
        //std::cout << "~~ Loading 32 bpp" << std::endl;
        for (unsigned short j = 0; j < hdr->height; j++) {
            unsigned char* p = dstRow;
            for (unsigned short i = 0; i < hdr->width; i++) {
                char b = *imgData++;
                char g = *imgData++;
                char r = *imgData++;
                char a = *imgData++;
                *p++ = r;
                *p++ = g;
                *p++ = b;
                *p++ = a;
            }
            dstRow += rowstep;
        }
        break;

    case 8:
        //std::cout << "~~ Loading 8 bpp" << std::endl;
        for (unsigned short j = 0; j < hdr->height; j++) {
            unsigned char* p = dstRow;
            for (unsigned short i = 0; i < hdr->width; i++) {
                *p++ = *imgData++;
            }
            dstRow += rowstep;
        }
        break;
    }
}

void Image::LoadTargaRLE(const TargaHeader* hdr, const unsigned char* imgData)
{
    int rowlen = (hdr->bpp / 8) * hdr->width;  // bytes per row
    int rowstep;
    unsigned char* dstRow;
    // check bit 5 of image descriptor to determine row ordering
    if (hdr->imageDesc & 0x20) {
        // bottom-to-top
        rowstep = -rowlen;
        dstRow = mData + rowlen * (hdr->height - 1);
    } else {
        // top-to-bottom
        rowstep = rowlen;
        dstRow = mData;
    }

    const unsigned numPixels = hdr->width * hdr->height;
    unsigned numPixelsRead = 0;
    unsigned short numPixelsInRow = 0;
    unsigned char* p = dstRow;

    switch (hdr->bpp) {
    case 24:
        //std::cout << "~~ Loading 24 bpp, RLE" << std::endl;
        while (numPixelsRead < numPixels) {
            unsigned char count = (unsigned char)*imgData++;
            if (count > 127) {
                // RLE packet
                count -= 127;
                char b = *imgData++;
                char g = *imgData++;
                char r = *imgData++;
                for (unsigned char i = 0; i < count; i++) {
                    *p++ = r;
                    *p++ = g;
                    *p++ = b;
                    if (++numPixelsInRow == hdr->width) {
                        // advance to next row
                        dstRow += rowstep;
                        p = dstRow;
                        numPixelsInRow = 0;
                    }
                }
            } else {
                // raw packet
                ++count;
                for (unsigned char i = 0; i < count; i++) {
                    char b = *imgData++;
                    char g = *imgData++;
                    char r = *imgData++;
                    *p++ = r;
                    *p++ = g;
                    *p++ = b;
                    if (++numPixelsInRow == hdr->width) {
                        // advance to next row
                        dstRow += rowstep;
                        p = dstRow;
                        numPixelsInRow = 0;
                    }
                }
            }
            numPixelsRead += count;
        }
        break;

    case 32:
        //std::cout << "~~ Loading 24 bpp, RLE" << std::endl;
        while (numPixelsRead < numPixels) {
            unsigned char count = (unsigned char)*imgData++;
            if (count > 127) {
                // RLE packet
                count -= 127;
                char b = *imgData++;
                char g = *imgData++;
                char r = *imgData++;
                char a = *imgData++;
                for (unsigned char i = 0; i < count; i++) {
                    *p++ = r;
                    *p++ = g;
                    *p++ = b;
                    *p++ = a;
                    if (++numPixelsInRow == hdr->width) {
                        // advance to next row
                        dstRow += rowstep;
                        p = dstRow;
                        numPixelsInRow = 0;
                    }
                }
            } else {
                // raw packet
                ++count;
                for (unsigned char i = 0; i < count; i++) {
                    char b = *imgData++;
                    char g = *imgData++;
                    char r = *imgData++;
                    char a = *imgData++;
                    *p++ = r;
                    *p++ = g;
                    *p++ = b;
                    *p++ = a;
                    if (++numPixelsInRow == hdr->width) {
                        // advance to next row
                        dstRow += rowstep;
                        p = dstRow;
                        numPixelsInRow = 0;
                    }
                }
            }
            numPixelsRead += count;
        }
        break;

    case 8:
        //std::cout << "~~ Loading 8 bpp, RLE" << std::endl;
        while (numPixelsRead < numPixels) {
            unsigned char count = (unsigned char)*imgData++;
            if (count > 127) {
                // RLE packet
                count -= 127;
                char val = *imgData++;
                for (unsigned char i = 0; i < count; i++) {
                    *p++ = val;
                    if (++numPixelsInRow == hdr->width) {
                        // advance to next row
                        dstRow += rowstep;
                        p = dstRow;
                        numPixelsInRow = 0;
                    }
                }
            } else {
                // raw packet
                ++count;
                for (unsigned char i = 0; i < count; i++) {
                    *p++ = *imgData++;
                    if (++numPixelsInRow == hdr->width) {
                        // advance to next row
                        dstRow += rowstep;
                        p = dstRow;
                        numPixelsInRow = 0;
                    }
                }
            }
            numPixelsRead += count;
        }
        break;
    }
}


bool Image::GenerateMipmaps(int minSize)
{
    // there must be an image loaded
    if (!isGood()) {
        return false;
    }

    // dimensions must be powers of two, otherwise we need a more sophisticated method
    if (!IsPowerOf2(mWidth) || !IsPowerOf2(mHeight)) {
        return false;
    }

    // sanity check
    if (minSize <= 0) {
        minSize = 1;
    }

    const Mipmap& prev = mMipmaps.back();
    int prevWidth = prev.width;
    int prevHeight = prev.height;
    int dstWidth = prevWidth >> 1;
    int dstHeight = prevHeight >> 1;

    const unsigned char* srcData = prev.data;
    int srcStride = prev.width * mBytesPerPixel;

    while (dstWidth >= minSize && dstHeight >= minSize) {

        unsigned char* dstData = new unsigned char [dstWidth * dstHeight * mBytesPerPixel];
        unsigned char* q = dstData;
        int dstStride = dstWidth * mBytesPerPixel;

        //int n = dstWidth * dstHeight * mBytesPerPixel;

        for (int j = 0; j < dstHeight; j++) {
            //unsigned char* dstRow = dstData + j * dstStride;

            const unsigned char* p1 = srcData + 2 * j * srcStride;
            const unsigned char* p2 = p1 + srcStride;

            for (int i = 0; i < dstStride; i++) {
                unsigned a = (unsigned)*p1 + (unsigned)*(p1 + mBytesPerPixel);
                unsigned b = (unsigned)*p2 + (unsigned)*(p2 + mBytesPerPixel);
                p1 += 2 * mBytesPerPixel;
                p2 += 2 * mBytesPerPixel;

                *q++ = (a + b) / 4;
            }
        }

        mMipmaps.push_back(Mipmap(dstWidth, dstHeight, dstData));

        prevWidth = dstWidth;
        prevHeight = dstHeight;
        dstWidth >>= 1;
        dstHeight >>= 1;
    }

    return true;
}

} // end of namespace

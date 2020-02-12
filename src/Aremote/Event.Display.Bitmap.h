
#pragma once

namespace Event
{
    namespace ABitmap
    {
        typedef struct __attribute__((__packed__))
        {
            uint16_t  bfType;
            uint32_t  bfSize;
            uint32_t  bfReserved; // 1,2
            uint32_t bfOffBits;
        } BITMAPFILEHEADER;

        typedef struct __attribute__((__packed__))
        {
            uint32_t biSize;
            long     biWidth;
            long     biHeight;
            uint16_t biPlanes;
            uint16_t biBitCount;
            uint32_t biCompression;
            uint32_t biSizeImage;
            long     biXPelsPerMeter;
            long     biYPelsPerMeter;
            uint32_t biClrUsed;
            uint32_t biClrImportant;
        } BITMAPINFOHEADER;

        typedef struct
        {
            ABitmap::BITMAPFILEHEADER fh;
            ABitmap::BITMAPINFOHEADER ih;
        } BMPHEADER;
    };
};

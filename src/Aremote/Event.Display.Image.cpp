
#define _DEBUG_IMAGEPACK_STAT 1
#include "ARemote.h"
#include "Event.Display.Bitmap.h"
#include "Event.Display.Debug.h"
#include "extern/lz4/lz4.h"

#define BMZID 0x315a4d42

namespace Event
{
    //
    enum pixelsConvertEnum
    {
        PIX_FMT16_BE,
        PIX_FMT16_LE,
        PIX_FMT24_BE,
        PIX_FMT24_LE,
        PIX_FMT32_BE,
        PIX_FMT32_LE,
        PIX_FMT32_SDL,
        PIX_FMT_ERROR
    };
    static pixelsConvertEnum pixelsConvertMatrix
                [ADisplay::ImgTypeFmt::F_FMT_TYPE_NONE + 1][ADisplay::ImgTypeOut::T_IMG_TYPE_NONE + 1] =
    {
        /*             T_RAW      ,    T_BMP    ,    T_BMZ    ,    T_SDL    ,    T_JPG   ,    T_PNG     , T_RAW_NOPAD,   Error     */
        /* 16 */    { PIX_FMT16_BE, PIX_FMT16_BE, PIX_FMT16_BE, PIX_FMT16_LE, PIX_FMT16_LE, PIX_FMT16_LE, PIX_FMT16_LE, PIX_FMT_ERROR },
        /* 24 */    { PIX_FMT24_BE, PIX_FMT24_BE, PIX_FMT24_BE, PIX_FMT24_LE, PIX_FMT24_LE, PIX_FMT24_LE, PIX_FMT24_LE, PIX_FMT_ERROR },
        /* 32 */    { PIX_FMT32_BE, PIX_FMT32_BE, PIX_FMT32_BE, PIX_FMT32_SDL,PIX_FMT32_LE, PIX_FMT32_LE, PIX_FMT32_LE, PIX_FMT_ERROR },
        /* Error */ { PIX_FMT_ERROR,PIX_FMT_ERROR,PIX_FMT_ERROR,PIX_FMT_ERROR,PIX_FMT_ERROR,PIX_FMT_ERROR,PIX_FMT_ERROR,PIX_FMT_ERROR }
    };

    void ADisplay::imageCapture(std::vector<uint8_t> & vout, ImgTypeOut t)
    {
#       if defined(_BUILD_STRICT)
        assert(m_dspdata.size() > 0);
        assert(t != ImgTypeOut::T_IMG_TYPE_NONE);
#       else
        if (__builtin_expect(!!(!m_dspdata.size()), 0))
            throw std::system_error(App::make_error_code(App::ErrorId::error_display_image_buffer_size));
        if (__builtin_expect(!!(t == ImgTypeOut::T_IMG_TYPE_NONE), 0))
            throw std::system_error(App::make_error_code(App::ErrorId::error_display_image_out_type));
#       endif

        CLKMARK(_t0);
        // DeadLock watcher
        {
            uint32_t cnt = 0U;
            do
            {
                if (cnt > 250)
                    return;

                HClockMark(c_tcond);
                Helper::Clock::TimeWatch w_tcond;
                {
                    std::shared_lock<std::shared_mutex> l_lock(m_lock);
                    w_tcond = m_tcond;
                }
                if (HClockDiff(c_tcond, w_tcond) < 700)
                    break;

                std::this_thread::yield();
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                cnt++;
            }
            while (true);
        }

        pixelsConvertEnum pixfmt = pixelsConvertMatrix[Conf.display.fmt][t];
#       if defined(_BUILD_STRICT)
        assert(pixfmt != PIX_FMT_ERROR);
#       else
        if (__builtin_expect(!!(pixfmt == PIX_FMT_ERROR), 0))
            throw std::system_error(App::make_error_code(App::ErrorId::error_display_image_pixel_format));
#       endif
        uint32_t ldata[eraw::I_DATA_END];

        switch (t)
        {
            case ImgTypeOut::T_RAW:
            case ImgTypeOut::T_BMP:
            case ImgTypeOut::T_BMZ:
            case ImgTypeOut::T_SDL:
                {
                    ldata[eraw::I_PAD] = (Conf.display.w % 4);
                    break;
                }
            case ImgTypeOut::T_PNG:
            case ImgTypeOut::T_JPG:
            case ImgTypeOut::T_RAW_NOPAD:
                {
                    ldata[eraw::I_PAD] = 0U;
                    break;
                }
            default:
                break;
        }
        ldata[eraw::I_X]        = Conf.display.w;
        ldata[eraw::I_Y]        = Conf.display.h;
        ldata[eraw::I_BPP]      = Conf.display.b;
        ldata[eraw::I_STRIDE]   = ((Conf.display.s > Conf.display.w) ?
                                    (Conf.display.s - Conf.display.w) : 0U);
        ldata[eraw::I_SZSRC]    = ((ldata[eraw::I_X] + ldata[eraw::I_STRIDE]) * (ldata[eraw::I_Y] - 1));
        ldata[eraw::I_SZDST]    = (((ldata[eraw::I_X] * ldata[eraw::I_Y]) * 3) + (ldata[eraw::I_PAD] * ldata[eraw::I_Y]));

        std::vector<uint8_t> vtmp(ldata[eraw::I_SZDST]);
        CLKMARK(_t1);
        {
            // locked capture block
            std::shared_lock<std::shared_mutex> l_lock(m_lock);
            //
            uint32_t y;
            __builtin_prefetch(ldata, 0, 1);
#           pragma omp parallel for private(y) schedule(dynamic) shared(ldata)
            for (y = 0U; y < ldata[eraw::I_Y]; y++)
            {
                uint32_t xpos[eraw::POS_IMAGE_END];
                xpos[eraw::POS_OFFSET_SRC] = ((ldata[eraw::I_X] + ldata[eraw::I_STRIDE]) * y);
                xpos[eraw::POS_OFFSET_DST] = (ldata[eraw::I_X] * y);
                xpos[eraw::POS_OUTPAD]     = (ldata[eraw::I_PAD] * y);

                __builtin_prefetch(xpos, 0, 1);
                //
                for (uint32_t x = 0U; x < ldata[eraw::I_X]; x++)
                {
                    uint32_t ipos[eraw::POS_SIMD_END];
                    switch (t)
                    {
                        // reverse padding % 4 RGB buffer
                        case ImgTypeOut::T_RAW:
                        case ImgTypeOut::T_BMP:
                            {
                                ipos[eraw::POS_OFFSET_SRC] =
                                    (((ldata[eraw::I_SZSRC] - xpos[eraw::POS_OFFSET_SRC]) + x) * ldata[eraw::I_BPP]);

                                ipos[eraw::POS_OFFSET_DST] =
                                    (((xpos[eraw::POS_OFFSET_DST] + x) * 3) + xpos[eraw::POS_OUTPAD]);
                                break;
                            }
                        // padding % 4 RGB buffer
                        case ImgTypeOut::T_BMZ:
                        case ImgTypeOut::T_SDL:
                            {
                                ipos[eraw::POS_OFFSET_SRC] =
                                    ((xpos[eraw::POS_OFFSET_SRC] + x) * ldata[eraw::I_BPP]);
                                ipos[eraw::POS_OFFSET_DST] =
                                    (((xpos[eraw::POS_OFFSET_DST] + x) * 3) + xpos[eraw::POS_OUTPAD]);
                                break;
                            }
                        // not padding RGB buffer
                        case ImgTypeOut::T_PNG:
                        case ImgTypeOut::T_JPG:
                        case ImgTypeOut::T_RAW_NOPAD:
                            {
                                ipos[eraw::POS_OFFSET_SRC] =
                                    ((xpos[eraw::POS_OFFSET_SRC] + x) * ldata[eraw::I_BPP]);
                                ipos[eraw::POS_OFFSET_DST] =
                                    ((xpos[eraw::POS_OFFSET_DST] + x) * 3);
                                break;
                            }
                        default:
                            break;
                    }
                    //
                    ADisplay::RGBDATA *rgb_d =
                                reinterpret_cast<ADisplay::RGBDATA*>(&vtmp[ipos[eraw::POS_OFFSET_DST]]);
                    //
                    switch (pixfmt)
                    {
                        case pixelsConvertEnum::PIX_FMT16_BE:
                            {
                                uint16_t *pixel16 = reinterpret_cast<uint16_t*>(
                                                        &m_dspdata[ipos[eraw::POS_OFFSET_SRC]]
                                                    );
                                rgb_d->b = (255 * (*(pixel16) & 0x001F))/32;
                                rgb_d->g = (255 * ((*(pixel16) & 0x07E0) >> 5))/64;
                                rgb_d->r = (255 * ((*(pixel16) & 0xF800) >> 11))/32;
                                break;
                            }
                        case pixelsConvertEnum::PIX_FMT16_LE:
                            {
                                uint16_t *pixel16 = reinterpret_cast<uint16_t*>(
                                                        &m_dspdata[ipos[eraw::POS_OFFSET_SRC]]
                                                    );
                                rgb_d->b = (255 * ((*(pixel16) & 0xF800) >> 11))/32;
                                rgb_d->g = (255 * ((*(pixel16) & 0x07E0) >> 5))/64;
                                rgb_d->r = (255 *  (*(pixel16) & 0x001F))/32;
                                break;
                            }
                        case pixelsConvertEnum::PIX_FMT24_BE:
                            {
                                ADisplay::RGBDATA *rgb_s =
                                    reinterpret_cast<ADisplay::RGBDATA*>(&m_dspdata[ipos[eraw::POS_OFFSET_SRC]]);
                                rgb_d->b = rgb_s->r;
                                rgb_d->g = rgb_s->g;
                                rgb_d->r = rgb_s->b;
                                break;
                            }
                        case pixelsConvertEnum::PIX_FMT24_LE:
                            {
                                std::memcpy(rgb_d, &m_dspdata[ipos[eraw::POS_OFFSET_SRC]], sizeof(ADisplay::RGBDATA));
                                break;
                            }
                        case pixelsConvertEnum::PIX_FMT32_BE:
                            {
                                uint32_t *pixel32 = reinterpret_cast<uint32_t*>(
                                                        &m_dspdata[ipos[eraw::POS_OFFSET_SRC]]
                                                    );
                                rgb_d->b =  *(pixel32) & 0x000000FF;
                                rgb_d->g = (*(pixel32) & 0x0000FF00) >> 8;
                                rgb_d->r = (*(pixel32) & 0x00FF0000) >> 16;
                                break;
                            }
                        case pixelsConvertEnum::PIX_FMT32_LE:
                            {
                                uint32_t *pixel32 = reinterpret_cast<uint32_t*>(
                                                        &m_dspdata[ipos[eraw::POS_OFFSET_SRC]]
                                                    );
                                rgb_d->b = (*(pixel32) & 0x00FF0000) >> 16;
                                rgb_d->g = (*(pixel32) & 0x0000FF00) >> 8;
                                rgb_d->r =  *(pixel32) & 0x000000FF;
                                break;
                            }
                        case pixelsConvertEnum::PIX_FMT32_SDL:
                            {
                                uint32_t *pixel32 = reinterpret_cast<uint32_t*>(
                                                        &m_dspdata[ipos[eraw::POS_OFFSET_SRC]]
                                                    );
                                rgb_d->b = (*(pixel32) & 0xFF000000) >> 24;
                                rgb_d->g = (*(pixel32) & 0x00FF0000) >> 16;
                                rgb_d->r = (*(pixel32) & 0xFF000000) >> 24;
                                break;
                            }
                        default:
                            break;
                    }
                    //
                    __builtin_prefetch(xpos, 0, 1);
                }
                __builtin_prefetch(ldata, 0, 1);
            }
        }
        //
        CLKMARK(_t2);

        switch (t)
        {
            case ImgTypeOut::T_RAW:
            case ImgTypeOut::T_SDL:
            case ImgTypeOut::T_RAW_NOPAD:
                {
                    vout = move(vtmp);
                    break;
                }
            case ImgTypeOut::T_BMP:
                {
                    vout = move(vtmp);
                    imageBmpHeader(vout);
                    break;
                }
            case ImgTypeOut::T_BMZ:
                {
                    if (!imageLz4(vtmp, vout))
                        throw std::system_error(App::make_error_code(App::ErrorId::error_display_image_create_bmz));
                    break;
                }
            case ImgTypeOut::T_PNG:
                {
                    uint32_t err = lodepng::encode(
                            vout,
                            vtmp,
                            ldata[eraw::I_X],
                            ldata[eraw::I_Y],
                            LCT_RGB,
                            8
                        );
                    if (err)
                        throw std::system_error(App::make_error_code(App::ErrorId::error_display_lodepng), lodepng_error_text(err));
                    break;
                }
            case ImgTypeOut::T_JPG:
                {
                    if (!TooJpeg::writeJpeg(
                            vtmp,
                            ldata[eraw::I_X],
                            ldata[eraw::I_Y],
                            vout,
                            TooJpeg::inputType::IMGTYPE_RGB,
                            90))
                        throw std::system_error(App::make_error_code(App::ErrorId::error_display_buffer_jpeg));
                    break;
                }
            default:
                break;
        }

        CLKMARK(_t3);

#       if defined(_DEBUG_IMAGEPACK_STAT)
        LOGDC("\t-> Out Image: ", ldata[eraw::I_X],
              "x",                ldata[eraw::I_Y],
              ", out type: ",     t, Event::ADisplay::getImgTypeString(t),
              ", origin size: ",  m_dspdata.size(),
              ", out size: ",     vout.size()
            );
        LOGDC("\t-> Begin-End: ", CLKDIFF(_t3, _t0),
              ", Start-End: ",    CLKDIFF(_t3, _t1),
              ", Repack: ",       CLKDIFF(_t2, _t1),
              ", Convert to: ",   CLKDIFF(_t3, _t2)
            );
#       endif
#       if defined(_BUILD_IMAGE_FILE_WRITE)
        {
            std::string s("/data/local/tmp/image-convert.");
            s.append(Event::ADisplay::getImgTypeString(t).c_str());
            FILE *fp = ::fopen(s.c_str(), "wb");
            if (!fp)
                throw std::system_error(App::make_error_code(App::ErrorId::error_display_open_file), s.c_str());
            ::fwrite(&vout[0], 1, vout.size(), fp);
            ::fclose(fp);
        }
#       endif
    }

    void ADisplay::imageBmpHeader(std::vector<uint8_t> & v)
    {
        Event::ABitmap::BMPHEADER bmph{};

        bmph.fh.bfType        = 0x4D42; // "BM"
        bmph.fh.bfSize        = (v.size() + sizeof(bmph));
        bmph.fh.bfOffBits     = sizeof(bmph);
        bmph.ih.biSize        = sizeof(bmph.ih);
        bmph.ih.biWidth       = static_cast<int32_t>(Conf.display.w);
        bmph.ih.biHeight      = static_cast<int32_t>(Conf.display.h);
        bmph.ih.biPlanes      = 1U;
        bmph.ih.biBitCount    = 24; //(b * 8);
        bmph.ih.biCompression = 0x0000; // BI_RGB
        bmph.ih.biSizeImage   = 0;

        uint8_t *obj = reinterpret_cast<uint8_t*>(&bmph);
        v.resize(bmph.fh.bfSize);
        v.insert(v.begin(), obj, obj + sizeof(bmph));
    }

    bool ADisplay::imageLz4(std::vector<uint8_t> const & vin, std::vector<uint8_t> & vout)
    {
        do
        {
            uint32_t sz = vin.size();
            uint32_t hdr[] = {
                    BMZID,
                    sz,
                    0U,
                    Conf.display.w,
                    Conf.display.h
                };

            int32_t lzrsz,
                    invsz = static_cast<int32_t>(sz),
                    lzbsz = ::LZ4_compressBound(invsz);

            if (!lzbsz)
                break;

            vout.resize(lzbsz + sizeof(hdr));

            lzrsz = ::LZ4_compress_fast(
                        reinterpret_cast<const char*>(&vin[0]),
                        reinterpret_cast<char*>(&vout[sizeof(hdr)]),
                        invsz,
                        lzbsz,
                        6 // fast 0 - 9
                    );

            if (lzrsz <= 0)
                break;

            hdr[2] = static_cast<uint32_t>(lzrsz);
            ::memcpy(&vout[0], hdr, sizeof(hdr));
            return true;
        }
        while (0);
        return false;
    }
};

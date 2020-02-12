
#include "ARemote.h"
#include "Event.Display.Debug.h"

namespace Event
{
    //
    void ADisplay::mjpgStream(std::vector<uint8_t> & vout, StreamRawData & ird)
    {
#       if defined(_BUILD_STRICT)
        assert(ird.sizeIn == m_dspdata.size());
        assert(ird.idata[eraw::I_PAD] != 0);
#       else
        if (__builtin_expect(!!(ird.sizeIn != m_dspdata.size()), 0))
            throw DisplayNoFatal(
                    __LINE__,
                    __PRETTY_FUNCTION__,
                    App::geterror(App::ErrorId::error_display_notfatal_not_equal) // display rotate manually ?
                );
        if (__builtin_expect(!!(!ird.idata[eraw::I_PAD]), 0))
            throw DisplayNoFatal(
                    __LINE__,
                    __PRETTY_FUNCTION__,
                    App::geterror(App::ErrorId::error_display_notfatal_padding) // display change setting ?
                );
#       endif
        //
        std::vector<uint8_t> vtmp(ird.idata[eraw::I_SZDST]);
        CLKMARK(_t1);
        {
            // locked capture block
            std::shared_lock<std::shared_mutex> l_lock(m_lock);
            //
            uint32_t y;
            const uint32_t *ldata = ird.idata;
            __builtin_prefetch(ldata, 0, 1);
#           pragma omp parallel for private(y) schedule(dynamic) shared(ldata)
            for (y = 0U; y < ird.y; y += ldata[eraw::I_PAD])
            {
                uint32_t xs_,
                         xd_,
                         yd_ = (y / ldata[eraw::I_PAD]),
                         xpos[eraw::POS_STREAM_END];

                __builtin_prefetch(xpos, 1, 1);
                //
                switch (ldata[eraw::I_ROTATE])
                {
                    case Orient::O_0: // (OK)
                        xpos[eraw::POS_OFFSET_SRC] = ((ird.x + ldata[eraw::I_STRIDE]) * y);
                        xpos[eraw::POS_OFFSET_DST] = (ldata[eraw::I_X] * yd_);
                        break;
                    case Orient::O_90: // (OK)
                        xpos[eraw::POS_OFFSET_SRC] = ((ird.x + ldata[eraw::I_STRIDE]) * y);
                        xpos[eraw::POS_OFFSET_DST] = (ldata[eraw::I_Y] - 1 - yd_);
                        break;
                    case Orient::O_180: // (OK)
                        xpos[eraw::POS_OFFSET_SRC] = (((ird.x + ldata[eraw::I_STRIDE]) * (ird.y - 1 - y)) + ird.x - 1);
                        xpos[eraw::POS_OFFSET_DST] = (ldata[eraw::I_X] * yd_);
                        break;
                    case Orient::O_270: // (OK)
                        xpos[eraw::POS_OFFSET_SRC] = ((ird.x + ldata[eraw::I_STRIDE]) * y);
                        xpos[eraw::POS_OFFSET_DST] = 0;
                        break;
                    case Orient::O_MIRROR: // (OK)
                        xpos[eraw::POS_OFFSET_SRC] = ((ird.x + ldata[eraw::I_STRIDE]) * y);
                        xpos[eraw::POS_OFFSET_DST] = (ldata[eraw::I_X] * yd_);
                        break;
                    default:
                        throw std::system_error(App::make_error_code(App::ErrorId::error_display_not_rotate));
                }
                //
                for (xs_ = 0, xd_ = 0; xs_ < ird.x; xs_ += ldata[eraw::I_PAD], xd_++)
                {
                    switch (ldata[eraw::I_ROTATE])
                    {
                        case Orient::O_0: // (OK)
                            xpos[eraw::POS_INPUT] =
                                ((xpos[eraw::POS_OFFSET_SRC] + xs_) * ldata[eraw::I_BPP]);
                            xpos[eraw::POS_OUTPUT] =
                                ((xpos[eraw::POS_OFFSET_DST] + xd_) * 3);
                            break;
                        case Orient::O_90: // (OK)
                            xpos[eraw::POS_INPUT] =
                                ((xpos[eraw::POS_OFFSET_SRC] + xs_) * ldata[eraw::I_BPP]);
                            xpos[eraw::POS_OUTPUT] =
                                ((xpos[eraw::POS_OFFSET_DST] + (xd_ * ldata[eraw::I_Y])) * 3);
                            break;
                        case Orient::O_180: // (OK)
                            xpos[eraw::POS_INPUT] =
                                ((xpos[eraw::POS_OFFSET_SRC] - xs_) * ldata[eraw::I_BPP]);
                            xpos[eraw::POS_OUTPUT] =
                                ((xpos[eraw::POS_OFFSET_DST] + xd_) * 3);
                            break;
                        case Orient::O_270: // (OK)
                            xpos[eraw::POS_INPUT] =
                                ((xpos[eraw::POS_OFFSET_SRC] + xs_) * ldata[eraw::I_BPP]);
                            xpos[eraw::POS_OUTPUT] =
                                ((((ldata[eraw::I_X] - 1 - xd_) * ldata[eraw::I_Y]) + yd_) * 3);
                            break;
                        case Orient::O_MIRROR: // (OK)
                            xpos[eraw::POS_INPUT] =
                                ((xpos[eraw::POS_OFFSET_SRC] + xs_) * ldata[eraw::I_BPP]);
                            xpos[eraw::POS_OUTPUT] =
                                ((xpos[eraw::POS_OFFSET_DST] + (ldata[eraw::I_X] - xd_)) * 3);
                            break;
                    }

#                   if defined(_BUILD_STRICT)
                    assert(xpos[eraw::POS_INPUT]  < ird.sizeIn);
                    assert(xpos[eraw::POS_OUTPUT] < ldata[eraw::I_SZDST]);
#                   else
                    if (__builtin_expect(!!(xpos[eraw::POS_INPUT] >= ird.sizeIn), 0))
                        throw DisplayNoFatal(
                                __LINE__,
                                __PRETTY_FUNCTION__,
                                App::geterror(App::ErrorId::error_display_notfatal_input)
                            );
                    if (__builtin_expect(!!(xpos[eraw::POS_OUTPUT] >= ldata[eraw::I_SZDST]), 0))
                        throw DisplayNoFatal(
                                __LINE__,
                                __PRETTY_FUNCTION__,
                                App::geterror(App::ErrorId::error_display_notfatal_output)
                            );
#                   endif

                    __builtin_prefetch(&m_dspdata[xpos[eraw::POS_INPUT] + sizeof(ADisplay::RGBDATA)], 0, 1);
                    __builtin_prefetch(&vtmp[xpos[eraw::POS_OUTPUT] + sizeof(ADisplay::RGBDATA)], 0, 1);
                    //
                    ADisplay::RGBDATA *rgb = reinterpret_cast<ADisplay::RGBDATA*>(&m_dspdata[xpos[eraw::POS_INPUT]]);
                    ::memcpy(&vtmp[xpos[eraw::POS_OUTPUT]], rgb, sizeof(ADisplay::RGBDATA));
                    //
                    __builtin_prefetch(xpos, 1, 1);
                }
                __builtin_prefetch(ldata, 0, 1);
            }
        }
        //
        CLKMARK(_t2);

        uint32_t xy[2] = { ird.idata[eraw::I_X], ird.idata[eraw::I_Y] };

        if (__builtin_expect(!!(ird.orientSwap), 0))
            std::swap(xy[0], xy[1]);

        if (!TooJpeg::writeJpeg(
                vtmp,
                xy[0],
                xy[1],
                vout,
                TooJpeg::inputType::IMGTYPE_RGB,
                ird.qualityOut))
            throw std::system_error(App::make_error_code(App::ErrorId::error_display_buffer_jpeg));

        CLKMARK(_t3);

#       if defined(_BUILD_STREAMPACK_STAT)
        LOGDC("\t-> Out Image: ", xy[0],
              "x",                xy[1],
              ", size origin: ",  ird.sizeIn,
              ", size resample: ",ird.idata[eraw::I_SZDST],
              ", size jpeg: ",    vout.size()
            );
        LOGDC("\t-> Start-End: ", CLKDIFF(_t3, _t1),
              ", Repack: ",       CLKDIFF(_t2, _t1),
              ", ToJpg: ",        CLKDIFF(_t3, _t2)
            );
#       endif
#       if defined(_BUILD_STREAM_FILE_WRITE)
        {
            FILE *fp = ::fopen("/data/local/tmp/stream-convert.jpg", "wb");
            if (!fp)
                throw std::system_error(App::make_error_code(App::ErrorId::error_display_open_file), "/data/local/tmp/stream-convert.jpg");
            ::fwrite(&v[0], 1, v.size(), fp);
            ::fclose(fp);
        }
#       endif
    }
};


#include "ARemote.h"
#include <ui/DisplayInfo.h>
#include <ui/PixelFormat.h>
#include "Event.Display.Debug.h"
#include "Event.Display.API.h"

namespace Event
{
    [[ maybe_unused ]] static inline const char *ImgTypeString[] =
    {
        "raw",
        "bmp",
        "bmz",
        "sdl",
        "jpg",
        "png",
        "rnp", // RAW no pad (as JPG, PNG bytes buffer format)
        "-"
    };
    [[ maybe_unused ]] static inline const char *DisplayOrientString[] =
    {
        "ROTATE 0",
        "ROTATE 90",
        "ROTATE 180",
        "ROTATE 270",
        "MIRROR",
        "PORTRAIT",
        "LANDSCAPE",
        "-"
    };
    [[ maybe_unused ]] static inline const char *DisplayBitFormatString[] =
    {
        "16 bit",
        "24 bit",
        "32 bit",
        "ERROR"
    };
    //
    ADisplay::ADisplay()
    {
        acap_init();
        if ((m_dsp = android::SurfaceComposerClient::getBuiltInDisplay(
                        android::ISurfaceComposer::eDisplayIdMain
            )) == nullptr)
            throw std::system_error(App::make_error_code(App::ErrorId::error_display_android_builtin));

        getDisplay(false);

        m_run = true;
        std::thread thcap(
            [&]
            {
                do
                {
                    errno = 0;
                    acap_release();
                    //
                    while(!m_clientcount.load())
                    {
                        if (!m_run)
                            return;

                        std::this_thread::yield();
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    }

                    if (__builtin_expect(!!(acap_capture() != android::NO_ERROR), 0))
                        continue;

#                   if (__ANDROID_VER__ >= 9)
                    if (__builtin_expect(!!(m_sc == nullptr), 0))
                    {
                        m_run = false;
                        break;
                    }
                    uint8_t *vbdata = nullptr;
                    if (acap_lock(reinterpret_cast<void**>(&vbdata)) != android::NO_ERROR)
                        continue;
#                   else
                    const uint8_t *vbdata = static_cast<const uint8_t*>(acap_getPixels());
#                   endif
                    if (!vbdata)
                        continue;

                    std::size_t vbsz = acap_size();
                    {
                        std::lock_guard<std::shared_mutex> l_lock(m_lock);
                        //
                        if (__builtin_expect(!!(vbsz != Conf.display.sz), 0))
                        {
                            Conf.display.sz = vbsz;
                            m_dspdata.resize(vbsz);
                        }
                        m_dspdata.assign(vbdata, vbdata + vbsz);
                        m_tcond = HClockNow();
                    }
#                   if (_BUILD_CAPTURE_STAT)
                    LOGDC("\t-> clients: ", m_clientcount.load());
                    LOGDC("\t-> point  : ", acap_getWidth(), "x", acap_getHeight());
                    LOGDC("\t-> stride : ", acap_getStride());
                    LOGDC("\t-> format : ", acap_getFormat(), "/", android::bytesPerPixel(acap_getFormat()));
                    LOGDC("\t-> size   : ", acap_size());
#                   endif
#                   if (_BUILD_CAPTURE_FILE_WRITE)
                    {
                    FILE *fp = ::fopen("/data/local/tmp/capture-buffer.raw", "wb");
                    if (!fp)
                        throw std::system_error(App::make_error_code(App::ErrorId::error_display_open_file), "/data/local/tmp/capture-buffer.raw");
                    ::fwrite(vbdata, 1, vbsz, fp);
                    ::fclose(fp);
                    }
#                   endif
                }
                while (m_run);

            });
        if (thcap.joinable())
            m_thcap = move(thcap);

        Conf.class_display = this;
    }

    ADisplay::~ADisplay()
    {
        m_run = ((m_run.load()) ? false : m_run.load());
        if (m_thcap.joinable())
            m_thcap.join();
        acap_release();
        Conf.class_display = nullptr;
    }

    void ADisplay::clientcountUp()
    {
        m_clientcount += 1;
    }
    void ADisplay::clientcountDown()
    {
        if (m_clientcount.load())
            m_clientcount -= 1;
    }
    bool ADisplay::clientcountActive()
    {
        return static_cast<bool>(m_clientcount.load());
    }

    void ADisplay::getDisplay(bool b)
    {
        ADisplay::Orient o = getOrientation();

        if ((b) && (Conf.display.o == o))
            return;

        if (acap_capture() != android::NO_ERROR)
            throw std::system_error(App::make_error_code(App::ErrorId::error_display_android_capture));

        Conf.display.w     = acap_getWidth();
        Conf.display.h     = acap_getHeight();
        Conf.display.s     = acap_getStride();
        Conf.display.f     = acap_getFormat();
        Conf.display.b     = android::bytesPerPixel(Conf.display.f);
        Conf.display.fmt   = getAndroidFormat(Conf.display.f);
        Conf.display.o     = o;
        Conf.display.changexy = false;
        uint32_t dsz       = acap_size();
        //
        acap_release();
        //
#       if defined(_BUILD_STRICT)
        assert(dsz > 0);
        assert(Conf.display.fmt != ImgTypeFmt::F_FMT_TYPE_NONE);
#       else
        if (__builtin_expect(!!(!dsz), 0))
            throw std::system_error(App::make_error_code(App::ErrorId::error_display_not_size));
        if (__builtin_expect(!!(Conf.display.fmt == ImgTypeFmt::F_FMT_TYPE_NONE), 0))
            throw std::system_error(App::make_error_code(App::ErrorId::error_display_not_type));
#       endif


        if ((Conf.display.x.max) && (Conf.display.y.max))
        {
            int32_t r = (Conf.display.h - Conf.display.x.max);
            if ((r >= 0) && (r <= 2))
            {
                r = (Conf.display.w - Conf.display.y.max);
                if ((r >= 0) && (r <= 2))
                    Conf.display.changexy = true;
            }
        }
        //
        if (Conf.display.sz != dsz)
        {
            std::lock_guard<std::shared_mutex> l_lock(m_lock);
            Conf.display.sz = dsz;
            m_dspdata.resize(dsz);
        }
    }

    ADisplay::Orient ADisplay::getOrientation() const
    {
        android::Vector<android::DisplayInfo> configs;
        android::SurfaceComposerClient::getDisplayConfigs(m_dsp, &configs);
        int32_t idx, idc = android::SurfaceComposerClient::getActiveConfig(m_dsp);
        if ((idc < 0) || (!configs.size()) || (static_cast<size_t>(idc) >= configs.size()))
            throw std::system_error(App::make_error_code(App::ErrorId::error_display_android_getconfig));

        switch ((idx = static_cast<int32_t>(configs[idc].orientation)))
        {
            case 0:
            case 1:
            case 2:
            case 3:  return static_cast<ADisplay::Orient>(idx);
            default: return ADisplay::Orient::O_0;
        }
    }

    ADisplay::ImgTypeFmt ADisplay::getAndroidFormat(uint16_t f) const
    {
        // TODO: format normalize
        switch(f)
        {
            /// * 32 BPP
            case android::PIXEL_FORMAT_RGBA_8888:
            case android::PIXEL_FORMAT_RGBX_8888:
            case android::PIXEL_FORMAT_BGRA_8888:
            /// > 5.1.1 duplicate PIXEL_FORMAT_RGBA_8888
            //case android::PIXEL_FORMAT_sRGB_A_8888:
            /// 5.1.1 AOSP
            //case android::PIXEL_FORMAT_sRGB_X_8888:
                return ImgTypeFmt::F_32;
            /// * 24 BPP
            case android::PIXEL_FORMAT_RGB_888:
                return ImgTypeFmt::F_24;
            /// * 16 BPP
            case android::PIXEL_FORMAT_RGB_565:
            case android::PIXEL_FORMAT_RGBA_5551:
            case android::PIXEL_FORMAT_RGBA_4444:
                return ImgTypeFmt::F_16;
            default: return ImgTypeFmt::F_FMT_TYPE_NONE;
        }
    }

    std::string ADisplay::getImgTypeString(ADisplay::ImgTypeOut t)
    {
        return ImgTypeString[t];
    }

    ADisplay::ImgTypeOut ADisplay::getImgTypeEnum(std::string const & s)
    {
        for (uint32_t i = 0U; i < __NELE(ImgTypeString); i++)
            if (s.compare(0, 3, ImgTypeString[i], 3) == 0)
                return static_cast<ADisplay::ImgTypeOut>(i);

        return ADisplay::ImgTypeOut::T_IMG_TYPE_NONE;
    }

    std::string ADisplay::getOrientationString(ADisplay::Orient t)
    {
        return DisplayOrientString[t];
    }

    ///

    void ADisplay::displayInfo(std::ostream & os) const
    {
#       if defined(_DEBUG) || defined(_BUILD_INFO_STAT)
        os << "Orientation -> " << Event::ADisplay::getOrientationString(Conf.display.o).c_str() << "\n";
        os << "Display ->";
        os << "  w: "    << Conf.display.w;
        os << ", h: "    << Conf.display.h;
        os << ", s: "    << Conf.display.s;
        os << ", b: "    << Conf.display.b;
        os << ", f: "    << Conf.display.f;
        os << ", format: " << DisplayBitFormatString[Conf.display.fmt];
        os << ", size: " << Conf.display.sz;
        os << ", change XY: " << ((Conf.display.changexy) ? "yes" : "no" );
        os << "\n";
        if ((Conf.display.x.max) || (Conf.display.y.max))
        {
            os << "Click range ->";
            os << "  X min: "  << Conf.display.x.min;
            os << ", X max: "  << Conf.display.x.max;
            os << ", Y min: "  << Conf.display.y.min;
            os << ", Y max: "  << Conf.display.y.max;
            os << "\n";
        }
        if (Conf.display.p.max)
        {
            os << "Pressure ->";
            os << "  min: "  << Conf.display.p.min;
            os << ", max: "  << Conf.display.p.max;
            os << "\n";
        }
        if (Conf.display.d.max)
        {
            os << "Distance ->";
            os << "  min: "  << Conf.display.d.min;
            os << ", max: "  << Conf.display.d.max;
            os << "\n";
        }
#       else
        os << "only build in _DEBUG mode..";
#       endif
    }

    void ADisplay::displayInfo(Helper::Xml & x) const
    {
        x.xmlsection("Display")
         .xmlpair("orientation", Event::ADisplay::getOrientationString(Conf.display.o))
         .xmlpair("width",    Conf.display.w)
         .xmlpair("height",   Conf.display.h)
         .xmlpair("stride",   Conf.display.s)
         .xmlpair("format",   Conf.display.f, DisplayBitFormatString[Conf.display.fmt])
         .xmlpair("bpp",      Conf.display.b)
         .xmlpair("buffsize", Conf.display.sz)
         .xmlpair("changexy", ((Conf.display.changexy) ? "yes" : "no" ));

        if (Conf.display.x.max)
            x.xmlsection("RangeX")
             .xmlpair("min", Conf.display.x.min)
             .xmlpair("max", Conf.display.x.max);

        if (Conf.display.y.max)
            x.xmlsection("RangeY")
             .xmlpair("min", Conf.display.y.min)
             .xmlpair("max", Conf.display.y.max);

        if (Conf.display.p.max)
            x.xmlsection("Pressure")
             .xmlpair("min", Conf.display.p.min)
             .xmlpair("max", Conf.display.p.max);

        if (Conf.display.d.max)
            x.xmlsection("Distance")
             .xmlpair("min", Conf.display.d.min)
             .xmlpair("max", Conf.display.d.max);

        if (Conf.keymap.size())
        {
            x.xmlsection("HWKeyMap");
            for (auto & [ key, dev ] : Conf.keymap)
            {
                std::string s;
                Conf.helperKeyDesc(key, s);
                x.xmlpair(key, dev, s);
            }
        }
    }
};

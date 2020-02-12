
#include "../../ARemote.h"

namespace Lua
{
    namespace Method
    {
        static bool f_string_to_time(sol::string_view & s, std::tm & tms)
        {
            try
            {
                if (s.empty())
                    return false;
                std::istringstream ss(s.data());
                ss >> std::get_time(&tms, "%H:%M:%S");

                if (ss.fail())
                    return false;
                return true;
            }
            catch (...)
            {
                LOGEXCEPT();
                return false;
            }
        }

        ///

        int32_t checkTime(Lua::Engine*, sol::string_view s, sol::string_view e)
        {
            int32_t tdiff = -1;

            do
            {
                std::time_t t = std::time(nullptr);
                std::tm *tml  = std::localtime(&t);
                std::tm  tms{};
                std::tm  tme{};

                if ((!tml)                      ||
                    (!f_string_to_time(e, tme)) ||
                    (!f_string_to_time(s, tms)))
                    break;

                tdiff = 0;

                int32_t sh = (tms.tm_hour - (*tml).tm_hour),
                        sm = (tms.tm_min  - (*tml).tm_min),
                        ss = (tms.tm_sec  - (*tml).tm_sec);

                if (sh > 0)
                    break;
                if (!sh)
                {
                    if (sm > 0)
                        break;
                    if (!sm)
                        if (ss > 0)
                            break;
                }

                int32_t eh = (tme.tm_hour - (*tml).tm_hour),
                        em = (tme.tm_min  - (*tml).tm_min),
                        es = (tme.tm_sec  - (*tml).tm_sec);

                if (eh < 0)
                    break;
                if (!eh)
                {
                    if (em < 0)
                        break;

                    if (!em)
                        if (es < 0)
                            break;
                }
                tdiff = 1;
            }
            while (0);
            return tdiff;
        }
        //
        bool checkPixelByPos(Lua::Engine*, uint32_t pos, uint8_t r, uint8_t g, uint8_t b)
        {
            if (!Conf.class_display)
                return false;

            try
            {
                App::CaptureTrigger cap;
                //
                ImageLite::ImgBuffer ifb;
                Conf.class_display->imageCapture(ifb, Event::ADisplay::ImgTypeOut::T_RAW_NOPAD);
                ImageLite::ImageRGBbuffer img(
                            ifb,
                            Conf.display.w,
                            Conf.display.h,
                            0U,
                            ImageLite::BufferType::IBT_RGB
                    );
                ImageLite::IPIX3 upix{ r, g, b };
                ImageLite::IPIX3 ipix = img.getpixel(pos);
                //
                if ((upix.c1 == ipix.c1) &&
                    (upix.c2 == ipix.c2) &&
                    (upix.c3 == ipix.c3))
                    return true;
                else
                    return false;
            }
            catch (...)
            {
                LOGEXCEPT();
                return false;
            }
        }
        bool checkPixelByPos(Lua::Engine*, uint32_t pos, sol::as_table_t<std::vector<uint8_t>> vt)
        {
            if (!Conf.class_display)
                return false;

            try
            {
                const auto & v = vt.value();
                if (!v.size())
                    return false;

                App::CaptureTrigger cap;
                //
                ImageLite::ImgBuffer ifb;
                Conf.class_display->imageCapture(ifb, Event::ADisplay::ImgTypeOut::T_RAW_NOPAD);
                ImageLite::ImageRGBbuffer img(
                            ifb,
                            Conf.display.w,
                            Conf.display.h,
                            0U,
                            ImageLite::BufferType::IBT_RGB
                    );
                ImageLite::IPIX3 ipix = img.getpixel(pos);
                //
                if ((v[0] == ipix.c1) &&
                    (v[1] == ipix.c2) &&
                    (v[2] == ipix.c3))
                    return true;
                else
                    return false;
            }
            catch (...)
            {
                LOGEXCEPT();
                return false;
            }
        }
        bool checkPixelByCord(Lua::Engine*, uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b)
        {
            if (!Conf.class_display)
                return false;

            try
            {
                App::CaptureTrigger cap;
                //
                ImageLite::ImgBuffer ifb;
                Conf.class_display->imageCapture(ifb, Event::ADisplay::ImgTypeOut::T_RAW_NOPAD);
                ImageLite::ImageRGBbuffer img(
                            ifb,
                            Conf.display.w,
                            Conf.display.h,
                            0U,
                            ImageLite::BufferType::IBT_RGB
                    );
                ImageLite::IPOINT<int32_t> ip{ static_cast<int32_t>(x), static_cast<int32_t>(y) };
                ImageLite::IPIX3 upix{ r, g, b };
                ImageLite::IPIX3 ipix = img.getpixel(ip);
                //
                if ((upix.c1 == ipix.c1) &&
                    (upix.c2 == ipix.c2) &&
                    (upix.c3 == ipix.c3))
                    return true;
                else
                    return false;
            }
            catch (...)
            {
                LOGEXCEPT();
                return false;
            }
        }
        bool checkPixelByCord(Lua::Engine*, uint32_t x, uint32_t y, sol::as_table_t<std::vector<uint8_t>> vt)
        {
            if (!Conf.class_display)
                return false;

            try
            {
                const auto & v = vt.value();
                if (!v.size())
                    return false;

                App::CaptureTrigger cap;
                //
                ImageLite::ImgBuffer ifb;
                Conf.class_display->imageCapture(ifb, Event::ADisplay::ImgTypeOut::T_RAW_NOPAD);
                ImageLite::ImageRGBbuffer img(
                            ifb,
                            Conf.display.w,
                            Conf.display.h,
                            0U,
                            ImageLite::BufferType::IBT_RGB
                    );
                ImageLite::IPOINT<int32_t> ip{ static_cast<int32_t>(x), static_cast<int32_t>(y) };
                ImageLite::IPIX3 ipix = img.getpixel(ip);
                //
                if ((v[0] == ipix.c1) &&
                    (v[1] == ipix.c2) &&
                    (v[2] == ipix.c3))
                    return true;
                else
                    return false;
            }
            catch (...)
            {
                LOGEXCEPT();
                return false;
            }
        }
        //
        bool checkPixelsByPos(Lua::Engine*, ImageLite::LuaArray4 v)
        {
            if (!Conf.class_display)
                return false;

            try
            {
                if (!v.data.size())
                    return false;

                App::CaptureTrigger cap;
                //
                ImageLite::ImgBuffer ifb;
                Conf.class_display->imageCapture(ifb, Event::ADisplay::ImgTypeOut::T_RAW_NOPAD);
                ImageLite::ImageRGBbuffer img(
                            ifb,
                            Conf.display.w,
                            Conf.display.h,
                            0U,
                            ImageLite::BufferType::IBT_RGB
                    );
                for (auto & [ pos, rgb ] : v.data)
                {
                    auto & [ r, g, b ] = rgb;
                    ImageLite::IPIX3 ipix = img.getpixel(pos);
                    if ((r != ipix.c1) ||
                        (g != ipix.c2) ||
                        (b != ipix.c3))
                        return false;
                }
                return true;
            }
            catch (...)
            {
                LOGEXCEPT();
                return false;
            }
        }
        bool checkPixelsByCord(Lua::Engine*, ImageLite::LuaArray5 v)
        {
            if (!Conf.class_display)
                return false;

            try
            {
                if (!v.data.size())
                    return false;

                App::CaptureTrigger cap;
                //
                ImageLite::ImgBuffer ifb;
                Conf.class_display->imageCapture(ifb, Event::ADisplay::ImgTypeOut::T_RAW_NOPAD);
                ImageLite::ImageRGBbuffer img(
                            ifb,
                            Conf.display.w,
                            Conf.display.h,
                            0U,
                            ImageLite::BufferType::IBT_RGB
                    );
                for (auto & [ x, y, rgb ] : v.data)
                {
                    auto & [ r, g, b ] = rgb;
                    ImageLite::IPIX3 ipix = img.getpixel(x, y);
                    if ((r != ipix.c1) ||
                        (g != ipix.c2) ||
                        (b != ipix.c3))
                        return false;
                }
                return true;
            }
            catch (...)
            {
                LOGEXCEPT();
                return false;
            }
        }
        //
        ImageLite::ImageTable screenGet(Lua::Engine *luae)
        {
            if (!Conf.class_display)
                return ImageLite::ImageTable();

            try
            {
                App::CaptureTrigger cap;
                //
                ImageLite::ImageTable itbl(Conf.display.w, Conf.display.h);
                Conf.class_display->imageCapture(itbl.data, Event::ADisplay::ImgTypeOut::T_RAW_NOPAD);
                return itbl;
            }
            catch (...)
            {
                LOGEXCEPT();
                return ImageLite::ImageTable();
            }
        }
        ImageLite::ImageTable screenGetRegion(Lua::Engine *luae, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        {
            if (!Conf.class_display)
                return ImageLite::ImageTable();

            try
            {
                App::CaptureTrigger cap;
                //
                ImageLite::ImageData idt{};
                idt.point.set(Conf.display.w, Conf.display.h);
                idt.stride = 0;
                //
                Conf.class_display->imageCapture(idt.buffer, Event::ADisplay::ImgTypeOut::T_RAW_NOPAD);
                //
                ImageLite::IRECT<int32_t> r(x, y, w, h);
                ImageLite::ImageRGBbuffer img(
                            idt,
                            r,
                            ImageLite::BufferType::IBT_RGB
                    );
                return img.get<ImageLite::ImageTable>();
            }
            catch (...)
            {
                LOGEXCEPT();
                return ImageLite::ImageTable();
            }
        }
        //
        void imageDefault(Lua::Engine*, sol::string_view s)
        {
            // ! development only
        }
        void imageShow(Lua::Engine*, sol::string_view s)
        {
            // ! development only
        }
        void imageTableShow(Lua::Engine*, ImageLite::ImageTable itbl)
        {
            // ! development only
        }
        //
        void imageSave(Lua::Engine *luae, sol::string_view s, uint32_t cnt)
        {
            if (!Conf.class_display)
                return;
            if (luae->m_savecnt > cnt)
                return;

            try
            {
                App::CaptureTrigger cap;
                //
                ImageLite::ImgBuffer ifb;
                Conf.class_display->imageCapture(ifb, Event::ADisplay::ImgTypeOut::T_RAW_NOPAD);
                ImageLite::ImageRGBbuffer img(
                            ifb,
                            Conf.display.w,
                            Conf.display.h,
                            0U,
                            ImageLite::BufferType::IBT_RGB
                    );
                img.save(s.data());
                luae->m_savecnt++;
            }
            catch (...)
            {
                LOGEXCEPT();
            }
        }
        double imageCompare(Lua::Engine*, sol::string_view s)
        {
            if (!Conf.class_display)
                return 0.0;

            try
            {
                App::CaptureTrigger cap;
                //
                ImageLite::ImageRGBbuffer imgl;
                imgl.load(s.data());
                //
                ImageLite::ImgBuffer ifb;
                Conf.class_display->imageCapture(ifb, Event::ADisplay::ImgTypeOut::T_RAW_NOPAD);
                ImageLite::ImageRGBbuffer img(
                            ifb,
                            Conf.display.w,
                            Conf.display.h,
                            0U,
                            ImageLite::BufferType::IBT_RGB
                    );
                return img.icompare(imgl);
            }
            catch (...)
            {
                LOGEXCEPT();
                return 0.0;
            }
        }
        double imageCompareRegion(Lua::Engine*, sol::string_view s, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        {
            if (!Conf.class_display)
                return 0.0;

            try
            {
                App::CaptureTrigger cap;
                //
                ImageLite::ImageRGBbuffer imgl;
                imgl.load(s.data());
                //
                ImageLite::ImgBuffer ifb;
                Conf.class_display->imageCapture(ifb, Event::ADisplay::ImgTypeOut::T_RAW_NOPAD);
                ImageLite::ImageRGBbuffer img(
                            ifb,
                            Conf.display.w,
                            Conf.display.h,
                            0U,
                            ImageLite::BufferType::IBT_RGB
                    );
                ImageLite::IRECT<int32_t> r(x, y, w, h);
                return img.icompare(imgl, r);
            }
            catch (...)
            {
                LOGEXCEPT();
                return 0.0;
            }
        }
        uint32_t imageGetPosition(Lua::Engine*, uint32_t x, uint32_t y)
        {
            if (!Conf.class_display)
                return 0U;
            if ((y > Conf.display.h) || (x > Conf.display.w))
                return 0U;

            return ((y * (Conf.display.w * 3)) + (x * 3));
        }
        //
        void adbClick(Lua::Engine*, uint32_t x, uint32_t y)
        {
            if (!Conf.class_input)
                return;
            Conf.class_input->tap(x, y);
        }
        void adbSwipe(Lua::Engine*, uint32_t x, uint32_t y, uint32_t xx, uint32_t yy, uint32_t d)
        {
            if (!Conf.class_input)
                return;
            Conf.class_input->swipe(x, y, xx, yy, d);
        }
        void adbKey(Lua::Engine*, uint16_t k)
        {
            if (!Conf.class_input)
                return;
            Conf.class_input->key(k);
        }
        void adbText(Lua::Engine*, sol::string_view s)
        {
            if (!Conf.class_input)
                return;
            Conf.class_input->text(s.data());
        }
        //
        void stateSet(Lua::Engine *luae, uint32_t n)
        {
            luae->m_state = n;
        }
        uint32_t stateGet(Lua::Engine *luae)
        {
            return luae->m_state.load();
        }
        void stateSleep(Lua::Engine *luae, uint32_t n)
        {
            try
            {
                auto s_start = std::chrono::high_resolution_clock::now();
                auto s_end = (s_start + std::chrono::seconds(n));

                while (std::chrono::high_resolution_clock::now() < s_end)
                {
                    if (!luae->m_isrun)
                        break;
                    std::this_thread::yield();
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
            catch (...)
            {
                LOGEXCEPT();
            }
        }
        void captureOn(Lua::Engine*)
        {
            if (Conf.class_display)
                Conf.class_display->clientcountUp();
        }
        void captureOff(Lua::Engine*)
        {
            if (Conf.class_display)
                Conf.class_display->clientcountDown();
        }
        void traceOn(Lua::Engine*)
        {
            // ! development only
        }
        void traceOff(Lua::Engine*)
        {
            // ! development only
        }
        void exitNow(Lua::Engine *luae)
        {
            luae->m_isrun = false;
        }
    }
}

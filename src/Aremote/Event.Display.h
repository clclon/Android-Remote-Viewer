
#pragma once

namespace App
{
    class Config;
};
namespace Event
{
    class StreamRawData;

    class ADisplay
    {
        private:
            //
#           if (__ANDROID_VER__ >= 9)
            android::sp<android::GraphicBuffer> m_sc;
#           else
            android::ScreenshotClient           m_sc;
#           endif
            android::sp<android::IBinder>       m_dsp;
            //
            std::vector<uint8_t>                m_dspdata;
            std::atomic<bool>                   m_run = false;
            std::atomic<uint32_t>               m_clientcount = 0U;
            std::thread                         m_thcap;
            std::shared_mutex                   m_lock;
            Helper::Clock::TimeWatch            m_tcond{};
            //
            void imageBmpHeader(std::vector<uint8_t>&);
            bool imageLz4(std::vector<uint8_t> const&, std::vector<uint8_t>&);

        public:
            //
            enum Orient
            {
                O_0 = 0,
                O_90,
                O_180,
                O_270,
                O_MIRROR,
                O_PORTRAIT,
                O_LANDSCAPE
            };
            enum ImgTypeOut
            {
                T_RAW = 0,
                T_BMP,
                T_BMZ,
                T_SDL,
                T_JPG,
                T_PNG,
                T_RAW_NOPAD,
                T_IMG_TYPE_END,
                T_IMG_TYPE_NONE = T_IMG_TYPE_END
            };
            enum ImgTypeFmt
            {
                F_16 = 0,
                F_24,
                F_32,
                F_FMT_TYPE_END,
                F_FMT_TYPE_NONE = F_FMT_TYPE_END
            };
            struct KeyMinMax
            {
                uint16_t min;
                uint16_t max;
            };
            struct Screen
            {
                uint16_t w;
                uint16_t h;
                uint16_t s;
                uint16_t f;
                uint16_t b;
                uint32_t sz;
                uint32_t fmt;
                bool     changexy;
                Orient    o;
                KeyMinMax x;
                KeyMinMax y;
                KeyMinMax p;
                KeyMinMax d;
            };
            struct RGBDATA
            {
                uint8_t r, g, b;
            };
            //
            ADisplay();
            virtual ~ADisplay();
            //
            ImgTypeFmt  getAndroidFormat(uint16_t) const;
            Orient      getOrientation() const;
            void        getDisplay(bool = true);
            void        mjpgStream(std::vector<uint8_t>&, StreamRawData&);
            void        imageCapture(std::vector<uint8_t>&, ImgTypeOut);
            void        clientcountUp();
            void        clientcountDown();
            bool        clientcountActive();
            void        displayInfo(std::ostream&) const;
            void        displayInfo(Helper::Xml&) const;
            //
            static std::string getImgTypeString(ImgTypeOut);
            static std::string getOrientationString(ADisplay::Orient);
            static ImgTypeOut  getImgTypeEnum(std::string const&);
            //
            friend std::ostream& operator << (std::ostream & os, ADisplay const & ad)
            {
                ad.displayInfo(os);
                return os;
            }
    };
    using dspo = ADisplay::Orient;
    //
    class StreamRawData
    {
        public:
            //
            enum StreamRatioEnum
            {
                R_1_1 = 0,
                R_1_2,
                R_1_3
            };
            enum ImgRawEnum
            {
                I_X = 0,
                I_Y,
                I_BPP,
                I_STRIDE,
                I_PAD,
                I_SZDST,
                I_SZSRC,
                I_ROTATE,
                I_DATA_END,
                POS_OFFSET_SRC = I_X,
                POS_OFFSET_DST,
                POS_INPUT,
                POS_OUTPUT,
                POS_STREAM_END,
                POS_OUTPAD    = POS_INPUT,
                POS_IMAGE_END = POS_OUTPUT,
                POS_SIMD_END  = POS_INPUT

            };
            uint32_t     x,
                         y,
                         sizeIn,
                         qualityOut,
                         idata[I_DATA_END]{};
            bool         orientSwap;
            dspo         orientOut;
            StreamRatioEnum ratioOut;
            //
            virtual ~StreamRawData();
            StreamRawData();
            StreamRawData(App::Config&,
                       StreamRatioEnum = StreamRatioEnum::R_1_1,
                       dspo = dspo::O_0,
                       uint32_t = 70U);
            void init(App::Config&,
                       StreamRatioEnum = StreamRatioEnum::R_1_1,
                       dspo = dspo::O_0,
                       uint32_t = 70U);
    };
    using eraw = StreamRawData::ImgRawEnum;
    using erat = StreamRawData::StreamRatioEnum;
    //
    class DisplayNoFatal : public std::exception
    {
        public:
            //
            uint32_t    line_ = -1;
            std::string fun_;
            std::string msg_;
            //
            DisplayNoFatal(std::string const & msg)
                : msg_(msg) {}
            DisplayNoFatal(uint32_t l, std::string const & fun, std::string const & msg)
                : line_(l), fun_(fun), msg_(msg) {}
            virtual ~DisplayNoFatal() {}
            //
            const char * msg() const noexcept
            {
                return msg_.c_str();
            }
            const char * what() const noexcept override
            {
                std::stringstream ss;
                if (!fun_.empty())
                    ss << fun_ << " : " << line_ << " -> ";
                ss << msg_;
                auto s = ss.str();
                return s.c_str();
            }
    };
};

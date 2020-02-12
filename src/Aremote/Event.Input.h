
#pragma once

namespace Event
{
    class Input
    {
        public:
            //
            enum MacroType
            {
                M_BEGIN = 0,
                M_KEY,
                M_TAP,
                M_SWIPE,
                M_TEXT,
                M_END
            };
            enum KeyType
            {
                K_ANDROID = 0,
                K_ASCII,
                K_JS,
                K_SDL,
                K_WIN
            };
            enum DevFeatures
            {
                F_ABS_NONE = 0,
                F_ABS_ACCEL,
                F_ABS_SENSOR,
                F_ABS_BUTTON,
                F_ABS_MTOUCH,
                F_ABS_KEYBOARD
            };
            struct dataevent_t
            {
                int32_t        fd;
                int32_t        evid;
                DevFeatures    evtype;
                std::string    evdesc;
            };
            struct baseevent_t
            {
                bool          touchslot;
                int32_t       touchid;
                dataevent_t   ev[20];
            };
            struct writeevent_t
            {
                struct timeval time;
                uint16_t       type;
                uint16_t       code;
                int32_t        value;
            };
            struct rotatexy_t
            {
                uint16_t         x,y;
                uint16_t         w,h;
                ADisplay::Orient o, d;
                StreamRawData::StreamRatioEnum r;
            };
            struct macroevent_t
            {
                time_t                   tdiff;
                MacroType                type;
                uint16_t                 x,y,xx,yy,k;
                std::string              text;
            };
            using macroarray_t = std::vector<macroevent_t>;
            struct basemacro_t
            {
                time_t                    tstart;
                time_t                    tend;
                std::atomic<bool>         enable;
                std::string               desc;
                macroarray_t              ev;
            };
            static const uint32_t LONG_PRESS_DEFAULT;
            static const char *execarray[5];
            //
            Input();
            virtual ~Input();
            //
            void open();
            void close();
            void key(uint16_t, uint16_t);
            void key(uint16_t);
            void tap(rotatexy_t&, uint32_t = 0);
            void tap(uint16_t, uint16_t, uint32_t = 0);
            void swipe(uint16_t, uint16_t, uint32_t);
            void swipe(uint16_t, uint16_t, uint16_t, uint16_t, uint32_t);
            void text(std::string const&);
            void inputInfo(std::ostream&) const;
            void inputInfo(Helper::Xml&) const;
            //
            std::string macroGet();
            std::string macroGet(uint32_t);
            std::string macroRecord(std::string const&, uint32_t);
            std::string macroList();
            void        macroPlay();
            void        macroPlay(uint32_t);
            void        macroPlay(std::string const&);
            void        macroStop();
            void        macroSave(bool = false);
            void        macroAdd(uint16_t);
            void        macroAdd(uint16_t, uint16_t, uint32_t);
            void        macroAdd(uint16_t, uint16_t, uint16_t, uint16_t, uint32_t);
            void        macroAdd(std::string const&);
            //
            friend std::ostream& operator << (std::ostream & os, Input const & in)
            {
                in.inputInfo(os);
                return os;
            }

        private:
            //
            std::atomic<bool> m_bplay = false;
            std::thread       m_thplay;
            baseevent_t       m_ev{};
            basemacro_t       m_macro{};
            //
            bool        sendraw(uint32_t, struct writeevent_t&, uint16_t, uint16_t, int32_t);
            bool        keyfilter(uint16_t);
            void        taprotate(rotatexy_t&);
            void        tapProtoMTOUCH(uint16_t, uint16_t, uint32_t);
            void        tapProtoBUTTON(uint16_t, uint16_t, uint32_t);
            void        macroRun(macroarray_t const&);
            void        macroParse(std::string const&);
    };
#   define __EVENT_RAW(DEVID,TYPE,ID,VAL) \
       if (!sendraw(DEVID, event, TYPE, ID, VAL)) break;
};


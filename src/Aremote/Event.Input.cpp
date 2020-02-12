
#include "ARemote.h"
#include <dirent.h>

#include <linux/input.h>
#include <linux/uinput.h>
#define UI_SET_PROPBIT _IOW(UINPUT_IOCTL_BASE, 110, int)

#define __EVENT_CONST_SIZE \
    (sizeof(long) * 8)
#define __EVENT_NBITS(A) \
    ((((A) - 1) / __EVENT_CONST_SIZE) + 1)
#define __EVENT_OFF(A) \
    ((A) % __EVENT_CONST_SIZE)
#define __EVENT_LONG(A) \
    ((A) / __EVENT_CONST_SIZE)
#define __EVENT_TEST_BIT(A,B) \
    ((B[__EVENT_LONG(A)] >> __EVENT_OFF(A)) & 1)

namespace Event
{
    //
    class InputData final
    {
        private:
            //
            std::vector<int32_t>             m_proto;
            std::vector<ADisplay::KeyMinMax> m_kmm;
#           define def_proto_size 6
#           define def_kmm_size   4

        public:
            //
            InputData()
            {
                m_proto.resize(def_proto_size);
                m_kmm.resize(def_kmm_size);
            }
            bool isslot()
            {
                return static_cast<bool>(m_proto[5]);
            }
            template <uint32_t idx>
            void up()
            {
                static_assert(idx < def_proto_size);
                m_proto[idx]++;
            }
            template <uint32_t idx>
            ADisplay::KeyMinMax * get()
            {
                static_assert(idx < def_kmm_size);
                return &m_kmm[idx];
            }
            Input::DevFeatures get()
            {
                if (m_proto[1])
                    return Input::DevFeatures::F_ABS_BUTTON;
                else if (m_proto[2] >= 3)
                    return Input::DevFeatures::F_ABS_MTOUCH;
                else if (m_proto[0] >= 2)
                    return Input::DevFeatures::F_ABS_ACCEL;
                else if ((m_proto[0] == 1) || (m_proto[4]))
                    return Input::DevFeatures::F_ABS_SENSOR;
                else if (m_proto[3])
                    return Input::DevFeatures::F_ABS_KEYBOARD;
                else
                    return Input::DevFeatures::F_ABS_NONE;
            }
            void print()
            {
                LOGDC("\tevent mask: [",
                          "\n\t\t\tis sensor:", m_proto[0],
                          "\n\t\t\tmulti touch (button protocol mode):", m_proto[1],
                          "\n\t\t\tmulti touch (standard protocol mode):", m_proto[2],
                          "\n\t\t\tkeys input support:", m_proto[3],
                          "\n\t\t\tis sensor (name deduct):", m_proto[4],
                          "\n\t\t\tis slot support:", m_proto[5],
                          "\n\t]"
                    );
                LOGDC("\tevent last KeyMinMax: [",
                          "\n\t\t\tx:", m_kmm[0].min, "/", m_kmm[0].max,
                          "\n\t\t\ty:", m_kmm[1].min, "/", m_kmm[1].max,
                          "\n\t\t\tp:", m_kmm[2].min, "/", m_kmm[2].max,
                          "\n\t\t\td:", m_kmm[3].min, "/", m_kmm[3].max,
                          "\n\t]"
                    );
            }
    };
    //
    static inline const char  l_devdir[]  = "/dev/input/";
    static inline const char  l_devname[] = "event";
    static inline const char *l_evtype[] =
    {
        "None",
        "Accelerator",
        "Sensor",
        "Touch (BUTTON + MULTI TOUCH mode)",
        "Touch (MULTI TOUCH mode)",
        "Key Input support"
    };
    //
    const uint32_t Input::LONG_PRESS_DEFAULT = 2U;
    const char    *Input::execarray[] =
    {
        "/system/bin/sh",
        "sh",
        "-c",
        "input keyevent ",
        "input text "
    };

    Input::Input()
    {
        m_ev.touchid     = -1;
        Conf.class_input = this;
    }

    Input::~Input()
    {
        close();
        m_bplay = false;
        if (m_thplay.joinable())
            m_thplay.join();
        Conf.class_input = nullptr;
    }

    void Input::close()
    {
        for (uint32_t i = 0; i < std::size(m_ev.ev); i++)
        {
            if (m_ev.ev[i].fd > ::fileno(stderr))
                ::close(m_ev.ev[i].fd);
            m_ev.ev[i].fd = -1;
        }
        if (Conf.keymap.size())
            Conf.keymap.clear();
        m_ev.touchid = -1;
    }

    bool Input::sendraw(uint32_t devid, struct writeevent_t & EVENT, uint16_t type, uint16_t code, int32_t value)
    {
        do
        {
            if ((devid < 0) || (m_ev.ev[devid].fd < 0))
                break;

            EVENT =
            {
                {0, 0},
                type,
                code,
                value
            };
            if (::write(m_ev.ev[devid].fd, &EVENT, sizeof(EVENT)) != sizeof(EVENT))
                break;
            return true;
        }
        while (0);
        return false;
    }

    void Input::open()
    {
        {
            close();
            Conf.display.x = {};
            Conf.display.y = {};
            Conf.display.p = {};
            Conf.display.d = {};
            Conf.display.changexy = false;
        }
        //
        uint32_t cnt = 0U;
        struct dirent *de;
        auto d = Helper::auto_shared<DIR>(
                    ::opendir(l_devdir),
                    [](DIR *d_) { ::closedir(d_); }
                );
        if (d == nullptr)
            throw std::system_error(App::make_error_code(App::ErrorId::error_input_event_open));

        while((de = ::readdir(d.get())) != nullptr)
        {
            std::string s        = de->d_name;

            if (cnt >= std::size(m_ev.ev))
                break;
            if (s.compare(0, 5, l_devname, 5) != 0)
                continue;
            //
            if (s.length() >= 6)
            {
                try
                {
                    std::string ts = s.substr(5, s.length() - 5);
                    if (!ts.empty())
                        m_ev.ev[cnt].evid = std::stoi(ts);
                    else
                        m_ev.ev[cnt].evid = -1;
                }
                catch (...) { m_ev.ev[cnt].evid = -1; }
            }
            //
            s.insert(0, l_devdir);
            LOGDC("+  EVENT FILE: (", s.c_str(), m_ev.ev[cnt].evid, ")");
            //
            if ((m_ev.ev[cnt].fd = ::open(s.c_str(), O_RDWR)) < 0)
                continue;
            if (m_ev.ev[cnt].fd <= ::fileno(stderr))
            {
                m_ev.ev[cnt].fd = -1;
                continue;
            }
            //
            InputData idata;
            //
            {
                std::vector<char> v(512);
                (void) ::ioctl(m_ev.ev[cnt].fd, EVIOCGNAME(v.size()), &v[0]);
                m_ev.ev[cnt].evdesc = &v[0];
                if ((!m_ev.ev[cnt].evdesc.empty()) &&
                    (m_ev.ev[cnt].evdesc.find("sensor") != std::string::npos))
                    idata.up<4>();
            }
            //
            {
                Helper::Array2D<uint32_t, EV_MAX, __EVENT_NBITS(KEY_MAX)> arr2d;
                auto bits = arr2d.get();
                (void) ::ioctl(m_ev.ev[cnt].fd, EVIOCGBIT(0, EV_MAX), bits[0]);

                for (int32_t i = 1; i < EV_MAX; i++)
                {
                    if (!__EVENT_TEST_BIT(i, bits[0]))
                       continue;

#                   if defined(_DEBUG)
                    {
                        std::string hs;
                        Conf.helperKeyType(i, hs);
                        if (!hs.empty())
                            LOGDC("\t\t+  EVENT FOUND: (", i, ") [", hs.c_str(), "] ->", s.c_str());
                        else
                            LOGDC("\t\t+  EVENT FOUND: (", i, ") ->", s.c_str());
                    }
#                   endif

                    (void) ::ioctl(m_ev.ev[cnt].fd, EVIOCGBIT(i, KEY_MAX), bits[i]);
                    for (int32_t n = 0; n < KEY_MAX; n++)
                    {
                        if (!__EVENT_TEST_BIT(n, bits[i]))
                            continue;

                        switch (i)
                        {
                            case EV_KEY:
                                {
                                    if (n == BTN_TOUCH)
                                    {
                                        idata.up<1>();
                                        LOGDC("\t\t+  EVENT BTN_TOUCH FOUND: (", cnt, "/", n, "/", i, ")");
                                        break;
                                    }
                                    [[maybe_unused]] auto [it, result] = Conf.keymap.try_emplace(static_cast<uint16_t>(n), cnt);
                                    if (!result)
                                    {
                                        LOGDC("\t!duplicate key: (", n, "/", cnt, ")");
                                    }
                                    idata.up<3>();
                                    break;
                                }
                            case EV_ABS:
                                {
                                    LOGDC("\t\t+  EVENT TOUCH FOUND: (", cnt, "/", n, ") -> Old devid: [", m_ev.touchid, "]");
                                    ADisplay::KeyMinMax *p = nullptr;

                                    switch (n)
                                    {
                                        case ABS_X:              { p = idata.get<0>(); idata.up<0>(); break; }
                                        case ABS_Y:              { p = idata.get<1>(); idata.up<0>(); break; }
                                        case ABS_Z:              { idata.up<0>(); break; }
                                        case ABS_DISTANCE:       { idata.up<0>(); break; }
                                        case ABS_PRESSURE:       { p = idata.get<2>(); break; }
                                        case ABS_MT_SLOT:        { idata.up<5>(); break; }
                                        case ABS_MT_PRESSURE:    { p = idata.get<2>(); break; }
                                        case ABS_MT_DISTANCE:    { p = idata.get<3>(); break; }
                                        case ABS_MT_TOUCH_MAJOR: { p = idata.get<3>(); idata.up<2>(); break; }
                                        case ABS_MT_TOUCH_MINOR: { p = idata.get<3>(); idata.up<2>(); break; }
                                        case ABS_MT_POSITION_X:  { p = idata.get<0>(); idata.up<2>(); break; }
                                        case ABS_MT_POSITION_Y:  { p = idata.get<1>(); idata.up<2>(); break; }
                                        case ABS_MT_TRACKING_ID: { idata.up<2>(); break; }
                                    }
                                    if (!p)
                                        break;

                                    std::vector<int32_t> val(5);
                                    (void) ::ioctl(m_ev.ev[cnt].fd, EVIOCGABS(n), &val[0]);

                                    p->min = val[1];
                                    p->max = val[2];

#                                   if defined(_DEBUG)
                                    {
                                        for (int32_t k = 0; k < val.size(); k++)
                                            LOGDC("\t\tvalue: (", k, ":", val[k], ")");
                                    }
#                                   endif
                                    break;
                                }
                            default:
                                break;
                        }
                    }
                }
                //
                m_ev.ev[cnt].evtype = idata.get();
                if ((m_ev.ev[cnt].evtype == DevFeatures::F_ABS_BUTTON) ||
                    (m_ev.ev[cnt].evtype == DevFeatures::F_ABS_MTOUCH))
                {
                    m_ev.touchid   =  cnt;
                    m_ev.touchslot =  idata.isslot();
                    Conf.display.x = *idata.get<0>();
                    Conf.display.y = *idata.get<1>();
                    Conf.display.p = *idata.get<2>();
                    Conf.display.d = *idata.get<3>();

                    ioctl(m_ev.ev[cnt].fd, UI_SET_PROPBIT, INPUT_PROP_DIRECT);

#                   if defined(_DEBUG)
                    idata.print();
#                   endif
                }
            }
            LOGDC("\tevent found: (", s.c_str(), ") [", cnt, "/", m_ev.ev[cnt].fd, "/", m_ev.ev[cnt].evid, "/", m_ev.ev[cnt].evtype, "]");
            cnt++;
        }
    }

    void Input::inputInfo(std::ostream & os) const
    {
#       if defined(_DEBUG) || defined(_BUILD_INFO_STAT)
        for (uint32_t i = 0U; i < __NELE(m_ev.ev); i++)
        {
            dataevent_t const *ev = static_cast<dataevent_t const*>(&m_ev.ev[i]);
            if (ev->fd <= ::fileno(stderr))
                continue;

            os << i << "] " << l_devname << ev->evid << "\n";
            if (i == m_ev.touchid)
                os << "\tActive touch input\n";
            os << "\tDescription : " << ev->evdesc << "\n";
            os << "\tDescriptor  : " << ev->fd << "\n";
            os << "\tType        : " << l_evtype[ev->evtype] << "\n";

            if (!Conf.keymap.size())
                continue;

            std::stringstream ss;
            for (auto & [ key, val ] : Conf.keymap)
            {
                if (val == i)
                    ss << key << ", ";
            }
            auto s = ss.str();
            if (s.empty())
                continue;

            os << "\tKeys        : " << s.c_str() << "\n";
        }
#       else
        os << "only build in _DEBUG mode..";
#       endif
    }

    void Input::inputInfo(Helper::Xml & x) const
    {
        for (uint32_t i = 0U; i < __NELE(m_ev.ev); i++)
        {
            dataevent_t const *ev = static_cast<dataevent_t const*>(&m_ev.ev[i]);
            if (ev->fd <= ::fileno(stderr))
                continue;

            std::string sn = l_devname;
            sn += std::to_string(ev->evid).c_str();

            x.xmlsection("Event")
             .xmlpair("id", i)
             .xmlpair("device", sn, ev->fd)
             .xmlpair("type", l_evtype[ev->evtype])
             .xmlpair("description", ev->evdesc);

            if (i == m_ev.touchid)
                x.xmlpair("active", "true");

            if (!Conf.keymap.size())
                continue;

            std::stringstream ss;
            for (auto & [ key, val ] : Conf.keymap)
            {
                if (val == i)
                {
                    std::string s;
                    Conf.helperKeyDesc(key, s);
                    x.xmlpair("key", key, s);
                }
            }
        }
    }
};

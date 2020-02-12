
#include "ARemote.h"
#define UI_SET_PROPBIT _IOW(UINPUT_IOCTL_BASE, 110, int)
// ioctl(uinput_fd, UI_SET_PROPBIT, INPUT_PROP_DIRECT)

/*
    Info:
    adb exec-out getevent -lp /dev/input/event1
    Test:
    adb exec-out getevent -lt /dev/input/event1
*/

namespace Event
{
    static uint16_t rotatetbl[][5] =
    {
                      /*    O_0, O_90, O_180, O_270, O_MIRROR */
        /* O_0         */ { 1,   2,    3,     4,     5  },
        /* O_90        */ { 6,   7,    8,     9,     10 },
        /* O_180       */ { 11,  12,   13,    14,    15 },
        /* O_270       */ { 16,  17,   18,    19,    20 },
        /* O_MIRROR    */ { 0,   0,    0,     0,     0  },
        /* O_PORTRAIT  */ { 0,   0,    0,     0,     0  },
        /* O_LANDSCAPE */ { 0,   0,    0,     0,     0  }
    };
    //

    void Input::taprotate(rotatexy_t & tr)
    {
#       if defined(_BUILD_STRICT)
        assert(tr.d < dspo::O_MIRROR);
#       else
        if (__builtin_expect(!!(tr.d >= dspo::O_MIRROR), 0))
            throw std::system_error(App::make_error_code(App::ErrorId::error_input_not_rotate_id));
#       endif
        switch(tr.o)
        {
            case dspo::O_PORTRAIT:  tr.o = dspo::O_0; break;
            case dspo::O_LANDSCAPE: tr.o = dspo::O_270; break;
            default: break;
        }
        switch (tr.r)
        {
            case StreamRawData::StreamRatioEnum::R_1_2:
                {
                    tr.x = tr.x * 2;
                    tr.y = tr.y * 2;
                    break;
                }
            case StreamRawData::StreamRatioEnum::R_1_3:
                {
                    tr.x = tr.x * 3;
                    tr.y = tr.y * 3;
                    break;
                }
            default:
                break;
        }
        switch(rotatetbl[tr.d][tr.o])
        {
            // Normalize Tap <-> Orientation.
            // Display rotate deduction method.
            // see: ADisplay.StreamRawData.cpp, array OrientMap
            //
            //            [ system / user ]
            case 1:       // O_0   / O_0
            case 7:       // O_90  / O_90
            case 14:      // O_180 / O_270
            case 19:      // O_270 / O_270
                {
                    break;
                }
            case 2:       // O_0   / O_90
            case 8:       // O_90  / O_180
            case 13:      // O_180 / O_180
            case 16:      // O_270 / O_0
                {
                    std::swap(tr.y, tr.x);
                    tr.y = (tr.h - tr.y);
                    break;
                }
            case 3:       // O_0   / O_180
            case 9:       // O_90  / O_270
            case 11:      // O_180 / O_0
            case 17:      // O_270 / O_90
                {
                    tr.x = (tr.w - tr.x);
                    tr.y = (tr.h - tr.y);
                    break;
                }
            case 4:       // O_0   / O_270
            case 6:       // O_90  / O_0
            case 12:      // O_180 / O_90
            case 18:      // O_270 / O_180
                {
                    std::swap(tr.y, tr.x);
                    tr.x = (tr.w - tr.x);
                    break;
                }
            case 5:       // O_0    / O_MIRROR
            case 10:      // O_90   / O_MIRROR
            case 15:      // O_180  / O_MIRROR
            case 20:      // O_270  / O_MIRROR
                {
                    tr.x = (tr.w - tr.x);
                    break;
                }
            default:
                throw std::system_error(App::make_error_code(App::ErrorId::error_input_not_orient_id));
        }
    }

    void Input::tap(rotatexy_t & tr, uint32_t longtap)
    {
        LOGDC("+  TAP pre-calculate (user:", tr.o,
              ", display:", Conf.display.o,
              ") -> w:", tr.w,
              ", x:", tr.x,
              ", h:", tr.h,
              ", y:", tr.y);
        taprotate(tr);
        tap(tr.x, tr.y, longtap);
    }

    void Input::tap(uint16_t x, uint16_t y, uint32_t longtap)
    {
        if (m_ev.touchid < 0)
            throw std::system_error(App::make_error_code(App::ErrorId::error_input_not_init_device));

        if (Conf.display.changexy)
        {
            x = (((Conf.display.y.max) && (Conf.display.y.max < x)) ? Conf.display.y.max : x);
            y = (((Conf.display.x.max) && (Conf.display.x.max < y)) ? Conf.display.x.max : y);
        }
        else
        {
            x = (((Conf.display.x.max) && (Conf.display.x.max < x)) ? Conf.display.x.max : x);
            y = (((Conf.display.y.max) && (Conf.display.y.max < y)) ? Conf.display.y.max : y);
        }
        if (Conf.display.p.max < longtap)
            longtap = Conf.display.p.max;

        LOGDC("=  TAP ( devid:", m_ev.touchid,
              "devnum:",         m_ev.ev[m_ev.touchid].evid,
              "mode:",           m_ev.ev[m_ev.touchid].evtype,
              "changeXY:",       ((Conf.display.changexy) ? "yes" : "no"),
              ") -> x:", x, ", y:", y);

        switch (m_ev.ev[m_ev.touchid].evtype)
        {
            case DevFeatures::F_ABS_BUTTON:   tapProtoBUTTON(x, y, longtap); break;
            case DevFeatures::F_ABS_MTOUCH:   tapProtoMTOUCH(x, y, longtap); break;
            case DevFeatures::F_ABS_ACCEL:
            case DevFeatures::F_ABS_SENSOR:
            case DevFeatures::F_ABS_KEYBOARD:
                    throw std::system_error(App::make_error_code(App::ErrorId::error_input_not_tap_support));
            case DevFeatures::F_ABS_NONE:
                    throw std::system_error(App::make_error_code(App::ErrorId::error_input_not_touch_support));
        }
        if (m_macro.enable)
            macroAdd(x, y, longtap);
    }

    void Input::tapProtoMTOUCH(uint16_t x, uint16_t y, uint32_t longtap)
    {
        struct writeevent_t event{};
        uint32_t cnt = 0U,
                 lt  = ((longtap < 8) ? (8 - longtap) : 6),
                 mt  = ((Conf.display.d.max) ? ((Conf.display.d.max < 40) ? Conf.display.d.max : 40) : 40);
        do
        {
            do
            {
                if (!cnt)
                {
                    if (m_ev.touchslot)
                        __EVENT_RAW(m_ev.touchid, EV_ABS, ABS_MT_SLOT, 0);
                    __EVENT_RAW(m_ev.touchid, EV_ABS, ABS_MT_TRACKING_ID, 0);
                    __EVENT_RAW(m_ev.touchid, EV_ABS, ABS_MT_POSITION_X,  x);
                    __EVENT_RAW(m_ev.touchid, EV_ABS, ABS_MT_POSITION_Y,  y);
                    __EVENT_RAW(m_ev.touchid, EV_ABS, ABS_MT_TOUCH_MAJOR, mt);
                    __EVENT_RAW(m_ev.touchid, EV_ABS, ABS_MT_WIDTH_MAJOR, mt);
                    __EVENT_RAW(m_ev.touchid, EV_SYN, SYN_REPORT, 0);
                    cnt++;
                    continue;
                }
                __EVENT_RAW(m_ev.touchid, EV_ABS, ABS_MT_POSITION_X, x);//(x + (cnt * 2)));
                __EVENT_RAW(m_ev.touchid, EV_ABS, ABS_MT_POSITION_Y, y);//(y + (cnt * 2)));
                __EVENT_RAW(m_ev.touchid, EV_SYN, SYN_REPORT, 0);
                std::this_thread::sleep_for(std::chrono::milliseconds(lt));
            }
            while (cnt++ < 3);
            __EVENT_RAW(m_ev.touchid, EV_ABS, ABS_MT_TRACKING_ID, 0xffffffff);
            __EVENT_RAW(m_ev.touchid, EV_SYN, SYN_REPORT, 0);
        }
        while (0);
    }

    void Input::tapProtoBUTTON(uint16_t x, uint16_t y, uint32_t longtap)
    {
        struct writeevent_t event{};
        uint32_t cnt = 0U,
                 lt  = ((Conf.display.d.max) ? (Conf.display.d.max / 2) : 70);
        do
        {
            if (!cnt)
            {
                __EVENT_RAW(m_ev.touchid, EV_KEY, BTN_TOUCH, 1);
                __EVENT_RAW(m_ev.touchid, EV_SYN, SYN_MT_REPORT, 0);
                __EVENT_RAW(m_ev.touchid, EV_SYN, SYN_REPORT, 0);
            }
            //
            __EVENT_RAW(m_ev.touchid, EV_ABS, ABS_MT_TOUCH_MAJOR, (30 + cnt));
            __EVENT_RAW(m_ev.touchid, EV_ABS, ABS_MT_TRACKING_ID, 0);
            __EVENT_RAW(m_ev.touchid, EV_ABS, ABS_MT_POSITION_X, x);
            __EVENT_RAW(m_ev.touchid, EV_ABS, ABS_MT_POSITION_Y, y);
            __EVENT_RAW(m_ev.touchid, EV_SYN, SYN_MT_REPORT, 0);
            __EVENT_RAW(m_ev.touchid, EV_SYN, SYN_REPORT, 0);
            //
            if (cnt++ < longtap)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(lt));
                continue;
            }
            //
            __EVENT_RAW(m_ev.touchid, EV_KEY, BTN_TOUCH, 0);
            __EVENT_RAW(m_ev.touchid, EV_SYN, SYN_MT_REPORT, 0);
            __EVENT_RAW(m_ev.touchid, EV_SYN, SYN_REPORT, 0);
            break;
        }
        while (true);
    }
};

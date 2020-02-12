
#include "ARemote.h"

namespace Event
{
    void Input::swipe(uint16_t x, uint16_t y, uint32_t longtap)
    {
        swipe(x, y, 5, 50, longtap);
    }

    void Input::swipe(uint16_t x, uint16_t y, uint16_t xx, uint16_t yy, uint32_t longtap)
    {
        if (m_ev.touchid < 0)
            throw std::system_error(App::make_error_code(App::ErrorId::error_input_not_init_device));

        if (Conf.display.p.max < longtap)
            longtap = Conf.display.p.max;

        if (m_macro.enable)
            macroAdd(x, y, xx, yy, longtap);

        struct writeevent_t event{};
        LOGDC("=  SWIPE -> x:", x, ", y:", y, ", xx:", xx, ", yy:", yy);

        do
        {
            __EVENT_RAW(m_ev.touchid, EV_ABS, 53, x);
            __EVENT_RAW(m_ev.touchid, EV_ABS, 54, y);
            __EVENT_RAW(m_ev.touchid, EV_ABS, 48, xx);
            __EVENT_RAW(m_ev.touchid, EV_ABS, 58, yy);
            __EVENT_RAW(m_ev.touchid, EV_SYN, SYN_MT_REPORT, 0);
            __EVENT_RAW(m_ev.touchid, EV_SYN, SYN_REPORT, 0);

            std::this_thread::sleep_for(std::chrono::milliseconds(
                    ((Conf.display.d.max) ? Conf.display.d.max : 100)
                ));

            __EVENT_RAW(m_ev.touchid, EV_SYN, SYN_MT_REPORT, 0);
            __EVENT_RAW(m_ev.touchid, EV_SYN, SYN_REPORT, 0);
        }
        while (0);
    }
};

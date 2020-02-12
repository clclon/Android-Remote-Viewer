
#include "ARemote.h"

namespace Event
{
    static uint16_t validkey[][5] =
    {
#       define KEYDEF(A,B,C,D,E) { A,B,C,D,E },
#       include "extern/KeyCode/keytableTranslate.h"
    };
    //
    static inline void key_exec(uint16_t k, uint16_t d)
    {
        if (!vfork())
        {
            std::string s = Input::execarray[3];
            s.append(std::to_string(k));
            ::execl(
                Input::execarray[0],
                Input::execarray[1],
                Input::execarray[2],
                s.c_str(),
                (char*)nullptr
            );
            LOGE("key_exec failed");
            exit(0);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(((d) ? d : 70)));
    }

    bool Input::keyfilter(uint16_t k)
    {
        // Android key filter,
        // no send printable keys direct to devices..
        switch (k)
        {
            case 7  ... 18:
            case 29 ... 56:
            case 61 ... 77:
            case 81 ... 82: return false;
            default: return true;
        }
    }

    void Input::key(uint16_t k, uint16_t id)
    {
        LOGDC("+  KEY GET: (", k,  ", type: ", id, ")");
        if (id > Input::KeyType::K_WIN)
            throw std::system_error(App::make_error_code(App::ErrorId::error_input_not_key_type));
        if (!k)
            throw std::system_error(App::make_error_code(App::ErrorId::error_input_not_key_value));

        Input::KeyType kt = static_cast<Input::KeyType>(id);

        for (uint32_t i = 0, n = (__NELE(validkey) - 1); i < (__NELE(validkey) / 2); i++, n--)
        {
            uint16_t isfound = ((validkey[i][kt] == k) ? i :
                                ((validkey[n][kt] == k) ? n : 0)
                        );
            if (isfound)
            {
                LOGDC("+  KEY FIND: (", validkey[isfound][Input::KeyType::K_ANDROID], "=", k,  ", type: ", id, ")");
                key(validkey[isfound][Input::KeyType::K_ANDROID]);
                return;
            }
        }
    }

    void Input::key(uint16_t k)
    {
        if (!k)
            return;

        if (m_macro.enable)
            macroAdd(k);

        struct writeevent_t event{};
        do
        {
            if (!keyfilter(k))
                break;

            auto obj = Conf.keymap.find(k);
            if (obj == Conf.keymap.end())
                break;

            int32_t devid = obj->second;
            if (devid < 0)
                break;

            LOGDC("=  HW KEY PRESS: (", k, ":", devid, ")");
            __EVENT_RAW(devid, EV_KEY, k, SYN_CONFIG);
            __EVENT_RAW(devid, EV_SYN, KEY_RESERVED, SYN_REPORT);

            std::this_thread::sleep_for(std::chrono::milliseconds(
                    ((Conf.display.d.max) ? Conf.display.d.max : 100)
                ));

            __EVENT_RAW(devid, EV_KEY, k, SYN_REPORT);
            __EVENT_RAW(devid, EV_SYN, KEY_RESERVED, SYN_REPORT);

            return;
        }
        while (0);
        LOGDC("=  SOFT KEY PRESS: (", k, ")");
        key_exec(k, Conf.display.p.max);
    }
};

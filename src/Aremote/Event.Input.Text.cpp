
#include "ARemote.h"

namespace Event
{
    static inline void text_exec(std::string const & s, uint16_t d)
    {
        if (s.empty())
            return;

        if (!vfork())
        {
            std::stringstream ss;
            ss << Input::execarray[4] << "\"";
            for (auto c : s)
            {
                if (c == ' ')
                    ss << "%s";
                else if (c == '"')
                    ss << "\\\"";
                else if (c == '&')
                    ss << "\\&";
                else
                    ss << c;
            }
            ss << "\"";
            std::string os = ss.str();
            //
            ::execl(
                Input::execarray[0],
                Input::execarray[1],
                Input::execarray[2],
                os.c_str(),
                (char*)nullptr
            );
            LOGE("text_exec failed");
            exit(0);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(((d) ? d : 100)));
    }

    void Input::text(std::string const & s)
    {
        LOGDC("=  TEXT -> \"", s,  "\"");
        text_exec(s, Conf.display.p.max);
        if (m_macro.enable)
            macroAdd(s);
    }
};

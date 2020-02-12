
#include "ARemote.h"
#include <dirent.h>

#define MacroCheck() \
    if (tCheck(m_macro.tend)) { macroStop(); return; }

namespace Event
{
    static inline const char l_macro_name[] = "-macro.xml";
    //
    static inline time_t tDiff(time_t tstart)
    {
        time_t tnow = std::time(nullptr);
        return (tnow - tstart);
    }
    static inline bool tCheck(time_t tend)
    {
        time_t tnow = std::time(nullptr);
        return ((tnow - tend) > 0);
    }
    static inline std::string mName(time_t id)
    {
        std::stringstream ss;
        ss << Conf.cmd.rootdir.c_str() << "/";
        ss << id << l_macro_name;
        auto s = ss.str();
        return s;
    }

    void Input::macroSave(bool b)
    {
        do
        {
            try
            {
                if ((!b) && (!Conf.cmd.ismacrosave))
                    break;

                std::string content = macroGet();
                auto s = mName(m_macro.tstart);
                //
                LOGDC("+  MACRO SAVE ->", s);
                //
                std::ofstream f(s);

                if(!f.is_open())
                    break;

                f << content;
                f.close();
            }
            catch (...)
            {
                LOGEXCEPT();
                throw;
            }
        }
        while (0);
    }

    std::string Input::macroGet(uint32_t id)
    {
        do
        {
            try
            {
                auto s = mName(id);
                //
                LOGDC("+  MACRO LOAD ->", s);
                //
                std::ifstream f(s);

                if(!f.is_open())
                    break;

                std::stringstream ss;
                ss << f.rdbuf();
                auto content = ss.str();
                f.close();
                return content;
            }
            catch (...)
            {
                LOGEXCEPT();
                throw;
            }
        }
        while (0);
        throw std::system_error(App::make_error_code(App::ErrorId::error_input_macro_not_found));
    }

    std::string Input::macroGet()
    {
        if (!m_macro.ev.size())
            throw std::system_error(App::make_error_code(App::ErrorId::error_input_macro_empty));

        macroStop();
        //
        Helper::Xml x("ARemote");

        x.xmlsection("Macro")
         .xmlpair("start", std::to_string(m_macro.tstart))
         .xmlpair("end",   std::to_string(m_macro.tend));

        if (!m_macro.desc.empty())
            x.xmlpair("desc",  m_macro.desc);

        for (auto & item : m_macro.ev)
        {
            x.xmlsection("Event")
            .xmlpair("timediff", std::to_string(item.tdiff));

            switch (item.type)
            {
                case MacroType::M_KEY:
                    {
                        x.xmlpair("type", "KEY")
                         .xmlpair("key", item.k);
                        break;
                    }
                case MacroType::M_TAP:
                    {
                        x.xmlpair("type", "TAP")
                         .xmlpair("cord", item.x, item.y)
                         .xmlpair("long", item.k);
                        break;
                    }
                case MacroType::M_SWIPE:
                    {
                        x.xmlpair("type", "SWIPE")
                         .xmlpair("cord", item.x, item.y)
                         .xmlpair("next", item.xx, item.yy)
                         .xmlpair("long", item.k);
                        break;
                    }
                case MacroType::M_TEXT:
                    {
                        x.xmlpair("type", "TEXT")
                         .xmlpair("text", item.text);
                        break;
                    }
                case MacroType::M_END:
                case MacroType::M_BEGIN:
                    {
                        x.xmlpair("type", ((item.type == MacroType::M_END) ? "END" : "BEGIN"));
                        break;
                    }
            }
        }
        //
        std::string content = x;
        return content;
    }

    std::string Input::macroRecord(std::string const & s, uint32_t duration)
    {
        if (m_macro.enable)
            throw std::system_error(App::make_error_code(App::ErrorId::error_input_macro_running));

        if (m_macro.ev.size())
        {
            macroSave();
            m_macro.ev.clear();
        }
        m_macro.enable = true;
        m_macro.tstart = std::time(nullptr);
        m_macro.tend   = (m_macro.tstart + duration);
        m_macro.desc   = s;

        LOGDC("+  MACRO RECORD -> BEGIN");

        Helper::Xml x("ARemote");

        x.xmlsection("Macro")
         .xmlpair("id", std::to_string(m_macro.tstart));

        if (!m_macro.desc.empty())
            x.xmlpair("desc", m_macro.desc);

        std::string content = x;
        return content;
    }
    void Input::macroStop()
    {
        if (m_macro.enable)
        {
            m_macro.enable = false;
            m_macro.tend   = std::time(nullptr);
            LOGDC("+  MACRO RECORD -> STOP");
        }
        if (m_bplay.load())
        {
            m_bplay = false;
            if (m_thplay.joinable())
                m_thplay.join();

            LOGDC("+  MACRO PLAY -> STOP");
        }
    }
    //
    void Input::macroAdd(uint16_t k)
    {
        MacroCheck()
        //
        macroevent_t me{};
        me.tdiff = tDiff(m_macro.tstart);
        me.type  = MacroType::M_KEY;
        me.k     = k;
        //
        m_macro.ev.push_back(me);
    }
    void Input::macroAdd(uint16_t x, uint16_t y, uint32_t longtap)
    {
        MacroCheck()
        //
        macroevent_t me{};
        me.tdiff = tDiff(m_macro.tstart);
        me.type  = MacroType::M_TAP;
        me.x     = x;
        me.y     = y;
        me.k     = static_cast<uint32_t>(longtap);
        //
        m_macro.ev.push_back(me);
    }
    void Input::macroAdd(uint16_t x, uint16_t y, uint16_t xx, uint16_t yy, uint32_t longtap)
    {
        MacroCheck()
        //
        macroevent_t me{};
        me.tdiff = tDiff(m_macro.tstart);
        me.type  = MacroType::M_SWIPE;
        me.x     = x;
        me.y     = y;
        me.xx    = xx;
        me.yy    = yy;
        me.k     = static_cast<uint32_t>(longtap);
        //
        m_macro.ev.push_back(me);
    }
    void Input::macroAdd(std::string const & s)
    {
        MacroCheck()
        //
        macroevent_t me{};
        me.tdiff = tDiff(m_macro.tstart);
        me.type  = MacroType::M_TEXT;
        me.text  = s;
        //
        m_macro.ev.push_back(me);
    }
    //
    void Input::macroPlay()
    {
        try
        {
            if (m_macro.enable)
                throw std::system_error(App::make_error_code(App::ErrorId::error_input_macro_running));

            if (!m_macro.ev.size())
                throw std::system_error(App::make_error_code(App::ErrorId::error_input_macro_empty));

            macroStop();
            macroRun(m_macro.ev);
        }
        catch (...)
        {
            LOGEXCEPT();
            throw;
        }
    }

    void Input::macroPlay(uint32_t id)
    {
        try
        {
            std::string content = macroGet(id);
            if (content.empty())
                throw std::system_error(App::make_error_code(App::ErrorId::error_input_macro_not_found));

            macroParse(content);
            if (!m_macro.ev.size())
                throw std::system_error(App::make_error_code(App::ErrorId::error_input_macro_empty));

            macroStop();
            macroRun(m_macro.ev);
        }
        catch (...)
        {
            LOGEXCEPT();
            throw;
        }
    }

    void Input::macroPlay(std::string const & content)
    {
        try
        {
            if (content.empty())
                throw std::system_error(App::make_error_code(App::ErrorId::error_input_macro_not_found));

            macroParse(content);
            if (!m_macro.ev.size())
                throw std::system_error(App::make_error_code(App::ErrorId::error_input_macro_empty));

            macroStop();
            macroRun(m_macro.ev);
        }
        catch (...)
        {
            LOGEXCEPT();
            throw;
        }
    }

    std::string Input::macroList()
    {
        //
        try
        {
            struct dirent *de;
            auto d = Helper::auto_shared<DIR>(
                    ::opendir(Conf.cmd.rootdir.c_str()),
                    [](DIR *d_) { ::closedir(d_); }
                );
            if (d == nullptr)
                throw std::system_error(App::make_error_code(App::ErrorId::error_input_macro_not_found));

            Helper::Xml x("ARemote");
            x.xmlsection("MacroList");

            while((de = ::readdir(d.get())) != nullptr)
            {
                std::string s = de->d_name;
                if ((s.length() < 11) ||
                    (s.find(l_macro_name) == std::string::npos))
                    continue;

                uint32_t    xml_start = 0U,
                            xml_end   = 0U;
                std::string xml_desc;
                std::string xml_data;
                {
                    std::stringstream ss;
                    ss << Conf.cmd.rootdir.c_str();
                    ss << "/" << s.c_str();
                    xml_data = ss.str();
                }
                //
                try
                {
                    auto xmlp = Helper::XmlParse(
                                    xml_data,
                                    Helper::XmlParse::XmlInputType::XIT_FILE
                            );
                    xmlp.parse(
                        "ARemote/Macro/Option",
                        { "name", "value", "desc" },
                        [&](std::vector<std::string> const & v)
                        {
                            try
                            {
                                if (v.size() < 2)
                                    return;
                                //
                                std::string const & sv = v[0];
                                     if (sv.compare("start") == 0)
                                        xml_start = std::stoul(v[1]);
                                else if (sv.compare("end") == 0)
                                        xml_end = std::stoul(v[1]);
                                else if (sv.compare("desc") == 0)
                                        xml_desc = v[1];
                            }
                            catch (...)
                            {
                                LOGEXCEPT();
                            }
                        }
                    );
                }
                catch (...)
                {
                    LOGEXCEPT();
                    xml_desc = "bad XML macro file!";
                }
                //
                s = s.substr(0, (s.length() - 10));
                xml_start = (((xml_start) && (xml_end)) ? (xml_end - xml_start) : 0);
                if (!xml_desc.empty())
                    x.xmlpair(s, xml_start, xml_desc);
                else
                    x.xmlpair(s, xml_start, "");
            }
            //
            std::string content = x;
            return content;
        }
        catch (...)
        {
            LOGEXCEPT();
            throw;
        }
    }

    /* private method */

    void Input::macroParse(std::string const & content)
    {
        try
        {
            if (m_macro.ev.size())
                m_macro.ev.clear();

            Input::macroarray_t vm;
            {
                Input::macroevent_t me{};
                vm.push_back(me);
            }
            auto xmlp = Helper::XmlParse(
                            content,
                            Helper::XmlParse::XmlInputType::XIT_STRING
                    );

            xmlp.parse(
                    "ARemote/Macro/Option",
                    { "name", "value", "desc" },
                    [&](std::vector<std::string> const & v)
                    {
                        try
                        {
                            if (v.size() < 2)
                                return;
                            //
                            std::string const & sv = v[0];
                            if (sv.compare("start") == 0)
                                m_macro.tstart = static_cast<time_t>(std::stoul(v[1]));
                            else if (sv.compare("end") == 0)
                                m_macro.tend   = static_cast<time_t>(std::stoul(v[1]));
                            else if (sv.compare("desc") == 0)
                                m_macro.desc   = v[1];
                        }
                        catch (...)
                        {
                            LOGEXCEPT();
                            throw;
                        }
                    }
                );
            xmlp.parse(
                    "ARemote/Event/Option",
                    { "name", "value", "desc" },
                    [&](std::vector<std::string> const & v)
                    {
                        try
                        {
                            if (!v.size())
                                return;
                            //
                            std::string const & s = v[0];
                            uint32_t n = vm.size() - 1;
                            //
                            if (s.compare("timediff") == 0)
                            {
                                if (v.size() < 2)
                                    throw std::system_error(App::make_error_code(App::ErrorId::error_input_macro_bad_xml));

                                if (vm[n].tdiff)
                                {
                                    n++;
                                    Input::macroevent_t me{};
                                    vm.push_back(me);
                                }
                                vm[n].tdiff = std::stoul(v[1]);
                            }
                            else if (s.compare("type") == 0)
                            {
                                if (!vm[n].tdiff)
                                    throw std::system_error(App::make_error_code(App::ErrorId::error_input_macro_bad_index));
                                if (v.size() < 2)
                                    throw std::system_error(App::make_error_code(App::ErrorId::error_input_macro_bad_xml));

                                     if (v[1].compare("KEY") == 0)
                                    vm[n].type = MacroType::M_KEY;
                                else if (v[1].compare("TAP") == 0)
                                    vm[n].type = MacroType::M_TAP;
                                else if (v[1].compare("SWIPE") == 0)
                                    vm[n].type = MacroType::M_SWIPE;
                                else if (v[1].compare("TEXT") == 0)
                                    vm[n].type = MacroType::M_TEXT;
                                else
                                    vm[n].type = MacroType::M_BEGIN;
                                /*
                                else if (v[1].compare("BEGIN") == 0)
                                    return;
                                else if (v[1].compare("END") == 0)
                                    return;
                                */
                            }
                            else if (s.compare("key") == 0)
                            {
                                if (!vm[n].tdiff)
                                    throw std::system_error(App::make_error_code(App::ErrorId::error_input_macro_bad_index));
                                if (v.size() < 2)
                                    throw std::system_error(App::make_error_code(App::ErrorId::error_input_macro_bad_xml));

                                if (vm[n].type == MacroType::M_KEY)
                                    vm[n].k = std::stoul(v[1]);
                            }
                            else if (s.compare("cord") == 0)
                            {
                                if (!vm[n].tdiff)
                                    throw std::system_error(App::make_error_code(App::ErrorId::error_input_macro_bad_index));
                                if (v.size() < 3)
                                    throw std::system_error(App::make_error_code(App::ErrorId::error_input_macro_bad_xml));

                                if ((vm[n].type == MacroType::M_TAP) ||
                                    (vm[n].type == MacroType::M_SWIPE))
                                {
                                    vm[n].x = std::stoul(v[1]);
                                    vm[n].y = std::stoul(v[2]);
                                }
                            }
                            else if (s.compare("next") == 0)
                            {
                                if (!vm[n].tdiff)
                                    throw std::system_error(App::make_error_code(App::ErrorId::error_input_macro_bad_index));
                                if (v.size() < 3)
                                    throw std::system_error(App::make_error_code(App::ErrorId::error_input_macro_bad_xml));

                                if (vm[n].type == MacroType::M_SWIPE)
                                {
                                    vm[n].xx = std::stoul(v[1]);
                                    vm[n].yy = std::stoul(v[2]);
                                }
                            }
                            else if (s.compare("long") == 0)
                            {
                                if (!vm[n].tdiff)
                                    throw std::system_error(App::make_error_code(App::ErrorId::error_input_macro_bad_index));
                                if (v.size() < 2)
                                    throw std::system_error(App::make_error_code(App::ErrorId::error_input_macro_bad_xml));

                                if ((vm[n].type == MacroType::M_TAP) ||
                                    (vm[n].type == MacroType::M_SWIPE))
                                    vm[n].k = std::stoul(v[1]);
                            }
                            else if (s.compare("text") == 0)
                            {
                                if (!vm[n].tdiff)
                                    throw std::system_error(App::make_error_code(App::ErrorId::error_input_macro_bad_index));
                                if (v.size() < 2)
                                    throw std::system_error(App::make_error_code(App::ErrorId::error_input_macro_bad_xml));

                                if (vm[n].type == MacroType::M_TEXT)
                                    vm[n].text = v[1];
                            }
                        }
                        catch (...)
                        {
                            LOGEXCEPT();
                            throw;
                        }
                    }
                );

            m_macro.ev = std::move(vm);
            return;
        }
        catch (...)
        {
            LOGEXCEPT();
            throw;
        }
    }

    void Input::macroRun(Input::macroarray_t const & vm)
    {
        if (m_bplay.load())
            return;

        if (!vm.size())
            return;

        if (m_thplay.joinable())
            m_thplay.join();

        std::thread thmp
        {
            [&]()
            {
                try
                {
                    m_bplay = true;
                    LOGDC("+  MACRO PLAY -> BEGIN");
                    time_t tstart = std::time(nullptr);

                    for (auto & item : vm)
                    {
                        while (true)
                        {
                            if (!m_bplay.load())
                                return;

                            time_t tnow  = std::time(nullptr),
                            tmark = tstart + item.tdiff;
                            if (tnow > tmark)
                                break;

                            std::this_thread::yield();
                            std::this_thread::sleep_for(std::chrono::milliseconds(500));
                        }
                        switch (item.type)
                        {
                            case MacroType::M_KEY:
                                {
                                    key(item.k);
                                    break;
                                }
                            case MacroType::M_TAP:
                                {
                                    tap(item.x, item.y, item.k);
                                    break;
                                }
                            case MacroType::M_SWIPE:
                                {
                                    swipe(item.x, item.y, item.xx, item.yy, item.k);
                                    break;
                                }
                            case MacroType::M_TEXT:
                                {
                                    text(item.text);
                                    break;
                                }
                            default:
                                break;
                        }
                    }
                    LOGDC("+  MACRO PLAY -> END");
                }
                catch (...)
                {
                    m_bplay = false;
                    LOGEXCEPT();
                }
                m_bplay = false;
            }
        };
        m_thplay = move(thmp);
    }

};

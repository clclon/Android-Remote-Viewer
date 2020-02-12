
#include "../../ARemote.h"
#include <cassert>

namespace Lua
{
    int32_t except_handler(lua_State* L, sol::optional<const std::exception&> ex_, sol::string_view desc)
    {
        if (ex_)
        {
            const std::exception& ex = *ex_;
            LOGEX("Lua panic:", ex.what());
        }
		else
        {
            LOGEX("Lua panic:", desc.data());
        }
        return sol::stack::push(L, desc);
    }

    Engine::Engine()
    {
    }

    Engine::~Engine()
    {
        if (m_isrun.load())
            m_isrun = false;
        if (m_thr.joinable())
            m_thr.join();
    }

    void Engine::stop()
    {
        if (m_isrun.load())
            m_isrun = false;
    }

    void Engine::reset_state()
    {
        if (m_isrun.load())
            return;
        m_state   = 0U;
        m_return  = 0U;
        m_savecnt = 0U;
        m_target  = {};
    }

    void Engine::init_cb(sol::state & lua)
    {
        auto modules = lua["aremote"].get_or_create<sol::table>();
                //
        modules.set_function("checkTime", [&](sol::string_view s, sol::string_view e)
                {
                    return Lua::Method::checkTime(this, s, e);
                });
        modules.set_function("checkPixelByPos", [&](uint32_t pos, uint8_t r, uint8_t g, uint8_t b)
                {
                    return Lua::Method::checkPixelByPos(this, pos, r, g, b);
                });
        modules.set_function("checkPixelByPos", [&](uint32_t pos, sol::as_table_t<std::vector<uint8_t>> v)
                {
                    return Lua::Method::checkPixelByPos(this, pos, std::move(v));
                });
        modules.set_function("checkPixelByCord", [&](uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b)
                {
                    return Lua::Method::checkPixelByCord(this, x, y, r, g, b);
                });
        modules.set_function("checkPixelByCord", [&](uint32_t x, uint32_t y, sol::as_table_t<std::vector<uint8_t>> v)
                {
                    return Lua::Method::checkPixelByCord(this, x, y, std::move(v));
                });
                //
        modules.set_function("checkPixelsByPos", [&](ImageLite::LuaArray4 v)
                {
                    return Lua::Method::checkPixelsByPos(this, std::move(v));
                });
        modules.set_function("checkPixelsByCord", [&](ImageLite::LuaArray5 v)
                {
                    return Lua::Method::checkPixelsByCord(this, std::move(v));
                });
                //
        modules.set_function("screenGet", [&]()
                {
                    return Lua::Method::screenGet(this);
                });
        modules.set_function("screenGetRegion", [&](uint32_t x, uint32_t y, uint32_t w, uint32_t h)
                {
                    return Lua::Method::screenGetRegion(this, x, y, w, h);
                });
        modules.set_function("screenGetRegion", [&](uint32_t x, uint32_t y, uint32_t w, uint32_t h)
                {
                    return Lua::Method::screenGetRegion(this, x, y, w, h);
                });
                //
        modules.set_function("imageCompare", [&](sol::string_view s)
                {
                    return Lua::Method::imageCompare(this, s);
                });
        modules.set_function("imageCompareRegion", [&](sol::string_view s, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
                {
                    return Lua::Method::imageCompareRegion(this, s, x, y, w, h);
                });
        modules.set_function("imageGetPosition", [&](uint32_t x, uint32_t y)
                {
                    return Lua::Method::imageGetPosition(this, x, y);
                });
        modules.set_function("imageDefault", [&](sol::string_view s)
                {
                    Lua::Method::imageDefault(this, s);
                });
        modules.set_function("imageShow", [&](sol::string_view s)
                {
                    Lua::Method::imageShow(this, s);
                });
        modules.set_function("imageSave", [&](sol::string_view s, uint32_t n)
                {
                    Lua::Method::imageSave(this, s, n);
                });
        modules.set_function("imageTableShow", [&](ImageLite::ImageTable itbl)
                {
                    Lua::Method::imageTableShow(this, std::move(itbl));
                });
                //
        modules.set_function("adbClick", [&](uint32_t x, uint32_t y)
                {
                    Lua::Method::adbClick(this, x, y);
                });
        modules.set_function("adbSwipe", [&](uint32_t x, uint32_t y, uint32_t xx, uint32_t yy, uint32_t d)
                {
                    Lua::Method::adbSwipe(this, x, y, xx, yy, d);
                });
        modules.set_function("adbKey", [&](uint16_t k)
                {
                    Lua::Method::adbKey(this, k);
                });
        modules.set_function("adbText", [&](sol::string_view s)
                {
                    Lua::Method::adbText(this, s);
                });
                //
        modules.set_function("stateGet", [&]()
                {
                    return Lua::Method::stateGet(this);
                });
        modules.set_function("stateSet", [&](uint32_t n)
                {
                    Lua::Method::stateSet(this, n);
                });
        modules.set_function("stateSleep", [&](uint32_t n)
                {
                    Lua::Method::stateSleep(this, n);
                });
                //
        modules.set_function("traceOn", [&]()
                {
                    Lua::Method::traceOn(this);
                });
        modules.set_function("traceOff", [&]()
                {
                    Lua::Method::traceOff(this);
                });
                //
        modules.set_function("captureOn", [&]()
                {
                    Lua::Method::captureOn(this);
                });
        modules.set_function("captureOff", [&]()
                {
                    Lua::Method::traceOff(this);
                });
                //
        modules.set_function("stop", [&]()
                {
                    Lua::Method::exitNow(this);
                });
                //
        ImageLite::Lua::register_lua_LuaImage(modules);
    }

    void Engine::load(sol::string_view const & s, std::error_code & ec)
    {
        if (m_isrun.load())
            return;

        try
        {
            std::string fname;
            {
                std::stringstream ss;
                ss << Conf.cmd.rootdir.c_str() << "/";
                ss << s.data();
                fname = ss.str();
            }
            std::ifstream f(fname);
            if(!f.is_open())
            {
                ec = Lua::make_error_code(Lua::ErrorId::error_lua_not_access);
                throw std::system_error(ec);
            }
            std::stringstream ss;
            ss << f.rdbuf();
            auto script = ss.str();
            f.close();
            //
            run(script, ec);
        }
        catch (...)
        {
            throw;
        }
    }

    void Engine::run(sol::string_view const & s, std::error_code & ec)
    {
        if (m_isrun.load())
            return;
        if (m_thr.joinable())
            m_thr.join();

        reset_state();

        if (s.empty())
        {
            ec = Lua::make_error_code(Lua::ErrorId::error_lua_empty_source);
            throw std::system_error(ec);
        }

        m_isrun = true;
        //
        try
        {
                    sol::state luatest;
                    //
                    luatest.set_exception_handler(&Lua::except_handler);
                    sol::load_result lcode = luatest.load(s);
                    if (!lcode.valid())
                    {
                        sol::error lerr = lcode;
                        sol::load_status status = lcode.status();
                        //
                        ec = Lua::make_error_code(Lua::ErrorId::error_lua_bad_source);
                        throw make_system_error(
                                ec,
                                lerr.what(),
                                sol::to_string(status)
                            );
                    }
                    //
                    sol::protected_function script = lcode;
                    m_target = script.dump();
        }
        catch (...)
        {
            m_isrun = false;
            throw;
        }
        //
        std::thread thmp
        {
            [&]()
            {
                try
                {
                    sol::state luaexec;
                    luaexec.open_libraries(sol::lib::base);
                    luaexec.set_exception_handler(&Lua::except_handler);
                    ImageLite::Lua::register_lua_imageTable(luaexec);
                    init_cb(luaexec);

                    auto result = luaexec.safe_script(m_target.as_string_view(), sol::script_pass_on_error);
                    if (!result.valid())
                    {
                        sol::error lerr = result;
                        sol::call_status status = result.status();
                        //
                        throw make_system_error(
                                Lua::ErrorId::error_lua_bad_exec,
                                lerr.what(),
                                sol::to_string(status)
                            );
                    }
                    //
                    sol::function fmain = luaexec["main"];
                    if (!fmain.valid())
                    {
                        throw std::system_error(
                                Lua::make_error_code(Lua::ErrorId::error_lua_not_main)
                            );
                    }
                    //
                    while (m_isrun.load())
                    {
                        m_return = fmain.call<uint32_t>(m_state.load());
                        LOGDC("+  LUA function 'main(state)' return:", m_return.load());

                        auto t_end = (std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(500));
                        while (std::chrono::high_resolution_clock::now() < t_end)
                        {
                            if (!m_isrun.load())
                                return;
                            std::this_thread::yield();
                            std::this_thread::sleep_for(std::chrono::milliseconds(15));
                        }
                    }
                }
                catch (...)
                {
                    m_isrun = false;
                    LOGEXCEPT();
                }
                m_isrun = false;
            }
        };
        m_thr = move(thmp);
    }
}

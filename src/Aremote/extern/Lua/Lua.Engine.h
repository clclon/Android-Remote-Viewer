
#pragma once

namespace Lua
{
    class Engine
    {
        private:
            //
            std::thread           m_thr;
            sol::bytecode         m_target;
            //
            void init_cb(sol::state&);
            void reset_state();

        public:
            int32_t i = 199;
            std::atomic<bool>     m_isrun   = false;
            std::atomic<uint32_t> m_state   = 0U;
            std::atomic<uint32_t> m_return  = 0U;
            std::atomic<uint32_t> m_savecnt = 0U;
            //
            Engine();
            ~Engine();
            //
            void load(sol::string_view const&, std::error_code&);
            void run(sol::string_view const&,  std::error_code&);
            void stop();
    };
}


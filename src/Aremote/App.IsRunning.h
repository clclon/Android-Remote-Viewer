
#pragma once

namespace App
{
    class IsRunning
    {
        public:
            //
            IsRunning(std::string const&);
            virtual ~IsRunning();
            //
    };
    //
    class IsRunningFound : public std::exception
    {
        public:
            //
            int32_t     mpid_ = -1,
                        cpid_ = -1;
            std::string msg_;
            //
            IsRunningFound(int32_t m, int32_t c, std::string const & msg)
                : mpid_(m), cpid_(c), msg_(msg) {}
            virtual ~IsRunningFound() {}
            //
            const char * what() const noexcept override
            {
                std::stringstream ss;
                ss << msg_;
                ss << ", PID: " << mpid_ << "/" << cpid_;
                auto s = ss.str();
                return s.c_str();
            }
    };
};

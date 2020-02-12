
#include "ARemote.h"
#include <dirent.h>

namespace App
{
    static inline const char l_procdir[]  = "/proc/";

    IsRunning::IsRunning(std::string const & a)
    {
        try
        {
            if (a.empty())
                throw std::system_error(make_error_code(ErrorId::error_running_name_empty));

            std::string cmd;
            size_t pos = a.find_last_of("/");
            if (pos != std::string::npos)
                cmd = a.substr((pos + 1), a.length());
            else
                cmd = a;

            struct dirent *de;
            auto d = Helper::auto_shared<DIR>(
                    ::opendir(l_procdir),
                    [](DIR *d_) { ::closedir(d_); }
                );
            if (d == nullptr)
                throw std::system_error(make_error_code(ErrorId::error_running_open_proc));

            int32_t mpid = ::getpid();
            if (mpid <= 0)
                throw std::system_error(make_error_code(ErrorId::error_running_get_pid));

            while((de = ::readdir(d.get())) != nullptr)
            {
                if (!std::isdigit(de->d_name[0]))
                    continue;

                int32_t cpid = 0;

                try
                {
                    std::string s = de->d_name;
                    cpid = std::stoi(s);
                    if ((!cpid) || (cpid == mpid))
                        continue;
                    //
                    {
                        std::stringstream ss;
                        ss << "/proc/" << cpid << "/cmdline";
                        s = ss.str();
                    }
                    //
                    std::ifstream f(s);
                    if(!f.is_open())
                        continue;
                    //
                    do
                    {
                        std::stringstream ss;
                        ss << f.rdbuf();
                        s = ss.str();
                        //
                        pos = s.find_last_of("/");
                        if (pos != std::string::npos)
                            s = s.substr((pos + 1), s.length());

                        if (s.empty())
                            break;

                        bool isempty = true;
                        for (auto c : s)
                        {
                            if ((c == '\n') ||
                                (c == '\r') ||
                                (c == '\t') ||
                                (c == '\v') ||
                                (c == '\0') ||
                                (c == ' '))
                                continue;
                            isempty = false;
                            break;
                        }
                        if (isempty)
                            break;

                        LOGDC("?  COMPARE: [",
                              cmd.c_str(),
                              "] :",
                              cmd.length(),
                              ", [",
                              s.c_str(),
                              "] :",
                              s.length(),
                              ", = (",
                              s.compare(0, s.length(), cmd.c_str(), s.length()),
                              ")"
                        );
                        //
                        if (s.compare(0, s.length(), cmd.c_str(), s.length()) == 0)
                        {
                            f.close();
                            throw IsRunningFound(mpid, cpid, App::geterror(App::ErrorId::error_running_found));
                        }
                        //
                        LOGDC("=  SKIP: ", cmd.c_str(), "->", s.c_str(), ":", cpid);
                    }
                    while (0);
                    f.close();
                }
                catch (IsRunningFound const&) { throw; }
                catch (...) { continue; }
            }
        }
        catch (...)
        {
            throw;
        }
    }

    IsRunning::~IsRunning() {}
};

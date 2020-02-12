
#if defined(_BUILD_SINGLE)
#  include <iostream>
#  include <fstream>
#  include <string>
#  include <sstream>
#  include <atomic>
#  include <memory>
#  include <mutex>
#  if defined(ANDROID)
#    include <stdio.h>
#    include <android/log.h>
#  endif
#  define __NELE(a) (sizeof(a) / sizeof(a[0]))
#  include "HelperLog.h"
#else
#  include "../../ARemote.h"
#endif

#if defined(__clang__)
#  include <cxxabi.h>
#endif

template<typename T>
std::string printableType_( [[ maybe_unused ]] T & val)
{
    std::string s;
#   if defined(__clang__)
      int32_t i;
      s = abi::__cxa_demangle(typeid(val).name(), 0, 0, &i);
#   endif
    return s;
}

namespace Helper
{
    static inline char             l_default_log_name[]  = "App";
    static inline char             l_default_exception[] = "unknown type exception";
    static inline const char      *l_severity_label[] =
    {
        "",
        "",
        "EXCEPTION",
        "ERROR",
        "WARN",
        "INFO",
        "DEBUG",
    };
    static inline const char      *l_type_ext[] =
    {
        ".info.log",
        ".error.log"
    };
    static FILE                   *l_handle[] =
    {
        stdout,
        stderr
    };
    //
    LogConfig::LogConfig()
        : m_name(l_default_log_name),
          m_type(LogSeverity::NONE)
    {
        static_assert(__NELE(l_type_ext) == __NELE(l_handle));
        m_output = LogOutType::STD_OUT;
    }
    LogConfig::~LogConfig() noexcept
    {
        std::cerr.flush();
        std::cout.flush();
    }

    LogConfig & LogConfig::instance()
    {
        static LogConfig m_instance{};
        return m_instance;
    }
    void LogConfig::fileOutInit()
    {
        for (uint32_t i = 0; i < std::size(l_type_ext); i++)
        {
            if (m_fp[i])
                continue;

            errno = 0;
            std::string s;
            {
                std::stringstream ss;
#               if defined(ANDROID)
                ss << "/data/local/tmp/";
#               endif
                ss << m_name.c_str() << l_type_ext[i];
                s = ss.str();
            }
            m_fp[i] = ::freopen(s.c_str(), "w", l_handle[i]);
            if (!m_fp[i])
            {
                if (errno)
                    s += ::strerror(errno);
                throw std::runtime_error(s.c_str());
            }
        }
    }
    void LogConfig::setName(std::string const & s)
    {
        if (s.empty())
            return;
        m_name = s;
        getFilename(m_name);
        if (m_name.empty())
            m_name = l_default_log_name;
    }
    void LogConfig::setSeverity(LogSeverity t)
    {
        m_type = t;
    }
    LogSeverity LogConfig::getSeverity()
    {
        return m_type.load();
    }
    LogSeverity LogConfig::getSeverity(std::string const & s)
    {
        for (uint32_t i = 0U; i < std::size(l_severity_label); i++)
        {
            if (s.compare(l_severity_label[i]) == 0)
                return static_cast<LogSeverity>(i);
        }
        return LogSeverity::WARN;
    }
    void LogConfig::setOut(LogOutType t)
    {
        if (m_output == LogOutType::STD_FILE)
            return;
        if (t == LogOutType::STD_FILE)
            fileOutInit();

        m_output = t;
    }
    LogOutType LogConfig::getOut(LogOutType t)
    {
        switch (t)
        {
            case LogOutType::STD_NONE:
            case LogOutType::STD_FILE:
            case LogOutType::STD_ANDROID: return m_output.load();
            default:                      return t;
        }
    }
    void LogConfig::getFilename(std::string & s)
    {
        std::size_t pos = s.find_last_of("/\\");
        if (pos != std::string::npos)
            s = s.substr((pos + 1), s.length());
    }
    void LogConfig::androidOut(std::string const & s)
    {
#       if defined(ANDROID)
        uint32_t id;
        switch (m_type.load())
        {
            case LogSeverity::INFO:  id = ANDROID_LOG_INFO;    break;
            case LogSeverity::DEBUG: id = ANDROID_LOG_DEBUG;   break;
            case LogSeverity::WARN:  id = ANDROID_LOG_WARN;    break;
            case LogSeverity::ERROR: id = ANDROID_LOG_ERROR;   break;
            case LogSeverity::CLEAR: id = ANDROID_LOG_DEFAULT; break;
            case LogSeverity::NONE:  id = ANDROID_LOG_SILENT;  break;
            default:                 id = ANDROID_LOG_UNKNOWN; break;
        }
        __android_log_print(id, m_name.c_str(), "%s", s.c_str());
#       endif
    }
    //
    template<class T>
    void LogConfig::head(
        T *o,
        LogSeverity t,
        bool b,
        std::string const & fn,
        std::string const & fp,
        std::string const & ff,
        int32_t ln)
    {
        if (!b)
            return;

        LogOutType  ot;
        LogSeverity st = ((t == LogSeverity::NONE) ?
                    m_type.load() : t
                );
        switch (st)
        {
            case LogSeverity::WARN:
            case LogSeverity::ERROR:
            case LogSeverity::EXCEPT:
                ot = getOut(LogOutType::STD_ERR);
                break;
            default:
                ot = getOut(LogOutType::STD_OUT);
                break;
            case LogSeverity::NONE:
                return;
        }
        switch (st)
        {
            case LogSeverity::DEBUG:
            {
                std::string s = fn;
                getFilename(s);
                o->print(ot, "[", l_severity_label[st], "->", s, ":", ln, "->", ff, "] ->\n  [", fp, "]:\n    ");
                break;
            }
            case LogSeverity::INFO:
            {
                std::string s = fn;
                getFilename(s);
                o->print(ot, "[", l_severity_label[st], "->", s, ":", ln, "->", ff, "]:\n  ");
                break;
            }
            case LogSeverity::WARN:
            case LogSeverity::ERROR:
            {
                o->print(ot, "[", l_severity_label[st], "]: ");
                break;
            }
            case LogSeverity::EXCEPT:
            {
                std::string s = fn;
                getFilename(s);
                o->print(ot, "[", l_severity_label[st], "->", s, ":", ln, "]: ");
                break;
            }
            default:
                break;
        }
    }
    //
    bool LogConfig::getException( [[ maybe_unused ]] std::exception_ptr const & pe)
    {
        std::stringstream ss;

        try
        {
            if (pe)
            {
#               if defined(__GNUC__)
#                 if defined(__clang__)
                    int32_t ret;
                    std::string extype = abi::__cxa_demangle(abi::__cxa_current_exception_type()->name(), 0, 0, &ret);
#                 else
                    std::string extype = pe.__cxa_exception_type()->name();
#                 endif
#               else
                  std::string extype;
#               endif
                if (!extype.empty())
                    ss << extype.c_str() << " : ";
                std::rethrow_exception(pe);
            }
        }
        catch(std::system_error const & e)  { ss << e.what(); }
        catch(std::runtime_error const & e) { ss << e.what(); }
        catch(std::exception const & e)     { ss << e.what(); }
        catch(std::string const & s)        { ss << s.c_str(); }
        catch(const char *s)                { ss << s; }
        catch(intmax_t i)                   { ss << i; }
        catch(uintmax_t u)                  { ss << u; }
        catch(...)                          { ss << l_default_exception; }
        //
        auto s = ss.str();
        if (!s.empty())
            toOut(LOGTYPEOUT(STD_ERR), s);
        else
            return false;
        return true;
    }
    //
    LOGSEVERITY_EXPAND_TEMPLATE(, NONE);
    LOGSEVERITY_EXPAND_TEMPLATE(, CLEAR);
    LOGSEVERITY_EXPAND_TEMPLATE(, DEBUG);
    LOGSEVERITY_EXPAND_TEMPLATE(, INFO);
    LOGSEVERITY_EXPAND_TEMPLATE(, WARN);
    LOGSEVERITY_EXPAND_TEMPLATE(, ERROR);
    LOGSEVERITY_EXPAND_TEMPLATE(, EXCEPT);
};

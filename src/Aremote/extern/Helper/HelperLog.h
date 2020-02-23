
#pragma once
#define LogConfiguration()  Helper::LogConfig::instance()
#define LOGTYPEOUT(A)       LogConfiguration().getOut(LogOutType::A)
#define LOGTYPEOUTDEFAULT() LogConfiguration().getOut()

namespace Helper
{
    enum LogSeverity
    {
        NONE = 0,
        CLEAR,
        EXCEPT,
        ERROR,
        WARN,
        INFO,
        DEBUG
    };
    enum LogOutType
    {
        STD_NONE,
        STD_OUT,
        STD_ERR,
        STD_FILE,
        STD_ANDROID
    };

    class LogConfig
    {
        private:
            //
            FILE                          *m_fp[2]{};
            std::string                    m_name;
            std::atomic<LogSeverity>       m_type;
            std::atomic<LogOutType>        m_output;
            std::mutex                     m_lock;
            //
            LogConfig();
            virtual ~LogConfig() noexcept;
            //
            void androidOut(std::string const &);
            void fileOutInit();

        public:
            //
            std::atomic<bool>    islabel = false;
            //
            static LogConfig & instance();
            //
			void        getFilename(std::string&);
            void        setName(std::string const&);
            void        setSeverity(LogSeverity);
            void        setOut(LogOutType);
            LogOutType  getOut(LogOutType = LogOutType::STD_NONE);
            LogSeverity getSeverity();
            LogSeverity getSeverity(std::string const&);
            bool        getException(std::exception_ptr const&);
            //
            template<class T>
            void head(
                T*,
                LogSeverity,
                bool,
                std::string const&,
                std::string const&,
                std::string const&,
                int32_t);
            //
            template <typename T>
            LogConfig & toOut(LogOutType t, T const & arg)
            {
                switch (t)
                {
                    case LogOutType::STD_ANDROID:
                        {
                            std::stringstream ss;
                            ss << arg;
                            {
                                std::lock_guard<std::mutex> l_lock(m_lock);
                                androidOut(ss.str());
                            }
                            break;
                        }
                    case LogOutType::STD_FILE:
                    case LogOutType::STD_OUT:
                        {
                            std::lock_guard<std::mutex> l_lock(m_lock);
                            std::cout << arg;
                            // std::cout.flush();
                            break;
                        }
                    case LogOutType::STD_ERR:
                        {
                            std::lock_guard<std::mutex> l_lock(m_lock);
                            std::cerr << arg;
                            std::cerr.flush();
                            break;
                        }
                    default:  break;
                }
                return *this;
            }
    };

    template <LogSeverity Ttype>
	class ToLog final
	{
		private:
			//
			bool m_isend = false;
			void setsend()
			{
			    m_isend = ((!m_isend) ? true : m_isend);
			}

		public:
			//
			ToLog() {}
			~ToLog()
			{
			   if (m_isend)
                    LogConfiguration().toOut(LOGTYPEOUTDEFAULT(), "\n");
			}
			//
			ToLog(
                	std::string const & fn,
                	std::string const & fp,
                	std::string const & ff,
                	int32_t ln)
			{
			    if (Ttype > LogConfiguration().getSeverity())
                                return;
			    LogConfig::instance().head<ToLog<Ttype>>(
                        this,
                        Ttype,
                        LogConfiguration().islabel,
                        fn,
                        fp,
                        ff,
                        ln);
			}
			ToLog(
                	bool b,
                	std::string const & fn,
                	std::string const & fp,
                	std::string const & ff,
                	int32_t ln)
			{
			    if (Ttype > LogConfiguration().getSeverity())
                                return;
			    LogConfig::instance().head<ToLog<Ttype>>(
                        this,
                        Ttype,
                        b,
                        fn,
                        fp,
                        ff,
                        ln);
			}
            ToLog & operator << (std::exception_ptr const & pe)
            {
                if (Ttype > LogConfiguration().getSeverity())
                    return *this;

                if (LogConfiguration().getException(pe))
                    setsend();
                return *this;
            }
            template<typename Targ>
            ToLog & operator << (Targ const & msg)
            {
                if (Ttype > LogConfiguration().getSeverity())
                    return *this;

                LogConfiguration().toOut(LOGTYPEOUTDEFAULT(), msg);
                setsend();
                return *this;
            }
            template<typename Arg, typename...Args>
            ToLog & print(LogOutType t, Arg && arg, Args && ... args)
            {
                if (Ttype > LogConfiguration().getSeverity())
                    return *this;

                std::stringstream ss;
                ((ss << ' ' << std::forward<Args>(args)), ...);
                LogConfiguration().toOut(t, std::forward<Arg>(arg));
                LogConfiguration().toOut(t, ss.str());
                setsend();
                return *this;
            }
            void print_if(std::function<std::string()> fun, bool b = false)
            {
                std::string s = fun();
                if (b)
                    throw std::runtime_error(s.c_str());

                if (s.empty())
                    return;
                LogConfiguration().toOut(LOGTYPEOUT(STD_ERR), s);
                setsend();
            }
            void print_if(std::string const & s, bool b = false)
            {
                if (b)
                    throw std::runtime_error(s.c_str());

                if (s.empty())
                    return;
                LogConfiguration().toOut(LOGTYPEOUT(STD_ERR), s);
                setsend();
            }
	};

#   define LOGSEVERITY_EXPAND_TEMPLATE(A,B)                                          \
        A template void LogConfig::head<ToLog<LogSeverity::B>>                       \
            (ToLog<LogSeverity::B>*,  LogSeverity, bool,                             \
             std::string const&, std::string const&, std::string const&, int32_t)

    LOGSEVERITY_EXPAND_TEMPLATE(extern, NONE);
    LOGSEVERITY_EXPAND_TEMPLATE(extern, CLEAR);
    LOGSEVERITY_EXPAND_TEMPLATE(extern, DEBUG);
    LOGSEVERITY_EXPAND_TEMPLATE(extern, INFO);
    LOGSEVERITY_EXPAND_TEMPLATE(extern, WARN);
    LOGSEVERITY_EXPAND_TEMPLATE(extern, ERROR);
    LOGSEVERITY_EXPAND_TEMPLATE(extern, EXCEPT);
};

using LogSeverity = Helper::LogSeverity;
using LogOutType  = Helper::LogOutType;

#if defined(_DEBUG)
#  define LOGDD(...)  LOG_(DEBUG,true).print(LOGTYPEOUTDEFAULT(), __VA_ARGS__)
#  define LOGDI(...)  LOG_(INFO, true).print(LOGTYPEOUTDEFAULT(), __VA_ARGS__)
#  define LOGDC(...)  LOG__(CLEAR).print(LOGTYPEOUTDEFAULT(),     __VA_ARGS__)
#else
#  define LOGDD(...)
#  define LOGDI(...)
#  define LOGDC(...)
#endif

#define LOGD(...)    LOG_(DEBUG, true).print(LOGTYPEOUTDEFAULT(), __VA_ARGS__)
#define LOGI(...)    LOG_(INFO,  true).print(LOGTYPEOUTDEFAULT(), __VA_ARGS__)
#define LOGW(...)    LOG_(WARN,  true).print(LOGTYPEOUT(STD_ERR), __VA_ARGS__)
#define LOGE(...)    LOG_(ERROR, true).print(LOGTYPEOUT(STD_ERR), __VA_ARGS__)
#define LOGEX(...)   LOG_(EXCEPT,true).print(LOGTYPEOUT(STD_ERR), __VA_ARGS__)
#define LOGEXEMPTY() LOG_(EXCEPT,true).print(LOGTYPEOUT(STD_ERR), __PRETTY_FUNCTION__, __FUNCTION__, __LINE__, "Unknown exception")
#define LOG(...)     LOG__(CLEAR).print(LOGTYPEOUTDEFAULT(),     __VA_ARGS__)
#define LOGSTREAM()  LOG__(CLEAR)
#define LOGEXCEPT()  LOG_(EXCEPT,true) << std::current_exception()
#define LOG_IF(X,A)       { if (__builtin_expect(!!(X), 0)) LOG_(ERROR,true).print_if(A); }
#define LOG_IF_THROW(X,A) { if (__builtin_expect(!!(X), 0)) LOG_(ERROR,true).print_if(A, true); }
#define LOG_CATCH()       catch (...) { LOGEXCEPT(); }
//
#define LOG_(A,B) Helper::ToLog<Helper::LogSeverity::A>(B, __FILE__, __PRETTY_FUNCTION__, __FUNCTION__, __LINE__)
#define LOG__(A)  Helper::ToLog<Helper::LogSeverity::A>()

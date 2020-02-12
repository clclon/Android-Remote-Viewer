
#include "ARemote.h"
namespace Net
{

    IP::IP()
    {
        init();
    }

    IP::~IP()
    {
    }

    std::string IP::get()
    {
        return m_ipa;
    }

    std::string IP::get(std::string const & iface)
    {
        std::string os;

        try
        {
            FILE *fp = nullptr;

            do
            {
                if (!(fp = ::popen("ifconfig", "r")))
                    break;

                char  *p       = nullptr;
                bool   isfound = false;
                size_t sz;

                while ((::getline(&p, &sz, fp) > 0) && (p))
                {
                    std::size_t pos;
                    std::string s(p, p + sz);
                    if ((isfound) &&
                        ((pos = s.find("inet addr:")) != std::string::npos))
                    {
                        std::size_t sp;
                        s = s.substr(pos + 10, s.length()- pos - 10);
                        if ((sp = s.find_first_of(" ")) != std::string::npos)
                            os  = s.substr(0, sp);
                        else
                            os  = s;
                        break;
                    }
                    else if (s.find(iface.c_str()) != std::string::npos)
                        isfound = true;
                }
            }
            while (0);

            if (fp)
                ::pclose(fp);
        }
        catch (...) {}
        return os;
    }

    bool IP::init()
    {
        try
        {
            asio::io_service               is_;
            asio::ip::udp::resolver        r_(is_);
            asio::ip::udp::resolver::query q_(asio::ip::udp::v4(), "8.8.8.8", "53");
            asio::ip::udp::socket          sk_(is_);
            sk_.connect(*r_.resolve(q_));
            m_ipa = sk_.local_endpoint().address().to_string();
        }
        catch (...) {}

        if (!m_ipa.empty())
            return true;

        m_ipa = get("wlan");

        return (!m_ipa.empty());
    }

};

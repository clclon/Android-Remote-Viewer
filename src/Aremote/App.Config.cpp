
#include "ARemote.h"

namespace App
{
    //
    Helper::CatalogMap l_evtype_map = Helper::CatalogMapInit
#   define __EVID(A,B) (A, B)
#   include "Event.Input.TypeHelper.h"
	;
	//
    Helper::CatalogMap l_evkey_map = Helper::CatalogMapInit
#   define __KEYID(A,E,D) (A, D)
#   include "Event.Input.KeyHelper.h"
	;
	//

    Config::Config()
    {
        //
        std::map<App::UriId, urituple_t> urim_ = Helper::Map<App::UriId, urituple_t>
#       define __URLAPI(A,B,C,D,E)        \
            (                             \
                App::UriId::uri_ ## A,    \
                { B, C, D, E }            \
            )
#       include "App.Uri.Items.h"
        ;
        urimap = move(urim_);
    }

    Config::~Config()
    {
    }

    Config& Config::instance()
    {
        static Config m_instance{};
        return m_instance;
    }

    bool Config::init(int32_t argc, char *argv[])
    {
        return cmd.process(argc, argv);
    }

    std::string Config::geturi(App::UriId id)
    {
        auto obj = urimap.find(id);
        if (obj == urimap.end())
            return std::string();
        return std::get<0>(obj->second);
    }

    void Config::helperKeyDesc(uint16_t key, std::string & s)
    {
        auto obj = l_evkey_map.find(key);
        if (obj == l_evkey_map.end())
            s = "";
        else
            s = obj->second;
    }

    void Config::helperKeyType(uint16_t key, std::string & s)
    {
        auto obj = l_evtype_map.find(key);
        if (obj == l_evtype_map.end())
            s = "";
        else
            s = obj->second;
    }

    void Config::helperKeyPrint(std::map<uint16_t, int32_t> const & m)
    {
        for (auto & [ key, id ] : m)
        {
            std::string s;
            helperKeyDesc(key, s);
            std::cout << "\t- " << key;
            if (!s.empty())
                std::cout << " [" << s.c_str() << "]";
            std::cout << " (" << id << ")" << std::endl;
        }
    }
};

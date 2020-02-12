
#pragma once

namespace App
{
    enum UriId
    {
#       define __URLAPI(A,...) uri_ ## A,
#       include "App.Uri.Items.h"
        uri_end
    };
    //
    class Config
    {
        private:
            //
            Config();
            ~Config();
            Config(const Config&) = delete;
            Config& operator=(const Config&) = delete;

        public:
            //
            using urituple_t = std::tuple<std::string, std::string, std::string, std::string>;
            //
            Net::IP                          ip;
            App::Cmdl                        cmd;
            Event::ADisplay::Screen          display{};
            std::map<uint16_t, int32_t>      keymap;
            std::map<App::UriId, urituple_t> urimap;
            //
            Event::Input                   * class_input = nullptr;
            Event::ADisplay                * class_display = nullptr;
            //
            static Config& instance();
            //
            bool init(int32_t, char *[]);
            void helperKeyDesc(uint16_t, std::string&);
            void helperKeyType(uint16_t, std::string&);
            void helperKeyPrint(std::map<uint16_t, int32_t> const&);
            //
            std::string geturi(App::UriId);
    };
    //
    class CaptureTrigger final
    {
        public:
            //
            CaptureTrigger()
            {
                if (App::Config::instance().class_display)
                    App::Config::instance().class_display->clientcountUp();
            }
            ~CaptureTrigger()
            {
                if (App::Config::instance().class_display)
                    App::Config::instance().class_display->clientcountDown();
            }
    };
}

#if defined(Conf)
#  undef Conf
#endif
#define Conf App::Config::instance()


#pragma once

namespace App
{
    enum CmdlCatId
    {
#       define __CMDLCAT(A,B,C)       cmdl_cat_ ## B,
#       include "App.Cmdl.Items.h"
        cmdl_cat_end
    };
    enum CmdlItemId
    {
#       define __CMDLITEM1(A,B,...)   cmdl_item_ ## B,
#       define __CMDLITEM2(A,B,C,...) cmdl_item_ ## C,
#       include "App.Cmdl.Items.h"
        cmdl_item_end
    };
    //
    class Cmdl
    {
        public:
            //
            bool                iswlanip    = false;
            bool                ismacrosave = false;
            int32_t             srvport     = 0;
            std::string         rootdir;
            Helper::LogSeverity logservity = Helper::LogSeverity::DEBUG;
            Helper::LogOutType  logtypeout = Helper::LogOutType::STD_OUT;
            //
            Cmdl();
            virtual ~Cmdl();
            //
            bool        process(int32_t, char *[]);

        private:
            //
            std::optional<std::tuple<App::CmdlCatId, std::string, std::string, std::string>> getitem(App::CmdlItemId);
            std::optional<std::string> getcat(App::CmdlCatId);
            void printh(std::stringstream&);
    };
};

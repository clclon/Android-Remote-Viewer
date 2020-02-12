
#include "ARemote.h"
#include "extern/argh.h"

namespace App
{
#   define TUPLE_ITEM_ std::tuple<App::CmdlCatId, std::string, std::string, std::string>
#   define MAP_ITEM_   App::CmdlItemId, TUPLE_ITEM_
    std::map<MAP_ITEM_> cmdlitem_map = Helper::Map<MAP_ITEM_>
#       define __CMDLITEM(A,B,C,D)  (                                                 \
                                         App::CmdlItemId::cmdl_item_ ## C,            \
                                       { static_cast<App::CmdlCatId>(A), B, #C, D }   \
                                    )
#       define __CMDLITEM1(A,C,D)   __CMDLITEM(A,std::string(),C,D)
#       define __CMDLITEM2(A,B,C,D) __CMDLITEM(A,B,C,D)
#       include "App.Cmdl.Items.h"
#       undef  __CMDLITEM
        ;

#   define MAP_CAT_ App::CmdlCatId, std::string
    std::map<MAP_CAT_> cmdlcat_map = Helper::Map<MAP_CAT_>
#       define __CMDLCAT(A,B,C) (static_cast<App::CmdlCatId>(A), { C })
#       include "App.Cmdl.Items.h"
        ;

    Cmdl::Cmdl()
    {
    }

    Cmdl::~Cmdl()
    {

    }

    std::optional<TUPLE_ITEM_> Cmdl::getitem(App::CmdlItemId id)
    {
        auto obj = cmdlitem_map.find(id);
        if (obj == cmdlitem_map.end())
            return {};
        return { obj->second };
    }

    std::optional<std::string> Cmdl::getcat(App::CmdlCatId id)
    {
        auto obj = cmdlcat_map.find(id);
        if (obj == cmdlcat_map.end())
            return {};
        return { obj->second };
    }

    void Cmdl::printh(std::stringstream & ss)
    {
        ss << "  Aremote v." << AINPUT_FULLVERSION_STRING
           << ", build: "  << AINPUT_DATE << "-" << AINPUT_MONTH << "-" << AINPUT_YEAR
           << std::endl
           << "  Git: https://clclon.github.io/Android-Remote-Viewer"
           << std::endl;
    }

    bool Cmdl::process(int32_t argc, char *argv[])
    {
        try
        {
            std::string s;
            auto cmdl = argh::parser(
                            argc,
                            argv,
                            argh::parser::PREFER_PARAM_FOR_UNREG_OPTION
                 );
            //
            auto item = getitem(CmdlItemId::cmdl_item_wlanip);
            if (item.has_value())
                iswlanip  = cmdl[{
                     std::get<1>(item.value()).c_str(),
                     std::get<2>(item.value()).c_str()
                 }];
            //
            item = getitem(CmdlItemId::cmdl_item_port);
            if (item.has_value())
                cmdl({
                     std::get<1>(item.value()).c_str(),
                     std::get<2>(item.value()).c_str()
                 }, __DEFAULT_HTTP_PORT) >> srvport;
            //
            item = getitem(CmdlItemId::cmdl_item_loglevel);
            if (item.has_value())
            {
                cmdl({
                     std::get<1>(item.value()).c_str(),
                     std::get<2>(item.value()).c_str()
                 }) >> s;
                 if (!s.empty())
                    logservity = LogConfiguration().getSeverity(s);
            }
            //
            item = getitem(CmdlItemId::cmdl_item_logfile);
            if (item.has_value())
                logtypeout = ((cmdl[{
                     std::get<1>(item.value()).c_str(),
                     std::get<2>(item.value()).c_str()
                 }]) ? Helper::LogOutType::STD_FILE : logtypeout);
            //
            item = getitem(CmdlItemId::cmdl_item_autosave);
            if (item.has_value())
                ismacrosave = cmdl[{
                     std::get<1>(item.value()).c_str(),
                     std::get<2>(item.value()).c_str()
                 }];
            //
            cmdl(0) >> rootdir;
            std::size_t sp;
            if ((sp = rootdir.find_last_of("/")) != std::string::npos)
                rootdir = rootdir.substr(0, sp);
            //
            do
            {
                uint32_t idx = 0U;
                {
                    auto item1 = getitem(CmdlItemId::cmdl_item_help),
                         item2 = getitem(CmdlItemId::cmdl_item_infoip),
                         item3 = getitem(CmdlItemId::cmdl_item_infodisplay),
                         item4 = getitem(CmdlItemId::cmdl_item_infoinput),
                         item5 = getitem(CmdlItemId::cmdl_item_inforestapi);
                    //
                    idx = (cmdl[{
                                    std::get<1>(item1.value()).c_str(),
                                    std::get<2>(item1.value()).c_str()
                                }] +
                          (cmdl[std::get<2>(item2.value()).c_str()] * 10) +
                          (cmdl[std::get<2>(item3.value()).c_str()] * 100) +
                          (cmdl[std::get<2>(item4.value()).c_str()] * 1000) +
                          (cmdl[std::get<2>(item5.value()).c_str()] * 10000)
                    );
                }
                if (!idx)
                    break;

                Helper::Xml x("ARemote");

                switch(idx)
                {
                    case 1:
                    //
                    case 11:
                    //
                    case 101:
                    case 111:
                    //
                    case 1001:
                    case 1011:
                    case 1111:
                    //
                    case 10001:
                    case 10011:
                    case 10111:
                    case 11111:
                        {
                            std::stringstream ss;
                            printh(ss);
                            for (auto & [ id, val ] : cmdlcat_map)
                            {
                                ss  << std::endl << "\t" << val.c_str() << " option:" << std::endl;
                                for (auto & [ iid, ival ] : cmdlitem_map)
                                {
                                    if (id == std::get<0>(ival))
                                    {
                                        if (std::get<1>(ival).empty())
                                            ss << "\t\t--" << std::get<2>(ival).c_str()
                                               << "\t" << std::get<3>(ival).c_str()
                                               << std::endl;
                                        else
                                            ss << "\t\t" << std::get<1>(ival).c_str()
                                               << ", --" << std::get<2>(ival).c_str()
                                               << "\t" << std::get<3>(ival).c_str()
                                               << std::endl;
                                    }
                                }
                            }
                            auto content = ss.str();
                            std::cout << content << std::endl;
                            return true;
                        }
                    case 10:
                    case 110:
                    case 1010:
                    case 10010:
                        {
                            x.xmlsection("Wlan")
                             .xmlpair("address", Conf.ip.get())
                             .xmlpair("port", srvport);
                            break;
                        }
                    case 100:
                    case 1100:
                    case 10100:
                        {
                            Event::ADisplay dsp;
                            dsp.displayInfo(x);
                            break;
                        }
                    case 1000:
                    case 11000:
                        {
                            Event::Input in;
                            in.open();
                            in.inputInfo(x);;
                            break;
                        }
                    case 10000:
                        {
                            std::stringstream ss;
                            printh(ss);
                            ss << std::endl << "\tREST server URI:" << std::endl;
                            ss << "\t\thttp://";
                            if (iswlanip)
                                ss << Conf.ip.get();
                            else
                                ss << "127.0.0.1";
                            ss << ":" << srvport << "/<REST API Url>" << std::endl;

                            ss << std::endl << "\tREST server API:" << std::endl;
                            for (auto & [ id, val ] : Conf.urimap)
                            {
                                ss  << "\t\t"
                                    << std::get<1>(val).c_str()
                                    << std::get<2>(val).c_str()
                                    << std::get<3>(val).c_str() << std::endl;
                            }
                            auto content = ss.str();
                            std::cout << content << std::endl;
                            return true;
                        }
                    default:
                            return false;

                }
                //
                std::string content = x;
                std::cout << content << std::endl;
                return true;
            }
            while (0);
        }
        LOG_CATCH();
        return false;
    }
};

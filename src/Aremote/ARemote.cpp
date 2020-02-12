
#include "ARemote.h"
#include "extern/argh.h"

#define INCBIN_PREFIX bin_
#include "extern/incbin.h"

INCBIN(web_html, "web/pack.html.gz");
INCBIN(web_splash, "web/pack.jpg");

#if defined(_DEBUG)
void lua_test(Lua::Engine & luae)
{
    auto sc = R"(

print("BEGIN!")
    local aaa = aremote.checkTime("10:00:59","20:30:35")
    print("check time 1: ", aaa)
    local bbb = aremote.checkTime("21:00:59","23:30:35")
    print("check time 2: ", bbb)

function main (stateIn)
    print("begin main")
    print("state In: ", stateIn)

    aremote.stateSleep(2)
    local buf = aremote.screenGet();
    local im  = aremote.image.new(buf)

    if im:empty() then
        print("image null length, exit: ", im:length())
        return stateIn
    end

    print("length: ", im:length())
    local w, h = im:size()
    print("size: ", w, h)

    local sname = "/data/local/tmp/test-save"

    if stateIn == 0 then
        im:sepia()
        sname = sname .. "-sepia"
    elseif stateIn == 1 then
        im:grey()
        sname = sname .. "-grey"
    elseif stateIn == 2 then
        im:bw()
        sname = sname .. "-bw"
    end

    im:save(sname .. ".jpg")

    if stateIn == 0 then
        local imcap = aremote.image.new()
        print("go capture image ...")
        local iscap = imcap:capture()
        print("image captured: ", iscap)

        if imcap:empty() then
            print("capture image null length, skip: ", imcap:length())
        else
            local ws, hs = imcap:size()
            print("capture image save: ", ws, hs)
            imcap:save("/data/local/tmp/test-lua-capture.png")
        end
    end

    aremote.stateSet(stateIn + 1)
    print("end main")
    return stateIn + 1
end
print("END!")

	)";
	std::error_code ec;
	luae.run(sc, ec);
	std::this_thread::sleep_for(std::chrono::milliseconds(20000));
	luae.stop();
}
#endif

int main(int argc, char *argv[])
{
    try
    {
        android::sp<android::ProcessState> proc(android::ProcessState::self());
        android::ProcessState::self()->startThreadPool();
        //
        LogConfiguration().setName(argv[0]);
        LogConfiguration().setOut(LogOutType::STD_OUT);
#       if defined(_DEBUG)
        LogConfiguration().setSeverity(LogSeverity::DEBUG);
#       else
        LogConfiguration().setSeverity(LogSeverity::WARN);
#       endif
        //
        if (Conf.init(argc, argv))
            return 0;
        //
        LogConfiguration().setOut(Conf.cmd.logtypeout);
        LogConfiguration().setSeverity(Conf.cmd.logservity);
        //
        LOGDC("+  SET LOG ->",
              "severity:", LogConfiguration().getSeverity(),
              "output:", LogConfiguration().getOut()
        );
        //
        {
            App::IsRunning isrun(argv[0]);
        }
        //
        Event::Input idev;
        idev.open();
        //
        Event::ADisplay ad;
        //
        Lua::Engine luae;

#       if defined(_DEBUG)
          std::cout << ad << std::endl;
          std::cout << idev << std::endl;
          lua_test(luae);
#       endif

        Net::HTTPServer srv;
        srv.rest(
            "GET",
            Conf.geturi(App::UriId::uri_akey2),
            [&](HttpResponse response, HttpRequest request)
            {
                try
                {
                    uint16_t k = std::stoul(request->path_match[1].str());
                    // only Android type KeyCode accept
                    // see: extern/KeyCode/keytableAndroid.h
                    idev.key(k);
                    //
                    Net::HTTPServer::sendheader(
                            response,
                            "text/text",
                            0U
                        );
                }
                catch (...) { *response << HttpCode(HttpStatus::HD_500); }
            }
        );
        srv.rest(
            "GET",
            Conf.geturi(App::UriId::uri_key1),
            [&](HttpResponse response, HttpRequest request)
            {
                try
                {
                    uint16_t k = std::stoul(request->path_match[1].str());
                    // accept raw key characters, no uppercase,
                    //   latin characters, punctuation and numeric only!
                    // 1U - is Event::Input::KeyType::K_ASCII
                    idev.key(k, 1U);
                    //
                    Net::HTTPServer::sendheader(
                            response,
                            "text/text",
                            0U
                        );
                }
                catch (...) { *response << HttpCode(HttpStatus::HD_500); }
            }
        );
        srv.rest(
            "GET",
            Conf.geturi(App::UriId::uri_key2),
            [&](HttpResponse response, HttpRequest request)
            {
                try
                {
                    uint16_t k = std::stoul(request->path_match[1].str());
                    uint16_t t = std::stoul(request->path_match[2].str());
                    // t - is Event::Input::KeyType
                    idev.key(k, t);
                    //
                    Net::HTTPServer::sendheader(
                            response,
                            "text/text",
                            0U
                        );
                }
                catch (...) { *response << HttpCode(HttpStatus::HD_500); }
            }
        );
        srv.rest(
            "GET",
            Conf.geturi(App::UriId::uri_atap2),
            [&](HttpResponse response, HttpRequest request)
            {
                try
                {
                    uint16_t x = std::stoul(request->path_match[1].str());
                    uint16_t y = std::stoul(request->path_match[2].str());
                    // only accept coordinates from default rotate (90)
                    idev.tap(x, y, Event::Input::LONG_PRESS_DEFAULT);
                    //
                    Net::HTTPServer::sendheader(
                            response,
                            "text/text",
                            0U
                        );
                }
                catch (...) { *response << HttpCode(HttpStatus::HD_500); }
            }
        );
        srv.rest(
            "GET",
            Conf.geturi(App::UriId::uri_tap4),
            [&](HttpResponse response, HttpRequest request)
            {
                try
                {
                    Event::Input::rotatexy_t tr{
                        static_cast<uint16_t>(
                                    std::stoul(request->path_match[1].str())
                                ),
                        static_cast<uint16_t>(
                                    std::stoul(request->path_match[2].str())
                                ),
                        ((Conf.display.changexy) ? Conf.display.h : Conf.display.w),
                        ((Conf.display.changexy) ? Conf.display.w : Conf.display.h),
                        static_cast<Event::ADisplay::Orient>(
                                    std::stoi(request->path_match[3].str())
                                ),
                        Conf.display.o,
                        static_cast<Event::StreamRawData::StreamRatioEnum>(
                                    std::stoi(request->path_match[4].str())
                                )
                    };
                    idev.tap(tr, Event::Input::LONG_PRESS_DEFAULT);
                    //
                    Net::HTTPServer::sendheader(
                            response,
                            "text/text",
                            0U
                        );
                }
                catch (...) { *response << HttpCode(HttpStatus::HD_500); }
            }
        );
        srv.rest(
            "POST",
            Conf.geturi(App::UriId::uri_text),
            [&](HttpResponse response, HttpRequest request)
            {
                try
                {
                    auto content = request->content.string();
                    idev.text(content);
                    //
                    Net::HTTPServer::sendheader(
                            response,
                            "text/text",
                            0U
                        );
                }
                catch (...) { *response << HttpCode(HttpStatus::HD_500); }
            }
        );
        srv.rest(
            "GET",
            Conf.geturi(App::UriId::uri_stream_live),
            [&](HttpResponse response, HttpRequest request)
            {
                try
                {
                    srv.stream(response, request);
                }
                catch (...) { *response << HttpCode(HttpStatus::HD_500); }
            }
        );
        srv.rest(
            "GET",
            Conf.geturi(App::UriId::uri_stream_player),
            [&](HttpResponse response, HttpRequest request)
            {
                try
                {
                    //
                    std::vector<char> v(
                            reinterpret_cast<const char*>(&bin_web_htmlData[0]),
                            reinterpret_cast<const char*>(&bin_web_htmlData[0] + bin_web_htmlSize)
                        );
                    Net::HTTPServer::sendheader(
                            response,
                            "text/html",
                            v.size(),
                            true
                        );
                    response->write(&v[0], v.size());
                }
                catch (...) { *response << HttpCode(HttpStatus::HD_500); }
            }
        );
        srv.rest(
            "GET",
            Conf.geturi(App::UriId::uri_stream_splash),
            [&](HttpResponse response, HttpRequest request)
            {
                try
                {
                    std::vector<char> v(
                            reinterpret_cast<const char*>(&bin_web_splashData[0]),
                            reinterpret_cast<const char*>(&bin_web_splashData[0] + bin_web_splashSize)
                        );
                    Net::HTTPServer::sendheader(
                            response,
                            "image/jpg",
                            v.size()
                        );
                    response->write(&v[0], v.size());
                }
                catch (...) { *response << HttpCode(HttpStatus::HD_500); }
            }
        );
        srv.rest(
            "GET",
            Conf.geturi(App::UriId::uri_capture),
            [&](HttpResponse response, HttpRequest request)
            {
                try
                {
                    srv.capture(response, request);
                }
                catch (...) { *response << HttpCode(HttpStatus::HD_500); }
            }
        );
        srv.rest(
            "GET",
            Conf.geturi(App::UriId::uri_input_xml),
            [&](HttpResponse response, HttpRequest request)
            {
                try
                {
                    Helper::Xml x("ARemote");
                    idev.inputInfo(x);
                    std::string content = x;
                    srv.send(response, content, "text/xml");
                }
                catch (std::system_error const & ex_)
                {
                    srv.sendxmlerror(response, __LINE__, ex_.what());
                }
                catch (...) { *response << HttpCode(HttpStatus::HD_500); }
            }
        );
        srv.rest(
            "GET",
            Conf.geturi(App::UriId::uri_display_xml),
            [&](HttpResponse response, HttpRequest request)
            {
                try
                {
                    Helper::Xml x("ARemote");
                    ad.displayInfo(x);
                    std::string content = x;
                    srv.send(response, content, "text/xml");
                }
                catch (std::system_error const & ex_)
                {
                    srv.sendxmlerror(response, __LINE__, ex_.what());
                }
                catch (...) { *response << HttpCode(HttpStatus::HD_500); }
            }
        );
        //
        srv.rest(
            "GET",
            Conf.geturi(App::UriId::uri_macro_record),
            [&](HttpResponse response, HttpRequest request)
            {
                try
                {
                    std::string content = idev.macroRecord("", 600);
                    srv.send(response, content, "text/xml");
                }
                catch (std::system_error const & ex_)
                {
                    srv.sendxmlerror(response, __LINE__, ex_.what());
                }
                catch (...) { *response << HttpCode(HttpStatus::HD_500); }
            }
        );
        srv.rest(
            "POST",
            Conf.geturi(App::UriId::uri_macro_record_tm),
            [&](HttpResponse response, HttpRequest request)
            {
                try
                {
                    uint32_t tmax = static_cast<uint32_t>(
                                    std::stoul(request->path_match[1].str())
                                );
                    auto content = request->content.string();
                    content = idev.macroRecord(content, tmax);
                    srv.send(response, content, "text/xml");
                }
                catch (std::system_error const & ex_)
                {
                    srv.sendxmlerror(response, __LINE__, ex_.what());
                }
                catch (...) { *response << HttpCode(HttpStatus::HD_500); }
            }
        );
        srv.rest(
            "GET",
            Conf.geturi(App::UriId::uri_macro_stop),
            [&](HttpResponse response, HttpRequest request)
            {
                try
                {
                    idev.macroStop();
                    //
                    Helper::Xml x("ARemote");
                    x.xmlsection("Macro")
                     .xmlpair("stop", "OK");
                    std::string content = x;
                    srv.send(response, content, "text/xml");
                }
                catch (std::system_error const & ex_)
                {
                    srv.sendxmlerror(response, __LINE__, ex_.what());
                }
                catch (...) { *response << HttpCode(HttpStatus::HD_500); }
            }
        );
        srv.rest(
            "GET",
            Conf.geturi(App::UriId::uri_macro_save),
            [&](HttpResponse response, HttpRequest request)
            {
                try
                {
                    idev.macroSave(true);
                    //
                    Helper::Xml x("ARemote");
                    x.xmlsection("Macro")
                     .xmlpair("save", "OK");
                    std::string content = x;
                    srv.send(response, content, "text/xml");
                }
                catch (std::system_error const & ex_)
                {
                    srv.sendxmlerror(response, __LINE__, ex_.what());
                }
                catch (...) { *response << HttpCode(HttpStatus::HD_500); }
            }
        );
        srv.rest(
            "GET",
            Conf.geturi(App::UriId::uri_macro_get),
            [&](HttpResponse response, HttpRequest request)
            {
                try
                {
                    std::string content = idev.macroGet();
                    srv.send(response, content, "text/xml");
                }
                catch (std::system_error const & ex_)
                {
                    srv.sendxmlerror(response, __LINE__, ex_.what());
                }
                catch (...) { *response << HttpCode(HttpStatus::HD_500); }
            }
        );
        srv.rest(
            "GET",
            Conf.geturi(App::UriId::uri_macro_get_id),
            [&](HttpResponse response, HttpRequest request)
            {
                try
                {
                    uint32_t tid = static_cast<uint32_t>(
                                    std::stoul(request->path_match[1].str())
                                );
                    std::string content = idev.macroGet(tid);
                    srv.send(response, content, "text/xml");
                }
                catch (std::system_error const & ex_)
                {
                    srv.sendxmlerror(response, __LINE__, ex_.what());
                }
                catch (...) { *response << HttpCode(HttpStatus::HD_500); }
            }
        );
        srv.rest(
            "GET",
            Conf.geturi(App::UriId::uri_macro_play_act),
            [&](HttpResponse response, HttpRequest request)
            {
                try
                {
                    idev.macroPlay();
                    //
                    Helper::Xml x("ARemote");
                    x.xmlsection("Macro")
                     .xmlpair("play", "START", "internal source");
                    std::string content = x;
                    srv.send(response, content, "text/xml");
                }
                catch (std::system_error const & ex_)
                {
                    srv.sendxmlerror(response, __LINE__, ex_.what());
                }
                catch (...) { *response << HttpCode(HttpStatus::HD_500); }
            }
        );
        srv.rest(
            "GET",
            Conf.geturi(App::UriId::uri_macro_play_id),
            [&](HttpResponse response, HttpRequest request)
            {
                try
                {
                    uint32_t tid = static_cast<uint32_t>(
                                    std::stoul(request->path_match[1].str())
                                );
                    idev.macroPlay(tid);
                    //
                    Helper::Xml x("ARemote");
                    x.xmlsection("Macro")
                     .xmlpair("play", "START", tid);
                    std::string content = x;
                    srv.send(response, content, "text/xml");
                }
                catch (std::system_error const & ex_)
                {
                    srv.sendxmlerror(response, __LINE__, ex_.what());
                }
                catch (...) { *response << HttpCode(HttpStatus::HD_500); }
            }
        );
        srv.rest(
            "POST",
            Conf.geturi(App::UriId::uri_macro_play_post),
            [&](HttpResponse response, HttpRequest request)
            {
                try
                {
                    auto xmltext = request->content.string();
                    idev.macroPlay(xmltext);
                    //
                    Helper::Xml x("ARemote");
                    x.xmlsection("Macro")
                     .xmlpair("play", "START", "xml string");
                    std::string content = x;
                    srv.send(response, content, "text/xml");
                }
                catch (std::system_error const & ex_)
                {
                    srv.sendxmlerror(response, __LINE__, ex_.what());
                }
                catch (...) { *response << HttpCode(HttpStatus::HD_500); }
            }
        );
        srv.rest(
            "GET",
            Conf.geturi(App::UriId::uri_macro_list),
            [&](HttpResponse response, HttpRequest request)
            {
                try
                {
                    std::string content = idev.macroList();
                    srv.send(response, content, "text/xml");
                }
                catch (std::system_error const & ex_)
                {
                    srv.sendxmlerror(response, __LINE__, ex_.what());
                }
                catch (...) { *response << HttpCode(HttpStatus::HD_500); }
            }
        );
        //
        srv.rest(
            "POST",
            Conf.geturi(App::UriId::uri_lua_script),
            [&](HttpResponse response, HttpRequest request)
            {
                try
                {
                    std::error_code ec;
                    auto script = request->content.string();
                    //
                    luae.run(script, ec);
                    if (ec)
                        throw std::system_error(ec);
                    //
                    Helper::Xml x("ARemote");
                    x.xmlsection("Script")
                     .xmlpair("lua", "RUN");
                    std::string content = x;
                    srv.send(response, content, "text/xml");
                }
                catch (std::system_error const & ex_)
                {
                    srv.sendxmlerror(response, __LINE__, ex_.what());
                }
                catch (...) { *response << HttpCode(HttpStatus::HD_500); }
            }
        );
        srv.rest(
            "GET",
            Conf.geturi(App::UriId::uri_lua_load),
            [&](HttpResponse response, HttpRequest request)
            {
                try
                {
                    std::error_code ec;
                    luae.load(request->path_match[1].str(), ec);
                    if (ec)
                        throw std::system_error(ec);
                    //
                    Helper::Xml x("ARemote");
                    x.xmlsection("Script")
                     .xmlpair("lua", "LOAD", request->path_match[1].str());
                    std::string content = x;
                    srv.send(response, content, "text/xml");
                }
                catch (std::system_error const & ex_)
                {
                    srv.sendxmlerror(response, __LINE__, ex_.what());
                }
                catch (...) { *response << HttpCode(HttpStatus::HD_500); }
            }
        );
        srv.rest(
            "GET",
            Conf.geturi(App::UriId::uri_lua_stop),
            [&](HttpResponse response, HttpRequest request)
            {
                try
                {
                    luae.stop();
                    //
                    Helper::Xml x("ARemote");
                    x.xmlsection("Script")
                     .xmlpair("lua", "STOP");
                    std::string content = x;
                    srv.send(response, content, "text/xml");
                }
                catch (std::system_error const & ex_)
                {
                    srv.sendxmlerror(response, __LINE__, ex_.what());
                }
                catch (...) { *response << HttpCode(HttpStatus::HD_500); }
            }
        );
        //
        if ((Conf.cmd.iswlanip) && (!Conf.ip.get().empty()))
        {
            do
            {
                try
                {
                    std::string s = argv[0];
                    s += "-ip.xml";
                    //
                    std::ofstream f(s);

                    if(!f.is_open())
                        break;

                    Helper::Xml x("ARemote");
                    x.xmlsection("Wlan")
                    .xmlpair("address", Conf.ip.get())
                    .xmlpair("port", Conf.cmd.srvport);

                    std::string content = x;
                    f << content;
                    f.close();
                }
                LOG_CATCH();
            }
            while (0);
        }
        srv.start();
        srv.wait();
    }
    catch (App::IsRunningFound const & ex_)
    {
        LOGEX("Check process:", ex_.what());
        LOGW("Exec arguments:", argc, argv[0]);
    }
    LOG_CATCH();
    //
    return 0;
}

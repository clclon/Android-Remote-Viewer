
#include "ARemote.h"

  /*
    Test helper:
      adb forward tcp:53777 tcp:53777
      curl -v -s http://localhost:53777/stream/live.mjpg?ratio=1&rotate=0 -o - --no-buffer
      URL: http://localhost:53777/stream/live.mjpg?ratio=2&rotate=3
      CAP: curl -v -s http://localhost:53777/capture/jpg -o - --no-buffer >cap-bitmap.jpg
   */

#define AINPUT_SERVER_STRING                                                     \
  "ARemote/"                                                                     \
  AINPUT_FULLVERSION_STRING "-"                                                  \
  AINPUT_STATUS " "                                                              \
  AINPUT_YEAR "."                                                                \
  AINPUT_MONTH "."                                                               \
  AINPUT_DATE

namespace Net
{
    Helper::CatalogMap l_hdrmsg_map = Helper::CatalogMapInit
    (HTTPServer::HTTPHeader::HD_500,       "HTTP/1.1 500 INTERNALSERVERERROR\r\nContent-Length: 0\r\n\r\n")
    (HTTPServer::HTTPHeader::HD_501,       "HTTP/1.1 501 NOTIMPLEMENTED\r\nContent-Length: 0\r\n\r\n")
    (HTTPServer::HTTPHeader::HD_200,       "HTTP/1.1 200 OK\r\nContent-Length: ")
    (HTTPServer::HTTPHeader::HD_TYPE,      "Content-Type")
    (HTTPServer::HTTPHeader::HD_LENGTH,    "Content-Length")
    (HTTPServer::HTTPHeader::HD_ENCODING,  "Content-Encoding")
    (HTTPServer::HTTPHeader::HD_CONNECT,   "Connection")
    (HTTPServer::HTTPHeader::HD_CACHEC,    "Cache-Control")
    (HTTPServer::HTTPHeader::HD_SERVER,    "Server")
    (HTTPServer::HTTPHeader::HD_MAXAGE,    "Max-Age")
    (HTTPServer::HTTPHeader::HD_EXPIRES,   "Expires")
    (HTTPServer::HTTPHeader::HD_PRAGMA,    "Pragma")
    (HTTPServer::HTTPHeader::HD_END,       "\r\n")
    (HTTPServer::HTTPHeader::HD_ENDHEADER, "\r\n\r\n")
    (HTTPServer::HTTPHeader::HD_OK,        "OK");

    static inline bool i_sendsync(HttpResponse response)
    {
        std::promise<bool> iserror;
        response->send(
            [&iserror](SimpleWeb::error_code const & ec)
            {
                iserror.set_value(static_cast<bool>(ec));
            });
            return !(iserror.get_future().get());
    }
    //

    HTTPServer::HTTPServer()
    {
        init();
    }

    HTTPServer::~HTTPServer()
    {
        wait();
    }

    std::string & HTTPServer::getstringcode(HTTPServer::HTTPHeader idx)
    {
        auto obj = l_hdrmsg_map.find(idx);
        if (obj == l_hdrmsg_map.end())
        {
            static std::string strempty;
            return strempty;
        }
        return obj->second;
    }

    void HTTPServer::rest(
              std::string const & method,
              std::string const & label,
              Http_cb fun)
    {
        m_server.resource[label.c_str()][method.c_str()] = fun;
    }

    void HTTPServer::start()
    {
        std::promise<uint16_t> tmp_port;
        std::thread srv(
          [&]()
          {
#             if defined(__WIN32__)
              ::SetThreadUILanguage(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));
#             endif
              //
              m_server.start(
                [&](uint16_t port)
                {
                    tmp_port.set_value(port);
                });
          });
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
#       if defined(_DEBUG) || defined(_BUILD_HTTP_INFO_ENABLE)
          LOGDC("\t -> Server listening on port: ", tmp_port.get_future().get());
#       endif

        if (srv.joinable())
          m_thsrv = move(srv);
        else
          throw std::system_error(App::make_error_code(App::ErrorId::error_server_http_not_start));
    }

    void HTTPServer::stop()
    {
        m_server.stop();
        m_server.io_service->reset();
        m_server.io_service->run();
        LOGDC("\t -> Server end loop..");
    }

    void HTTPServer::wait()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        //
        if (m_thsrv.joinable())
          m_thsrv.join();
    }

    void HTTPServer::init()
    {
        try
        {
        m_server.config.address          = ((Conf.cmd.iswlanip) ? Conf.ip.get() : "");
        m_server.config.port             = ((Conf.cmd.srvport)  ? Conf.cmd.srvport : __DEFAULT_HTTP_PORT);
        m_server.config.thread_pool_size = 8;
        m_server.config.fast_open        = true;
        m_server.config.reuse_address    = true;
        m_server.config.timeout_request  = 15;
        //
        m_server.default_resource["GET"] =
          [&](HttpResponse response, HttpRequest)
          {
            try
            {
              *response << HttpCode(HttpStatus::HD_501);
            }
            LOG_CATCH()
          };
        m_server.on_error =
          [](HttpRequest, [[ maybe_unused ]] HttpError const & ec)
          {
            try
            {
                // error filter:
                switch (ec.value())
                {
                    case 2:    /* End of file */
                    case 125:  /* Operation aborted */
                        return;
                    default:
                        break;
                }

#               if defined(_DEBUG) || defined(_BUILD_HTTP_INFO_ENABLE)
#                 if defined(__WIN32__)
                    ::SetThreadUILanguage(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));
#                 endif
                  LOGDC("REST server error: (", ec, ")", ec.message());
#               endif
            }
            LOG_CATCH()
          };
        m_server.resource[Conf.geturi(App::UriId::uri_rexit)]["GET"] =
          [&](HttpResponse response, HttpRequest request)
          {
            try
            {
              LOGDC("Stopped request from: ",
                        request->remote_endpoint().address().to_string(),
                        ":",
                        request->remote_endpoint().port());
              stop();
            }
            LOG_CATCH()
          };
        m_server.resource[Conf.geturi(App::UriId::uri_client_xml)]["GET"] =
          [&](HttpResponse response, HttpRequest request)
          {
            try
            {
              Helper::Xml x("ARemote");
              //
                x.xmlsection("Client")
                 .xmlpair("address", request->remote_endpoint().address().to_string())
                 .xmlpair("port", request->remote_endpoint().port());

                x.xmlsection("Request")
                 .xmlpair("version", request->http_version)
                 .xmlpair("method", request->method)
                 .xmlpair("path", request->path);

                x.xmlsection("Query")
                 .xmlmap(request->parse_query_string());

                x.xmlsection("Header")
                 .xmlmap(request->header);
              //
              std::string content = x;
              *response << HttpCode(HttpStatus::HD_200)  << content.length()
                        << HttpCode(HttpStatus::HD_END)
                        << HttpCode(HttpStatus::HD_TYPE) << ": text/xml"
                        << HttpCode(HttpStatus::HD_ENDHEADER)
                        << content;
            }
            LOG_CATCH()
          };
        }
        LOG_CATCH()
    }

    void HTTPServer::sendheader(HttpResponse response, std::string const & s, uint32_t sz, bool isgz)
    {
        SimpleWeb::CaseInsensitiveMultimap h;
        h.emplace(HttpCode(HttpStatus::HD_SERVER),  AINPUT_SERVER_STRING);
        h.emplace(HttpCode(HttpStatus::HD_CONNECT), "close");
        h.emplace(HttpCode(HttpStatus::HD_MAXAGE),  "0");
        h.emplace(HttpCode(HttpStatus::HD_EXPIRES), "0");
        h.emplace(HttpCode(HttpStatus::HD_PRAGMA),  "no-cache");
        h.emplace(HttpCode(HttpStatus::HD_CACHEC),  "no-cache, private");
        h.emplace(HttpCode(HttpStatus::HD_TYPE),    s.c_str());
        if (isgz)
            h.emplace(HttpCode(HttpStatus::HD_ENCODING), "gzip");
        if (sz)
            h.emplace(HttpCode(HttpStatus::HD_LENGTH),   std::to_string(sz));
        response->write(SimpleWeb::StatusCode::success_ok, h);
    }

    void HTTPServer::send(HttpResponse response, std::string const & s, std::string const & mime)
    {
        *response << HttpCode(HttpStatus::HD_200)  << s.length()
                  << HttpCode(HttpStatus::HD_END)
                  << HttpCode(HttpStatus::HD_TYPE) << ": " << mime.c_str()
                  << HttpCode(HttpStatus::HD_ENDHEADER)
                  << s;
    }

    void HTTPServer::sendxmlerror(HttpResponse response, uint32_t ln, std::string const & s)
    {
        Helper::Xml x("ARemote");
        x.xmlsection("ErrorRequest")
         .xmlpair("error", ln, s);
        std::string content = x;
        send(response, content, "text/xml");
    }

    void HTTPServer::capture(HttpResponse response, HttpRequest request)
    {
        try
        {
            if (!Conf.class_display)
                throw std::system_error(App::make_error_code(App::ErrorId::error_server_http_not_display_run));

            App::CaptureTrigger cap;
            Event::ADisplay::ImgTypeOut t;
            std::string mime = "image/";
            //
            {
                auto s = request->path_match[1].str();
                if (s.empty())
                    throw std::system_error(App::make_error_code(App::ErrorId::error_server_http_uri_empty));

                if ((t = Event::ADisplay::getImgTypeEnum(s)) == Event::ADisplay::ImgTypeOut::T_IMG_TYPE_NONE)
                    throw std::system_error(App::make_error_code(App::ErrorId::error_server_http_not_image_type));

                mime.append(s.c_str());
            }
            //
            std::vector<uint8_t> v;
            Conf.class_display->imageCapture(v, t);
            if (!v.size())
                throw std::system_error(App::make_error_code(App::ErrorId::error_server_http_not_image_create));
            //
            response->close_connection_after_response = true;
            Net::HTTPServer::sendheader(
                    response,
                    mime,
                    v.size()
              );
            //
            if (!i_sendsync(response))
                throw std::system_error(App::make_error_code(App::ErrorId::error_server_http_not_capture_send));
            response->write(reinterpret_cast<char*>(&v[0]), v.size());
            if (!i_sendsync(response))
                throw std::system_error(App::make_error_code(App::ErrorId::error_server_http_not_capture_send_part));
        }
        LOG_CATCH()
    }

    void HTTPServer::stream(HttpResponse response, HttpRequest request)
    {
        if (!Conf.class_display)
            throw std::system_error(App::make_error_code(App::ErrorId::error_server_http_not_display_run));

        HTTPServer *srv = this;
        //
        std::thread thstream(
            [response, request, srv]
            {
                class StreamSender final
                {
                    private:
                      //
                      HttpResponse         m_response;
                      Event::StreamRawData m_imgdata;
                      //
                      bool sendsync()
                      {
                          return i_sendsync(m_response);
                      }

                    public:
                      //
                      StreamSender(HttpResponse response, HttpRequest request)
                        : m_response(response)
                      {
                          Event::ADisplay::Orient               o = Event::ADisplay::Orient::O_0;
                          Event::StreamRawData::StreamRatioEnum d = Event::StreamRawData::StreamRatioEnum::R_1_1;

                          try
                          {
                            auto qs = request->parse_query_string();
                            for (auto & [ key, val ] : qs)
                            {
                                try
                                {
                                    if (key.compare(0, 5, "ratio", 5) == 0)
                                        d = static_cast<Event::StreamRawData::StreamRatioEnum>(std::stoi(val));
                                    else if (key.compare(0, 6, "rotate", 6) == 0)
                                        o = static_cast<Event::ADisplay::Orient>(std::stoi(val));
                                }
                                catch (...) { LOGDI("bad compare part of query string"); }
                            }
#                           if defined(_DEBUG) || defined(_BUILD_HTTP_INFO_ENABLE)
                              LOGDI("query string: ratio =", d, ", rotate =", o);
#                           endif
                          }
                          catch (...) { LOGDI("bad parse query string"); }

                          Conf.class_display->getDisplay();
                          m_imgdata.init(Conf, d, o);
                          Conf.class_display->clientcountUp();
                      }
                      ~StreamSender()
                      {
                          Conf.class_display->clientcountDown();
                      }
                      bool handshake()
                      {
                          m_response->close_connection_after_response = true;
                          Net::HTTPServer::sendheader(
                                m_response,
                                "multipart/x-mixed-replace; boundary=--BoundaryData",
                                0
                            );
                          return sendsync();
                      }
                      void loop()
                      {
                          while (true)
                          {
                              if (!Conf.class_display)
                                break;

                              std::vector<uint8_t> v;
                              Conf.class_display->mjpgStream(v, m_imgdata);
                              if (!v.size())
                              {
                                  throw Event::DisplayNoFatal(
                                            __LINE__,
                                            __PRETTY_FUNCTION__,
                                            "buffer empty -> client go to reconnect.."
                                        );
                              }
                              //
                              std::stringstream ss;
                              ss << HttpCode(HttpStatus::HD_END);
                              ss << "--BoundaryData";
                              ss << HttpCode(HttpStatus::HD_END);
                              ss << HttpCode(HttpStatus::HD_TYPE) << ": image/jpeg";
                              ss << HttpCode(HttpStatus::HD_END);
                              ss << HttpCode(HttpStatus::HD_LENGTH) << ": " << v.size();
                              ss << HttpCode(HttpStatus::HD_ENDHEADER);
                              //
                              auto content = ss.str();
                              m_response->write(content.c_str(), content.length());
                              if (!sendsync())
                                  break;
                              m_response->write(reinterpret_cast<char*>(&v[0]), v.size());
                              if (!sendsync())
                                  break;
                          }
                      }
                };
                //
                try
                {
                    StreamSender a(response, request);
                    if (!a.handshake())
                        return;
                    a.loop();
                }
                catch (Event::DisplayNoFatal const & ex_)
                {
                    LOGEX(ex_.what());
                    srv->stream(response, request);
                    return;
                }
                LOG_CATCH()
            });
          if (thstream.joinable())
              thstream.detach();
    }
};

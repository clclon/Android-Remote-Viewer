
#pragma once

using HttpServer   = SimpleWeb::Server<SimpleWeb::HTTP>;
using HttpResponse = std::shared_ptr<HttpServer::Response>;
using HttpRequest  = std::shared_ptr<HttpServer::Request>;

namespace Net
{
    class HTTPServer
    {
        public:
            //
            enum HTTPHeader
            {
                HD_500,
                HD_501,
                HD_200,
                HD_TYPE,
                HD_LENGTH,
                HD_ENCODING,
                HD_CONNECT,
                HD_CACHEC,
                HD_SERVER,
                HD_MAXAGE,
                HD_EXPIRES,
                HD_PRAGMA,
                HD_END,
                HD_ENDHEADER,
                HD_OK
            };
            //
            using HttpError    = SimpleWeb::error_code;
            using Http_cb      = std::function<void(HttpResponse, HttpRequest)>;
            //
            HTTPServer();
            virtual ~HTTPServer();
            //
            void start();
            void stop();
            void wait();
            void send(HttpResponse, std::string const&, std::string const&);
            void sendxmlerror(HttpResponse, uint32_t, std::string const&);
            void rest(std::string const&, std::string const&, Http_cb);
            void stream(HttpResponse, HttpRequest);
            void capture(HttpResponse, HttpRequest);
            //
            static void sendheader(HttpResponse, std::string const&, uint32_t = 0U, bool = false);
            static std::string & getstringcode(HTTPServer::HTTPHeader);
            static HTTPServer *server;

        private:
            //
            std::thread m_thsrv;
            HttpServer  m_server;
            //
            void init();
    };
};

using HttpStatus = Net::HTTPServer::HTTPHeader;
#define HttpCode   Net::HTTPServer::getstringcode

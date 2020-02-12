
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <atomic>
#include <memory>
#include <mutex>
#include "../HelperLog.h"
#include <App.Error.h>

#include <asio.hpp>
#include <server_http.hpp>

#include "main.h"

#include <stdio.h>
#include <unistd.h>

#include <binder/ProcessState.h>
#include <binder/IPCThreadState.h>

int main(int32_t argc, char *argv[])
{
    android::sp<android::ProcessState> proc(android::ProcessState::self());
    android::ProcessState::self()->startThreadPool();

    LogConfiguration().setName(argv[0]);
    LogConfiguration().setOut(LogOutType::STD_OUT);
    //LogConfiguration().setOut(LogOutType::STD_FILE);
    //Helper::LogConfig::instance().setSeverity(LogSeverity::DEBUG);
    Helper::LogConfig::instance().setSeverity(LogSeverity::WARN);
    //Helper::LogConfig::instance().setSeverity(LogSeverity::ERROR);
    //Helper::ToLog<LogType::DEBUG>() << "AAAAAAAAAAAAAAAA";

    LOGD("DDDDDDDDDDDDDDDD");
    LOGI("IIIIIIIIIIIIIIII");
    LOGW("WWWWWWWWWWWWWWWW");
    LOGE("EEEEEEEEEEEEEEEE");
    LOG("CCCCCCCCCCCCCCCC");

    LOG("aaa", 1234, 5678, "bbb");

    int32_t t1 = 4;

    LOG_IF((t1 >= 2), []() { return "Axxxxxxxxxxxxxxxxx!"; });
    LOG_IF((t1 >= 9), []() { return "Oxxxxxxxxxxxxxxxxx!"; });
    LOG_IF((t1 == 9), []() { return "Zxxxxxxxxxxxxxxxxx!"; });
    LOG_IF((t1 <= 5), []() { return "Yxxxxxxxxxxxxxxxxx!"; });
    //LOG_IF_THROW((t1 == 9), []() { return "Zxxxxxxxxxxxxxxxxx!"; });
    LOG_IF((t1 >= 90), "Exxxxxxxxxxxxxxxxx!");

    try
    {
        //throw std::runtime_error("AAA exception test");
        throw App::IsRunningFound(__LINE__, 187, "Test exception");
    }
    catch (...)
    {
        LOGEXCEPT();
    }
    try
    {
        //throw std::runtime_error("LOG_CATCH exception test");
        //throw SimpleWeb::system_error(1024, std::system_category(), "SimpleWeb exception test");
        //std::error_code ec (24, std::generic_category());
        //std::error_code ec = App::ErrorId::error_display_buffer_size;
        //throw std::system_error(ec, "SimpleWeb exception test");
        throw std::system_error(App::make_error_code(App::ErrorId::error_display_not_size));
    }
    catch(std::system_error const & ex_)
    {
        std::cout << "TEST output: " << ex_.what() << std::endl;
        LOGDI("LOGDI");
        LOGDD("LOGDD");
        LOGDC("LOGDC");
        LOGI("LOGI");
        LOGD("LOGD");
        LOGE("LOGE");
        LOGW("LOGW");
        LOGEXEMPTY();
        LOGEX("Check process:", ex_.what());
        LOGEXCEPT();
        std::cout << "TEST end" << std::endl;
    }
    LOG_CATCH();

    //std::error_code ec = App::ErrorId::error_display_not_size;
    //std::cout << "error_code: " << ec << std::endl;

    return 0;
}

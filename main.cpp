#include <iostream>
#include "Acceptor.h"

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

int main() {
    try {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::debug);
        console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%t] [%^%l%$] %@ %v");

        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/net_controller.log",
                1024 * 1024, 1, true);
        file_sink->set_level(spdlog::level::info);
        file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%t] [%^%l%$] %@ %v");

        spdlog::sinks_init_list sink_list = {file_sink, console_sink};

        spdlog::init_thread_pool(10000, 2);
        auto logger = std::make_shared<spdlog::async_logger>("multi_sink", sink_list.begin(),
                sink_list.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
        logger->set_level(spdlog::level::trace);
        logger->flush_level(spdlog::level::trace);
        spdlog::set_default_logger(logger);
    }
    catch (const spdlog::spdlog_ex &ex) {
        std::cout << "Log initialization failed: " << ex.what() << std::endl;
    }
    SPDLOG_INFO("Log init success!");

    asio::io_context ioc;
    auto acceptor = std::make_shared<Acceptor>(ioc);
    acceptor->start();
    ioc.run();
    return 0;
}

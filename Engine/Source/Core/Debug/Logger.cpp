#include "Logger.h"

namespace rb
{
    void Logger::Init()
    {
        std::remove("log.txt");
        auto logger = spdlog::basic_logger_mt("log", "log.txt");

        logger->set_pattern("%v");

        logger->info("\t\t\tThank you for staying with us.\n\t\t\t  (c) Yume Games 2020-2021\n");

        logger->set_pattern("[%H:%M:%S] [%l] %v");
    }
}
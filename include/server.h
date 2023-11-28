#ifndef CCEMU_SERVER_H    
#define CCEMU_SERVER_H

#include "callcenter.h"

#include "uuid_v4.h"
#include "httplib.h"
#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/base_sink.h"

#include <string>
#include <iostream>
#include <fstream>
#include <memory>

class CallCenter;

class Server {
public:    
    Server();

    void Run();

private:
    inline static const std::string config_path {"../config.json"};
    inline static const std::string log_path {"../logs/server.log"};   
    std::shared_ptr<spdlog::logger> _logger;

    int _port{};
    std::string _host{};

    std::unique_ptr<CallCenter> _callcenter {nullptr};

    UUIDv4::UUIDGenerator<std::mt19937_64> _uuid_generator;

    void SettingsInit();
    void LoggerInit();
};

#endif
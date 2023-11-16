#include "httplib.h"
#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#include <string>
#include <iostream>
#include <fstream>

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

    void ServerSettingsInit();
    void LoggerInit();
};
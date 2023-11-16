#include "httplib.h"
#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"


#include <string>
#include <iostream>
#include <fstream>

class Client {
public:
    Client();

    void Run();

private:
    inline static const std::string config_path {"../config.json"};
    inline static const std::string log_path {"../logs/client.log"};   
    std::shared_ptr<spdlog::logger> _logger;

    int _port {};
    std::string _host{};
    std::string _request{};

    

    void ClientSettingsInit();
    void LoggerInit();
};
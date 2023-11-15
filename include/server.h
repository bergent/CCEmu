#include "httplib.h"
#include "nlohmann/json.hpp"
#include <string>
#include <iostream>
#include <fstream>

class Server {
public:    
    Server();

    void Run();

private:
    inline static const std::string config_path {"../config.json"};
    inline static const std::string log_path {"../server.log"};   

    int _port{};
    std::string _host{};

    void ServerSettingsInit();
};
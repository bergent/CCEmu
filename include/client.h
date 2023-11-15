#include "httplib.h"
#include "nlohmann/json.hpp"

#include <string>
#include <iostream>
#include <fstream>

class Client {
public:
    Client();

    void Run();

private:
    inline static const std::string config_path {"../config.json"};
    inline static const std::string log_path {"../client.log"};   

    int _port {};
    std::string _host{};
    std::string _request{};

    void ClientSettingsInit();
};
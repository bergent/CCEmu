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
    int _port{};
    std::string _host{};

    void ServerSettingsInit();
};
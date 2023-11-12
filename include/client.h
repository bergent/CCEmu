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
    int _port {};
    std::string _host{};
    std::string _request{};

    void ClientSettingsInit();
};
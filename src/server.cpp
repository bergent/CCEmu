#include "server.h"

using json = nlohmann::json;


Server::Server() {
    ServerSettingsInit();
}

void Server::ServerSettingsInit() {
    std::fstream jsonfile {"../config.json"};
    json cfg {json::parse(jsonfile)};

    _port = cfg[0]["server"]["port"];
    _host = cfg[0]["server"]["host"].get<std::string>();
}

void Server::Run() {
    httplib::Server server;

    // GET Request handler for phone numbers
    // Will return values matching the regexp
    // Example: +79005553535 -> 9005553535
    server.Get(R"(/\+?[7-8](\d{10}))", [&](const httplib::Request& req, httplib::Response& res) {
        auto numbers = req.matches[1];
        res.set_content(numbers, "text/plain");
    }); 

    if (!server.is_valid()) {
       std::cerr << "ERROR: Server is not valid! Check the configuration file\n";
       return;
    }
    std::cout << "Server is running properly\n";
    server.listen(_host, _port);
}

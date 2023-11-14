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
    server.Get(R"((/)(\+?[7-8](\d{10})))", [&](const httplib::Request& req, httplib::Response& res) {
        auto numbers = req.matches[2];
        res.set_content(numbers, "text/plain");
    }); 

    // Check if server is valid
    if (!server.is_valid()) {
       std::cerr << "ERROR: Server is not valid! Check the configuration file\n";
       return;
    }
    std::cout << "Server is running properly\n";

    // Make server listen to the designated host and port
    server.listen(_host, _port);
}

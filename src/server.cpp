#include "server.h"
#include "call.h"

using json = nlohmann::json;


Server::Server() {
    ServerSettingsInit();
}

void Server::ServerSettingsInit() {
    std::fstream jsonfile {Server::config_path};

    if (jsonfile.fail()) {
        std::cerr << "ERROR: Unable to read config file.\n";
        _port = -1;
        _host = "";
        return;
    }

    json cfg {json::parse(jsonfile)};

    _port = cfg[0]["server"]["port"];
    _host = cfg[0]["server"]["host"].get<std::string>();
}

void Server::Run() {
    // Check if JSON was read correctly
    if (_port == -1 || _host == "") {
        std::cerr << "ERROR: Server initialization failed. Shutting down.\n";
        return;
    }

    httplib::Server server;

    // GET Request handler for phone numbers
    // Will return phone number matching the regexp
    server.Get(R"((/)(\+?[7-8](\d{10})))", [&](const httplib::Request& req, httplib::Response& res) {
        auto numbers = req.matches[2];

        Call incoming_call {numbers};

        res.set_content(incoming_call.getID(), "text/plain");
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

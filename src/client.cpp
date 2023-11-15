#include "client.h"

using json = nlohmann::json;

Client::Client() {
    ClientSettingsInit();
}

void Client::ClientSettingsInit() {
    std::ifstream jsonfile(Client::config_path);

    if (jsonfile.fail()) {
        std::cerr << "ERROR: Unable to read config file\n";
        _port = -1;
        _host = "";
        return;
    }

    json cfg {json::parse(jsonfile)};

    _port = cfg[0]["client"]["port"];
    _host = cfg[0]["client"]["host"].get<std::string>();
}

void Client::Run() {
    // Check if JSON file was read correctly
    if (_port == -1 || _host == "") {
        std::cerr << "ERROR: Client initialization failed. Shutting down\n";
        return;
    }

    httplib::Client client {_host, _port};

    // Check if client is OK
    if (!client.is_valid()) {
        std::cerr << "ERROR: Client is not valid! Check the configuration file\n";
        return;
    }

    std::cout << "The client is running properly\n";

    // Main loop to send requests to server
    httplib::Result res {};
    while (true) {
        std::cout << "Enter request: ";
        std::cin >> _request;

        if (res = client.Get("/"+_request)) {
            if (res->status != 200) {
                std::cerr << "WARNING: The request is invalid!\n\n"; 
                continue;
            }
        }
        else {
            std::cerr << "ERROR: " << res.error() << '\n'; // Will notify if server is down
            continue;
        }

        std::cout << "Response: " << res->body << '\n';
    }
}
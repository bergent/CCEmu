#include "client.h"

using json = nlohmann::json;

Client::Client() {
    ClientSettingsInit();
}

void Client::ClientSettingsInit() {
    std::ifstream jsonfile("../config.json");
    json cfg {json::parse(jsonfile)};

    _port = cfg[0]["client"]["port"];
    _host = cfg[0]["client"]["host"].get<std::string>();
}

void Client::Run() {
    httplib::Client client {_host, _port};

    if (!client.is_valid()) {
        std::cerr << "ERROR: Client is not valid! Check the configuration file\n";
        return;
    }

    std::cout << "The client is running properly\n";

    httplib::Result res {};
    while (true) {
        std::cout << "Enter request: ";
        std::cin >> _request;

        res = client.Get("/"+_request);

        if (res->status != 200) {
            std::cerr << "WARNING: The request is incorrect!\n";
            continue;
        }

        std::cout << "Response: " << res->body << '\n';
    }
}
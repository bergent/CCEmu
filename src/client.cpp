#include "client.h"

using json = nlohmann::json;

Client::Client() {
    LoggerInit();
    ClientSettingsInit();
}

void Client::LoggerInit() {
    try {
        _logger = spdlog::basic_logger_mt("client_log", Client::log_path,
                                          true);
    }
    catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Client log init failed: " << ex.what() << '\n';
    }
    spdlog::flush_on(spdlog::level::trace);
    spdlog::set_default_logger(_logger);
    _logger->info("Logger initialized successfully");
}

void Client::ClientSettingsInit() {
    std::ifstream jsonfile(Client::config_path);

    if (jsonfile.fail()) {
        _logger->error("Unable to read configuration file");
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
        std::cerr << "[ERROR] Client initialization failed. Shutting down.\n";
        _logger->error("Client initialization failed. Shutting down.");
        return;
    }

    httplib::Client client {_host, _port};

    // Check if client is OK
    if (!client.is_valid()) {
        _logger->error("Client is not valid! Shutting down.");
        return;
    }

    _logger->info("The client is running properly");

    // Main loop to send requests to server
    httplib::Result res {};
    while (true) {
        std::cout << "Enter request: ";
        std::cin >> _request;

        _logger->info("Sent request to server: " + _request);
        if (res = client.Get("/"+_request)) {
            if (res->status != 200) {
                std::cerr << "[WARNING] The request is invalid!\n\n"; 
                _logger->warn("Incorrect request: " + _request);
                continue;
            }
        }
        else {
            std::cerr << "[ERROR] Can't establish connection to server.\n\n";
            _logger->error("Can't establish connection to server.");
            continue;
        }
        std::cout << "Response: " << res->body << "\n\n";
        _logger->info("Server response: " + res->body);
    }
}
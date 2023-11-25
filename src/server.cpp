#include "server.h"


using json = nlohmann::json;

Server::Server() {
    LoggerInit();
    SettingsInit();
    _callcenter = std::make_unique<CallCenter>();
}

void Server::LoggerInit() {
    try {
        _logger = spdlog::basic_logger_mt("server_log", Server::log_path,
                                          true);
    }
    catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Server log init failed: " << ex.what() << '\n';
    }
    spdlog::flush_on(spdlog::level::trace);
    spdlog::set_default_logger(_logger);
    _logger->info("Logger initialized successfully");
}

void Server::SettingsInit() {
    std::fstream jsonfile {Server::config_path};

    if (jsonfile.fail()) {
        _logger->error("Unable to read config file.");
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
        std::cerr << "[ERROR] Server initialization failed. Shutting down.\n";
        _logger->error("Server initialization failed. Shutting down.");
        return;
    }

    httplib::Server server;

    // GET Request handler for phone numbers
    // Will return phone number matching the regexp
    server.Get(R"((/)(\+?[7-8](\d{10})))", [&](const httplib::Request& req, httplib::Response& res) {
        _logger->info("Received request: {}", std::string(req.matches[0]));
        auto numbers = req.matches[2];

        UUIDv4::UUID uuid { _uuid_generator.getUUID() };

        _logger->info("Matched phone number: {}", std::string(numbers));
        std::shared_ptr<Call> incoming_call {std::make_shared<Call>(std::string(numbers), uuid.str())};

        auto status = _callcenter->ReceiveCall(incoming_call);

        if (status == IncomingStatus::OK) {
            res.set_content(incoming_call->getID() + " | OK", "text/plain");
            incoming_call->getCDR().status = "OK";
        } 
        else if (status == IncomingStatus::Overload) {
            res.set_content(incoming_call->getID() + " | Overload", "text/plain");
            incoming_call->getCDR().status = "Overload";
            _callcenter->WriteCDR(incoming_call->getCDR());
        } 
        else if (status == IncomingStatus::Duplication) {
            res.set_content(incoming_call->getID() + " | Duplication", "text/plain");
            incoming_call->getCDR().status = "Duplication";
            _callcenter->WriteCDR(incoming_call->getCDR());
        } 
        else if (status == IncomingStatus::Queued) {
            res.set_content(incoming_call->getID() + " | Queued", "text/plain");
            incoming_call->getCDR().status = "OK";
        }
        else {
            res.set_content(incoming_call->getID() + " | Undefined Error", "text/plain");
            incoming_call->getCDR().status = "Error";
            _callcenter->WriteCDR(incoming_call->getCDR());
        }

        _logger->info("Sent response to the client: " + incoming_call->getID());
    }); 

    // Check if server is valid
    if (!server.is_valid()) {
        _logger->error("Server is not valid! Shutting down.");
       return;
    }
    _logger->info("The server is running properly.");

    // Make server listen to the designated host and port
    server.listen(_host, _port);
}

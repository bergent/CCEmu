#include "callcenter.h"

using json = nlohmann::json;

CallCenter::CallCenter() {
    LoggerInit();
    SettingsInit();
}

void CallCenter::LoggerInit() {
    try {
        _logger = spdlog::basic_logger_mt("callcenter_log", CallCenter::log_path,
                                          true);
    }
    catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "CallCenter log init failed: " << ex.what() << '\n';
    }
    spdlog::flush_on(spdlog::level::trace);
    spdlog::set_default_logger(_logger);
    _logger->info("Logger initialized successfully");
}

void CallCenter::SettingsInit() {
    std::ifstream jsonfile(CallCenter::config_path);

    if (jsonfile.fail()) {
        _logger->error("Unable to read configuration file");
        return;
    }

    json cfg {json::parse(jsonfile)};

    _num_operators = cfg[0]["callcenter"]["num_operators"];
    _queue_size = cfg[0]["callcenter"]["queue_size"];

    std::string_view cfg_policy {cfg[0]["callcenter"]["same_number_policy"].get<std::string>()};
    if ( cfg_policy == "warning") {
        _policy = Policy::Warning;
    } 
    else if (cfg_policy == "replace") {
        _policy = Policy::Replace;
    }
    else {
        std::cerr << "[WARNING] Policy parameter is incorrect. Warning policy will be set by default\n";
        _logger->warn("Policy parameter is incorrect. Warning policy will be set by default");
    }

   _min_queue_time = cfg[0]["callcenter"]["min_queue_time"];
   _max_queue_time = cfg[0]["callcenter"]["max_queue_time"];
   
   _min_call_time = cfg[0]["callcenter"]["min_call_time"];
   _max_call_time = cfg[0]["callcenter"]["max_call_time"];
}
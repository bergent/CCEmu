#include "callcenter.h"

using json = nlohmann::json;

CallCenter::CallCenter() {
    LoggerInit();
    SettingsInit();
    InitOperators();
    randomizer = std::make_unique<Randomizer>(_min_queue_time, _max_queue_time,
                                             _min_call_time, _max_call_time);
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
    _queue_max_size = cfg[0]["callcenter"]["queue_max_size"];

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

void CallCenter::InitOperators() {
    _operators.reserve(_num_operators);

    for (std::size_t idx; idx < _num_operators; ++idx) {
        auto& operator_ref = _operators.emplace_back(Operator(this));
        _free_operators.push_front(operator_ref);
    }
    _logger->info("Operators ready");
}

bool CallCenter::IsOperatorsAvailable() const {
    return !(_free_operators.empty());
}

bool CallCenter::IsQueueEmpty() const {
    return _call_queue.empty();
}

bool CallCenter::IsQueueFull() const {
    if (_call_queue.size() == _queue_max_size)
        return true;
    return false;
}

bool CallCenter::IsDuplication(const Call& call) const {
    if (IsQueueEmpty())
        return false;

    if (std::find(_call_queue.begin(), _call_queue.end(), call) != _call_queue.end()) {
        return true;
    }
    return false;
}

IncomingStatus CallCenter::ReceiveCall(Call& call) {
    if (IsQueueFull()) {
        _logger->warn("Call " + call.getNumber() + " declined due to overload");
        return IncomingStatus::Overload;
    }
    else if (IsDuplication(call)) {
        _logger->warn("Call " + call.getNumber() + " declined due to call duplication");
        return IncomingStatus::Duplication;
    }
    else if (IsQueueEmpty() && IsOperatorsAvailable()) {
        Connect(_free_operators.back(), call);
        _logger->info("Call " + call.getNumber() + " accepted by operator #" + _free_operators.back().get().getIDStr());
        _free_operators.pop_back();
        return IncomingStatus::OK;
    }
    else {
        _call_queue.push_front(call);
        _logger->info("Call " + call.getNumber() + " placed in queue");
        return IncomingStatus::Queued;
    }
}

void CallCenter::Connect(Operator& oper, Call& call) {
    int call_time = randomizer->getCallTime();
    call.getCDR().operator_response_time = boost::posix_time::microsec_clock::local_time();
    call.getCDR().operator_id = oper.getID();
    call.getCDR().call_duration = call_time;

}

void CallCenter::WriteCDR(const CDREntry& cdr) {
    std::cout << "Hello!\n";
}
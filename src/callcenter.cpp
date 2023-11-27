#include "callcenter.h"

using json = nlohmann::json;

std::mutex watchdog_lock;

CallCenter::CallCenter() {
    LoggerInit();
    SettingsInit();
    InitOperators();
    InitRandomizer();
    InitQueueWatchdog();
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
   _queue_refresh_time = cfg[0]["callcenter"]["queue_refresh_time"];
   
   _min_call_time = cfg[0]["callcenter"]["min_call_time"];
   _max_call_time = cfg[0]["callcenter"]["max_call_time"];
}

void CallCenter::InitOperators() {
    for (std::size_t idx {0}; idx < _num_operators; ++idx) {
        _operators.push_back(std::make_shared<Operator>(this));
        _free_operators.push_front(std::shared_ptr<Operator>(_operators.back()));
        _logger->info("Added operator {}", _operators.back()->getID());
    }
    _logger->info("Operators ready");
}


void CallCenter::InitRandomizer() {
    _randomizer = std::make_unique<Randomizer>(_min_queue_time, _max_queue_time,
                                             _min_call_time, _max_call_time);
    _logger->info("Randomizer ready");
}

bool CallCenter::IsOperatorsAvailable() const {
    _logger->debug("Free operators: {}", _free_operators.size());
    return (_free_operators.size() > 0);
}

bool CallCenter::IsQueueEmpty() const {
    _logger->debug("Checking if call queue is empty: {}", _call_queue.empty());
    return _call_queue.empty();
}

bool CallCenter::IsQueueFull() const {
    _logger->debug("Checking if call queue is full: {}", _call_queue.size() >= _queue_max_size);
    return (_call_queue.size() >= _queue_max_size);
}

bool CallCenter::IsDuplication(const Call* const call) const {
    _logger->debug("Checking if call is a duplication");

    auto queue_result = std::find_if(_call_queue.begin(), _call_queue.end(), [call](Call* queued_call){
        return call->getNumber() == queued_call->getNumber();
    });

    if (queue_result != _call_queue.end()) {
        _logger->debug("Found call {} duplictaion in call queue", call->getNumber());
        return true;
    }

    if (auto session_result = _sessions.find(call->getNumber()); session_result != _sessions.end()) {
        _logger->debug("Found call {} duplication in active sessions", call->getNumber());
        return true;
    }

    _logger->debug("No duplication of call {}", call->getNumber());
    return false;
}

IncomingStatus CallCenter::ReceiveCall(std::shared_ptr<Call> call) {
    if (IsQueueFull()) {
        _logger->warn("For number {} status is {}", call->getNumber(), "Overload");
        return IncomingStatus::Overload;
    }
    else if (IsDuplication(call.get())) {
        _logger->warn("For number {} status is {}", call->getNumber(), "Duplication");
        return IncomingStatus::Duplication;
    }
    else if (IsQueueEmpty() && IsOperatorsAvailable()) {
        _logger->info("Connecting number {} to operator #{}", call->getNumber(), _free_operators.back()->getID());
        Connect(_free_operators.back(), call.get());
        _free_operators.pop_back();
        return IncomingStatus::OK;
    }
    else {
        int queue_waiting_time = _randomizer->getQueueTime(); 
        call->SetQueueTime(queue_waiting_time);
        _call_queue.push_front(call.get());
        _logger->info("Number {} was placed in call queue. Current position {}/{}", call->getNumber(), _call_queue.size(), _queue_max_size);
        return IncomingStatus::Queued;
    }
    return IncomingStatus::Undefined;
}

void CallCenter::Connect(std::shared_ptr<Operator> oper, Call* call) {
    int call_duration = _randomizer->getCallTime();
    _sessions.insert(call->getNumber());
    oper->setCurrentCall(call);
    call->getCDR().operator_response_time = boost::posix_time::microsec_clock::local_time();
    call->getCDR().operator_id = oper->getID();
    call->getCDR().call_duration = call_duration;

    oper->RunCall(call_duration);
}

void CallCenter::EndActiveSession(const std::string& number) {
    if (auto iter = _sessions.find(number); iter != _sessions.end())
        _sessions.erase(iter);
}

void CallCenter::SetOperatorReady(Operator* oper) {
    auto iter = std::find_if(_operators.begin(), _operators.end(),
    [oper](std::shared_ptr<Operator> base_oper){
        return oper->getID() == base_oper->getID();
    });

    _free_operators.push_front(*iter);
}

void CallCenter::WriteCDR(const CDREntry& cdr) {
    std::cout << "Hello!\n";
}

void CallCenter::InitQueueWatchdog() {
    std::thread watchdog {&CallCenter::RefreshQueue, this, _queue_refresh_time};
    watchdog.detach();
    _logger->info("Queue watchdog initialized");
}

void CallCenter::RefreshQueue(int refresh_time) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(refresh_time));

        if (IsQueueEmpty()) {
            continue;
        }

        auto time_now {boost::posix_time::microsec_clock::local_time()};
        if (time_now > _call_queue.back()->GetQueueTime()) {
            _call_queue.back()->getCDR().status = "Timeout";
            _call_queue.back()->getCDR().release_time = time_now;

            std::unique_lock lock(watchdog_lock);

            WriteCDR(_call_queue.back()->getCDR());
            _call_queue.pop_back();

            lock.unlock();

        } 
        else {
            if (IsOperatorsAvailable()) {
                _logger->info("Connecting number {} to operator #{}", _call_queue.back()->getNumber(),
                                                                      _free_operators.back()->getID());
                
                std::unique_lock lock(watchdog_lock);

                Connect(_free_operators.back(), _call_queue.back());
                _free_operators.pop_back();
                _call_queue.pop_back();

                lock.unlock();
            }
        }
    }
}



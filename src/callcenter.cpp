#include "callcenter.h"

using json = nlohmann::json;

std::mutex watchdog_lock;

// CDR Header line
// Used to easily conver resulting cdr.txt file to
// (for example) .csv file and process it with Excel/Calc
inline const std::string CDR_header {"IncomingDT;CallID;CgPN;ReleaseDT;Status;ResponseDT;OperatorID;CallDuration"};

// CallCenter constructor
CallCenter::CallCenter() {
    LoggerInit();
    SettingsInit();
    InitOperators();
    InitCDRFile();
    InitRandomizer();
    InitQueueWatchdog();
}

// Initialize logger
void CallCenter::LoggerInit() {
    try {
        _logger = spdlog::basic_logger_mt("callcenter_log", CallCenter::log_path,
                                          true);
    }
    catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "CallCenter log init failed: " << ex.what() << '\n';
    }
    spdlog::flush_on(spdlog::level::trace);
    _logger->info("Logger initialized successfully");
}

// Read settings from JSON
void CallCenter::SettingsInit() {
    std::ifstream jsonfile(CallCenter::config_path);

    if (jsonfile.fail()) {
        _logger->error("Unable to read configuration file");
        return;
    }

    json cfg {json::parse(jsonfile)};

    _debug_log = cfg[0]["debug_log"];
    if (_debug_log)
        _logger->set_level(spdlog::level::trace);
    else
        _logger->set_level(spdlog::level::info);

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

// Create _num_opearors objects of class Operator
void CallCenter::InitOperators() {
    for (std::size_t idx {0}; idx < _num_operators; ++idx) {
        _operators.push_back(std::make_shared<Operator>(this));
        _free_operators.push_front(std::shared_ptr<Operator>(_operators.back()));
        _logger->debug("Added operator {}", _operators.back()->getID());
    }
    _logger->info("Operators ready");
}

// Instantiate randomizer object
void CallCenter::InitRandomizer() {
    _randomizer = std::make_unique<Randomizer>(_min_queue_time, _max_queue_time,
                                             _min_call_time, _max_call_time);
    _logger->info("Randomizer ready");
}

// Prepare cdr.txt file for output
void CallCenter::InitCDRFile() {
    try {
        _cdr_logger = spdlog::basic_logger_mt("cdr_journal", CallCenter::cdr_path,
                                          true);
    }
    catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "CDR log init failed: " << ex.what() << '\n';
    }

    _cdr_logger->flush_on(spdlog::level::trace);
    _cdr_logger->set_level(spdlog::level::trace);
    _cdr_logger->set_pattern("%v");
    _cdr_logger->trace("{}", CDR_header);
    _cdr_ss.imbue(std::locale(_cdr_ss.getloc(), _pfacet.get()));
    _logger->info("CDR logger initialized successfully");
}

// Checks if there are operators in _free_operators deque
bool CallCenter::IsOperatorsAvailable() const {
    _logger->debug("Free operators: {}", _free_operators.size());
    return (_free_operators.size() > 0);
}

// Checks if _call_queue is empty
bool CallCenter::IsQueueEmpty() const {
    _logger->debug("Checking if call queue is empty: {}", _call_queue.empty());
    return _call_queue.empty();
}

// Checks if _call_queue is full
bool CallCenter::IsQueueFull() const {
    _logger->debug("Checking if call queue is full: {}", _call_queue.size() >= _queue_max_size);
    return (_call_queue.size() >= _queue_max_size);
}

// Checks for call duplication in _call_queue and _sessions
bool CallCenter::IsDuplication(const Call* const call) const {
    _logger->debug("Checking if call is a duplication");

    auto queue_result = std::find_if(_call_queue.begin(), _call_queue.end(), [call](std::shared_ptr<Call> queued_call){
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

// Incoming call handler. Produces status for incoming call and
// passes it down to _call_queue or directly to Operator instance 
// (in case if there are free operators and _call_queue is empty) or
// rejecting the incoming call in case call queue overload or call duplication
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
        Connect(_free_operators.back(), call);
        _free_operators.pop_back();
        return IncomingStatus::OK;
    }
    else {
        int queue_waiting_time = _randomizer->getQueueTime(); 
        call->SetQueueTime(queue_waiting_time);
        _call_queue.push_front(call);
        _logger->info("Number {} was placed in call queue. Current position {}/{}", call->getNumber(), _call_queue.size(), _queue_max_size);
        return IncomingStatus::Queued;
    }
    return IncomingStatus::Undefined;
}

// Connect Operator instance to specific call
void CallCenter::Connect(std::shared_ptr<Operator> oper, std::shared_ptr<Call> call) {
    int call_duration = _randomizer->getCallTime();
    _sessions.insert(call->getNumber());
    oper->setCurrentCall(call);
    call->getCDR().operator_response_time = boost::posix_time::microsec_clock::local_time();
    call->getCDR().operator_id = oper->getID();
    call->getCDR().call_duration = call_duration;

    oper->RunCall(call_duration);
}

// Remove call number from _sessions set
void CallCenter::EndActiveSession(const std::string& number) {
    if (auto iter = _sessions.find(number); iter != _sessions.end())
        _sessions.erase(iter);
}

// Put Operator instance to _free_operators deque's front
void CallCenter::SetOperatorReady(Operator* oper) {
    auto iter = std::find_if(_operators.begin(), _operators.end(),
    [oper](std::shared_ptr<Operator> base_oper){
        return oper->getID() == base_oper->getID();
    });

    _free_operators.push_front(*iter);
}

// Writing CDR Journal entry to output file
void CallCenter::WriteCDR(const CDREntry& cdr) {
    _cdr_ss << cdr;
    std::string cdr_line {_cdr_ss.str()};
    _cdr_logger->trace("{}", cdr_line);
    _logger->info("Wrote CDR entry for number {}", cdr.phone_number);
    _cdr_ss.str(std::string());
}

// Initialize watchdog thread designed to manage the call queue
void CallCenter::InitQueueWatchdog() {
    std::thread watchdog {&CallCenter::RefreshQueue, this, _queue_refresh_time};
    watchdog.detach();
    _logger->info("Queue watchdog initialized");
}

// Watchdog thread's execution function.
// Checks call queue evere refresh_time miliseconds
// If there are free operators it will connect the first call
// in deque to it. 
// If current time is greater than queue waiting time of first call in deque
// will reject this call with Timeout status
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



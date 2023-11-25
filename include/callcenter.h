#ifndef CCEMU_CALLCENTER_H
#define CCEMU_CALLCENTER_H

#include "randomizer.h"
#include "call.h"
#include "operator.h"
#include "cdrentry.h"

#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "boost/date_time/posix_time/posix_time.hpp"

#include <iostream>
#include <fstream>
#include <memory>
#include <functional>
#include <unordered_map>
#include <vector>
#include <deque>

class Operator;

// TODO: Different policies for duplication of incoming calls
enum class Policy {
    Warning,
    Replace,
};

// Status codes for incoming call
enum class IncomingStatus {
    OK,
    Overload,
    Duplication,
    Queued,
    Undefined,
};

enum class SessionStatus {
    Active,
    Inactive
};

class CallCenter {
public:
    CallCenter();

    // Main function to handle incoming call
    IncomingStatus ReceiveCall(std::shared_ptr<Call> call);
    void WriteCDR(const CDREntry& cdr);
    
private:
    // Config and log
    inline static const std::string config_path {"../config.json"};
    inline static const std::string log_path {"../logs/callcenter.log"};
    std::shared_ptr<spdlog::logger> _logger;

    // CDR file location
    inline static const std::string cdr_path {"../cdr.txt"};

    // Config variables 
    std::size_t _num_operators{};
    std::size_t _queue_max_size{};

    // Timings in milliseconds
    int _min_queue_time;
    int _max_queue_time;

    int _min_call_time;
    int _max_call_time;

    // Duplication policy 
    Policy _policy;

    // Containers
    std::deque<Call*> _call_queue;
    std::vector<Operator> _operators;
    std::deque<Operator*> _free_operators;
    std::unordered_map<std::string, SessionStatus> _sessions;

    // Randomizer
    std::unique_ptr<Randomizer> randomizer;

    // Private functions
    void Connect(Operator* oper, Call* call);
    bool IsOperatorsAvailable() const;
    bool IsQueueFull() const;
    bool IsQueueEmpty() const;
    bool IsDuplication(const Call* const call) const;

    // Initialization functions
    void SettingsInit();
    void LoggerInit();
    void InitOperators();
    void InitRandomizer();
};

#endif
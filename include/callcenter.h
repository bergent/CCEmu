#ifndef CCEMU_CALLCENTER_H
#define CCEMU_CALLCENTER_H

#include "call.h"
#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#include <iostream>
#include <fstream>
#include <deque>

enum class Policy {
    Warning,
    Replace
};

class CallCenter {
public:
    CallCenter();
    
private:
    // Config and log
    inline static const std::string config_path {"../config.json"};
    inline static const std::string log_path {"../logs/callcenter.log"};
    std::shared_ptr<spdlog::logger> _logger;

    // Config variables 
    unsigned int _num_operators{};
    std::size_t _queue_size{};

    // Timings in seconds
    unsigned int _min_queue_time;
    unsigned int _max_queue_time;

    unsigned int _min_call_time;
    unsigned int _max_call_time;

    Policy _policy;

    // Containers

    // Initialization functions
    void SettingsInit();
    void LoggerInit();
};

#endif
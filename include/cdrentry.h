#ifndef CCEMU_CDRENTRY_H
#define CCEMU_CDRENTRY_H

#include "boost/date_time/posix_time/posix_time.hpp"
#include <string>

struct CDREntry {
    boost::posix_time::ptime incoming_time;
    boost::posix_time::ptime release_time;
    boost::posix_time::ptime operator_response_time;
    boost::posix_time::ptime call_duration;
    std::string call_id{};
    std::string phone_number{};
    std::string status{};
    int operator_id{};
};

#endif
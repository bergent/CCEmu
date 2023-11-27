#ifndef CCEMU_CDRENTRY_H
#define CCEMU_CDRENTRY_H

#include "boost/date_time/posix_time/posix_time.hpp"
#include <string>

struct CDREntry {
    boost::posix_time::ptime incoming_time {};
    boost::posix_time::ptime release_time {};
    boost::posix_time::ptime operator_response_time {};
    int call_duration {0};
    std::string call_id{};
    std::string phone_number{};
    std::string status{};
    int operator_id {-1};

    friend std::ostream& operator<< (std::ostream& out, const CDREntry& entry) {
        if (!entry.incoming_time.is_not_a_date_time()) {
            out << entry.incoming_time << ';';
        } else {
            out << ';';
        }

        out << entry.call_id << ';' << entry.phone_number << ';';
        
        if (!entry.release_time.is_not_a_date_time()) {
            out << entry.release_time << ';';
        } else {
            out << ';';
        }

        out << entry.status << ';';
        
        if (!entry.operator_response_time.is_not_a_date_time()) {
            out << entry.operator_response_time << ';';
        } else {
            out << ';';
        }

        out << entry.operator_id << ';' << entry.call_duration;

        return out;
    }
};



#endif
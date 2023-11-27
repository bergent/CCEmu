#include "call.h"

Call::Call(const std::string& number, const std::string& id) 
    : _phone_number{ number }, _id{ id }
{
    _call_cdr.incoming_time = boost::posix_time::microsec_clock::local_time();
    _call_cdr.call_id = _id;
    _call_cdr.phone_number = _phone_number;
}

const std::string& Call::getID() const {
    return _id;
}

const std::string& Call::getNumber() const {
    return _phone_number;
}

void Call::SetQueueTime(int ms) {
    auto duration {boost::posix_time::time_duration(boost::posix_time::milliseconds(ms))};
    _queue_time = boost::posix_time::microsec_clock::local_time() + duration;
}

const boost::posix_time::ptime& Call::GetQueueTime() {
    return _queue_time;
}

CDREntry& Call::getCDR() {
    return _call_cdr;
}

const CDREntry& Call::getCDR() const {
    return _call_cdr;
}
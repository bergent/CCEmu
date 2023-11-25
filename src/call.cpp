#include "call.h"

Call::Call(const std::string& number, const std::string& id) 
    : _phone_number{ number }, _id{ id }
{
    _time_incoming = boost::posix_time::microsec_clock::local_time();
    _call_cdr.incoming_time = _time_incoming;
    _call_cdr.call_id = _id;
    _call_cdr.phone_number = _phone_number;
}

const std::string& Call::getID() const {
    return _id;
}

const std::string& Call::getNumber() const {
    return _phone_number;
}

const boost::posix_time::ptime& Call::getTimeIncoming() const {
    return _time_incoming;
}

CDREntry& Call::getCDR() {
    return _call_cdr;
}

const CDREntry& Call::getCDR() const {
    return _call_cdr;
}